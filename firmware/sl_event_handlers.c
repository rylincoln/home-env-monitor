/*
 * sl_event_handlers.c - SimpleLink Wi-Fi event handler stubs
 *
 * The SimpleLink host driver requires the application to define these
 * callbacks. For this project, we use minimal stubs — expand as needed
 * for production error handling.
 */

#include <ti/drivers/net/wifi/simplelink.h>

void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    (void)pWlanEvent;
}

void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    (void)pNetAppEvent;
}

void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    (void)pDevEvent;
}

void SimpleLinkFatalErrorEventHandler(SlDeviceFatal_t *slFatalErrorEvent)
{
    (void)slFatalErrorEvent;
    /* In production, log error and reset device */
    while (1) {}
}

void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    (void)pSock;
}

void SimpleLinkHttpServerEventHandler(SlNetAppHttpServerEvent_t *pHttpEvent,
                                       SlNetAppHttpServerResponse_t *pHttpResponse)
{
    (void)pHttpEvent;
    (void)pHttpResponse;
}

void SimpleLinkNetAppRequestEventHandler(SlNetAppRequest_t *pNetAppRequest,
                                          SlNetAppResponse_t *pNetAppResponse)
{
    (void)pNetAppRequest;
    (void)pNetAppResponse;
}

void SimpleLinkNetAppRequestMemFreeEventHandler(uint8_t *buffer)
{
    (void)buffer;
}
