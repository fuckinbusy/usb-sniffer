#include "scanner.h"

void stpush(Node **stack, LPCWSTR path, LPCWSTR outputPath)
{
    Node *newNode = (Node*)malloc(sizeof(Node));
    wcscpy(newNode->path, path);
    wcscpy(newNode->outputPath, outputPath);
    newNode->next = *stack;
    *stack = newNode;
}

void stpop(Node **stack, WCHAR *path, WCHAR *outputPath)
{
    if (*stack != NULL)
    {
        Node *node = *stack;
        wcscpy(path, node->path);
        wcscpy(outputPath, node->outputPath);
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
            if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0)
            {
                continue;
            }

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
    unsigned char buffer[4096];
    int bytesRead;

    if (in == NULL)
    {
        perror("\nfile in ERROR");
        printf("%ld", GetLastError());
        return 0;
    }    

    if (out == NULL)
    {
        perror("\nfile out ERROR");
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