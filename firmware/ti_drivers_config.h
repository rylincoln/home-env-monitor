/*
 * ti_drivers_config.h - Manual driver configuration for CC3220SF
 *
 * This file replaces the SysConfig-generated configuration.
 * It defines the driver instance indices and config tables needed
 * by the TI SimpleLink SDK drivers.
 */

#ifndef ti_drivers_config_h
#define ti_drivers_config_h

#include <stdint.h>

/* Board indices (these map to the config array positions) */
#define CONFIG_I2C_0        0
#define CONFIG_ADC_CO       0
#define CONFIG_ADC_MIC      1
#define CONFIG_GPIO_LED_0   0
#define CONFIG_GPIO_LED_1   1
#define CONFIG_GPIO_BUZZER  2

#define CONFIG_GPIO_COUNT   3

#endif /* ti_drivers_config_h */
