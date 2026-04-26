#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "console_io.h"
#include "file_io.h"
#include "waveform.h"

#define TARGET_DIR "/home/seth/Desktop/test"

int main(void)
{
    #ifdef OS_NOT_FOUND // Fail if system is not explicitly supported
    console_write(ERROR,"MAIN","Could not detect a compatible operating system!");
    exit(1);
    #endif

    char filepath[FILEPATH_BUFFER_SIZE] = {0};
    int count=1;

    dirList dir = directory_list_create();
    directory_read(&dir,TARGET_DIR);        // Reads all the readable files in the directory
    directory_start(&dir);                  // Go to the first file

    while (directory_next_file(&dir,filepath))  // Go to the next file if there is one
    {
        console_write(INFO,"MAIN","Reading file");
        console_write(INFO,"MAIN",filepath);

        csvFile csv;
        read_csv_file(&csv,filepath);   // Turns the CSV file on disk into a struct

        Waveform* waveform =waveform_create(csv.rowCount-CSV_NON_DATA_ROWS);
        csv_to_waveform(waveform,&csv); // Converts the CSV struct into a waveform

        waveform_process(waveform);     // Performs all the mathematical operation
                                        //   Creates the results for the report

        char reportfile[FILEPATH_BUFFER_SIZE] = {0};
        char buffer[FILEPATH_BUFFER_SIZE] = {0};

        strcat(reportfile,dir.path);
        strcat(reportfile,FILEPATH_SEPARATOR);
        sprintf(buffer,"report %d.txt",count);   // Creates a file name for the report
        strcat(reportfile,buffer);

        // Writes the report into a text file struct
        txtFile* txt_file = write_results(waveform,reportfile,dir.currentFile->fileName);

        if (txt_write(txt_file)==0) // Write the text file to disk
        {
            console_write(INFO,"MAIN","Report written successfully\n");
        }else
        {
            console_write(ERROR,"MAIN","Report could not be written to the disk\n");

        }

        txt_free(txt_file);     // Free allocated memory
        csv_free(&csv);
        waveform_free(waveform);
        count++;
    }

    directory_free(&dir);

    return 0;
}
