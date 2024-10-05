#include "scanner.h"

#define FILE_BUFFER_SIZE 4096

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

void ScanDirFiles(LPCWSTR path, LPCWSTR outputPath)
{
    WIN32_FIND_DATAW findData = {0};    
    Node *stack = NULL;

    stpush(&stack, path, outputPath);

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
            if (wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0)
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
                    if (CopyDirFiles(pathIn, pathOut))
                    {
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
    printf("Done!\n");
}

void ScanDriveFiles(LPCWSTR path, WCHAR *driveName)
{
    CreateDirectoryW(driveName, NULL);
    ScanDirFiles(path, driveName);
}

BOOL CopyDirFiles(WCHAR *pathIn, WCHAR *pathOut)
{
    FILE *in = _wfopen(pathIn, L"rb");
    FILE *out = _wfopen(pathOut, L"wb");
    unsigned char buffer[FILE_BUFFER_SIZE];
    int bytesRead;

    if (in == NULL || out == NULL)
    {
        perror("\nFile open error");
        printf("%ld", GetLastError());
        return 0;
    }    

    while ((bytesRead = fread(&buffer, sizeof(unsigned char), sizeof(buffer), in)) > 0)
    {
        fwrite(&buffer, sizeof(unsigned char), bytesRead, out);
    }

    fclose(in);
    fclose(out);
    return 1;
}