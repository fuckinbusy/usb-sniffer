#include "drive.h"

#define DRIVE_ROOT_PATH_SIZE 4

// Appends drive to drives list
BOOL AppendDrive(DRIVES *drives, const char driveName)
{
    if (drives->counter == 16)
    {
        return FALSE;
    }
    drives->cache[drives->counter] = driveName;
    drives->counter++;
    return TRUE;
}

// Returns TRUE if drive was found and FALSE if not
BOOL FindDrive(DRIVES *drives, const char driveName)
{
    for (size_t i = 0; i < drives->counter; i++)
    {
        if (drives->cache[i] == driveName)
        {
            return TRUE;
        }
    }
    return FALSE;
}

void BuildDriveRootPath(char *drive, WCHAR *driveRootPath)
{
    driveRootPath[0] = (WCHAR)(*drive);
    driveRootPath[1] = L':';
    driveRootPath[2] = L'\\';
    driveRootPath[3] = L'\0';
}

void PrintVolumeInformation(VOLUME_INFORMATION_P pVolumeInformation)
{
    printf("Name: %S\n", pVolumeInformation->lpVolumeNameBuffer);
    printf("Serial: %d\n", pVolumeInformation->lpVolumeSerialNumber);
    printf("Max filename len: %d\n", pVolumeInformation->lpMaximumComponentLength);
    printf("File system: %S\n", pVolumeInformation->lpFileSystemNameBuffer);
    printf("Flags: %0*lx\n", sizeof(pVolumeInformation->lpFileSystemFlags) * 2, pVolumeInformation->lpFileSystemFlags);
}

// Starts drives scanning
void ScanDrives(int intervalms)
{
    DWORD prevDrives = 0;
    DRIVES drivesList = {0};
    VOLUME_INFORMATION volumeInfo = {0};
    int counter = 0;

    while (counter != 30)
    {
        DWORD drives = GetLogicalDrives();

        if (drives != prevDrives)
        {
            for (int i = 0; i < 25; i++)
            {
                char drive = 'A' + i;
                WCHAR driveRootPath[DRIVE_ROOT_PATH_SIZE];
                BuildDriveRootPath(&drive, driveRootPath);

                if (drives & (1 << i) && GetDriveTypeW(driveRootPath) == DRIVE_REMOVABLE)
                {
                    if (!AppendDrive(&drivesList, drive))
                    {
                        printf("Maximum drives count reached.\n");
                        return;
                    }
                    printf("FlashUSB %c: connected\n", drive);
                    // Getting volume information
                    GetVolumeInformationW(
                        driveRootPath,
                        volumeInfo.lpVolumeNameBuffer,
                        VOLUME_NAME_SIZE,
                        &volumeInfo.lpVolumeSerialNumber,
                        &volumeInfo.lpMaximumComponentLength,
                        &volumeInfo.lpFileSystemFlags,
                        volumeInfo.lpFileSystemNameBuffer,
                        VOLUME_NAME_SIZE);
                    PrintVolumeInformation(&volumeInfo);
                    ScanDriveFiles((LPCWSTR)(&driveRootPath), volumeInfo.lpVolumeNameBuffer);
                }
                else
                {
                    if (FindDrive(&drivesList, drive))
                    {
                        printf("FlashUSB %c: disconnected\n", drive);
                    }
                }
            }
        }
        prevDrives = drives;
        counter++;
        Sleep(intervalms);
    }
}