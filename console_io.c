//
// Created by seth on 02/04/2026.
//

#include "console_io.h"

#include <stdio.h>


void console_write(enum WRITE_TYPE type, char* location,char* message)
{
    if (CONSOLE_PRINT_LOCATION !=1)
    {
        location=NULL;
    }

    switch (type)
    {
    case ERROR:
        if (CONSOLE_SHOW_ERROR==1)
        {
            console_modify(ANSI_BOLD);
            console_modify(ANSI_RED);
            printf("ERROR | %s:", location);
            console_modify(ANSI_RESET);
            printf(" %s\n", message);
        }
        return;
    case WARN:
        if (CONSOLE_SHOW_WARN==1)
        {
            console_modify(ANSI_BOLD);
            console_modify(ANSI_YELLOW);
            printf(" WARN | %s:", location);
            console_modify(ANSI_RESET);
            printf(" %s\n", message);
        }
        return;
    case INFO:
        if (CONSOLE_SHOW_INFO==1)
        {
            console_modify(ANSI_BOLD);
            console_modify(ANSI_CYAN);
            printf(" INFO | %s:", location);
            console_modify(ANSI_RESET);
            printf(" %s\n", message);
        }
        return;
    case DEBUG:
        if (CONSOLE_SHOW_DEBUG==1)
        {
            console_modify(ANSI_BOLD);
            console_modify(ANSI_MAGENTA);
            printf("DEBUG | %s:", location);
            console_modify(ANSI_RESET);
            printf(" %s\n", message);
        }
        return;

    default:
        console_modify(ANSI_BOLD);
        printf("  MSG | %s:", location);
        console_modify(ANSI_RESET);
        printf(" %s\n", message);
    }
}

void console_modify(char* modifier)
{
    printf("%s", modifier);
}