#include "sensor_mq7.h"
#include <math.h>
#include <stdint.h>

/*
 * MQ-7 Carbon Monoxide Sensor Driver
 *
 * The MQ-7 analog output passes through a 10k/20k voltage divider
 * to bring the 5V signal into the CC3220's 0-1.4V ADC range.
 *
 * Rs/R0 ratio is converted to ppm using the power curve from
 * the MQ-7 datasheet. R0 must be calibrated in clean air.
 */

#define MQ7_R0          10000.0f    /* Sensor resistance in clean air (calibrate!) */
#define MQ7_RL          10000.0f    /* Load resistor on module */
#define ADC_VREF        1.4f        /* CC3220 ADC reference voltage */
#define DIVIDER_RATIO   0.333f      /* 10k/(10k+20k) voltage divider */

void MQ7_init(ADC_Handle adc)
{
    /* No special initialization needed for the ADC channel.
     * The MQ-7 heater needs ~60s warmup after power-on. */
    (void)adc;
}

float MQ7_readPPM(ADC_Handle adc)
{
    uint16_t adcRaw;
    ADC_convert(adc, &adcRaw);

    /* Convert ADC to actual sensor voltage */
    float vAdc = (adcRaw / 4095.0f) * ADC_VREF;
    float vSensor = vAdc / DIVIDER_RATIO;

    /* Avoid division by zero */
    if (vSensor < 0.01f) return 0.0f;

    /* Calculate sensor resistance */
    float rs = MQ7_RL * (5.0f - vSensor) / vSensor;

    /* Rs/R0 ratio to ppm (power curve from datasheet) */
    float ratio = rs / MQ7_R0;
    float ppm = 98.322f * powf(ratio, -1.458f);

    return ppm;
}
