#include "scanner.h"

// unsigned char *FILE_BUFFER = NULL;

// unsigned char *FileBufferInit(int size)
// {
//     if (FILE_BUFFER == NULL)
//     {
//         unsigned char *p = (unsigned char*)malloc(FILE_BUFFER_SIZE * sizeof(unsigned char));
//         if (p != NULL)
//         {
//             printf("Buffer memory allocated %d bytes.\n", FILE_BUFFER_SIZE);
//             return p;
//         }
//         return NULL;
//     }
//     printf("WARNING: Buffer already initialized.\n");
//     return FILE_BUFFER;
// }

// BOOL FileBufferClear(unsigned char *buffer)
// {
//     if (buffer == NULL)
//     {
//         printf("WARNING: Cannot free buffer memory.\n");
//         return FALSE;
//     }
//     free(buffer);
//     printf("Buffer memory cleared.\n");
//     return TRUE;
// }

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
    unsigned char buffer[FILE_BUFFER_SIZE];
    int timerStart, timerEnd, timerResult;

    WIN32_FIND_DATAW findData = {0};    
    Node *stack = NULL;

    stpush(&stack, path, outputPath);
    timerStart = time(NULL);

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
                    if (CopyDirFiles(pathIn, pathOut, buffer))
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
    timerEnd = time(NULL);
    timerResult = timerEnd - timerStart;
    printf("Done! %d sec\n", timerResult);
}

void ScanDriveFiles(LPCWSTR path, WCHAR *driveName)
{
    CreateDirectoryW(driveName, NULL);
    ScanDirFiles(path, driveName);
}

BOOL CopyDirFiles(WCHAR *pathIn, WCHAR *pathOut, unsigned char *buffer)
{
    FILE *in = _wfopen(pathIn, L"rb");
    FILE *out = _wfopen(pathOut, L"wb");
    int bytesRead;

    if (in == NULL || out == NULL)
    {
        perror("\nFile open error");
        printf("%ld", GetLastError());
        return FALSE;
    }    

    while ((bytesRead = fread(buffer, sizeof(unsigned char), FILE_BUFFER_SIZE, in)) > 0)
    {
        fwrite(buffer, sizeof(unsigned char), bytesRead, out);
    }

    fclose(in);
    fclose(out);
    return TRUE;
}