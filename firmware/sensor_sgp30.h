#ifndef SENSOR_SGP30_H
#define SENSOR_SGP30_H

#include <ti/drivers/I2C.h>
#include <stdint.h>
#include <stdbool.h>

/* Initialize SGP30 air quality sensor.
 * Sends iaq_init command to start measurement mode. */
void SGP30_init(I2C_Handle i2c);

/* Must be called every 1 second to maintain the SGP30's on-chip
 * baseline compensation algorithm. Sends measure_iaq and caches
 * the latest eCO2/TVOC values internally. */
bool SGP30_tick(I2C_Handle i2c);

/* Return the most recently cached eCO2 (ppm) and TVOC (ppb) values
 * from the last SGP30_tick() call. */
void SGP30_read(uint16_t *eco2, uint16_t *tvoc);

#endif
