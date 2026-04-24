cmake_minimum_required(VERSION 4.1)
project(waveform_analyser C)

set(CMAKE_C_STANDARD 99)

add_executable(waveform_analyser main.c
        file_io.c
        file_io.h
        console_io.c
        console_io.h
        waveform.c
        waveform.h)

target_link_libraries(waveform_analyser m)