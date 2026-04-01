// This file is used to define file input and output operations.
// Created by seth on 01/04/2026.
//

#include "file_io.h"
#include <stdlib.h>
#include <dirent.h> // For directory operations
#include <stdio.h>

directoryList directory_list_create()
{
    dirList list;
    list.length=0;
    list.fileCounter=0;
    list.files=NULL;
    return list;
}

int directory_read(dirList* list, char* path)
{
    DIR* dirstream;
    struct dirent* file;

    dirstream=opendir(".");
    if (dirstream == NULL)
    {
        printf("ERROR: FILE-IO - directory_read: Failed to open directory\n");
        return 1;
    }

    char* files[MAX_FILE_COUNT];
    int counter = 0;
    while ( (file = readdir(dirstream)) )
    {
        counter+=1;
        if (counter>MAX_FILE_COUNT)
        {

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

        // TODO: malloc and write linked list elements
    }
    // TODO: return values
}