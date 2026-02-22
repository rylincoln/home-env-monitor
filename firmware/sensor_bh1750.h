#ifndef SENSOR_BH1750_H
#define SENSOR_BH1750_H

#include <ti/drivers/I2C.h>
#include <stdint.h>

/* Initialize BH1750 ambient light sensor.
 * Sets continuous high-resolution mode. */
void BH1750_init(I2C_Handle i2c);

/* Read ambient light level in lux. */
void BH1750_read(I2C_Handle i2c, uint16_t *lux);

#endif
