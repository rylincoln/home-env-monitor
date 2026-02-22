#ifndef SENSOR_MIC_H
#define SENSOR_MIC_H

#include <ti/drivers/ADC.h>

/* Initialize MEMS microphone ADC channel. */
void MIC_init(ADC_Handle adc);

/* Read ambient noise level in dB SPL.
 * Samples the MEMS mic output over a short window
 * and converts RMS voltage to decibels. */
float MIC_readDB(ADC_Handle adc);

#endif
