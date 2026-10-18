#include "mode_control.h"
#include "config.h"
#include "motor_hal.h"

void mode_apply(Mode mode)
{
    if (mode == MODE_DRIVING)
    {
        hal_set_servo(1, cfg::MODE1_SERVO1);
        hal_set_servo(2, cfg::MODE1_SERVO2);
        hal_set_actuator(1, false);
        hal_set_actuator(2, false);
        hal_set_led(255, 255, 255); // white
    }
    else if (mode == MODE_FLYING)
    {
        hal_set_servo(1, cfg::MODE3_SERVO1);
        hal_set_servo(2, cfg::MODE3_SERVO2);
        hal_set_actuator(1, true);
        hal_set_actuator(2, true);
        // set appropriate for flying
        hal_set_led(255, 255, 255);
    }
    else
    {
        hal_set_servo(1, cfg::MODE2_SERVO1);
        hal_set_servo(2, cfg::MODE2_SERVO2);
        hal_set_actuator(1, false);
        hal_set_actuator(2, false);
        // for landing and the safe choice for anything unexpected
        hal_set_led(1, 1, 255);
    }
}

bool mode_allows_ground_drive(Mode mode)
{
    if (mode == MODE_DRIVING) {
        return true; 
    }

    // Only true if mode is for driving
    return false;
}