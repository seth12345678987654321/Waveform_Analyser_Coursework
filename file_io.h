// This file is used to define file input and output operations.
// Created by seth on 01/04/2026.
//

#ifndef WAVEFORM_ANALYSER_FILE_IO_H
#define WAVEFORM_ANALYSER_FILE_IO_H

#define MAX_FILE_COUNT 100
#define FILE_SIZE_LIMIT 1000000
#define LINE_BUFFER_SIZE 256
#define CSV_ROW_DELIMITER '\n'
#define CSV_COLUMN_DELIMITER ','

typedef enum csvValueType
{
    STRING,
    FLOAT
}csvValueType;


typedef struct csvData
{   // 16 Bytes
    int row;   // 4 Bytes
    int column; // 4 Bytes
    double data; // 8 Bytes, Pointer array of multiple types
}csvData;

typedef struct csvFile
{
    short rowCount;
    short columnCount;
    csvData* data;
}csvFile;


typedef struct dirFile
{ // Structure to hold information on a file
    char* filePath;
    struct dirFile *nextFile;
} dirFile;

typedef struct
{ // Structure to hold information on a directory
    short length;   // Length of 'files' linked list
    short fileCounter;
    char* path;
    dirFile *indexFile;  // Pointer to the index dirFile struct, linked list
    dirFile *currentFile;  // Current file being accessed
} dirList;


// Checks if a file is readable
int check_file(char* filepath);

int read_csv_file(csvFile* csv,char* filepath);

// Create a directory list structure
dirList directory_list_create();

// Reads a directory and pushes file names to 'list'
int directory_read(dirList* list, char* path);

// Writes the next file to read in 'filepath'
int directory_next_file(dirList* list,char* filepath);

int directory_start(dirList* list);

// Returns 1 if the directory still has files to read
int directory_not_complete(dirList list);

// Returns the pointer to a given CSV element, returns NULL if invalid
csvData* csv_access(csvFile* csv, int column, int row);

// Fetches the data from the CSV element and writes to val
int csv_get(csvFile* csv, double* val, int column, int row);

// Writes the data in val to the CSV element
int csv_set(csvFile* csv, double* val, int column, int row);

// Frees the directory list struct from memory
void free_directory_list(char* );

// Concatenates separate file and path into a filepath
void file_path_concat(char* filepath, const char* path, const char* file);

#endif //WAVEFORM_ANALYSER_FILE_IO_H