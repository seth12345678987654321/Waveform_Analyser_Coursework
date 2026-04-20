#include <stdio.h>
#include "console_io.h"
#include "file_io.h"

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
        console_write_head(INFO,"MAIN");
        for (int i=0;i>csv.columnCount;i++)
        {
            printf("%lf ",csv.rows[5].data[i]);
        }
        printf("\n");

    }
    return 0;
}
