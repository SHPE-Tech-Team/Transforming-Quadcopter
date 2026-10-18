#include "motor_hal.h"
#include "config.h"
#include "mathutil.h"
#include <stdio.h>

#ifdef ARDUINO
#include <Arduino.h>
#endif

static HalState current_state;
static HalState logged_state;
static bool have_logged = false;
static bool logging_on = true;
static uint32_t time_ms = 0;

float command_to_pulse_us(float command)
{
    // Step 1: change -1..1 into 0..1  (0.5 means stopped)
    float scaled = 0.5f + 0.5f * command;
    // Step 2: change 0..1 into a pulse length. Note it runs backwards:
    // scaled 1.0 gives the shortest pulse, scaled 0.0 gives the longest.
    float range = cfg::PULSE_MAX_US - cfg::PULSE_MIN_US;
    float pulse = cfg::PULSE_MAX_US - scaled * range;
    // Step 3: never leave the allowed range
    return clamp_value(pulse, cfg::PULSE_MIN_US, cfg::PULSE_MAX_US);
}

void hal_init()
{
    current_state.pulse_left_us = command_to_pulse_us(0.0f);
    current_state.pulse_right_us = command_to_pulse_us(0.0f);
    current_state.servo1_deg = 0;
    current_state.servo2_deg = 0;
    current_state.actuator1_signal1_high = false;
    current_state.actuator2_signal1_high = false;
    current_state.led_red = 0;
    current_state.led_green = 0;
    current_state.led_blue = 0;
    have_logged = false;
}

void hal_set_time_ms(uint32_t now_ms)
{
    // set the time_ms var
}

void hal_set_logging(bool logging)
{
    // set the logging_on var
}

void hal_set_ground_motor(int side, float command)
{
    // use clamp_value(command, -1.0f, 1.0f) to limit the command and then convert to pulse w/ helper func

    // set the pulse_left_us or pulse_right_us element for current_state based on the side
    // if () {

    // } else {

    // }
}

void hal_set_servo(int servo_number, int angle_deg)
{
    // do data validation of servo data by making sure angles are set to max if over and set to min if under

    // save validated data into either current_state.servo1_deg
    // if ()
    // {
    //     current_state.servo1_deg =;
    // }
    // else if ()
    // {
    //     current_state.servo2_deg = ;
    // }
}

void hal_set_actuator(int actuator_number, bool signal1_high)
{
    // if ()
    // {
    //     current_state.actuator1_signal1_high = ;
    // }
    // else if ()
    // {
    //     current_state.actuator2_signal1_high = ;
    // }
}

void hal_set_led(int red, int green, int blue)
{
    // set the 3 led elems for current_state
}

HalState hal_get_state()
{
    return current_state;
}

static bool state_changed()
{
    if (have_logged == false)
        return true;
    // Pulses must move more than 0.5 us before we print again (keeps the log short)
    if (absolute_value(current_state.pulse_left_us - logged_state.pulse_left_us) > 0.5f)
        return true;
    if (absolute_value(current_state.pulse_right_us - logged_state.pulse_right_us) > 0.5f)
        return true;
    if (current_state.servo1_deg != logged_state.servo1_deg)
        return true;
    if (current_state.servo2_deg != logged_state.servo2_deg)
        return true;
    if (current_state.actuator1_signal1_high != logged_state.actuator1_signal1_high)
        return true;
    if (current_state.actuator2_signal1_high != logged_state.actuator2_signal1_high)
        return true;
    if (current_state.led_red != logged_state.led_red)
        return true;
    if (current_state.led_green != logged_state.led_green)
        return true;
    if (current_state.led_blue != logged_state.led_blue)
        return true;
    return false;
}

static void print_state()
{
    char line[200];
    snprintf(line, sizeof(line),
             "t=%6lums  pulse L=%6.1f R=%6.1f us | servo %3d %3d | act1 sig1=%d act2 sig1=%d | led %d,%d,%d",
             (unsigned long)time_ms,
             current_state.pulse_left_us, current_state.pulse_right_us,
             current_state.servo1_deg, current_state.servo2_deg,
             current_state.actuator1_signal1_high ? 1 : 0,
             current_state.actuator2_signal1_high ? 1 : 0,
             current_state.led_red, current_state.led_green, current_state.led_blue);
#ifdef ARDUINO
    Serial.println(line);
#else
    printf("%s\n", line);
#endif
}

void hal_update()
{
    // later the real pin writes go here (motor pulses, servos, actuator pins, LED).
    if (logging_on == false)
    {
        return;
    }
    if (state_changed())
    {
        print_state();
        logged_state = current_state;
        have_logged = true;
    }
}
