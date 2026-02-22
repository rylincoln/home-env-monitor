#include "sensor_bmv080.h"
#include "Board.h"
#include <stdint.h>
#include <unistd.h>

/*
 * BMV080 Particulate Matter Sensor Driver
 *
 * The BMV080 is Bosch's ultra-compact PM sensor (4.4x3.0x3.0mm).
 * It measures PM1, PM2.5, and PM10 via I2C without a fan.
 *
 * This driver uses the simplified register-based interface.
 * For production use, integrate the official Bosch BMV080 API.
 */

/* BMV080 Registers (simplified) */
#define BMV080_REG_CHIP_ID      0x00
#define BMV080_REG_STATUS       0x01
#define BMV080_REG_CTRL         0x10
#define BMV080_REG_PM1_L        0x20
#define BMV080_REG_PM1_H        0x21
#define BMV080_REG_PM25_L       0x22
#define BMV080_REG_PM25_H       0x23
#define BMV080_REG_PM10_L       0x24
#define BMV080_REG_PM10_H       0x25

#define BMV080_CTRL_START       0x01

static void i2c_write_reg(I2C_Handle i2c, uint8_t reg, uint8_t val)
{
    uint8_t txBuf[2] = {reg, val};
    I2C_Transaction txn = {0};
    txn.slaveAddress = BMV080_I2C_ADDR;
    txn.writeBuf = txBuf;
    txn.writeCount = 2;
    txn.readBuf = NULL;
    txn.readCount = 0;
    I2C_transfer(i2c, &txn);
}

static void i2c_read_regs(I2C_Handle i2c, uint8_t reg, uint8_t *buf, uint8_t len)
{
    I2C_Transaction txn = {0};
    txn.slaveAddress = BMV080_I2C_ADDR;
    txn.writeBuf = &reg;
    txn.writeCount = 1;
    txn.readBuf = buf;
    txn.readCount = len;
    I2C_transfer(i2c, &txn);
}

void BMV080_init(I2C_Handle i2c)
{
    /* Start continuous measurement mode */
    i2c_write_reg(i2c, BMV080_REG_CTRL, BMV080_CTRL_START);
    usleep(100000);  /* Allow sensor warmup */
}

void BMV080_read(I2C_Handle i2c, float *pm1, float *pm25, float *pm10)
{
    uint8_t buf[6];
    i2c_read_regs(i2c, BMV080_REG_PM1_L, buf, 6);

    /* Raw 16-bit values, scaled to ug/m3 (scale factor per datasheet) */
    uint16_t raw_pm1  = (uint16_t)(buf[1] << 8 | buf[0]);
    uint16_t raw_pm25 = (uint16_t)(buf[3] << 8 | buf[2]);
    uint16_t raw_pm10 = (uint16_t)(buf[5] << 8 | buf[4]);

    *pm1  = raw_pm1  / 10.0f;
    *pm25 = raw_pm25 / 10.0f;
    *pm10 = raw_pm10 / 10.0f;
}
