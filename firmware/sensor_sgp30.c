#include "sensor_sgp30.h"
#include "Board.h"
#include <unistd.h>

/* SGP30 I2C Commands (2-byte command words) */
#define SGP30_CMD_IAQ_INIT_H    0x20
#define SGP30_CMD_IAQ_INIT_L    0x03
#define SGP30_CMD_MEASURE_H     0x20
#define SGP30_CMD_MEASURE_L     0x08

static void sgp30_send_cmd(I2C_Handle i2c, uint8_t cmd_h, uint8_t cmd_l)
{
    uint8_t txBuf[2] = {cmd_h, cmd_l};
    I2C_Transaction txn = {0};
    txn.slaveAddress = SGP30_I2C_ADDR;
    txn.writeBuf = txBuf;
    txn.writeCount = 2;
    txn.readBuf = NULL;
    txn.readCount = 0;
    I2C_transfer(i2c, &txn);
}

static void sgp30_read_data(I2C_Handle i2c, uint8_t *buf, uint8_t len)
{
    I2C_Transaction txn = {0};
    txn.slaveAddress = SGP30_I2C_ADDR;
    txn.writeBuf = NULL;
    txn.writeCount = 0;
    txn.readBuf = buf;
    txn.readCount = len;
    I2C_transfer(i2c, &txn);
}

void SGP30_init(I2C_Handle i2c)
{
    /* Send iaq_init to start continuous measurement mode.
     * The sensor needs ~15s to produce first valid readings. */
    sgp30_send_cmd(i2c, SGP30_CMD_IAQ_INIT_H, SGP30_CMD_IAQ_INIT_L);
    usleep(10000);  /* 10ms startup */
}

void SGP30_read(I2C_Handle i2c, uint16_t *eco2, uint16_t *tvoc)
{
    /* Send measure_iaq command */
    sgp30_send_cmd(i2c, SGP30_CMD_MEASURE_H, SGP30_CMD_MEASURE_L);

    /* Wait 12ms for measurement to complete */
    usleep(12000);

    /* Read 6 bytes: [CO2_H, CO2_L, CRC, TVOC_H, TVOC_L, CRC] */
    uint8_t buf[6];
    sgp30_read_data(i2c, buf, 6);

    *eco2 = (uint16_t)(buf[0] << 8 | buf[1]);
    *tvoc = (uint16_t)(buf[3] << 8 | buf[4]);
}
