//
// Created by seth on 11/04/2026.
//

#ifndef WAVEFORM_ANALYSER_WAVEFORM_H
#define WAVEFORM_ANALYSER_WAVEFORM_H


#include <stdint.h>
#include "file_io.h"

#define VOLTAGE_TOLERANCE_LOW 207
#define VOLTAGE_TOLERANCE_HIGH 253
#define THD_ELEVATED 5
#define THD_EXCESSIVE 8
#define DC_OFFSET_LIMIT 0.1
#define DC_OFFSET_LIMIT 0.1
#define CLIPPING_THRESHOLD 324.9

typedef struct WaveformSample
{
    double timestamp;
    double phase_A;
    double phase_B;
    double phase_C;
    double line_current;
    double frequency;
    double power_factor;
    double thd_percent;
    uint8_t clipping_A:1;
    uint8_t clipping_B:1;
    uint8_t clipping_C:1;
    uint8_t uncompliant_A:1;
    uint8_t uncompliant_B:1;
    uint8_t uncompliant_C:1;

}WaveformSample;

typedef struct Waveform{
    WaveformSample* samples;
    int waveform_size;
    double Vrms_Phase_A;
    double Vrms_Phase_B;
    double Vrms_Phase_C;
    double Voff_Phase_A;
    double Voff_Phase_B;
    double Voff_Phase_C;
    double Vpp_Phase_A;
    double Vpp_Phase_B;
    double Vpp_Phase_C;
    double Irms_current;
    double avg_power_factor;
    double avg_thd_percent;
    double max_thd_percent;
    double avg_frequency;
    double std_deviation_A;
    double std_deviation_B;
    double std_deviation_C;
    double variance_A;
    double variance_B;
    double variance_C;
    int samples_clipped;
    int samples_uncompliant;
}Waveform;

Waveform* waveform_create(int size);
// Allocates memory for a waveform and initialises values
void waveform_free(Waveform *waveform);
// Frees the waveform from memory

// Writes one sample of information
Waveform* waveform_sample_write();

// Converts a CSV file into a waveform
int csv_to_waveform(Waveform* waveform, csvFile* csv);

int waveform_process(Waveform* waveform);

#endif //WAVEFORM_ANALYSER_WAVEFORM_H