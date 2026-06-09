import chess
import chess.pgn
import json

WHITEKINGCASTLE  = 0
WHITEQUEENCASTLE = 1
BLACKKINGCASTLE  = 2
BLACKQUEENCASTLE = 3


def parsePGN(filePath: str, keys : object):
    pgn = open(filePath)

    currentGame = chess.pgn.read_game(pgn)
    boardToMoveDict = dict()

    while(currentGame != None):
        board = currentGame.board()
        n = 0
        for move in currentGame.mainline_moves():
            if(n > 12):
                break

            # Moves are 16 bit unions first 6 bits are source
            # next 6 are dest and final 4 are flags
            writeMove = move.from_square
            writeMove |= (move.to_square << 6)
            writeMove |= (getFlags(board, move) << 12)

            zobristHash = boardToZobristHash(board, keys) 

            if(zobristHash not in boardToMoveDict):
                boardToMoveDict[zobristHash] = dict()
            if(writeMove in boardToMoveDict[zobristHash]):
                boardToMoveDict[zobristHash][writeMove] += 1
            else:
                boardToMoveDict[zobristHash][writeMove] = 1

            board.push(move)
            n += 1
        currentGame = chess.pgn.read_game(pgn)

    pgn.close()
    exportDict = dict()
    for zHash, moves in boardToMoveDict.items():
        # Converts {move1: weight, move2: weight} -> [{"move": move1, "weight": weight}, ...]
        exportDict[str(zHash)] = [{"move": m, "weight": w} for m, w in moves.items()]

    with open("../build/data/openBook.json", "w") as file:
        json.dump(exportDict, file, indent=2)


def getFlags(board: chess.Board, move: chess.Move) -> int:
    if move.promotion is not None:
        promo_offset = move.promotion - 2
        
        if board.is_capture(move):
            return 0b1100 + promo_offset # 12 + offset
        else:
            return 0b1000 + promo_offset # 8 + offset

    if board.is_en_passant(move):
        return 0b0101

    if board.is_castling(move):
        if board.is_kingside_castling(move):
            return 0b0010
        else:
            return 0b0011

    if board.is_capture(move):
        return 0b0100

    piece = board.piece_at(move.from_square)
    if piece is not None and piece.piece_type == chess.PAWN:
        # A double push is exactly 16 squares (2 ranks * 8 files)
        if abs(move.from_square - move.to_square) == 16:
            return 0b0001

    return 0b0000


def loadKeys():
    jsonFile = open("../build/data/zobristKeys.json")
    keys = json.load(jsonFile)
    jsonFile.close()

    return keys

def boardToZobristHash(board : chess.board, keys : object):
    zobristHash = 0
    for square in chess.SQUARES:
        piece = board.piece_at(square)
        if(piece is None):
            continue

        colorOffset = 0
        if(piece.color == chess.BLACK):
            colorOffset = 1
        # These pieceOffset are determined in physics.hpp
        pieceOffset = piece.piece_type
        if(piece.piece_type == chess.KNIGHT):
            pieceOffset = 2
        elif(piece.piece_type == chess.BISHOP):
            pieceOffset = 1
        else:
            pieceOffset -= 1

        zobristHash ^= keys["zobristTable"][6 * colorOffset + pieceOffset][square]

    if(board.turn == chess.BLACK):
        zobristHash ^= keys["zobristSideToMove"]

    # White can castle king side
    if(board.castling_rights & chess.BB_H1):
        zobristHash ^= keys["zobristCastle"][WHITEKINGCASTLE]
    # White can castle queen side
    if(board.castling_rights & chess.BB_A1):
        zobristHash ^= keys["zobristCastle"][WHITEQUEENCASTLE]
    # Black can castle king side
    if(board.castling_rights & chess.BB_H8):
        zobristHash ^= keys["zobristCastle"][BLACKKINGCASTLE]
    # Black can castle queen side
    if(board.castling_rights & chess.BB_A8):
        zobristHash ^= keys["zobristCastle"][BLACKQUEENCASTLE]

    if(board.ep_square != None):
        zobristHash ^= keys["zobristEnPassant"][chess.square_file(board.ep_square)]

    return zobristHash

if __name__ == "__main__":
    keys = loadKeys()
    parsePGN("games/Nunn_Openings.pgn", keys)    

