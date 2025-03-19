#include "drive.h"

void FreeDrivesArray(pDrivesArray drives)
{
    if (drives == NULL) return;
    for (size_t i = 0; i < drives->len; ++i)
    {
        if (drives->drives[i] != NULL) { free(drives->drives[i]); }
    }
}

BOOL UpdateDrive(pDrivesArray drives, DWORD serialNum, pVolumeInfo volumeInfo)
{
    if (drives == NULL || serialNum == 0 || volumeInfo == NULL) return FALSE;
    
    pDriveInfo drive = NULL;
    for (size_t i = 0; i < drives->len; ++i)
    {
        if (drives->drives[i]->serial == serialNum)
            drive = drives->drives[i];
            break;
    }

    if (drive == NULL) return FALSE;

    memset(drive, 0, sizeof(DriveInfo));
    wcscpy_s(drive->name, MAX_PATH, volumeInfo->name);
    wcscpy_s(drive->fileSysName, MAX_PATH + 1, volumeInfo->fileSysName);
    drive->serial = volumeInfo->serialNum;
    drive->isConnected = TRUE;
    drive->letter = volumeInfo->letter;
    return TRUE;
}

// Appends drive to drives list
BOOL AppendDrive(pDrivesArray drives, pVolumeInfo volumeInfo)
{
    if (drives == NULL || volumeInfo == NULL) return FALSE;

    if (FindDrive(drives, volumeInfo->serialNum))
    {
        if (UpdateDrive(drives, volumeInfo->serialNum, volumeInfo))
        {
            return TRUE;
        }
        return FALSE;
    }

    if (drives->len >= MAX_DRIVES) {
        fprintf_s(stderr, "Drives array is full.\n");
        return FALSE;
    }

    pDriveInfo drive = (pDriveInfo)malloc(sizeof(DriveInfo));
    if (drive == NULL)
    {
        fprintf_s(stderr, "Memory allocation failed.\n");
        return FALSE;
    }

    wcscpy_s(drive->name, MAX_PATH, volumeInfo->name);
    wcscpy_s(drive->fileSysName, MAX_PATH + 1, volumeInfo->fileSysName);
    drive->serial = volumeInfo->serialNum;
    drive->isConnected = TRUE;
    drive->letter = volumeInfo->letter;

    drives->drives[drives->len] = drive;
    drives->len++;
    return TRUE;
}

// Returns TRUE if drive was found and FALSE if not
BOOL FindDrive(pDrivesArray drives, DWORD serialNum)
{
    if (drives == NULL || serialNum == 0) return FALSE;

    for (char i = 0; i < drives->len; i++)
    {
        pDriveInfo drive = drives->drives[i];
        if (drive != NULL && drive->serial == serialNum)
        {
            printf_s("%lu | %lu\n", drive->serial, serialNum);
            return TRUE;
        }
    }
    
    return FALSE;
}

BOOL isDriverConnected(pDrivesArray drives, DWORD serialNum)
{
    for (char i = 0; i < MAX_DRIVES; ++i)
    {
        pDriveInfo drive = drives->drives[i];
        if (drive->serial == serialNum && drive->isConnected)
        {
            return TRUE;
        }
    }
    return FALSE;
}

void BuildDriveRootPath(const char *drive, WCHAR *driveRootPath)
{
    driveRootPath[0] = (WCHAR)*drive;
    driveRootPath[1] = L':';
    driveRootPath[2] = L'\\';
    driveRootPath[3] = L'\0';
}

void PrintVolumeInformation(pVolumeInfo pVolumeInformation)
{
    printf("Name: %S\n", pVolumeInformation->name);
    printf("Serial: %d\n", pVolumeInformation->serialNum);
    printf("Max filename len: %d\n", pVolumeInformation->maxComponentLen);
    printf("File system: %S\n", pVolumeInformation->fileSysName);
    printf("Flags: %0*lx\n", sizeof(pVolumeInformation->fileSysFlags) * 2, pVolumeInformation->fileSysFlags);
}

DWORD GetDriveSerial(const WCHAR *driveRootPath)
{
    DWORD serial = 0;
    GetVolumeInformationW(
        driveRootPath,
        NULL,
        VOLUME_NAME_SIZE,
        &serial,
        NULL,
        NULL,
        NULL,
        VOLUME_NAME_SIZE
    );
    return serial;
}

// Starts drives scanning
void ScanDrives(int intervalms)
{
    DWORD drives = 0;
    size_t writtenBytesTotal = 0;
    int runtime = intervalms;
    
    DrivesArray drivesList = {0};

    while (runtime)
    {
        DWORD tmp = GetLogicalDrives();
        if (drives != tmp)
        {
            drives = tmp;
            for (char i = 0; i != MAX_DRIVES; i++)
            {
                char drive = 'A' + i;
                WCHAR driveRootPath[DRIVE_ROOT_PATH_SIZE];
                BuildDriveRootPath(&drive, driveRootPath);

                if (drives & (1 << i) && GetDriveTypeW(driveRootPath) == DRIVE_REMOVABLE)
                {
                    printf("\nDrive %c: connected\n", drive);

                    VolumeInfo volumeInfo = {0};
                    if (!GetVolumeInformationW(
                        driveRootPath,
                        volumeInfo.name,
                        MAX_PATH,
                        &volumeInfo.serialNum,
                        &volumeInfo.maxComponentLen,
                        &volumeInfo.fileSysFlags,
                        volumeInfo.fileSysName,
                        VOLUME_NAME_SIZE)) {
                        fprintf(stderr, "Cannot get volume info.\n"); 
                        continue; 
                    }
                    volumeInfo.letter = drive;

                    PrintVolumeInformation(&volumeInfo);
                    size_t writtenBytes = ScanDriveFiles((LPCWSTR)(&driveRootPath), volumeInfo.name);
                    writtenBytesTotal += writtenBytes;
                    
                    if (!AppendDrive(&drivesList, &volumeInfo))
                    {
                        continue;
                    }
                }
                else
                {
                    DWORD drSrl = GetDriveSerial(driveRootPath);
                    for (char j = 0; j < drivesList.len; ++j)
                    {
                        pDriveInfo dr = drivesList.drives[j];
                        if (dr->letter == drive && drSrl == 0)
                        {
                            printf("Drive %c: disconnected\n\n", drive);
                        }
                    }
                }
            }
            // if (writtenBytesTotal > 0)
            // {
            //     printf("Bytes written total: %d | 0x%04X\n", writtenBytesTotal, writtenBytesTotal);
            //     printf("First drive address: %p\n", drivesList.drives[0]);
            //     printf("Name in drives list: %S | %S | %lu\n", 
            //         drivesList.drives[0]->fileSysName, 
            //         drivesList.drives[0]->name,
            //         drivesList.drives[0]->serial
            //     );
            //     writtenBytesTotal = 0;
            // }
        }
        runtime--;
        Sleep(intervalms);
    }
    FreeDrivesArray(&drivesList);
}