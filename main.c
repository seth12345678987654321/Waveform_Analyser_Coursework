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
    #ifdef OS_NOT_FOUND
    console_write(ERROR,"MAIN","Could not detect a compatible operating system!");
    exit(1);
    #endif
    char filepath[FILEPATH_BUFFER_SIZE] = {0};
    int count=1;

    dirList dir = directory_list_create();
    directory_read(&dir,TARGET_DIR);
    directory_start(&dir);

    while (directory_next_file(&dir,filepath))
    {
        console_write(INFO,"MAIN","Reading file");
        console_write(INFO,"MAIN",filepath);
        csvFile csv;
        read_csv_file(&csv,filepath);

        Waveform* waveform =waveform_create(csv.rowCount-CSV_NON_DATA_ROWS);
        csv_to_waveform(waveform,&csv);
        waveform_process(waveform);

        char reportfile[FILEPATH_BUFFER_SIZE] = {0};
        char buffer[FILEPATH_BUFFER_SIZE] = {0};

        strcat(reportfile,dir.path);
        strcat(reportfile,FILEPATH_SEPARATOR);
        sprintf(buffer,"report %d.txt",count);
        strcat(reportfile,buffer);
        txtFile* txt_file = write_results(waveform,reportfile,dir.currentFile->fileName);


        if (txt_write(txt_file)==0)
        {
            console_write(INFO,"MAIN","Report written successfully\n");
        }else
        {
            console_write(ERROR,"MAIN","Report could not be written to the disk\n");

        }
        txt_free(txt_file);

        csv_free(&csv);
        waveform_free(waveform);
        count++;
    }
    return 0;
}
