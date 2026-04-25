#include <stdio.h>
#include "console_io.h"
#include "file_io.h"
#include "waveform.h"

int main(void)
{
    dirList dir = directory_list_create();
    directory_read(&dir,"/home/seth/Desktop/test");
    directory_start(&dir);
    char filepath[256];
    while (directory_next_file(&dir,filepath))
    {
        console_write(INFO,"MAIN",filepath);
        csvFile csv;
        read_csv_file(&csv,filepath);
        console_write_head(INFO,"MAIN");
        printf("%d %d\n",csv.rowCount,csv.columnCount);

        Waveform* waveform =waveform_create(csv.rowCount-2);
        csv_to_waveform(waveform,&csv);

        waveform_process(waveform);

        console_write_head(INFO,"MAIN");
        printf("%f %f\n",waveform->variance_A,waveform->std_deviation_A);

        printf("\n");

        csv_free(&csv);
        waveform_free(waveform);

    }
    return 0;
}
