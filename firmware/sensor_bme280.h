#ifndef SENSOR_BME280_H
#define SENSOR_BME280_H

#include <ti/drivers/I2C.h>

/* Initialize BME280 sensor on I2C bus.
 * Configures oversampling and filter settings. */
void BME280_init(I2C_Handle i2c);

/* Read temperature (C), humidity (%RH), and pressure (hPa). */
void BME280_read(I2C_Handle i2c, float *temp, float *hum, float *press);

#endif
