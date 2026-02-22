#include "wifi_mqtt.h"
#include "config.h"

#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/net/mqtt/mqttclient.h>
#include <string.h>
#include <unistd.h>

/*
 * Wi-Fi + MQTT connectivity for CC3220SF
 *
 * Based on the TI SimpleLink mqtt_client example project.
 * Import that example in Code Composer Studio for the full
 * Wi-Fi provisioning and TLS infrastructure.
 *
 * This file provides the simplified interface used by main.c.
 */

static MQTTClient_Handle mqttClient;

/* Store connection params for reconnect */
static const char *stored_broker;
static uint16_t stored_port;
static const char *stored_client_id;

bool WiFi_connect(const char *ssid, const char *password)
{
    SlWlanSecParams_t secParams;
    secParams.Type = SL_WLAN_SEC_TYPE_WPA_WPA2;
    secParams.Key = (signed char *)password;
    secParams.KeyLen = strlen(password);

    /* Set device to station mode */
    int16_t role = sl_Start(NULL, NULL, NULL);
    if (role < 0) return false;

    if (role != ROLE_STA) {
        sl_WlanSetMode(ROLE_STA);
        sl_Stop(200);
        if (sl_Start(NULL, NULL, NULL) < 0) return false;
    }

    /* Connect to AP */
    int16_t ret = sl_WlanConnect((signed char *)ssid, strlen(ssid),
                                 NULL, &secParams, NULL);
    if (ret < 0) return false;

    /* Wait for IP acquired using proper buffer */
    int retries = 30;
    while (retries-- > 0) {
        SlNetCfgIpV4Args_t ipV4 = {0};
        uint16_t len = sizeof(ipV4);
        uint8_t  dhcpIsOn = 0;
        int16_t status = sl_NetCfgGet(SL_NETCFG_IPV4_STA_ADDR_MODE,
                                       &dhcpIsOn, &len, (uint8_t *)&ipV4);
        if (status >= 0 && ipV4.Ip != 0) return true;
        sleep(1);
    }

    return false;  /* Timed out waiting for IP */
}

bool MQTT_connect(const char *broker, uint16_t port, const char *client_id)
{
    /* Store for reconnect */
    stored_broker = broker;
    stored_port = port;
    stored_client_id = client_id;

    MQTTClient_ConnParams connParams = MQTTClient_defaultConnParams;
    connParams.serverAddr = broker;
    connParams.port = port;

    MQTTClient_Params mqttParams;
    mqttParams.clientId = client_id;
    mqttParams.connParams = &connParams;

    mqttClient = MQTTClient_create(NULL, &mqttParams);
    if (mqttClient == NULL) return false;

    /* Set optional username/password */
    MQTTClient_set(mqttClient, MQTTClient_USER_NAME,
                   MQTT_USER, strlen(MQTT_USER));
    MQTTClient_set(mqttClient, MQTTClient_PASSWORD,
                   MQTT_PASS, strlen(MQTT_PASS));

    /* Connect (retry on failure) */
    int retries = 5;
    while (retries-- > 0) {
        if (MQTTClient_connect(mqttClient) == 0) return true;
        sleep(2);
    }

    /* All retries failed — clean up */
    MQTTClient_delete(mqttClient);
    mqttClient = NULL;
    return false;
}

bool MQTT_publish(const char *topic, const char *payload)
{
    if (mqttClient == NULL) return false;

    int ret = MQTTClient_publish(mqttClient,
                                  (char *)topic, strlen(topic),
                                  (char *)payload, strlen(payload),
                                  MQTTCLIENT_QOS_0);
    return (ret == 0);
}

bool MQTT_reconnect(void)
{
    if (mqttClient != NULL) {
        MQTTClient_disconnect(mqttClient);
        MQTTClient_delete(mqttClient);
        mqttClient = NULL;
    }
    return MQTT_connect(stored_broker, stored_port, stored_client_id);
}

void MQTT_disconnect(void)
{
    if (mqttClient != NULL) {
        MQTTClient_disconnect(mqttClient);
        MQTTClient_delete(mqttClient);
        mqttClient = NULL;
    }
    sl_Stop(200);
}
