//
// Created by seth on 02/04/2026.
//

#include "console_io.h"

#include <stdio.h>
#include <string.h>


int console_write_head(enum WRITE_TYPE type, char* location)
{
    char prefix[100];

    if (CONSOLE_PRINT_LOCATION !=1)
    {
        prefix[0] = '\0';
    }else
    {
        strcpy(prefix,location);
        strcat(prefix,CONSOLE_LOCATION_SUFFIX);
    }

    switch (type)
    {
    case ERROR:
        if (CONSOLE_SHOW_ERROR==1)
        {
            console_modify(ANSI_BOLD);
            console_modify(ANSI_RED);
            printf("ERROR %s%s", CONSOLE_TYPE_SUFFIX, prefix);
            console_modify(ANSI_RESET);
            return 1;
        }
        return 0;
    case WARN:
        if (CONSOLE_SHOW_WARN==1)
        {
            console_modify(ANSI_BOLD);
            console_modify(ANSI_YELLOW);
            printf(" WARN %s%s", CONSOLE_TYPE_SUFFIX, prefix);
            console_modify(ANSI_RESET);
            return 1;
        }
        return 0;
    case INFO:
        if (CONSOLE_SHOW_INFO==1)
        {
            console_modify(ANSI_BOLD);
            console_modify(ANSI_CYAN);
            printf(" INFO %s%s", CONSOLE_TYPE_SUFFIX, prefix);
            console_modify(ANSI_RESET);
            return 1;
        }
        return 0;
    case DEBUG:
        if (CONSOLE_SHOW_DEBUG==1)
        {
            console_modify(ANSI_BOLD);
            console_modify(ANSI_MAGENTA);
            printf("DEBUG %s%s", CONSOLE_TYPE_SUFFIX, prefix);
            console_modify(ANSI_RESET);
            return 1;
        }
        return 0;

    default:
        console_modify(ANSI_BOLD);
        printf("  MSG %s%s", CONSOLE_TYPE_SUFFIX, prefix);
        console_modify(ANSI_RESET);
        return 1;
    }
    return 0;
}
void console_write(enum WRITE_TYPE type, char* location, char* message)
{
    if (console_write_head(type,location))
    {
        printf("%s\n", message);
    }
}


void console_modify(char* modifier)
{
    printf("%s", modifier);
}