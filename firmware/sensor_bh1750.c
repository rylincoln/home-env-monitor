#include "sensor_bh1750.h"
#include "Board.h"
#include <unistd.h>
#include <stdbool.h>

/* BH1750 Commands */
#define BH1750_POWER_ON             0x01
#define BH1750_CONTINUOUS_HIGH_RES  0x10  /* 1 lux resolution, typ 120ms / max 180ms */

static bool bh1750_send_cmd(I2C_Handle i2c, uint8_t cmd)
{
    I2C_Transaction txn = {0};
    txn.slaveAddress = BH1750_I2C_ADDR;
    txn.writeBuf = &cmd;
    txn.writeCount = 1;
    txn.readBuf = NULL;
    txn.readCount = 0;
    return I2C_transfer(i2c, &txn);
}

void BH1750_init(I2C_Handle i2c)
{
    bh1750_send_cmd(i2c, BH1750_POWER_ON);
    usleep(10000);
    bh1750_send_cmd(i2c, BH1750_CONTINUOUS_HIGH_RES);
    usleep(180000);  /* First measurement takes up to 180ms */
}

void BH1750_read(I2C_Handle i2c, uint16_t *lux)
{
    uint8_t buf[2] = {0, 0};
    I2C_Transaction txn = {0};
    txn.slaveAddress = BH1750_I2C_ADDR;
    txn.writeBuf = NULL;
    txn.writeCount = 0;
    txn.readBuf = buf;
    txn.readCount = 2;

    if (!I2C_transfer(i2c, &txn)) {
        *lux = 0;
        return;
    }

    /* Raw value / 1.2 = lux (per datasheet) */
    uint16_t raw = (uint16_t)((uint16_t)buf[0] << 8 | buf[1]);
    *lux = (uint16_t)((uint32_t)raw * 5 / 6);  /* Integer equivalent of raw/1.2 */
}
