// This file is used to define file input and output operations.
// Created by seth on 01/04/2026.
//

#ifndef WAVEFORM_ANALYSER_FILE_IO_H
#define WAVEFORM_ANALYSER_FILE_IO_H

#define MAX_FILE_COUNT 100


typedef struct dirFile
{ // Structure to hold information on a directory
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

void read_file(dirFile file);

// Create a directory list structure
dirList directory_list_create();

// Reads a directory and pushes file names to 'list'
int directory_read(dirList* list, char* path);

// Writes the next file to read in 'filepath'
int directory_next_file(dirList* list,char* filepath);

int directory_start(dirList* list);

// Returns 1 if the directory still has files to read
int directory_not_complete(dirList list);



// Frees the directory list struct from memory
void free_directory_list(char* );

// Concatenates separate file and path into a filepath
void file_path_concat(char* filepath, const char* path, const char* file);

#endif //WAVEFORM_ANALYSER_FILE_IO_H