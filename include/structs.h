#pragma once
#include <Windows.h>

#define VOLUME_NAME_SIZE MAX_PATH + 1

typedef struct Node
{
    WCHAR path[MAX_PATH];
    WCHAR outputPath[MAX_PATH];
    struct Node *next;
} Node;

typedef struct
{
    WCHAR lpVolumeNameBuffer[MAX_PATH];
    DWORD lpVolumeSerialNumber;
    DWORD lpMaximumComponentLength;
    DWORD lpFileSystemFlags;
    WCHAR lpFileSystemNameBuffer[VOLUME_NAME_SIZE];

} VOLUME_INFORMATION, *VOLUME_INFORMATION_P;

typedef struct 
{
    const WCHAR *path;
    WCHAR name[MAX_PATH];
    DWORD serialNumber;
    WCHAR fileSystemName[VOLUME_NAME_SIZE];
} DRIVE;
extern DRIVE GLOBAL_DRIVE;

typedef struct
{
    int counter;
    char cache[16];
} DRIVES;