#include <stdio.h>
#include "console_io.h"
int main(void)
{
    console_write(ERROR,"main","hello!");
    console_write(DEBUG,"main","hello!");
    console_write(INFO,"main","hello!");
    console_write(WARN,"main","hello!");
    console_write(MESSAGE,"main","hello!");
    return 0;
}