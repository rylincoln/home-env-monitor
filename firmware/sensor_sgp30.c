#include "sensor_sgp30.h"
#include "Board.h"
#include <unistd.h>

/* SGP30 I2C Commands (2-byte command words) */
#define SGP30_CMD_IAQ_INIT_H    0x20
#define SGP30_CMD_IAQ_INIT_L    0x03
#define SGP30_CMD_MEASURE_H     0x20
#define SGP30_CMD_MEASURE_L     0x08

/* Cached values from the most recent tick */
static uint16_t cached_eco2 = 400;  /* SGP30 default */
static uint16_t cached_tvoc = 0;

static bool sgp30_send_cmd(I2C_Handle i2c, uint8_t cmd_h, uint8_t cmd_l)
{
    uint8_t txBuf[2] = {cmd_h, cmd_l};
    I2C_Transaction txn = {0};
    txn.slaveAddress = SGP30_I2C_ADDR;
    txn.writeBuf = txBuf;
    txn.writeCount = 2;
    txn.readBuf = NULL;
    txn.readCount = 0;
    return I2C_transfer(i2c, &txn);
}

static bool sgp30_read_data(I2C_Handle i2c, uint8_t *buf, uint8_t len)
{
    I2C_Transaction txn = {0};
    txn.slaveAddress = SGP30_I2C_ADDR;
    txn.writeBuf = NULL;
    txn.writeCount = 0;
    txn.readBuf = buf;
    txn.readCount = len;
    return I2C_transfer(i2c, &txn);
}

/* CRC-8 per SGP30 datasheet: polynomial 0x31, init 0xFF */
static uint8_t sgp30_crc(const uint8_t *data, uint8_t len)
{
    uint8_t crc = 0xFF;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t b = 0; b < 8; b++) {
            crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0x31)
                               : (uint8_t)(crc << 1);
        }
    }
    return crc;
}

void SGP30_init(I2C_Handle i2c)
{
    /* Send iaq_init to start continuous measurement mode.
     * The sensor needs ~15s to produce first valid readings. */
    sgp30_send_cmd(i2c, SGP30_CMD_IAQ_INIT_H, SGP30_CMD_IAQ_INIT_L);
    usleep(10000);  /* 10ms for iaq_init to complete */
}

bool SGP30_tick(I2C_Handle i2c)
{
    /* Send measure_iaq command */
    if (!sgp30_send_cmd(i2c, SGP30_CMD_MEASURE_H, SGP30_CMD_MEASURE_L)) {
        return false;
    }

    /* Wait 13ms: 12ms max conversion + 1ms margin for RTOS tick */
    usleep(13000);

    /* Read 6 bytes: [CO2_H, CO2_L, CRC, TVOC_H, TVOC_L, CRC] */
    uint8_t buf[6];
    if (!sgp30_read_data(i2c, buf, 6)) {
        return false;
    }

    /* Validate CRC for each 2-byte word */
    if (sgp30_crc(&buf[0], 2) != buf[2]) return false;
    if (sgp30_crc(&buf[3], 2) != buf[5]) return false;

    cached_eco2 = (uint16_t)((uint16_t)buf[0] << 8 | buf[1]);
    cached_tvoc = (uint16_t)((uint16_t)buf[3] << 8 | buf[4]);
    return true;
}

void SGP30_read(uint16_t *eco2, uint16_t *tvoc)
{
    *eco2 = cached_eco2;
    *tvoc = cached_tvoc;
}
