#ifndef SENSOR_BMV080_H
#define SENSOR_BMV080_H

#include <ti/drivers/I2C.h>

/* Initialize BMV080 particulate matter sensor.
 * Configures continuous measurement mode via I2C. */
void BMV080_init(I2C_Handle i2c);

/* Read PM1, PM2.5, and PM10 concentrations in ug/m3. */
void BMV080_read(I2C_Handle i2c, float *pm1, float *pm25, float *pm10);

#endif
