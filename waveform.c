//
// Created by seth on 11/04/2026.
//

#include "waveform.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
    waveform->avg_frequency=0;
    waveform->avg_power_factor=0;
    waveform->avg_thd_percent=0;
    waveform->max_thd_percent=0;
    waveform->samples_clipped_phaseA=0;
    waveform->samples_clipped_phaseB=0;
    waveform->samples_clipped_phaseC=0;
    waveform->Irms_current=0;
    for (int i=0;i<size;i++)
    {
        WaveformSample sample=waveform->samples[i];
        sample.phase_A=0;
        sample.phase_B=0;
        sample.phase_C=0;
        sample.line_current=0;
        sample.clipping_A=0;
        sample.clipping_B=0;
        sample.clipping_C=0;
        sample.uncompliant_A=0;
        sample.uncompliant_B=0;
        sample.uncompliant_C=0;
        sample.frequency=0;
        sample.power_factor=0;
    }
    return waveform;
}

void waveform_free(Waveform *waveform)
{
    free(waveform->samples);
    free(waveform);
}

int csv_to_waveform(Waveform* waveform, csvFile* csv)
{
    for (int i=1; i < csv->rowCount-1; i++)
    {
        waveform->samples[i-1].timestamp    = csv_get(csv,0,i);
        waveform->samples[i-1].phase_A      = csv_get(csv,1,i);
        waveform->samples[i-1].phase_B      = csv_get(csv,2,i);
        waveform->samples[i-1].phase_C      = csv_get(csv,3,i);
        waveform->samples[i-1].line_current = csv_get(csv,4,i);
        waveform->samples[i-1].frequency    = csv_get(csv,5,i);
        waveform->samples[i-1].power_factor = csv_get(csv,6,i);
        waveform->samples[i-1].thd_percent  = csv_get(csv,7,i);
    }
}



int is_clipping(const double voltage)
{
    if (voltage>=CLIPPING_THRESHOLD)
    {
        return 1;
    }
    if (voltage<=-CLIPPING_THRESHOLD)
    {
        return 1;
    }
    return 0;
}

int is_tolerant(double voltage)
{
    voltage = fabs(voltage);
    if (voltage<VOLTAGE_TOLERANCE_LOW)
    {
        return 1;
    }
    if (voltage>VOLTAGE_TOLERANCE_HIGH)
    {
        return 1;
    }
    return 0;
}

int waveform_process(Waveform* waveform)
{
    double PhaseA_Max=0;
    double PhaseB_Max=0;
    double PhaseC_Max=0;
    double PhaseA_Min=0;
    double PhaseB_Min=0;
    double PhaseC_Min=0;

    double PhaseA_Sum=0;
    double PhaseB_Sum=0;
    double PhaseC_Sum=0;

    double Current_SquareSum=0;
    double PhaseA_SquareSum=0;
    double PhaseB_SquareSum=0;
    double PhaseC_SquareSum=0;

    int PhaseA_clipping_events=0;
    int PhaseB_clipping_events=0;
    int PhaseC_clipping_events=0;

    int PhaseA_out_of_tolerance=0;
    int PhaseB_out_of_tolerance=0;
    int PhaseC_out_of_tolerance=0;

    double Frequency_sum=0;
    double THD_Sum=0;
    double THD_Max=0;
    double Power_Factor_Sum=0;

    int size = waveform->waveform_size;
    for (int i=0; i < size; i++)
    {

        // Minimum and maximum for peak to peak voltage calculation
        PhaseA_Max = fmax(waveform->samples[i].phase_A,PhaseA_Max);
        PhaseB_Max = fmax(waveform->samples[i].phase_B,PhaseB_Max);
        PhaseC_Max = fmax(waveform->samples[i].phase_C,PhaseC_Max);
        PhaseA_Min = fmin(waveform->samples[i].phase_A,PhaseA_Min);
        PhaseB_Min = fmin(waveform->samples[i].phase_B,PhaseB_Min);
        PhaseC_Min = fmin(waveform->samples[i].phase_C,PhaseC_Min);

        // Sum for mean / DC offset calculation
        PhaseA_Sum = waveform->samples[i].phase_A;
        PhaseB_Sum = waveform->samples[i].phase_B;
        PhaseC_Sum = waveform->samples[i].phase_C;

        // Square sum for RMS calculation
        Current_SquareSum += pow(waveform->samples[i].line_current, 2);
        PhaseA_SquareSum += pow(waveform->samples[i].phase_A, 2);
        PhaseB_SquareSum += pow(waveform->samples[i].phase_B, 2);
        PhaseC_SquareSum += pow(waveform->samples[i].phase_C, 2);


        // Voltage Clipping Checks

        if (is_clipping(waveform->samples[i].phase_A)==1)
        {
            PhaseA_clipping_events +=1;
            waveform->samples[i].clipping_A=1;
        }
        if (is_clipping(waveform->samples[i].phase_B)==1)
        {
            PhaseB_clipping_events +=1;
            waveform->samples[i].clipping_B=1;
        }
        if (is_clipping(waveform->samples[i].phase_C)==1)
        {
            PhaseC_clipping_events +=1;
            waveform->samples[i].clipping_C=1;
        }

        // Voltage Tolerance Checks

        if (is_tolerant(waveform->samples[i].phase_A)==1)
        {
            PhaseA_out_of_tolerance+=1;
            waveform->samples[i].uncompliant_A=1;
        }
        if (is_tolerant(waveform->samples[i].phase_B)==1)
        {
            PhaseB_out_of_tolerance+=1;
            waveform->samples[i].uncompliant_B=1;
        }
        if (is_tolerant(waveform->samples[i].phase_C)==1)
        {
            PhaseC_out_of_tolerance+=1;
            waveform->samples[i].uncompliant_C=1;
        }

        Power_Factor_Sum+=waveform->samples[i].power_factor;
        THD_Sum+=waveform->samples[i].thd_percent;
        THD_Max=fmax(waveform->samples[i].thd_percent,THD_Max);
        Frequency_sum+=waveform->samples[i].frequency;
    }

    waveform->Irms_current=sqrt(Current_SquareSum/size);
    waveform->Vrms_Phase_A=sqrt(PhaseA_SquareSum/size);
    waveform->Vrms_Phase_B=sqrt(PhaseB_SquareSum/size);
    waveform->Vrms_Phase_C=sqrt(PhaseC_SquareSum/size);

    waveform->samples_clipped_phaseA=PhaseA_clipping_events;
    waveform->samples_clipped_phaseB=PhaseB_clipping_events;
    waveform->samples_clipped_phaseC=PhaseC_clipping_events;


    waveform->Voff_Phase_A=PhaseA_Sum/size;
    waveform->Voff_Phase_B=PhaseB_Sum/size;
    waveform->Voff_Phase_C=PhaseC_Sum/size;

    waveform->Vpp_Phase_A=PhaseA_Max-PhaseA_Min;
    waveform->Vpp_Phase_B=PhaseB_Max-PhaseB_Min;
    waveform->Vpp_Phase_C=PhaseC_Max-PhaseC_Min;

    waveform->avg_frequency=Frequency_sum/size;
    waveform->avg_thd_percent=THD_Sum/size;
    waveform->max_thd_percent=THD_Max;

    waveform->avg_power_factor=Power_Factor_Sum/size;

    double PhaseA_ACSum=0; // Sum of AC component, v(i)-Vdc
    double PhaseB_ACSum=0;
    double PhaseC_ACSum=0;

    for (int i=0; i < size; i++)
    {
        PhaseA_ACSum += waveform->samples[i].phase_A - waveform->Voff_Phase_A;
        PhaseB_ACSum += waveform->samples[i].phase_B - waveform->Voff_Phase_B;
        PhaseC_ACSum += waveform->samples[i].phase_C - waveform->Voff_Phase_C;
    }

    waveform->variance_A = pow(PhaseA_ACSum,2) / size;
    waveform->variance_B = pow(PhaseB_ACSum,2) / size;
    waveform->variance_C = pow(PhaseC_ACSum,2) / size;

    waveform->std_deviation_A =  sqrt(waveform->variance_A);
    waveform->std_deviation_B =  sqrt(waveform->variance_B);
    waveform->std_deviation_C =  sqrt(waveform->variance_C);



    return 0;
}




txtFile* write_results(Waveform* waveform,char* filename,char* reportname)
{
    time_t currentTime;
    time(&currentTime);
    char output[TXTFILE_BUFFER_SIZE];
    snprintf(output,TXTFILE_BUFFER_SIZE,
    //printf(
        "-=- WAVEFORM CAPTURE REPORT-=-\n"
        "Date: %s"
        "Waveform File: %s\n"
        "Waveform Size: %i\n"
        "\n"
        "RMS Voltage\n"
        " - Phase A: %.2fVrms\n"
        " - Phase B: %.2fVrms\n"
        " - Phase C: %.2fVrms\n"
        "\n"
        "DC Offset Voltage\n"
        " - Phase A: %.3fV\n"
        " - Phase B: %.3fV\n"
        " - Phase C: %.3fV\n"
        "\n"
        "Peak to Peak Voltage\n"
        " - Phase A: %.2fVpp\n"
        " - Phase B: %.2fVpp\n"
        " - Phase C: %.2fVpp\n"
        "\n"
        "Line Current: %.2fArms\n"
        "Average Power Factor: %.2f%%\n"
        "Average THD: %.2f%%\n"
        "Maximum THD: %.2f%%\n"
        "Average Frequency: %.3fHz\n"
        "\n"
        "Samples Clipped\n"
        " - Phase A: %d samples\n"
        " - Phase B: %d samples\n"
        " - Phase C: %d samples\n"
        "\n"
        "Variance and Standard deviation\n"
        " - Phase A: σ²=%.2f σ=%.2f\n"
        " - Phase B: σ²=%.2f σ=%.2f\n"
        " - Phase C: σ²=%.2f σ=%.2f\n",

        ctime(&currentTime),
        reportname,
        waveform->waveform_size,
        waveform->Vrms_Phase_A,
        waveform->Vrms_Phase_B,
        waveform->Vrms_Phase_C,
        waveform->Voff_Phase_A,
        waveform->Voff_Phase_B,
        waveform->Voff_Phase_C,
        waveform->Vpp_Phase_A,
        waveform->Vpp_Phase_B,
        waveform->Vpp_Phase_C,
        waveform->Irms_current,
        waveform->avg_power_factor,
        waveform->avg_thd_percent,
        waveform->max_thd_percent,
        waveform->avg_frequency,
        waveform->samples_clipped_phaseA,
        waveform->samples_clipped_phaseB,
        waveform->samples_clipped_phaseC,
        waveform->variance_A,
        waveform->std_deviation_A,
        waveform->variance_B,
        waveform->std_deviation_B,
        waveform->variance_C,
        waveform->std_deviation_C
    );

    txtFile* file = malloc(sizeof(txtFile));

    file->fileContents=malloc(sizeof(char)*(strlen(output)+2));
    strcpy(file->fileContents,output);
    file->fileSize=(short)strlen(output);

    file->fileName=malloc(sizeof(char)*(strlen(filename)+2));
    strcpy(file->fileName,filename);
    file->fileNameLength=(short)strlen(filename);

    return file;
}

