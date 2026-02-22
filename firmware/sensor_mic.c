#include "sensor_mic.h"
#include <math.h>
#include <stdint.h>

/*
 * MEMS Microphone Driver (SPH8878LR5H-1)
 *
 * The breakout board includes an op-amp that brings the mic output
 * to a usable level (~200mV peak-to-peak for normal speech).
 * We sample a window of ADC readings into a buffer, compute RMS
 * voltage over the same data set, and convert to approximate dB SPL.
 */

#define ADC_VREF        1.4f        /* CC3220 ADC reference voltage */
#define ADC_MAX         4095.0f     /* 12-bit ADC */
#define MIC_SAMPLES     256         /* Samples per measurement window */
#define MIC_REF_VRMS    0.00631f    /* Reference voltage for 0 dB (calibrate) */
#define MIC_GAIN_DB     20.0f       /* Op-amp gain offset on breakout board */

void MIC_init(ADC_Handle adc)
{
    (void)adc;
}

float MIC_readDB(ADC_Handle adc)
{
    /* Capture all samples into a single buffer so DC offset and RMS
     * are computed over the exact same data set. */
    uint16_t samples[MIC_SAMPLES];
    float dc_offset = 0.0f;

    for (int i = 0; i < MIC_SAMPLES; i++) {
        samples[i] = 0;
        if (ADC_convert(adc, &samples[i]) != ADC_STATUS_SUCCESS) {
            samples[i] = 0;
        }
        dc_offset += (samples[i] / ADC_MAX) * ADC_VREF;
    }
    dc_offset /= MIC_SAMPLES;

    /* Compute RMS of AC component over the same samples */
    float sum_sq = 0.0f;
    for (int i = 0; i < MIC_SAMPLES; i++) {
        float v = (samples[i] / ADC_MAX) * ADC_VREF;
        float ac = v - dc_offset;
        sum_sq += ac * ac;
    }

    float vrms = sqrtf(sum_sq / MIC_SAMPLES);

    /* Convert to dB SPL (approximate) */
    if (vrms < 0.0001f) return 0.0f;
    float db = 20.0f * log10f(vrms / MIC_REF_VRMS) + MIC_GAIN_DB;

    return (db < 0.0f) ? 0.0f : db;
}
