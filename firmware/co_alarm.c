#include "co_alarm.h"
#include "Board.h"
#include <ti/drivers/GPIO.h>

static float threshold_on;
static float threshold_off;
static bool alarm_active;

void COAlarm_init(float alarm_ppm, float clear_ppm)
{
    /* Validate thresholds: alarm must be above clear for hysteresis */
    if (alarm_ppm <= clear_ppm) {
        /* Swap to safe defaults if misconfigured */
        threshold_on = clear_ppm;
        threshold_off = alarm_ppm;
    } else {
        threshold_on = alarm_ppm;
        threshold_off = clear_ppm;
    }
    alarm_active = false;

    GPIO_setConfig(Board_GPIO_BUZZER,
                   GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    /* Ensure buzzer is off after warm reset */
    GPIO_write(Board_GPIO_BUZZER, 0);
}

bool COAlarm_check(float co_ppm)
{
    /* Guard against NaN from upstream sensor errors */
    if (!(co_ppm == co_ppm)) return alarm_active;  /* NaN != NaN */

    if (!alarm_active && co_ppm >= threshold_on) {
        alarm_active = true;
        GPIO_write(Board_GPIO_BUZZER, 1);
    } else if (alarm_active && co_ppm < threshold_off) {
        alarm_active = false;
        GPIO_write(Board_GPIO_BUZZER, 0);
    }

    return alarm_active;
}
