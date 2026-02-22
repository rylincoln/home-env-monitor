#ifndef CONFIG_H
#define CONFIG_H

/* Wi-Fi */
#define WIFI_SSID         "YourNetworkName"
#define WIFI_PASS         "YourPassword"

/* MQTT Broker (Mosquitto on Raspberry Pi) */
#define MQTT_BROKER       "192.168.1.100"   /* Pi's static IP */
#define MQTT_PORT         1883              /* Local, no TLS needed on LAN */
#define MQTT_CLIENT_ID    "env_monitor_01"
#define MQTT_USER         "monitor"         /* Optional auth */
#define MQTT_PASS         "yourpassword"

/* MQTT Topic */
#define MQTT_TOPIC        "home/env"

/* Timing */
#define READ_INTERVAL_MS  30000

/* CO Alarm Thresholds */
#define CO_ALARM_PPM      50
#define CO_CLEAR_PPM      25

#endif
