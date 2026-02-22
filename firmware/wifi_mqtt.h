#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include <stdint.h>
#include <stdbool.h>

/* Connect to Wi-Fi access point.
 * Returns true on success, false if connection failed after retries. */
bool WiFi_connect(const char *ssid, const char *password);

/* Connect to MQTT broker.
 * Returns true on success, false if connection failed after retries. */
bool MQTT_connect(const char *broker, uint16_t port, const char *client_id);

/* Publish a message to an MQTT topic.
 * Returns true on success, false on failure. */
bool MQTT_publish(const char *topic, const char *payload);

/* Attempt to reconnect to the MQTT broker using previously stored params.
 * Call this after MQTT_publish returns false.
 * Returns true on success, false if reconnect failed. */
bool MQTT_reconnect(void);

/* Disconnect from MQTT broker and Wi-Fi. */
void MQTT_disconnect(void);

#endif
