#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include "structs.h"

void BuildPath(WCHAR *destination, const WCHAR *directory, const WCHAR *filename);
void ScanDirFiles(LPCWSTR path, LPCWSTR outputPath);
void ScanDriveFiles(LPCWSTR path, WCHAR *driveName);
BOOL CopyDirFiles(WCHAR *pathIn, WCHAR *pathOut, unsigned char *buffer);
void stpush(Node **stack, LPCWSTR path, LPCWSTR outputPath);
void stpop(Node **stack, WCHAR *path, WCHAR *outputPath);
// unsigned char *FileBufferInit(int size);
// BOOL FileBufferClear(unsigned char *buffer);