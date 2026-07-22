# Add NNUE Evaluation (training + inference pipeline)

## Context

The engine currently evaluates positions with a hand-crafted material + piece-square-table
function, `int evaluate(BoardState& boardState)` (`src/evaluate.cpp:219-252`), called only
from `quiescenceSearch` (`src/search.cpp:299,304`) — `minimax` never evaluates directly, it
always drops into quiescence at `depth == 0`. The user wants to replace this with a small
NNUE (Efficiently Updatable Neural Network) evaluator, following the "Basic NNUE" topology
from https://www.chessprogramming.org/NNUE: two 256-wide perspective accumulators → concat
(side-to-move first) → Clipped ReLU → 32 → Clipped ReLU → 32 → Clipped ReLU → 1, scaled to
centipawns.

This is a new subsystem spanning two languages by explicit user choice: **self-play data
generation and engine inference stay pure C++** (reusing this engine's own search/eval to
generate labeled positions, matching how `createKeys.cpp` and `matchManager.cpp`'s existing
self-play loop already work in this codebase), while **the actual gradient-descent training
loop is a new PyTorch script**, since hand-rolling autodiff/backprop in C++ for this task
would be significant extra effort for no real benefit over an already-solved problem.

Key existing facts this plan builds on (verified by direct read this session):
- Piece/color order is **non-standard**: `PAWN=0, BISHOP=1, KNIGHT=2, ROOK=3, QUEEN=4, KING=5`,
  `WHITE=0, BLACK=1` (`src/physics.hpp:13-21`). Squares are LERF, a1=0..h8=63.
- `BoardState.pieces[color][pieceType]` are the bitboards; `pieceArray[64]` is a type-only
  mailbox (`src/physics.hpp:69-81`).
- `makeMove`/`unmakeMove` (`src/physics.cpp:579-811`, `494-577`) already do an **inline
  incremental zobrist-hash update** per switch-case, and a **wholesale snapshot-restore**
  (`boardState.zobristHash = undoState.oldZobristHash;` at `physics.cpp:576`) on unmake rather
  than an inverse per-case update. Both of these are the precedents this plan follows for the
  NNUE accumulator: incremental add on make, snapshot-restore on unmake.
- `SearchInfo` (`src/search.hpp:37-46`) already carries one per-thread value member,
  `GameHistorySearch history`, added last session specifically because the engine's 4-5
  lazy-SMP threads each get independent copies via by-value `BoardState`/`SearchInfo`
  arguments to `std::thread`/`searchBestMoveIt` (`src/engine.cpp`, `src/search.cpp:3,11`).
  The NNUE accumulator follows the exact same per-thread-value-member pattern.
- `openBook.cpp`'s `boardStateHash()` (loop over all 12 bitboards, XOR per occupied square,
  `src/openBook.cpp:137-149`) is a direct structural precedent for "refresh from scratch."
- `createKeys.cpp` (7-line `main()` calling one generator, own makefile target, writes JSON
  under `data/`) is the precedent for the new self-play data-gen binary.
- `lib/` only contains vendored `nlohmann/json.hpp`; no SIMD/linear-algebra library exists.
  `makefile`'s `CXXFLAGS` has no SIMD flags. **v1 will be plain float32 scalar C++, no
  quantization, no AVX intrinsics** — that is explicitly deferred as future work, matching
  the user's ask for a *minimal* setup.

---

## 1. Feature encoding (must match byte-for-byte between C++ and Python)

768 inputs = 6 piece types × 2 "relative colors" × 64 squares. For a given **perspective**
(WHITE or BLACK) and a piece of a given actual `color`/`pieceType`/`square`:

```cpp
int relativeColor  = (color == perspective) ? 0 : 1;              // 0 = "my piece", 1 = "their piece"
int relativeSquare = (perspective == WHITE) ? square : (square ^ 56); // mirror vertically for BLACK's view
int featureIndex   = pieceType * 128 + relativeColor * 64 + relativeSquare;   // range [0, 767]
```

Two accumulators are computed per position, one per perspective, from the *same* board. Every
occupied square contributes exactly one feature to **each** perspective's accumulator (two
feature activations per physical piece). Because features are relative-color/mirrored-square
rather than king-relative (i.e. not HalfKP), a king move — including castling — is handled by
the *ordinary* piece add/remove path with no special "recompute everything" case. This is the
simplification that makes the plain 768-feature "Basic NNUE" tractable for a minimal v1.

At the output stage, concatenate `[acc[sideToMove], acc[opponent]]` (side-to-move's
accumulator first) before the clipped-ReLU/hidden layers — this lets the same net naturally
produce a side-to-move-relative score. The final wrapper converts to white-absolute
centipawns (`evaluate()`'s existing contract) by negating when black is to move.

---

## 2. C++ inference module — new `src/nnue.hpp` / `src/nnue.cpp`

```cpp
#define NNUE_INPUT_SIZE 768
#define NNUE_FT_SIZE    256
#define NNUE_L1_SIZE    32
#define NNUE_L2_SIZE    32

struct NNUEAccumulator {
    float values[2][NNUE_FT_SIZE];   // [perspective][neuron], perspective indexed by WHITE/BLACK
};

void loadNNUEWeights(const std::string& fileName);
void nnueRefreshAccumulator(BoardState& boardState, NNUEAccumulator& acc);   // full recompute
void nnueAddPiece(NNUEAccumulator& acc, int color, int pieceType, int square);
void nnueRemovePiece(NNUEAccumulator& acc, int color, int pieceType, int square);
int  nnueEvaluate(BoardState& boardState, NNUEAccumulator& acc);  // white-absolute centipawns
```

`nnue.cpp` holds the weight arrays as globals (`float ftWeight[768][256]; float ftBias[256];
float l1Weight[512][32]; float l1Bias[32]; float l2Weight[32][32]; float l2Bias[32];
float l3Weight[32]; float l3Bias;`), loaded by `loadNNUEWeights` via plain `fread` in the
exact order the Python export writes them (§5). `nnueRefreshAccumulator` zeroes to `ftBias`
then loops `for color in {WHITE,BLACK} for pieceType in PAWN..KING` over
`boardState.pieces[color][pieceType]`, calling `nnueAddPiece` per set bit — structurally
identical to `boardStateHash()` (`openBook.cpp:137-149`). `nnueAddPiece`/`nnueRemovePiece`
add/subtract row `featureIndex(WHITE,...)` of `ftWeight` into `acc.values[WHITE]` and row
`featureIndex(BLACK,...)` into `acc.values[BLACK]`. `nnueEvaluate` builds the 512-wide
concatenated+clipped-ReLU vector, applies L1→ClippedReLU→L2→ClippedReLU→L3, scales the scalar
output to centipawns, and negates for black-to-move to match `evaluate()`'s contract.

---

## 3. Incremental update integration

**`makeMove` gains one new optional parameter**, default `nullptr` so every other caller
(`generateLegalMoves`, perft, `matchManager`, etc.) is unaffected:

```cpp
void makeMove(BoardState& boardState, Move move, UndoState& undoState, NNUEAccumulator* acc = nullptr);
```

Exactly one generic removal is added right where the source square is already cleared
generically for *every* move type (`physics.cpp:648-650`, before the `switch`):
`if(acc) nnueRemovePiece(*acc, boardState.sideToMove, pieceType, move.source);`

Then, mirroring each existing `zobristHash ^= zobristTable[...]` line inside the switch,
add the matching `nnueAddPiece`/`nnueRemovePiece` call using the *same* `(color, pieceType,
square)` triple that line's `zobristPieceIndex`/square already encode:

| Case | Existing zobrist lines (physics.cpp) | Matching NNUE calls |
|---|---|---|
| QUIETMOVE / DOUBLEMOVE | add at dest (654/661) | `add(stm, pieceType, dest)` |
| KINGCASTLE / QUEENCASTLE | add king at dest + rook relocate (674, 682-684 / 690, 698-700) | `add(stm, pieceType/*KING*/, dest)`, `remove(stm, ROOK, rookSrc)`, `add(stm, ROOK, rookDest)` |
| CAPTUREMOVE | add at dest (705) + remove captured (709-710) | `add(stm, pieceType, dest)`, `remove(opp, destPieceType, dest)` |
| ENPASSANTCAPTURE | add at dest (715) + remove captured pawn at dest∓8 (722/727) | `add(stm, pieceType, dest)`, `remove(opp, PAWN, dest∓8)` |
| `*PROMO` | add promoted type at dest (735/743/751/759) | `add(stm, promotedType, dest)` (no separate "remove pawn" — already covered by the generic source removal) |
| `*PROMOCAPTURE` | add promoted type + remove captured (per-case) | `add(stm, promotedType, dest)`, `remove(opp, destPieceType, dest)` |

`unmakeMove`'s signature is **not changed at all**. Instead, following the same
"snapshot-restore, not inverse-update" pattern the codebase already uses for `zobristHash`
(`physics.cpp:576`), `SearchInfo` owns a small fixed-size accumulator stack, mirroring
`GameHistorySearch`'s own `stack[]`/`head` shape (`search.hpp:17-35`):

```cpp
struct SearchInfo {
    ...
    NNUEAccumulator nnueAcc;
    NNUEAccumulator accumulatorStack[256];   // 256 >> realistic max search ply
    int accumulatorHead = 0;
};
```

In `search.cpp`, at every existing `makeMove(...)`/`unmakeMove(...)` pair (the 3 sites in
`minimax` and `quiescenceSearch` that already bracket these calls with
`searchInfo.history.push()`/`.pop()`), add:
- immediately before `makeMove`: `searchInfo.accumulatorStack[searchInfo.accumulatorHead++] = searchInfo.nnueAcc;`
- change the `makeMove` call to pass `&searchInfo.nnueAcc`
- immediately after `unmakeMove`: `searchInfo.nnueAcc = searchInfo.accumulatorStack[--searchInfo.accumulatorHead];`

At the top of `searchBestMoveIt` (`search.cpp:3-16`), alongside the existing
`searchInfo.history = gameHistorySearch;` seeding line, add:
`nnueRefreshAccumulator(boardState, searchInfo.nnueAcc); searchInfo.accumulatorHead = 0;`

---

## 4. Swapping the eval call sites

Add a single toggle near the top of `search.hpp`: `constexpr bool USE_NNUE_EVAL = false;`
(flip to `true` once a trained net is validated). In `quiescenceSearch` (`search.cpp:299,304`),
replace both `evaluate(boardState)` calls with a small wrapper that branches on the constant:

```cpp
inline int evalPosition(BoardState& boardState, SearchInfo& searchInfo) {
    return USE_NNUE_EVAL ? nnueEvaluate(boardState, searchInfo.nnueAcc) : evaluate(boardState);
}
```

This keeps the classical evaluator available for comparison throughout development and keeps
the change to `search.cpp` itself to two call-site edits.

---

## 5. Self-play data generation binary — new `src/nnueDataGen.cpp`

Follows `createKeys.cpp`'s minimal-binary pattern: own `main()`, own makefile target, links
directly against the engine's existing `.o` files (`physics.o evaluate.o search.o openBook.o
transpositionTable.o`) — **no sockets, no separate process**, unlike `matchManager`, since
we just need raw throughput generating millions of positions in-process.

For N games (CLI arg, default e.g. 20000): pick a start FEN (reuse `getRandomFen()` from
`openBook.cpp:24-46` against `data/fenList.txt` if present, else `STARTFEN`), then play the
game using the existing single-threaded `minimax` at a fixed shallow-moderate depth (e.g. 6)
with the **current classical `evaluate()`** — this bootstraps the net toward the existing
eval, the standard first step for a from-scratch NNUE. Buffer per-position records in memory
for the current game (score not yet knowable as win/draw/loss until the game ends); once the
game concludes, backfill `gameResult` on all buffered records and append them to the output
file. Skip: the first ~8 ply, any position where the side to move is in check, and positions
immediately preceding a capture (reduces tactical noise in the training signal — standard
texel-tuning-style filtering).

Record format — packed, no compiler padding, `#pragma pack(push,1)` around the struct:

```cpp
#pragma pack(push, 1)
struct NNUETrainingRecord {
    uint64_t whiteBitboards[6];  // pieces[WHITE][PAWN..KING], in that index order
    uint64_t blackBitboards[6];  // pieces[BLACK][PAWN..KING]
    int8_t   sideToMove;         // 0=WHITE, 1=BLACK
    int16_t  searchScoreCp;      // white-absolute centipawns, clamp to [-3000, 3000]
    int8_t   gameResult;         // 0=black win, 1=draw, 2=white win (white-absolute; backfilled post-game)
};
#pragma pack(pop)
// sizeof == 100 bytes exactly, no padding
```

Written via raw `fwrite` as a flat array — no JSON (would not scale to millions of positions).
New makefile target `nnueDataGen`, added to `all:`.

---

## 6. PyTorch training script — new `scripts/trainNNUE.py`

Loads the binary file via `numpy.fromfile(path, dtype=RECORD_DTYPE)` with a `numpy.dtype`
mirroring the packed struct exactly (`[('whiteBB','<u8',6),('blackBB','<u8',6),
('stm','i1'),('score','<i2'),('result','i1')]`). Per batch, decodes each record's 12
bitboards into occupied `(color, pieceType, square)` triples (bit-scan loop, same shape as
the C++ side) and builds two dense `(batch, 768)` float feature tensors (white-perspective,
black-perspective) using the **identical** `pieceType*128 + relativeColor*64 + relativeSquare`
formula from §1 — this formula must be kept in sync by hand in both languages; call this out
prominently in the script's docstring/comments as the single highest-risk source of a
train/inference mismatch bug.

Model (`torch.nn.Module`):
```
ft = Linear(768, 256)                       # shared weights, applied to both perspectives
forward: acc_stm  = clipped_relu(ft(feat_stm))
         acc_nstm = clipped_relu(ft(feat_nstm))
         x = cat([acc_stm, acc_nstm])        # 512
         x = clipped_relu(l1(x))             # l1: 512->32
         x = clipped_relu(l2(x))             # l2: 32->32
         out = l3(x)                         # l3: 32->1, linear
```
Train with MSE loss against the side-to-move-relative score derived from the record's
white-absolute `searchScoreCp` (negate when `stm==BLACK`), scaled to a small range (e.g.
divide by 100). Blending in `gameResult` via a WDL-sigmoid term is standard practice and
worth a one-line mention as a natural follow-up, but is **not** in v1 scope — plain score
regression is enough for a minimal first net. Adam optimizer, a handful of epochs, simple
train/val split.

Export weights to a flat binary file, **float32, little-endian, in exactly this order**
(row-major, PyTorch's native `.numpy()` layout — no transposition needed since the C++ side's
arrays are declared with matching shapes):
`ft.weight (768×256) → ft.bias (256) → l1.weight (32×512) → l1.bias (32) → l2.weight (32×32)
→ l2.bias (32) → l3.weight (1×32) → l3.bias (1)`.

---

## 7. Build system — `makefile`

- New rule: `objects/nnue.o: src/nnue.cpp` → `$(CXX) $(CXXFLAGS) -c -o objects/nnue.o src/nnue.cpp`
- Add `objects/nnue.o` to `engine`'s prerequisite list and link line (currently
  `objects/{engine,physics,evaluate,search,openBook,transpositionTable}.o`).
- Append `src/nnue.cpp` to `debugEngine`'s source list (it currently compiles sources
  directly rather than via `.o` files).
- New rule + target for the data-gen binary, mirroring `createKeys`:
  `objects/nnueDataGen.o: src/nnueDataGen.cpp`, then
  `nnueDataGen: objects/nnueDataGen.o objects/physics.o objects/evaluate.o objects/search.o objects/openBook.o objects/transpositionTable.o`.
  Add `nnueDataGen` to the `all:` target list.
- `src/engine.cpp`'s `main()`: add `loadNNUEWeights("data/nnue.bin");` next to the existing
  `initZobristTable(...)`/`loadOpeningBook(...)` calls.

---

## 8. Verification

1. **Accumulator-consistency self-check (the critical NNUE correctness gate)**: a small debug
   routine that starts from a position, plays ~20 random legal moves updating
   `searchInfo.nnueAcc` incrementally via the new `makeMove` pointer param + push/pop stack,
   and at every ply asserts `nnueEvaluate` computed from the incrementally-maintained
   accumulator matches one computed by calling `nnueRefreshAccumulator` fresh from the same
   `boardState`, within float epsilon (~1e-3). Any mismatch means an add/remove call is wrong
   or missing in one of the switch cases in §3 — this must pass before trusting anything else.
2. `make clean && make -j 16` — clean build, no new warnings, mirroring how the last build fix
   was verified.
3. Reuse the stdin `position ... / go ...` smoke test (as used for the multithreading fix) on
   a few positions — startpos, a midgame FEN, an endgame FEN — with `USE_NNUE_EVAL` both
   `true` and `false`, confirming no crash and a legal-looking `bestmove` either way.
4. Spot-check `nnueEvaluate` output on a couple of hand-picked positions (start position ≈ 0,
   a position up a queen ≈ +900-ish) as a basic sanity smoke test — not a rigorous eval-quality
   check.
5. Out of scope for this plan, but worth noting as the natural next step: real strength
   validation via `matchManager`'s existing self-play/SPRT loop (NNUE-engine vs
   classical-eval-engine), once training data volume/quality is scaled up beyond this minimal
   pipeline.
