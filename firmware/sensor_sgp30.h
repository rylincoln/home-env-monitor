#ifndef SENSOR_SGP30_H
#define SENSOR_SGP30_H

#include <ti/drivers/I2C.h>
#include <stdint.h>

/* Initialize SGP30 air quality sensor.
 * Sends iaq_init command to start measurement mode. */
void SGP30_init(I2C_Handle i2c);

/* Read eCO2 (ppm) and TVOC (ppb) values. */
void SGP30_read(I2C_Handle i2c, uint16_t *eco2, uint16_t *tvoc);

#endif
