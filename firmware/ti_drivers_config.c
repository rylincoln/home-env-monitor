/*
 * ti_drivers_config.c - Manual driver configuration for CC3220SF
 *
 * This file replaces the SysConfig-generated configuration.
 * It defines the hardware config tables for I2C, ADC, and GPIO
 * used by the TI SimpleLink SDK drivers.
 */

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOCC32XX.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC32XX.h>
#include <ti/drivers/ADC.h>
#include <ti/drivers/adc/ADCCC32XX.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPICC32XXDMA.h>
#include <ti/drivers/power/PowerCC32XX.h>
#include <ti/devices/cc32xx/inc/hw_memmap.h>
#include <ti/devices/cc32xx/inc/hw_ints.h>
#include <ti/devices/cc32xx/driverlib/adc.h>
#include <ti/devices/cc32xx/driverlib/udma.h>
#include <ti/devices/cc32xx/driverlib/prcm.h>
#include <ti/devices/cc32xx/driverlib/spi.h>
#include <ti/drivers/dma/UDMACC32XX.h>

#include <FreeRTOS.h>
#include <queue.h>

#include "ti_drivers_config.h"

/*
 * ======== GPIO ========
 *
 * Index 0: LED D10 (GPIO 9)  - OUTPUT
 * Index 1: LED D9  (GPIO 10) - OUTPUT
 * Index 2: Buzzer  (P64)     - OUTPUT, via 2N2222 transistor
 */
GPIO_PinConfig gpioPinConfigs[CONFIG_GPIO_COUNT] = {
    GPIO_CFG_OUTPUT | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW,  /* LED D10 */
    GPIO_CFG_OUTPUT | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW,  /* LED D9  */
    GPIO_CFG_OUTPUT | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW,  /* Buzzer  */
};

GPIO_CallbackFxn gpioCallbackFunctions[CONFIG_GPIO_COUNT] = {
    NULL, NULL, NULL,
};

const GPIO_Config GPIO_config = {
    .configs   = (GPIO_PinConfig *)gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *)gpioCallbackFunctions,
    .intPriority = (~0),
};

/*
 * ======== I2C ========
 *
 * I2C0: SCL=P01, SDA=P02
 * Devices: BME280 (0x76), SGP30 (0x58), BH1750 (0x23), BMV080 (0x57)
 */
const I2CCC32XX_HWAttrsV1 i2cCC32XXHWAttrs[1] = {
    {
        .baseAddr = I2CA0_BASE,
        .intNum   = INT_I2CA0,
        .intPriority = (~0),
        .sclTimeout  = 0,
        .clkPin  = I2CCC32XX_PIN_01_I2C_SCL,
        .dataPin = I2CCC32XX_PIN_02_I2C_SDA,
    },
};

I2CCC32XX_Object i2cCC32XXObjects[1];

const I2C_Config I2C_config[1] = {
    {
        .object  = &i2cCC32XXObjects[0],
        .hwAttrs = &i2cCC32XXHWAttrs[0],
    },
};

const uint_least8_t I2C_count = 1;

/*
 * ======== ADC ========
 *
 * Channel 0: P59 / ADC CH2 - MQ-7 CO sensor
 * Channel 1: P60 / ADC CH3 - MEMS microphone
 */
const ADCCC32XX_HWAttrsV1 adcCC32XXHWAttrs[2] = {
    { .adcPin = ADCCC32XX_PIN_59_CH_2 },
    { .adcPin = ADCCC32XX_PIN_60_CH_3 },
};

ADCCC32XX_Object adcCC32XXObjects[2];

const ADC_Config ADC_config[2] = {
    {
        .fxnTablePtr = &ADCCC32XX_fxnTable,
        .object  = &adcCC32XXObjects[0],
        .hwAttrs = &adcCC32XXHWAttrs[0],
    },
    {
        .fxnTablePtr = &ADCCC32XX_fxnTable,
        .object  = &adcCC32XXObjects[1],
        .hwAttrs = &adcCC32XXHWAttrs[1],
    },
};

const uint_least8_t ADC_count = 2;

/*
 * ======== GPIO upper bound (used by GPIOCC32XX driver) ========
 */
const uint_least8_t GPIO_pinUpperBound = CONFIG_GPIO_COUNT - 1;

/*
 * ======== UDMA (required by SPI DMA driver) ========
 */
static tDMAControlTable dmaControlTable[64] __attribute__((aligned(1024)));

UDMACC32XX_Object udmaCC32XXObject;

const UDMACC32XX_HWAttrs udmaCC32XXHWAttrs = {
    .controlBaseAddr = (void *)dmaControlTable,
    .dmaErrorFxn     = NULL,
    .intNum          = INT_UDMAERR,
    .intPriority     = (~0),
};

const UDMACC32XX_Config UDMACC32XX_config = {
    .object  = &udmaCC32XXObject,
    .hwAttrs = &udmaCC32XXHWAttrs,
};

/*
 * ======== SPI (NWP host interface) ========
 *
 * The SimpleLink Wi-Fi NWP uses the LS-SPI (Low-Speed SPI) internally.
 * One SPI instance must be configured for the host driver.
 */
static uint32_t spiCC32XXDMAscratchBuf[1];
static SPICC32XXDMA_Object spiCC32XXDMAObjects[1];

static const SPICC32XXDMA_HWAttrsV1 spiCC32XXDMAHWAttrs[1] = {
    {
        .baseAddr           = LSPI_BASE,
        .intNum             = INT_LSPI,
        .intPriority        = (~0),
        .spiPRCM            = PRCM_LSPI,
        .csControl          = SPI_SW_CTRL_CS,
        .csPolarity         = SPI_CS_ACTIVEHIGH,
        .pinMode            = SPI_4PIN_MODE,
        .turboMode          = SPI_TURBO_OFF,
        .scratchBufPtr      = &spiCC32XXDMAscratchBuf[0],
        .defaultTxBufValue  = 0,
        .rxChannelIndex     = UDMA_CH12_LSPI_RX,
        .txChannelIndex     = UDMA_CH13_LSPI_TX,
        .minDmaTransferSize = 100,
        .picoPin            = SPICC32XXDMA_PIN_NO_CONFIG,
        .pociPin            = SPICC32XXDMA_PIN_NO_CONFIG,
        .clkPin             = SPICC32XXDMA_PIN_NO_CONFIG,
        .csnPin             = SPICC32XXDMA_PIN_NO_CONFIG,
    },
};

const SPI_Config SPI_config[1] = {
    {
        .fxnTablePtr = &SPICC32XXDMA_fxnTable,
        .object  = &spiCC32XXDMAObjects[0],
        .hwAttrs = &spiCC32XXDMAHWAttrs[0],
    },
};

const uint_least8_t SPI_count = 1;

/*
 * ======== Power ========
 */
const PowerCC32XX_ConfigV1 PowerCC32XX_config = {
    .policyInitFxn             = NULL,
    .policyFxn                 = NULL,
    .enterLPDSHookFxn          = NULL,
    .resumeLPDSHookFxn         = NULL,
    .enablePolicy              = false,
    .enableGPIOWakeupLPDS      = false,
    .enableGPIOWakeupShutdown  = false,
    .enableNetworkWakeupLPDS   = false,
    .wakeupGPIOSourceLPDS      = 0,
    .wakeupGPIOTypeLPDS        = 0,
    .wakeupGPIOFxnLPDS         = NULL,
    .wakeupGPIOFxnLPDSArg      = 0,
    .wakeupGPIOSourceShutdown  = 0,
    .wakeupGPIOTypeShutdown    = 0,
    .ramRetentionMaskLPDS      = 0,
    .ioRetentionShutdown       = 0,
    .pinParkDefs               = NULL,
    .numPins                   = 0,
    .keepDebugActiveDuringLPDS = false,
    .latencyForLPDS            = 20000,
};

/*
 * ======== FreeRTOS queue registry wrappers (normally from SysConfig) ========
 */
void vQueueAddToRegistryWrapper(QueueHandle_t xQueue, const char *pcQueueName)
{
    (void)xQueue;
    (void)pcQueueName;
}

void vQueueUnregisterQueueWrapper(QueueHandle_t xQueue)
{
    (void)xQueue;
}

/*
 * ======== Board_init ========
 */
void Board_init(void)
{
    Power_init();
    GPIO_init();
    I2C_init();
    ADC_init();
    SPI_init();
}
