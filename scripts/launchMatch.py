import subprocess;
import time;

def main() -> None:

    '''
    makeProcess = subprocess.run(["make", "-j", "16"], cwd="../", capture_output=True, text=True);
    print(makeProcess.stdout);
    print(makeProcess.stderr);
    '''

    matchManager = subprocess.Popen(
        ["../build/matchManager", "-f", "7k/5K2/8/8/8/8/8/6Q1 w - - 0 1",
            "-wt", "30000", "-bt", "30000", "--benchmark", "2"
        ],
        cwd="../build",
        text = True
    );

    time.sleep(2);

    whitePlayer = subprocess.Popen(
        ["../build/engineClient", "-p", "3001"],
        cwd="../build",
        text = True
    );
    blackPlayer = subprocess.Popen(
        ["../build/engineClient", "-p", "3002"],
        cwd="../build",
        text = True
    );

    while (True):
        for current_line in matchManager.stdout:
            print(f"Match Manager stdout: {current_line}");
        for current_line in whitePlayer.stdout:
            print(f"White Player stdout: {current_line}");
        for current_line in blackPlayer.stdout:
            print(f"Black Player stdout: {current_line}");

        if (matchManager.poll() is not None):
            break;
        if (whitePlayer.poll() is not None):
            break;
        if (blackPlayer.poll() is not None):
            break;




    


if (__name__ == "__main__"):
    main();
