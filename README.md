# USB Sniffer

USB Sniffer is an application for monitoring connected USB devices and automatically copying files from USB flash drives. USB Sniffer is created exclusively for educational and research purposes and is not intended for harm or unauthorized use. The user bears full responsibility for the lawful application of the program in accordance with local laws and regulations.

> **Important:** This application is developed solely for educational purposes. The author is not responsible for any unlawful actions that may be committed using this software.

## Installation

To build and run USB Sniffer, you will need to install `gcc` or `MinGW32-make`. 

## Steps to Build:

1. **Install MinGW**: If you do not have MinGW installed, you can download it from the [official website](https://osdn.net/projects/mingw/releases/) and follow the installation instructions.

2. **Add MinGW to PATH**: Ensure that the path to the folder containing `gcc` and `MinGW32-make` is added to the `PATH` environment variable so that you can run them from the command line.

3. **Open Command Prompt**: Press `Win + R`, type `cmd`, and press `Enter`.

4. **Navigate to the Source Code Directory**: Use the `cd` command to go to the folder containing the USB Sniffer project:
   ```bash
   cd yourpath/USBsniffer
   gcc -I include -o usb src/usb.c src/drive.c src/scanner.c
   ```
   You can also use the Makefile to build the project:
   ```bash
   cd yourpath/USBsniffer
   mingw32-make all
   ```

5. **Run the application**:
    ```bash
    usb ?[-flag]
    ```
    All files will be copied in the folder with the usb flash name.

## Flags
`-show`: Keeps the console visible. This is the default value if no flag is specified.

`-hide`: Hides the console during the program's execution. Use this flag if you want the program to run in the background.

