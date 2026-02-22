#ifndef SENSOR_MQ7_H
#define SENSOR_MQ7_H

#include <ti/drivers/ADC.h>

/* Initialize MQ-7 CO sensor ADC channel. */
void MQ7_init(ADC_Handle adc);

/* Read CO concentration in ppm from ADC via voltage divider.
 * Uses the MQ-7 sensitivity curve for Rs/R0 -> ppm conversion. */
float MQ7_readPPM(ADC_Handle adc);

#endif
