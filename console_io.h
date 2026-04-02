//
// Created by seth on 02/04/2026.
//

#ifndef WAVEFORM_ANALYSER_CONSOLE_IO_H
#define WAVEFORM_ANALYSER_CONSOLE_IO_H

#endif //WAVEFORM_ANALYSER_CONSOLE_IO_H

#define CONSOLE_SHOW_DEBUG 1
#define CONSOLE_SHOW_WARN 1
#define CONSOLE_SHOW_ERROR 1
#define CONSOLE_SHOW_INFO 1
#define CONSOLE_PRINT_LOCATION 1

#define CONSOLE_LOCATION_SUFFIX ": "
#define CONSOLE_TYPE_SUFFIX "| "

#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN    "\x1b[36m"
#define ANSI_RESET   "\x1b[0m"
#define ANSI_BOLD   "\x1b[1m"

enum WRITE_TYPE
{
    MESSAGE,
    DEBUG,
    WARN,
    ERROR,
    INFO,
};

// Writes a formatted message to the console
void console_write(enum WRITE_TYPE type, char* location, char* message);

// Only writes the head of the message. To be used with printf.
int console_write_head(enum WRITE_TYPE type, char* location);

// Prints a modifier
void console_modify(char* modifier);