#pragma once
#include <Windows.h>

#define SLEEP_TIMEOUT_MS 1000
#define VOLUME_NAME_SIZE MAX_PATH + 1
#define FILE_BUFFER_SIZE 4096
#define DRIVE_ROOT_PATH_SIZE 4

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
    int counter;
    char cache[16];
} DRIVES;