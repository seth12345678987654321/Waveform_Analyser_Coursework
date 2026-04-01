cmake_minimum_required(VERSION 4.1)
project(waveform_analyser C)

set(CMAKE_C_STANDARD 99)

add_executable(waveform_analyser main.c
        file_io.c
        file_io.h)
