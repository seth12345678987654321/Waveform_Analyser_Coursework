// This file is used to define file input and output operations.
// Created by seth on 01/04/2026.
//

#include "file_io.h"
#include <stdlib.h>
#include <dirent.h> // For directory operations
#include <stdio.h>
#include <string.h>

dirList directory_list_create()
{
    dirList list;
    list.length=0;
    list.fileCounter=0;
    list.indexFile=NULL;
    return list;
}

int directory_read(dirList* list, char* path)
{
    struct dirent* file;
    DIR* dirstream = opendir(".");

    if (dirstream == NULL)
    {
        printf("ERROR: FILE-IO - directory_read: Failed to open directory\n");
        return 1;
    }

    char* files[MAX_FILE_COUNT];
    int counter = 0;
    dirFile* prevFile = NULL;

    while ( (file = readdir(dirstream)) )
    {


        if (counter>=MAX_FILE_COUNT)
        {
            printf("WARN: FILE-IO - directory_read: Exceeded file limit!\n", file->d_name);
            break; // Do not process any more files
        }
        if (file->d_type!=DT_REG)
        { // Only include regular files and exclude other directories
            printf("WARN: FILE-IO - directory_read: File '%s' is not a regular file\n", file->d_name);
            continue;
        }

        if (check_file(file->d_name)!=0)
        { // Only include files that can be opened and read
            printf("WARN: FILE-IO - directory_read: File '%s' could not be accessed\n", file->d_name);
            continue;
        }
        // Passed all checks!

        dirFile* newFile = (dirFile*)malloc(sizeof(dirFile));
        char* filename = strdup(file->d_name); // Duplicate the string in new memory location
        newFile->fileName=filename;
        if (counter==0)
        {   // Index pointer should point to the first file
            list->indexFile = newFile;
        }else
        {   // Previous file's pointer should point to the new file
            prevFile->nextFile=newFile;
        }
        prevFile=newFile;
        counter+=1;
    }
    return 0;
}

int check_file(char* file)
{
    return 1;
}