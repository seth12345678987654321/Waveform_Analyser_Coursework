// This file is used to define file input and output operations.
// Created by seth on 01/04/2026.
//

#include "file_io.h"
#include <stdlib.h>
#include <dirent.h> // For directory operations
#include <stdio.h>
#include <string.h>

#include "console_io.h"

dirList directory_list_create()
{
    dirList list;
    list.length=0;
    list.fileCounter=0;
    list.indexFile=NULL;
    list.path=NULL;
    list.currentFile=NULL;
    return list;
}

int directory_read(dirList* list, char* path)
{
    struct dirent* file;
    DIR* dirstream = opendir(path);

    if (console_write_head(DEBUG,"DIR-READ")){
        printf("Directory path '%s'\n", path);
    }

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
        if (console_write_head(DEBUG,"DIR-READ")){
            printf("File '%s'\n", file->d_name);
        }

        if (counter>=MAX_FILE_COUNT)
        {
            console_write(WARN, "DIR-READ", "Exceeded file limit!");
            break; // Do not process any more files
        }
        if (file->d_type!=DT_REG)
        { // Only include regular files and exclude other directories
            if (strcmp(file->d_name,".")==0){continue;} // Ignore these filenames
            if (strcmp(file->d_name,"..")==0){continue;}
            if (console_write_head(WARN,"DIR-READ")){
                printf("File '%s' is not a regular file\n", file->d_name);
            }
            continue;
        }
        console_write(DEBUG, "DIR-READ", "1");
        char filepath[256];
        file_path_concat(filepath, path, file->d_name);
        console_write(DEBUG, "DIR-READ", "2");
        if (check_file(filepath)!=0)
        { // Only include files that can be opened and read
            if (console_write_head(WARN,"DIR-READ")){
                printf("File '%s' is inaccessable\n", file->d_name);
            }
            continue;
        }
        // Passed all checks!

        dirFile* newFile = (dirFile*)malloc(sizeof(dirFile));
        newFile->filePath=strdup(filepath);
        newFile->nextFile=NULL;
        if (counter==0)
        {   // Index pointer should point to the first file
            console_write(DEBUG, "DIR-READ", "First File");
            list->indexFile = newFile;
        }else
        {   // Previous file's pointer should point to the new file
            console_write(DEBUG, "DIR-READ", "Next File");
            prevFile->nextFile=newFile;
        }
        prevFile=newFile;
        counter+=1;
    }
    return 0;
}

int directory_start(dirList* list)
{
    if (list->indexFile==NULL)
    {
        console_write(ERROR, "DIR-START", "No index file defined!");
        return 1;
    }
    list->fileCounter=1;
    console_write(DEBUG, "DIR-START", "File pointer returned to index;");
    list->currentFile=list->indexFile;
    return 0;
}

int check_file(char* filepath)
{

    FILE *f = fopen(filepath,"r");

    if (f == NULL)
    {
        perror("Error creating file");
        return 1;
    }
    fclose(f);
    return 0;
}

void file_path_concat(char* filepath, const char* path, const char* file)
{
    strcpy(filepath, path);
    strcat(filepath, "/");
    strcat(filepath, file);
}

int directory_next_file(dirList* list, char* filepath)
{
    if (list->currentFile==NULL){return 0;}
    if (list->fileCounter>=MAX_FILE_COUNT){return 0;}
    strcpy(filepath,list->currentFile->filePath);
    list->fileCounter+=1;

    dirFile *next = list->currentFile->nextFile;
    list->currentFile=next;
    return 1;
}