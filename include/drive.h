#pragma once
#include "scanner.h"

#define MAX_DRIVES 24

typedef struct _DRIVE_INFO 
{
    WCHAR name[MAX_PATH];
    WCHAR fileSysName[MAX_PATH + 1];
    DWORD serial;
    char letter;
    BOOL isConnected;

} DriveInfo, *pDriveInfo;

typedef struct _DRIVES
{
    DriveInfo drives[MAX_DRIVES];
    size_t len;
} DrivesArray, *pDrivesArray;

typedef struct _VOLUME_INFO
{
    WCHAR name[MAX_PATH];
    DWORD serialNum;
    DWORD maxComponentLen;
    DWORD fileSysFlags;
    WCHAR fileSysName[MAX_PATH + 1];
    char letter;

} VolumeInfo, *pVolumeInfo;

BOOL FindDrive(pDrivesArray drives, DWORD serialNum);
void PrintVolumeInformation(pVolumeInfo pVolumeInformation);
void ScanDrives(int intervalms);
void BuildDriveRootPath(const char *drive, WCHAR *driveRootPath);
BOOL UpdateDrive(pDrivesArray drives, DWORD serialNum, pVolumeInfo volumeInfo);
BOOL AppendDrive(pDrivesArray drives, pVolumeInfo volumeInfo);