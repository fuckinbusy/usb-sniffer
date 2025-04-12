#include "scanner.h"

void stpush(Node **stack, LPCWSTR path, LPCWSTR outputPath)
{
    Node *newNode = (Node*)malloc(sizeof(Node));
    wcsncpy(newNode->path, path, MAX_PATH);
    wcsncpy(newNode->outputPath, outputPath, MAX_PATH);
    newNode->next = *stack;
    *stack = newNode;
}   

void stpop(Node **stack, WCHAR *path, WCHAR *outputPath)
{
    if (*stack != NULL)
    {
        Node *node = *stack;
        wcsncpy(path, node->path, MAX_PATH);
        wcsncpy(outputPath, node->outputPath, MAX_PATH);
        *stack = node->next;
        free(node);
    }
}

void BuildPath(WCHAR *destination, const WCHAR *directory, const WCHAR *filename) 
{
    int len = wcslen(directory);
    if (directory[len - 1] == L'\\') {
        swprintf(destination, MAX_PATH, L"%s%s", directory, filename);
    } else {
        swprintf(destination, MAX_PATH, L"%s\\%s", directory, filename);
    }
}

size_t ScanDirFiles(LPCWSTR path, LPCWSTR outputPath)
{
    unsigned char buffer[FILE_BUFFER_SIZE];
    clock_t timerStart, timerEnd;
    double timerResult;
    
    WIN32_FIND_DATAW findData = {0};    
    Node *stack = NULL;
    
    stpush(&stack, path, outputPath);
    timerStart = clock();

    size_t writtenBytesTotal = 0;

    while (stack != NULL)
    {
        WCHAR currentPath[MAX_PATH], currentOutputPath[MAX_PATH], searchPath[MAX_PATH];
        stpop(&stack, currentPath, currentOutputPath);
        BuildPath(searchPath, currentPath, L"*");

        HANDLE hFind = FindFirstFileW(searchPath, &findData);
        if (hFind == INVALID_HANDLE_VALUE)
        {
            perror("ERROR");
            continue;
        }

        do
        {
            if (_wcsicmp(findData.cFileName, L".") != 0 && _wcsicmp(findData.cFileName, L"..") != 0)
            {
                WCHAR pathIn[MAX_PATH], pathOut[MAX_PATH];
                BuildPath(pathIn, currentPath, findData.cFileName);
                BuildPath(pathOut, currentOutputPath, findData.cFileName);

                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    CreateDirectoryW(pathOut, NULL);
                    stpush(&stack, pathIn, pathOut);
                }
                else
                {
                    size_t writtenBytes = 0;
                    if ((writtenBytes = CopyDirFiles(pathIn, pathOut, buffer)) > 0)
                    {
                        writtenBytesTotal += writtenBytes;
                        printf("[%S] copy success\n", pathIn);
                    }
                    else
                    {
                        printf("[%S] copy failed\n", pathIn);
                    }
                }  
            }
        } while (FindNextFileW(hFind, &findData));

        FindClose(hFind);
    }
    
    timerEnd = clock();
    timerResult = ((double)(timerEnd - timerStart)) * 1000.0 / CLOCKS_PER_SEC;
    printf("Done! %.2fms\n", timerResult);
    return writtenBytesTotal;
}

size_t ScanDriveFiles(LPCWSTR path, WCHAR *driveName)
{
    CreateDirectoryW(driveName, NULL);
    return ScanDirFiles(path, driveName);
}

size_t CopyDirFiles(WCHAR *pathIn, WCHAR *pathOut, unsigned char *buffer)
{
    FILE *in = _wfopen(pathIn, L"rb");
    FILE *out = _wfopen(pathOut, L"wb");
    size_t bytesRead;
    size_t bytesWritten;

    if (in == NULL || out == NULL)
    {
        perror("\nFile open error");
        printf("%ld", GetLastError());
        return FALSE;
    }    

    while ((bytesRead = fread(buffer, sizeof(unsigned char), FILE_BUFFER_SIZE, in)) > 0)
    {
        bytesWritten = fwrite(buffer, sizeof(unsigned char), bytesRead, out);
    }

    fclose(in);
    fclose(out);
    return bytesWritten;
}