import subprocess;
import time;

def main() -> None:

    makeProcess = subprocess.run(["make", "-j", "16"], cwd="../", capture_output=True, text=True);
    print(makeProcess.stdout);
    print(makeProcess.stderr);

    if (makeProcess.returncode != 0):
        print("BUILDING THE PROJECT FAILED. ABORTING RUN SCRIPT");

    matchManager = subprocess.Popen(
        ["../build/matchManager", "-f", "7k/5K2/8/8/8/8/8/6Q1 w - - 0 1",
            "-wt", "30000", "-bt", "30000", "--benchmark", "1"
        ],
        cwd="../build",
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        text = True
    );

    time.sleep(2); # hack

    whitePlayer = subprocess.Popen(
        ["../build/engineClient", "-p", "3001"],
        cwd="../build",
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        text = True
    );
    blackPlayer = subprocess.Popen(
        ["../build/engineClient", "-p", "3002"],
        cwd="../build",
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        text = True
    );

    while (True):
    #    for current_line in matchManager.stdout:
    #        print(f"Match Manager stdout: {current_line}");
    #    for current_line in whitePlayer.stdout:
    #        print(f"White Player stdout: {current_line}");
    #    for current_line in blackPlayer.stdout:
    #        print(f"Black Player stdout: {current_line}");
        in_str = input("Python script command:");
        if (in_str == "quit"):
            break;
        elif (in_str == "poll"):
            if (matchManager.poll() is not None):
                break;
            if (whitePlayer.poll() is not None):
                break;
            if (blackPlayer.poll() is not None):
                break;
        else:
            print("Command not recognized");



    matchManager.kill();
    whitePlayer.kill();
    blackPlayer.kill();

    print("CLOSED ALL SUBPROCESSES");
    return;
    
    #matchManager.send_signal(SIGKILL);
    #whitePlayer.send_signal(SIGKILL);
    #black_player.send_signal(SIGKILL);

    


if (__name__ == "__main__"):
    main();
