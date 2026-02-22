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

void WiFi_connect(const char *ssid, const char *password)
{
    SlWlanSecParams_t secParams;
    secParams.Type = SL_WLAN_SEC_TYPE_WPA_WPA2;
    secParams.Key = (signed char *)password;
    secParams.KeyLen = strlen(password);

    /* Set device to station mode */
    sl_Start(NULL, NULL, NULL);
    sl_WlanSetMode(ROLE_STA);
    sl_Stop(200);
    sl_Start(NULL, NULL, NULL);

    /* Connect to AP */
    sl_WlanConnect((signed char *)ssid, strlen(ssid), NULL, &secParams, NULL);

    /* Wait for IP acquired (simplified - production code should
     * use event callbacks from the SimpleLink event handlers) */
    int retries = 30;
    while (retries-- > 0) {
        uint16_t len = 0;
        uint8_t  dhcpIsOn = 0;
        sl_NetCfgGet(SL_NETCFG_IPV4_STA_ADDR_MODE, &dhcpIsOn, &len, NULL);
        if (dhcpIsOn) break;
        sleep(1);
    }
}

void MQTT_connect(const char *broker, uint16_t port, const char *client_id)
{
    MQTTClient_ConnParams connParams = MQTTClient_defaultConnParams;
    connParams.serverAddr = broker;
    connParams.port = port;

    MQTTClient_Params mqttParams;
    mqttParams.clientId = client_id;
    mqttParams.connParams = &connParams;

    mqttClient = MQTTClient_create(NULL, &mqttParams);

    /* Set optional username/password */
    MQTTClient_set(mqttClient, MQTTClient_USER_NAME,
                   MQTT_USER, strlen(MQTT_USER));
    MQTTClient_set(mqttClient, MQTTClient_PASSWORD,
                   MQTT_PASS, strlen(MQTT_PASS));

    /* Connect (retry on failure) */
    int retries = 5;
    while (retries-- > 0) {
        if (MQTTClient_connect(mqttClient) == 0) break;
        sleep(2);
    }
}

void MQTT_publish(const char *topic, const char *payload)
{
    MQTTClient_publish(mqttClient,
                       (char *)topic, strlen(topic),
                       (char *)payload, strlen(payload),
                       MQTTCLIENT_QOS_0);
}

void MQTT_disconnect(void)
{
    MQTTClient_disconnect(mqttClient);
    MQTTClient_delete(mqttClient);
    sl_Stop(200);
}
