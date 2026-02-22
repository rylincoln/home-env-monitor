#ifndef CO_ALARM_H
#define CO_ALARM_H

#include <stdbool.h>

/* Initialize CO alarm with threshold and clear levels (ppm).
 * Configures the buzzer GPIO pin. */
void COAlarm_init(float alarm_ppm, float clear_ppm);

/* Check CO level and control buzzer.
 * Returns true if alarm is active.
 * Uses hysteresis: activates at alarm_ppm, clears at clear_ppm. */
bool COAlarm_check(float co_ppm);

#endif
