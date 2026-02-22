#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include <stdint.h>

/* Connect to Wi-Fi access point.
 * Blocks until connected or fails after retries. */
void WiFi_connect(const char *ssid, const char *password);

/* Connect to MQTT broker.
 * Blocks until connected. */
void MQTT_connect(const char *broker, uint16_t port, const char *client_id);

/* Publish a message to an MQTT topic. */
void MQTT_publish(const char *topic, const char *payload);

/* Disconnect from MQTT broker and Wi-Fi. */
void MQTT_disconnect(void);

#endif
