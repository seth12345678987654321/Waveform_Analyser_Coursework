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
{  // Initialises values for dirList
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
    list->path=strdup(path); // Make a copy of the directory path to dirList

    struct dirent* file; // Open the directory stream
    DIR* dirstream = opendir(path);

    if (console_write_head(DEBUG,"DIR-READ")){
        printf("Directory path '%s'\n", path);}

    if (dirstream == NULL)
    {
        console_write(ERROR,"DIR-READ","Could not find the given directory.");
        exit(1);
    }

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
#if defined(__unix__) || defined(linux) // NOT SUPPORTED ON WINDOWS
        if (file->d_type!=DT_REG)
        { // Only include regular files and exclude other directories
            if (strcmp(file->d_name,".")==0){continue;} // Ignore these filenames
            if (strcmp(file->d_name,"..")==0){continue;}
            if (console_write_head(DEBUG,"DIR-READ")){
                printf("Skipping, File '%s' is not a regular file\n", file->d_name);
            }
            continue;
        }
#endif
        if (strstr(file->d_name,".csv")==NULL)
        { // Only include regular files and exclude other directories
            if (console_write_head(DEBUG,"DIR-READ")){
                printf("Skipping, File '%s' is not a csv file\n", file->d_name);
            }
            continue;
        }

        char filepath[256];
        file_path_concat(filepath, path, file->d_name);
        if (check_file(filepath)!=0)
        { // Only include files that can be opened and read
            if (console_write_head(WARN,"DIR-READ")){
                printf("File '%s' is inaccessable\n", file->d_name);
            }
            continue;
        }
        // Passed all checks!

        dirFile* newFile = (dirFile*)malloc(sizeof(dirFile)); // Allocate new file
        newFile->filePath=strdup(filepath); // Allocate and copy filepath
        newFile->fileName=strdup(file->d_name);  // Allocate and copy filename
        newFile->nextFile=NULL;

        // Construct linked list
        if (counter==0)
        {   // Index pointer should point to the first file
            console_write(DEBUG, "DIR-READ", "First File");
            list->indexFile = newFile;
        }else
        {   // Previous file's pointer should point to the new file
            console_write(DEBUG, "DIR-READ", "Next File");
            prevFile->nextFile=newFile;
        }

        prevFile=newFile; // Remember the previous file
        counter+=1;
    }
    if (counter==0)
    {
        console_write(ERROR, "DIR-READ", "No csv files found!");
        exit(1);
    }

    free(file);      // Free file and dir stream
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
    list->fileCounter=1; // Initialise the file counter
    console_write(DEBUG, "DIR-START", "File pointer returned to index;");
    list->nextFile=list->indexFile; // Reset the next file pointer to the index
    return 0;
}

int check_file(char* filepath)
{

    FILE *f = fopen(filepath,"r"); // Attempt to open file

    if (f == NULL)
    {   // Failed to open
        perror("Error creating file");
        return 1;
    }
    fclose(f); // Close file
    return 0; // Successfully opened file
}

void file_path_concat(char* filepath, const char* path, const char* file)
{
    // Combines a directory and filename into a filepath
    strcpy(filepath, path);
    strcat(filepath, FILEPATH_SEPARATOR);
    strcat(filepath, file);
}

int directory_next_file(dirList* list, char* filepath)
{
    if (list->nextFile==NULL){return 0;} // Next file does not exist
    if (list->fileCounter>=MAX_FILE_COUNT){return 0;} // Reached file read limit

    strcpy(filepath,list->nextFile->filePath); // Output next file's filepath
    list->fileCounter+=1;
    list->currentFile = list->nextFile;        // Move to next file in linked list
    dirFile *next = list->nextFile->nextFile;
    list->nextFile=next;                       // Update next file pointer value

    return 1;
}

csvData* csv_access(csvFile* csv,int column, int row)
{
    int position = (row * csv->columnCount) + column; // Calculate offset from the index pointer

    csvData* dataptr = csv->data + position; // Increment pointer by the offset

    if ( dataptr->column != column  || dataptr->row != row )
    {   // Check if expected column and row match the result
        if (console_write_head(ERROR, "CSV-ACCESS"))
        {
            printf("Invalid access at 0x%p r=%d c=%d\n",dataptr,row,column);
            console_write_head(ERROR, "CSV-ACCESS");
            printf("  - target: r=%d c=%d\n",row,column);
            printf("\n");
        }
        return NULL;
    }
    return dataptr;
}

double csv_get(csvFile* csv, int column, int row)
{   // Get data from an element of the csvFile
    csvData* dataptr;
    if ( (dataptr = csv_access(csv,column,row)) != NULL )
    {   // Check data is valid
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
        if (console_write_head(ERROR, "FILE-CSV-READ")){
            printf("Could not open file '%s'\n", filepath);}
        return 1;
    }
    console_write(DEBUG, "FILE-CSV-READ","seeking");
    fseek(fp,0,SEEK_END);       // Go to the end of the file
    long fileSize = ftell(fp);  // Return file pointer position (size of file)
    rewind(fp);                 // Return to the start of the file

    if (fileSize==0)
    {   // File is empty
        if (console_write_head(ERROR, "FILE-CSV-READ")){
            printf("File is empty! '%s'\n", filepath);}
        return 1;
    }

    if (fileSize>=FILE_SIZE_LIMIT)
    {   // File is too large
        if (console_write_head(ERROR, "FILE-CSV-READ")){
            printf("File size exceeds limit! '%s'\n", filepath);}
        return 1;
    }

    int fileRows = 1;
    char ch;
    // Find the number of rows in the file
    for (int i=0; i<fileSize; i++)
    {
        ch = (char)fgetc(fp); // Get next character
        if (ch==CSV_ROW_DELIMITER) // Find new row characters
        {
            fileRows++;
        }
    }

    rewind(fp); // Return to the start of the file

    if (fileRows<=ROWS_WARNING_LIMIT)
    {
        console_write(WARN, "FILE-CSV-READ", "Not many rows");
    }

    if (console_write_head(DEBUG, "FILE-CSV-READ")){
        printf("Size: %ld ,Rows: %d\n", fileSize,fileRows);}

    csv->rowCount=(short)fileRows;

    int fileColumns=CSV_COLUMNS;// TODO IMPLEMENT COLUMN NUMBER SEARCH
    csv->data=(csvData*)malloc(fileRows * fileColumns * sizeof(csvData));

    if (csv->data==NULL)
    {   // malloc() failed
        console_write(ERROR,"FILE-CSV-READ","Memory allocation failed, out of memory?");
    }

    csv->columnCount=(short)fileColumns;
    char linestr[LINE_BUFFER_SIZE];

    for (int line=0; line<fileRows; line++) // Iterate though every row
    {
        fgets(linestr, sizeof(linestr), fp); // Get 1 line of the file

        char delimiter[2];  // Convert delimiter char into a string
        delimiter[0]=CSV_COLUMN_DELIMITER;
        delimiter[1]='\0';

        for (int i=0; i<fileColumns; i++) // Iterate though every column
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

            csv->data[(line*csv->columnCount)+i].data=strtod(token,NULL); // Convert token into double
            csv->data[(line*csv->columnCount)+i].column=i; // Store the column value
            csv->data[(line*csv->columnCount)+i].row=line; // Store the row value


        }
    }

    return 0;
}

void csv_free(csvFile* csv)
{
    free(csv->data);  // Free array from memory
    csv->data=NULL;   // Erase pointer value for safety
}

int txt_write(txtFile* file)
{
    FILE* fptr;

    fptr = fopen(file->fileName, "w");
    if (fptr==NULL)
    {   // File could not be opened
        if (console_write_head(ERROR, "TXT-WRITE")){
            printf("Could not open file %s\n",file->fileName);}
        return 1;
    }
    // Write contents of txtFile to the file on disk
    fwrite(file->fileContents,sizeof(char),file->fileSize,fptr);
    return 0;
}

void txt_free(txtFile* file)
{
    // Frees a txtFile structure completely.
    free(file->fileContents);
    file->fileContents = NULL; // Erase pointer value for safety
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
    {   // Loop until every file has been proced
        nextFile=currentFile->nextFile; // Move to next file
        free(currentFile->fileName); // Free strings
        free(currentFile->filePath);
        currentFile->fileName=NULL; // Erase pointer value for safety
        currentFile->filePath=NULL;
        free(currentFile); // Free file
        currentFile=NULL;
        currentFile=nextFile;
    }
    free(dir->path); // Free directory
    dir->path=NULL;

}