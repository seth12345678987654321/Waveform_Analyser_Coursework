//
// Created by seth on 11/04/2026.
//

#include "waveform.h"

#include <stdlib.h>

Waveform* waveform_create(int size)
{
    Waveform *waveform = malloc(sizeof(Waveform));
    WaveformSample *samplearray = malloc(sizeof(WaveformSample)*size);
    waveform->samples=samplearray;
    waveform->waveform_size=size;
    waveform->Voff_Phase_A=0;
    waveform->Voff_Phase_B=0;
    waveform->Voff_Phase_C=0;
    waveform->Vrms_Phase_A=0;
    waveform->Vrms_Phase_B=0;
    waveform->Vrms_Phase_C=0;
    for (int i=0;i<size;i++)
    {
        WaveformSample sample=waveform->samples[i];
        sample.phase_A=0;
        sample.phase_B=0;
        sample.phase_C=0;
        sample.line_current=0;
        sample.clipping=0;
        sample.frequency=0;
        sample.power_factor=0;
    }
    return waveform;
}

void waveform_free(Waveform *waveform)
{
    free(waveform->waveform);
    free(waveform);
}

