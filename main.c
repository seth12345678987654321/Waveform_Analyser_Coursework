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
    }
    return 0;
}
