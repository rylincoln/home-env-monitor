#include "co_alarm.h"
#include "Board.h"
#include <ti/drivers/GPIO.h>

static float threshold_on;
static float threshold_off;
static bool alarm_active;

void COAlarm_init(float alarm_ppm, float clear_ppm)
{
    threshold_on = alarm_ppm;
    threshold_off = clear_ppm;
    alarm_active = false;

    GPIO_setConfig(Board_GPIO_BUZZER,
                   GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
}

bool COAlarm_check(float co_ppm)
{
    if (!alarm_active && co_ppm >= threshold_on) {
        alarm_active = true;
        GPIO_write(Board_GPIO_BUZZER, 1);
    } else if (alarm_active && co_ppm < threshold_off) {
        alarm_active = false;
        GPIO_write(Board_GPIO_BUZZER, 0);
    }

    return alarm_active;
}
