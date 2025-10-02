#include "../include/drive.h"

BOOL UpdateDrive(pDrivesArray drives, DWORD serialNum, pVolumeInfo volumeInfo)
{
    if (drives == NULL || serialNum == 0 || volumeInfo == NULL) return FALSE;
    
    pDriveInfo drive = NULL;
    for (size_t i = 0; i < drives->len; ++i)
    {
        if ((&drives->drives[i])->serial == serialNum) {
            drive = &drives->drives[i];
        }
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

    pDriveInfo drive = &drives->drives[drives->len++];
    drive->serial = volumeInfo->serialNum;
    drive->isConnected = TRUE;
    drive->letter = volumeInfo->letter;
    wcscpy_s(drive->name, MAX_PATH, volumeInfo->name);
    wcscpy_s(drive->fileSysName, MAX_PATH + 1, volumeInfo->fileSysName);
    
    return TRUE;
}

// Returns TRUE if drive was found and FALSE if not
BOOL FindDrive(pDrivesArray drives, DWORD serialNum)
{
    if (drives == NULL || serialNum == 0) return FALSE;

    for (size_t i = 0; i < drives->len; i++)
    {
        pDriveInfo drive = &(drives->drives[i]);
        if (drive != NULL && drive->serial == serialNum)
        {
            return TRUE;
        }
    }
    
    return FALSE;
}

BOOL isDriverConnected(pDrivesArray drives, DWORD serialNum)
{
    for (size_t i = 0; i < MAX_DRIVES; ++i)
    {
        pDriveInfo drive = &(drives->drives[i]);
        if (drive->serial == serialNum && drive->isConnected)
        {
            return TRUE;
        }
    }
    return FALSE;
}

void BuildDriveRootPath(const char *drive, WCHAR *driveRootPath)
{
    driveRootPath[0] = (WCHAR)(*drive);
    driveRootPath[1] = L':';
    driveRootPath[2] = L'\\';
    driveRootPath[3] = L'\0';
}

void PrintVolumeInformation(pVolumeInfo pVolumeInformation)
{
    printf("Name: %S\n", pVolumeInformation->name);
    printf("Serial: %lu\n", pVolumeInformation->serialNum);
    printf("Max filename len: %lu\n", pVolumeInformation->maxComponentLen);
    printf("File system: %S\n", pVolumeInformation->fileSysName);
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
            for (size_t i = 0; i != MAX_DRIVES; i++)
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
                        VOLUME_NAME_SIZE))
                    {
                        fprintf(stderr, "ERROR: Cannot get volume info.\n"); 
                        continue; 
                    }

                    if (FindDrive(&drivesList, volumeInfo.serialNum))
                    {
                        printf_s("Drive already 'sniffed'.\n");
                        continue;
                    }

                    volumeInfo.letter = drive;

                    PrintVolumeInformation(&volumeInfo);
                    size_t writtenBytes = ScanDriveFiles((LPCWSTR)(&driveRootPath), volumeInfo.name);
                    writtenBytesTotal += writtenBytes;
                    
                    AppendDrive(&drivesList, &volumeInfo);
                }
                else
                {
                    DWORD drSrl = GetDriveSerial(driveRootPath);
                    for (size_t j = 0; j < drivesList.len; ++j)
                    {
                        pDriveInfo dr = &(drivesList.drives[j]);
                        if (dr->letter == drive && drSrl == 0)
                        {
                            printf("Drive %c: disconnected\n\n", drive);
                        }
                    }
                }
            }
        }
        runtime--;
        Sleep(intervalms);
    }
}