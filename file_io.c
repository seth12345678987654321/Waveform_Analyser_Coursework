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
    list.nextFile=NULL;
    return list;
}

int directory_read(dirList* list, char* path)
{
    list->path=strdup(path);

    struct dirent* file;
    DIR* dirstream = opendir(path);

    if (console_write_head(DEBUG,"DIR-READ")){
        printf("Directory path '%s'\n", path);
    }

    if (dirstream == NULL)
    {
        console_write(ERROR,"DIR-READ","Could not find the given directory.");
        exit(1);
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
            if (console_write_head(DEBUG,"DIR-READ")){
                printf("Skipping, File '%s' is not a regular file\n", file->d_name);
            }
            continue;
        }
        if (strstr(file->d_name,".csv")==NULL)
        { // Only include regular files and exclude other directories
            if (console_write_head(DEBUG,"DIR-READ")){
                printf("Skipping, File '%s' is not a csv file\n", file->d_name);
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
        newFile->fileName=strdup(file->d_name);
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
    if (counter==0)
    {
        console_write(ERROR, "DIR-READ", "No csv files found!");
        exit(1);
    }

    free(file);
    free(dirstream);
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
    list->nextFile=list->indexFile;
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
    strcat(filepath, FILEPATH_SEPARATOR);
    strcat(filepath, file);
}

int directory_next_file(dirList* list, char* filepath)
{
    if (list->nextFile==NULL){return 0;}
    if (list->fileCounter>=MAX_FILE_COUNT){return 0;}
    strcpy(filepath,list->nextFile->filePath);
    list->fileCounter+=1;
    list->currentFile = list->nextFile;
    dirFile *next = list->nextFile->nextFile;
    list->nextFile=next;

    return 1;
}

csvData* csv_access(csvFile* csv,int column, int row)
{
    int position = (row * csv->columnCount) + column;
    csvData* dataptr = csv->data + position;
    if ( (dataptr->column != column)  || (dataptr->row != row) )
    {
        if (console_write_head(ERROR, "CSV-ACCESS"))
        {
            printf("Invalid access at 0x%p r=%d c=%d\n",dataptr,row,column);
            console_write_head(ERROR, "CSV-ACCESS");
            printf("  - target: r=%d c=%d\n",dataptr,row,column);
            printf("\n");
        }
        return NULL;
    }
    return dataptr;
}

double csv_get(csvFile* csv, int column, int row)
{
    csvData* dataptr;
    if ( (dataptr = csv_access(csv,column,row)) != NULL )
    {
        return dataptr->data;
    }
    return 0;
}

int read_csv_file(csvFile* csv,char* filepath)
{
    console_write(DEBUG, "FILE-CSV-READ","entry");
    FILE* fp = fopen(filepath,"r");
    if (fp == NULL)
    {
        if (console_write_head(ERROR, "FILE-CSV-READ"))
        {
            printf("Could not open file '%s'\n", filepath);
        }

        return 1;
    }
    console_write(DEBUG, "FILE-CSV-READ","seeking");
    fseek(fp,0,SEEK_END);           // Go to the end of the file
    long fileSize = ftell(fp);  // Return file pointer position
    rewind(fp);                     // Return to the start of the file
    if (fileSize==0)
    {   // File is empty
        if (console_write_head(ERROR, "FILE-CSV-READ"))
        {
            printf("File is empty! '%s'\n", filepath);
        }

        return 1;
    }
    if (fileSize>=FILE_SIZE_LIMIT)
    {   // File is too large
        if (console_write_head(ERROR, "FILE-CSV-READ"))
        {
            printf("File size exceeds limit! '%s'\n", filepath);
        }
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

    if (console_write_head(DEBUG, "FILE-CSV-READ"))
    {
        printf("Size: %ld ,Rows: %d\n", fileSize,fileRows);
    }

    csv->rowCount=fileRows;

    int fileColumns=8;// TODO IMPLEMENT COLUMN NUMBER SEARCH
    csv->data=(csvData*)malloc(fileRows * fileColumns * sizeof(csvData));

    if (csv->data==NULL)
    {
        console_write(ERROR,"FILE-CSV-READ","Memory allocation failed, out of memory?");
    }

    csv->columnCount=fileColumns;
    char linestr[LINE_BUFFER_SIZE];

    for (int line=0; line<fileRows; line++)
    {
        fgets(linestr, sizeof(linestr), fp);

        char delimiter[2];
        delimiter[0]=CSV_COLUMN_DELIMITER;
        delimiter[1]='\0';


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
                console_write(WARN,"FILE-CSV-READ","Ignoring empty element (null token)");
                break;
            }
            if (1==1) // TODO implement number verification
            {
                csv->data[(line*csv->columnCount)+i].data=strtod(token,NULL);
                csv->data[(line*csv->columnCount)+i].column=i;
                csv->data[(line*csv->columnCount)+i].row=line;
            }else
            {
                csv->data[(line*csv->columnCount)+i].data=0;
                // TODO Not a number error
            }

        }
    }

    return 0;
}

void csv_free(csvFile* csv)
{
    free(csv->data);
    csv->data=NULL;
}

int txt_write(txtFile* file)
{
    FILE* fptr;

    fptr = fopen(file->fileName, "w");
    if (fptr==NULL)
    {
        if (console_write_head(ERROR, "TXT-WRITE"))
        {
            printf("Could not open file %s\n",file->fileName);
        }
        return 1;
    }
    fwrite(file->fileContents,sizeof(char),file->fileSize,fptr);
    return 0;
}

void txt_free(txtFile* file)
{
    free(file->fileContents);
    file->fileContents = NULL;
    free(file->fileName);
    file->fileName = NULL;
    free(file);
    file = NULL;
}


void directory_free(dirList* dir)
{
    dirFile* currentFile = dir->indexFile;
    dirFile* nextFile;
    while(currentFile!=NULL)
    {
        nextFile=currentFile->nextFile;
        free(currentFile->fileName);
        free(currentFile->filePath);
        currentFile->fileName=NULL;
        currentFile->filePath=NULL;
        free(currentFile);
        currentFile=NULL;
        currentFile=nextFile;
    }
    free(dir->path);
    dir->path=NULL;

}