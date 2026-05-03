# 3 Phase Waveform Analyser
### UWE Coursework Project
### By Seth Thompson 
This program processes a batch of CSV files produced by monitoring equipment for a 3 phase power supply, and creates a text report for each file.

### Github Link
https://github.com/seth12345678987654321/Waveform_Analyser_Coursework

### Instructions
The program requires a directory containing one or more correctly formatted CSV files in order to run.  
The directory can be specified by the macro `TARGET_DIR` inside main.c.

Reports are normally named as `report n.txt` where n is an integer.  
The report filename can be changed by the macros `REPORT_NAME` and `REPORT_EXT` in main.c.  
Reports are written to the same directory to the one that is read (in `TARGET_DIR`)


| Macro                    | File       | Description                                                   |
|--------------------------|------------|---------------------------------------------------------------|
| `TARGET_DIR`             | main.c     | Directory to read and write files                             |
| `REPORT_NAME`            | main.c     | Name of the report                                            |
| `REPORT_EXT`             | main.c     | File extension of the report                                  |
| `MAX_FILE_COUNT`         | file_io.h  | Maximum number of files to process                            |
| `FILE_SIZE_LIMIT`        | file_io.h  | Ignore files larger than this (in bytes)                      |
| `TXTFILE_BUFFER_SIZE`    | file_io.h  | Buffer allocated for writing the report                       |
| `LINE_BUFFER_SIZE`       | file_io.h  | Buffer allocated for procesing 1 CSV line                     |
| `CSV_ROW_DELIMITER`      | file_io.h  | What characters seperate rows in the CSV                      |
| `CSV_COLUMN_DELIMITER`   | file_io.h  | What characters seperate columns in the CSV                   |
| `FILEPATH_BUFFER_SIZE`   | file_io.h  | Buffer allocated for filepaths, increase if path is too long. |
| `CSV_NON_DATA_ROWS`      | file_io.h  | How many rows of the CSV do not contain useful data?          |
| `CSV_COLUMNS`            | file_io.h  | How many columns does the CSV have?                           |
| `ROWS_WARNING_LIMIT`     | file_io.h  | Issue a warning if there are less than this many rows         |
| `VOLTAGE_TOLERANCE_LOW`  | waveform.h | Low limit for RMS voltages for EN 50160 compliance            |
| `VOLTAGE_TOLERANCE_HIGH` | waveform.h | High limit for RMS voltages for EN 50160 compliance           |
| `THD_ELEVATED`           | waveform.h | Threshold for a 'Elevated' THD                                |
| `THD_EXCESSIVE`          | waveform.h | Threshold for a 'Excessive' THD                               |
| `DC_OFFSET_LIMIT`        | waveform.h | Threshold for a DC Offset Warning                             |
| `CLIPPING_THRESHOLD`     | waveform.h | Threshold for clipped samples detection                       |
