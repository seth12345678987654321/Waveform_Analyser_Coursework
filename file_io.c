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



int read_csv_file(csvFile* csv,char* filepath)
{
    console_write(DEBUG, "FILE-CSV-READ","entry");
    FILE* fp = fopen(filepath,"r");
    if (fp == NULL)
    {
        console_write_head(ERROR, "FILE-CSV-READ");
        printf("Could not open file '%s'\n", filepath);
        return 1;
    }
    console_write(DEBUG, "FILE-CSV-READ","seeking");
    fseek(fp,0,SEEK_END);           // Go to the end of the file
    long fileSize = ftell(fp);  // Return file pointer position
    rewind(fp);                     // Return to the start of the file
    if (fileSize==0)
    {   // File is empty
        console_write_head(ERROR, "FILE-CSV-READ");
        printf("File is empty! '%s'\n", filepath);
        return 1;
    }
    if (fileSize>=FILE_SIZE_LIMIT)
    {   // File is too large
        console_write_head(ERROR, "FILE-CSV-READ");
        printf("File size exceeds limit! '%s'\n", filepath);
        return 1;
    }
    int fileRows = 1;
    for (int i=0; i<fileSize; i++)
    {
        char ch = fgetc(fp);
        if (ch==CSV_ROW_DELIMITER)
        {
            fileRows++;
        }
    }
    if (fileRows<=3)
    {
        console_write(WARN, "FILE-CSV-READ", "Not many rows");
    }
    rewind(fp);
    console_write(DEBUG, "FILE-CSV-READ","findrows");
    console_write_head(DEBUG, "FILE-CSV-READ");
    printf("Size: %ld ,Rows: %d\n", fileSize,fileRows);

    csv->rowCount=fileRows;
    csv->rows=(csvRow*)malloc(fileRows * sizeof(csvRow*));
    int fileColumns=9;// TODO IMPLEMENT COLUMN NUMBER SEARCH
    csv->columnCount=fileColumns;
    char linestr[LINE_BUFFER_SIZE];
    for (int line=0; line<fileRows; line++)
    {
        fgets(linestr, sizeof(linestr), fp);

        char delimiter[2];
        delimiter[0]=CSV_COLUMN_DELIMITER;
        delimiter[1]='\0';
        csv->rows[line].data=(double*)malloc(fileColumns * sizeof(double));;
        for (int i=0; i<fileColumns; i++)
        {
            char* token;

            if (i==0)
            {   // Read the first token
                token = strtok(linestr,delimiter);
            }else
            {   // Read subsequent tokens
                token = strtok(NULL,delimiter);
            }
            if (token==NULL)
            {
                console_write(WARN,"FILE-CSV-READ","Null Token");
                break;
            }
            if (1==1) // TODO implement number verification
            {
                csv->rows[line].data[i]=strtod(token,NULL);
            }else
            {
                csv->rows[line].data[i]=0;
                // TODO Not a number error
            }

        }
    }

    return 0;


}