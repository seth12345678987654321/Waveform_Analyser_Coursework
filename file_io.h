// This file is used to define file input and output operations.
// Created by seth on 01/04/2026.
//

#ifndef WAVEFORM_ANALYSER_FILE_IO_H
#define WAVEFORM_ANALYSER_FILE_IO_H

#define MAX_FILE_COUNT 100


typedef struct dirFile
{ // Structure to hold information on a directory
    char* fileName;
    struct dirFile *nextFile;
} dirFile;

typedef struct
{ // Structure to hold information on a directory
    short length;   // Length of 'files' linked list
    short fileCounter;
    dirFile *indexFile;  // Pointer to the index dirFile struct, linked list
    dirFile *currentFile;  // Current file being accessed
} dirList;


// Checks if a file is readable
int check_file(char* file);

void read_file(dirFile file);

// Create a directory list structure
dirList directory_list_create();

// Reads a directory and pushes file names to 'list'
int directory_read(dirList* list, char* path);

// Returns the next file to read
char* directory_next_file(dirList list);

// Returns 1 if the directory still has files to read
int directory_not_complete(dirList list);

// Frees the directory list struct from memory
void free_directory_list(char* );

#endif //WAVEFORM_ANALYSER_FILE_IO_H