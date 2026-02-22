#include "sensor_mq7.h"
#include <math.h>
#include <stdint.h>

/*
 * MQ-7 Carbon Monoxide Sensor Driver
 *
 * The MQ-7 analog output passes through a 10k/26k voltage divider
 * to bring the 5V signal into the CC3220's 0-1.4V ADC range.
 *
 * Divider ratio: 10k / (10k + 26k) = 0.2778
 * Max ADC input at 5V sensor output: 5.0 * 0.2778 = 1.389V (within 1.4V ref)
 *
 * Rs/R0 ratio is converted to ppm using the power curve from
 * the MQ-7 datasheet. R0 must be calibrated in clean air.
 */

#define MQ7_R0          10000.0f    /* Sensor resistance in clean air (calibrate!) */
#define MQ7_RL          10000.0f    /* Load resistor on module */
#define ADC_VREF        1.4f        /* CC3220 ADC reference voltage */
#define DIVIDER_RATIO   (10.0f / 36.0f)  /* 10k / (10k + 26k) voltage divider */
#define MQ7_VCC         5.0f        /* MQ-7 supply voltage */

void MQ7_init(ADC_Handle adc)
{
    /* No special initialization needed for the ADC channel.
     * The MQ-7 heater needs ~60s warmup after power-on. */
    (void)adc;
}

float MQ7_readPPM(ADC_Handle adc)
{
    uint16_t adcRaw = 0;
    int_fast16_t status = ADC_convert(adc, &adcRaw);
    if (status != ADC_STATUS_SUCCESS) return -1.0f;

    /* Convert ADC to actual sensor voltage (pre-divider) */
    float vAdc = (adcRaw / 4095.0f) * ADC_VREF;
    float vSensor = vAdc / DIVIDER_RATIO;

    /* Avoid division by zero */
    if (vSensor < 0.01f) return 0.0f;

    /* Calculate sensor resistance: Rs = RL * (Vc - Vs) / Vs */
    float rs = MQ7_RL * (MQ7_VCC - vSensor) / vSensor;

    /* Guard against negative rs (clipped ADC or divider mismatch) */
    if (rs < 0.0f) return 0.0f;

    /* Rs/R0 ratio to ppm (power curve from datasheet) */
    float ratio = rs / MQ7_R0;
    float ppm = 98.322f * powf(ratio, -1.458f);

    return ppm;
}
