#include "sensor_bmv080.h"
#include "Board.h"
#include <stdint.h>

/*
 * BMV080 Particulate Matter Sensor Driver — STUB
 *
 * TODO: The BMV080 does NOT expose a public I2C register map for PM data.
 * Bosch ships this sensor with a proprietary pre-compiled SDK that contains
 * the particle detection algorithms. Raw register reads will return garbage.
 *
 * To get working PM readings, integrate the official Bosch BMV080 SDK:
 *   https://www.bosch-sensortec.com/software-tools/software/previous-sdk-bmv-080-versions/
 *
 * The SDK requires:
 *   1. I2C read/write callback shims wrapping TI I2C_transfer()
 *   2. bmv080_init() with the I2C interface struct
 *   3. bmv080_serve_interrupt() called at least 1 Hz
 *   4. bmv080_get_data() to retrieve PM1/PM2.5/PM10 values
 *
 * Until the SDK is integrated, this driver returns -1.0 sentinel values
 * so downstream code can detect that PM data is unavailable.
 */

void BMV080_init(I2C_Handle i2c)
{
    (void)i2c;
    /* Stub — no hardware initialization without Bosch SDK */
}

void BMV080_read(I2C_Handle i2c, float *pm1, float *pm25, float *pm10)
{
    (void)i2c;
    /* Return sentinel values indicating PM data is unavailable */
    *pm1  = -1.0f;
    *pm25 = -1.0f;
    *pm10 = -1.0f;
}
