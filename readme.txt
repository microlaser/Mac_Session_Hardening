Session Guard
Session Guard is a macOS security utility written in C designed for session monitoring and system hardening.

Installation & Setup
On modern macOS (Apple Silicon), binaries must be explicitly signed and wrapped to run as silent background processes. Follow these steps to ensure the utility launches correctly at boot without opening a Terminal window.

1. Build and Install
Compile the source and move the binary to a standard system path.

Bash

# Compile the utility
gcc -o session_guard main.c

# Move to a system path
sudo mkdir -p /usr/local/bin
sudo mv session_guard /usr/local/bin/
sudo chmod +x /usr/local/bin/session_guard
2. Ad-Hoc Code Signing
MacOS integrity protection (AMFI) will terminate unsigned binaries immediately with a SIGKILL -9. You must apply an ad-hoc signature to allow it to run locally:

Bash

sudo codesign --force --deep -s - /usr/local/bin/session_guard
3. Create the Silent Background Runner
Adding a raw binary directly to macOS Login Items causes a Terminal window to open at startup. To run the process silently in the background, use an AppleScript wrapper:

Open Script Editor (found in /Applications/Utilities/).

Paste the following code:

AppleScript

do shell script "/usr/local/bin/session_guard > /dev/null 2>&1 &"
Go to File > Export.

Set the options as follows:

File Format: Application

Name: SessionGuardRunner

Save it to your /Applications folder.

4. Enable at Startup
Open System Settings and navigate to General > Login Items.

Under the Open at Login section, click the (+) button.

Select SessionGuardRunner.app from your Applications folder.

Permissions (Full Disk Access)
If your version of Session Guard monitors sensitive system files or network events, you must grant it elevated permissions:

Go to System Settings > Privacy & Security > Full Disk Access.

Open Finder and press Cmd + Shift + G, then type /usr/local/bin.

Drag and Drop the session_guard binary directly into the list in System Settings.

Ensure the toggle next to it is switched ON.

Verification
To verify that Session Guard is active in the background after a reboot or manual launch, run:

Bash

ps aux | grep session_guard
To view any runtime errors (if configured in your C code), you can check the system logs:

Bash

log show --predicate 'process == "session_guard"' --last 1h
