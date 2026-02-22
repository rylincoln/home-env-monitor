#ifndef BOARD_H
#define BOARD_H

/*
 * CC3220SF LaunchPad Pin Definitions
 *
 * P01 - I2C SCL  -> BME280, SGP30, BH1750, BMV080
 * P02 - I2C SDA  -> BME280, SGP30, BH1750, BMV080
 * P59 - ADC CH2  -> MQ-7 Analog (via 10k/26k voltage divider)
 * P60 - ADC CH3  -> MEMS Microphone AUD output
 * P64 - GPIO OUT -> Buzzer (via 2N2222)
 * 3V3 - 3.3V     -> Sensor VCC
 * 5V  - 5V       -> MQ-7 Heater VCC
 * GND - Ground   -> Common ground
 */

/* I2C Bus */
#define Board_I2C0          0

/* ADC Channels */
#define Board_ADC_CH2       0   /* MQ-7 CO sensor */
#define Board_ADC_CH3       1   /* MEMS microphone */

/* GPIO — indices 0,1 are used by LaunchPad LEDs (D10, D9) in SDK */
#define Board_GPIO_BUZZER   2   /* P64 - buzzer via transistor */

/* I2C Addresses */
#define BME280_I2C_ADDR     0x76
#define SGP30_I2C_ADDR      0x58
#define BH1750_I2C_ADDR     0x23
#define BMV080_I2C_ADDR     0x57

#endif
