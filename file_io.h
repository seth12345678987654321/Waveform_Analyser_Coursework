// This file is used to define file input and output operations.
// Created by seth on 01/04/2026.
//

#ifndef WAVEFORM_ANALYSER_FILE_IO_H
#define WAVEFORM_ANALYSER_FILE_IO_H

#define MAX_FILE_COUNT 100
#define FILE_SIZE_LIMIT 1000000
#define TXTFILE_BUFFER_SIZE 10000
#define LINE_BUFFER_SIZE 256
#define CSV_ROW_DELIMITER '\n'
#define CSV_COLUMN_DELIMITER ','
#define FILEPATH_BUFFER_SIZE 256
#define CSV_NON_DATA_ROWS 2
#define CSV_COLUMNS 8
#define ROWS_WARNING_LIMIT 5


#if defined(__unix__) || defined(linux)
#define FILEPATH_SEPARATOR "/"
#elif defined(_WIN32) || defined(WIN32)
#define FILEPATH_SEPARATOR "\\"
#else
#define OS_NOT_FOUND
#endif


typedef struct csvData
{   // 16 Bytes
    int row;   // 4 Bytes
    int column; // 4 Bytes
    double data; // 8 Bytes
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
    char* fileName;
    struct dirFile *nextFile;
} dirFile;

typedef struct dirList
{ // Structure to hold information on a directory
    short length;   // Length of 'files' linked list
    short fileCounter;
    char* path;
    dirFile *indexFile;   // Pointer to the index dirFile struct, linked list
    dirFile *nextFile;    // Next file to be accessed
    dirFile *currentFile; // Current file being accessed
} dirList;

typedef struct txtFile
{
    short fileNameLength;
    short fileSize;
    char* fileName;
    char* fileContents;
}txtFile;



// DIRECTORY FUNCTIONS  ------------------------------------------

// Checks if a file is readable
int check_file(char* filepath);

// Create a directory list structure
dirList directory_list_create();

// Reads a directory and pushes file names to 'list'
int directory_read(dirList* list, char* path);

// Writes the next file to read in 'filepath'
int directory_next_file(dirList* list,char* filepath);

// Returns the directory list to the index file
int directory_start(dirList* list);

// Returns 1 if the directory still has files to read
int directory_not_complete(dirList list);

// Frees the directory list struct from memory
void directory_free(dirList* dir);

// Concatenates separate file and path into a filepath
void file_path_concat(char* filepath, const char* path, const char* file);



// CSV FUNCTIONS  ------------------------------------------

// Returns the pointer to a given CSV element, returns NULL if invalid
csvData* csv_access(csvFile* csv, int column, int row);

// Reads a csv file and writes it to memory as an csvFile array
int read_csv_file(csvFile* csv,char* filepath);

// Fetches the data from the CSV element and writes to val
double csv_get(csvFile* csv, int column, int row);

// Writes the data in val to the CSV element
int csv_set(csvFile* csv, double* val, int column, int row);

// Frees a CSV file from memory
void csv_free(csvFile* csv);



// TXT FUNCTIONS ------------------------------------------

// Writes data in a txtFile struct
int txt_write(txtFile* file);

// Frees a TXT file from memory
void txt_free(txtFile* file);



#endif //WAVEFORM_ANALYSER_FILE_IO_H