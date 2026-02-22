#include "sensor_bme280.h"
#include "Board.h"
#include <stdint.h>
#include <stdbool.h>

/* BME280 Register Addresses */
#define BME280_REG_ID           0xD0
#define BME280_REG_CTRL_HUM     0xF2
#define BME280_REG_CTRL_MEAS    0xF4
#define BME280_REG_CONFIG       0xF5
#define BME280_REG_DATA_START   0xF7

/* Expected chip ID */
#define BME280_CHIP_ID          0x60

/* Calibration data stored after reading from sensor */
static struct {
    uint16_t dig_T1;
    int16_t  dig_T2, dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2, dig_P3, dig_P4, dig_P5;
    int16_t  dig_P6, dig_P7, dig_P8, dig_P9;
    uint8_t  dig_H1, dig_H3;
    int16_t  dig_H2, dig_H4, dig_H5;
    int8_t   dig_H6;
} cal;

static int32_t t_fine;

static bool i2c_write_reg(I2C_Handle i2c, uint8_t reg, uint8_t val)
{
    uint8_t txBuf[2] = {reg, val};
    I2C_Transaction txn = {0};
    txn.targetAddress = BME280_I2C_ADDR;
    txn.writeBuf = txBuf;
    txn.writeCount = 2;
    txn.readBuf = NULL;
    txn.readCount = 0;
    return I2C_transfer(i2c, &txn);
}

static bool i2c_read_regs(I2C_Handle i2c, uint8_t reg, uint8_t *buf, uint8_t len)
{
    I2C_Transaction txn = {0};
    txn.targetAddress = BME280_I2C_ADDR;
    txn.writeBuf = &reg;
    txn.writeCount = 1;
    txn.readBuf = buf;
    txn.readCount = len;
    return I2C_transfer(i2c, &txn);
}

static void read_calibration(I2C_Handle i2c)
{
    uint8_t buf[26];

    /* Temperature and pressure calibration: 0x88..0xA1 */
    i2c_read_regs(i2c, 0x88, buf, 26);
    cal.dig_T1 = (uint16_t)((uint16_t)buf[1] << 8 | buf[0]);
    cal.dig_T2 = (int16_t)((uint16_t)buf[3] << 8 | buf[2]);
    cal.dig_T3 = (int16_t)((uint16_t)buf[5] << 8 | buf[4]);
    cal.dig_P1 = (uint16_t)((uint16_t)buf[7] << 8 | buf[6]);
    cal.dig_P2 = (int16_t)((uint16_t)buf[9] << 8 | buf[8]);
    cal.dig_P3 = (int16_t)((uint16_t)buf[11] << 8 | buf[10]);
    cal.dig_P4 = (int16_t)((uint16_t)buf[13] << 8 | buf[12]);
    cal.dig_P5 = (int16_t)((uint16_t)buf[15] << 8 | buf[14]);
    cal.dig_P6 = (int16_t)((uint16_t)buf[17] << 8 | buf[16]);
    cal.dig_P7 = (int16_t)((uint16_t)buf[19] << 8 | buf[18]);
    cal.dig_P8 = (int16_t)((uint16_t)buf[21] << 8 | buf[20]);
    cal.dig_P9 = (int16_t)((uint16_t)buf[23] << 8 | buf[22]);

    /* Humidity calibration: 0xA1, then 0xE1..0xE7 */
    i2c_read_regs(i2c, 0xA1, buf, 1);
    cal.dig_H1 = buf[0];

    i2c_read_regs(i2c, 0xE1, buf, 7);
    cal.dig_H2 = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);
    cal.dig_H3 = buf[2];

    /* dig_H4 and dig_H5 are 12-bit signed values packed across 3 bytes.
     * buf[3]=0xE4, buf[4]=0xE5, buf[5]=0xE6.
     * Assemble as unsigned 12-bit then sign-extend. */
    int16_t h4_raw = (int16_t)(((uint16_t)buf[3] << 4) | (buf[4] & 0x0F));
    cal.dig_H4 = (h4_raw & 0x0800) ? (h4_raw | 0xF000) : h4_raw;

    int16_t h5_raw = (int16_t)(((uint16_t)buf[5] << 4) | (buf[4] >> 4));
    cal.dig_H5 = (h5_raw & 0x0800) ? (h5_raw | 0xF000) : h5_raw;

    cal.dig_H6 = (int8_t)buf[6];
}

static float compensate_temperature(int32_t adc_T)
{
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)cal.dig_T1 << 1))) *
                    ((int32_t)cal.dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)cal.dig_T1)) *
                      ((adc_T >> 4) - ((int32_t)cal.dig_T1))) >> 12) *
                    ((int32_t)cal.dig_T3)) >> 14;
    t_fine = var1 + var2;
    return (t_fine * 5 + 128) / 25600.0f;
}

static float compensate_pressure(int32_t adc_P)
{
    int64_t var1 = ((int64_t)t_fine) - 128000;
    int64_t var2 = var1 * var1 * (int64_t)cal.dig_P6;
    var2 = var2 + ((var1 * (int64_t)cal.dig_P5) << 17);
    var2 = var2 + (((int64_t)cal.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)cal.dig_P3) >> 8) +
           ((var1 * (int64_t)cal.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)cal.dig_P1) >> 33;
    if (var1 == 0) return 0;

    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)cal.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)cal.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)cal.dig_P7) << 4);
    return (float)p / 25600.0f;  /* Pa -> hPa */
}

static float compensate_humidity(int32_t adc_H)
{
    int32_t v = t_fine - 76800;
    v = (((((adc_H << 14) - (((int32_t)cal.dig_H4) << 20) -
            (((int32_t)cal.dig_H5) * v)) + 16384) >> 15) *
         (((((((v * ((int32_t)cal.dig_H6)) >> 10) *
              (((v * ((int32_t)cal.dig_H3)) >> 11) + 32768)) >> 10) +
            2097152) * ((int32_t)cal.dig_H2) + 8192) >> 14));
    v = v - (((((v >> 15) * (v >> 15)) >> 7) * ((int32_t)cal.dig_H1)) >> 4);
    v = (v < 0) ? 0 : v;
    v = (v > 419430400) ? 419430400 : v;
    return (float)(v >> 12) / 1024.0f;
}

void BME280_init(I2C_Handle i2c)
{
    read_calibration(i2c);

    /* Humidity oversampling x1 */
    i2c_write_reg(i2c, BME280_REG_CTRL_HUM, 0x01);

    /* Config: standby 500ms, IIR filter coeff 16 */
    i2c_write_reg(i2c, BME280_REG_CONFIG, 0x90);

    /* Ctrl_meas: temp x2, press x16, normal mode */
    i2c_write_reg(i2c, BME280_REG_CTRL_MEAS, 0x57);
}

void BME280_read(I2C_Handle i2c, float *temp, float *hum, float *press)
{
    uint8_t buf[8];
    if (!i2c_read_regs(i2c, BME280_REG_DATA_START, buf, 8)) {
        *temp = 0.0f;
        *hum = 0.0f;
        *press = 0.0f;
        return;
    }

    int32_t adc_P = ((int32_t)buf[0] << 12) | ((int32_t)buf[1] << 4) | (buf[2] >> 4);
    int32_t adc_T = ((int32_t)buf[3] << 12) | ((int32_t)buf[4] << 4) | (buf[5] >> 4);
    int32_t adc_H = ((int32_t)buf[6] << 8) | buf[7];

    /* Temperature must be computed first — sets t_fine for pressure and humidity */
    *temp  = compensate_temperature(adc_T);
    *press = compensate_pressure(adc_P);
    *hum   = compensate_humidity(adc_H);
}
