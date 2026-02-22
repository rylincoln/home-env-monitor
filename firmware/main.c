/*
 * Home Environment Monitor - Main Application
 *
 * CC3220SF SimpleLink SDK
 *
 * Reads all sensors every 30 seconds, checks CO thresholds,
 * and publishes JSON data to a local MQTT broker.
 */

#include <ti/drivers/I2C.h>
#include <ti/drivers/ADC.h>
#include <ti/drivers/GPIO.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "config.h"
#include "Board.h"
#include "sensor_bme280.h"
#include "sensor_sgp30.h"
#include "sensor_bh1750.h"
#include "sensor_bmv080.h"
#include "sensor_mq7.h"
#include "sensor_mic.h"
#include "co_alarm.h"
#include "wifi_mqtt.h"

typedef struct {
    float    temperature;   /* C */
    float    humidity;      /* %RH */
    float    pressure;      /* hPa */
    uint16_t eco2;          /* ppm */
    uint16_t tvoc;          /* ppb */
    float    co_ppm;        /* ppm (MQ-7) */
    uint16_t lux;           /* lux */
    float    pm1;           /* ug/m3 (BMV080) */
    float    pm25;          /* ug/m3 (BMV080) */
    float    pm10;          /* ug/m3 (BMV080) */
    float    noise_db;      /* dB (MEMS mic) */
    bool     co_alarm;      /* true if CO above threshold */
} EnvData_t;

void mainThread(void *arg0)
{
    (void)arg0;

    /* Initialize drivers */
    I2C_Handle i2c     = I2C_open(Board_I2C0, NULL);
    ADC_Handle adc_co  = ADC_open(Board_ADC_CH2, NULL);
    ADC_Handle adc_mic = ADC_open(Board_ADC_CH3, NULL);
    GPIO_init();

    /* Initialize sensors */
    BME280_init(i2c);
    SGP30_init(i2c);
    BH1750_init(i2c);
    BMV080_init(i2c);
    MQ7_init(adc_co);
    MIC_init(adc_mic);
    COAlarm_init(CO_ALARM_PPM, CO_CLEAR_PPM);

    /* Connect to Wi-Fi & MQTT broker */
    WiFi_connect(WIFI_SSID, WIFI_PASS);
    MQTT_connect(MQTT_BROKER, MQTT_PORT, MQTT_CLIENT_ID);

    EnvData_t data;

    while (1) {
        /* --- Read all sensors --- */
        BME280_read(i2c, &data.temperature,
                    &data.humidity, &data.pressure);
        SGP30_read(i2c, &data.eco2, &data.tvoc);
        BH1750_read(i2c, &data.lux);
        BMV080_read(i2c, &data.pm1, &data.pm25, &data.pm10);
        data.co_ppm   = MQ7_readPPM(adc_co);
        data.noise_db = MIC_readDB(adc_mic);

        /* --- CO safety check (with hysteresis) --- */
        data.co_alarm = COAlarm_check(data.co_ppm);

        /* --- Build JSON payload --- */
        char payload[512];
        snprintf(payload, sizeof(payload),
            "{"
            "\"temp\":%.1f,"
            "\"hum\":%.1f,"
            "\"press\":%.1f,"
            "\"eco2\":%u,"
            "\"tvoc\":%u,"
            "\"co_ppm\":%.1f,"
            "\"lux\":%u,"
            "\"pm1\":%.1f,"
            "\"pm25\":%.1f,"
            "\"pm10\":%.1f,"
            "\"noise_db\":%.1f,"
            "\"co_alert\":%s"
            "}",
            data.temperature, data.humidity,
            data.pressure, data.eco2, data.tvoc,
            data.co_ppm, data.lux,
            data.pm1, data.pm25, data.pm10,
            data.noise_db,
            data.co_alarm ? "true" : "false");

        /* --- Publish --- */
        MQTT_publish(MQTT_TOPIC, payload);

        sleep(READ_INTERVAL_MS / 1000);
    }
}
