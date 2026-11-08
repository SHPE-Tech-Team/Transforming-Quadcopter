#include "motor_hal.h"
#include "config.h"
#include "mathutil.h"
#include <stdio.h>

#ifdef ARDUINO
#include <Arduino.h>
#include <PWMServo.h>
#include <FastLED.h>

static PWMServo servo1;
static PWMServo servo2;
static CRGB leds[cfg::NUM_LEDS];
static bool hardware_ready = false;

// Remember the last LED color we sent, because sending a color takes time.
static int written_led_red = -1;
static int written_led_green = -1;
static int written_led_blue = -1;
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

#ifdef ARDUINO
static void hardware_setup()
{
    if (hardware_ready)
    {
        return; // only set things up once
    }

    // Ground motor signal pins
    pinMode(cfg::PIN_GROUND_MOTOR_RIGHT, OUTPUT);
    pinMode(cfg::PIN_GROUND_MOTOR_LEFT, OUTPUT);
    digitalWrite(cfg::PIN_GROUND_MOTOR_RIGHT, LOW);
    digitalWrite(cfg::PIN_GROUND_MOTOR_LEFT, LOW);

    // lifting servos
    servo1.attach(cfg::PIN_SERVO1, cfg::SERVO1_MIN_US, cfg::SERVO1_MAX_US);
    servo2.attach(cfg::PIN_SERVO2, cfg::SERVO2_MIN_US, cfg::SERVO2_MAX_US);

    // linear actuators
    pinMode(cfg::PIN_ACT1_SIGNAL1, OUTPUT);
    pinMode(cfg::PIN_ACT1_SIGNAL2, OUTPUT);
    pinMode(cfg::PIN_ACT2_SIGNAL1, OUTPUT);
    pinMode(cfg::PIN_ACT2_SIGNAL2, OUTPUT);

    // status LED
    FastLED.addLeds<WS2812, cfg::PIN_LED, GRB>(leds, cfg::NUM_LEDS);
    FastLED.clear();
    FastLED.show();

    hardware_ready = true;
}

// Sends one pulse to each ground motor
// Both pins go HIGH together, then each goes LOW when its pulse length is reached
static void send_motor_pulses()
{
    bool right_done = false;
    bool left_done = false;

    digitalWrite(cfg::PIN_GROUND_MOTOR_RIGHT, HIGH);
    digitalWrite(cfg::PIN_GROUND_MOTOR_LEFT, HIGH);
    uint32_t start = micros();

    while (right_done == false || left_done == false)
    {
        uint32_t elapsed = micros() - start;

        if (right_done == false && elapsed >= current_state.pulse_right_us)
        {
            digitalWrite(cfg::PIN_GROUND_MOTOR_RIGHT, LOW);
            right_done = true;
        }
        if (left_done == false && elapsed >= current_state.pulse_left_us)
        {
            digitalWrite(cfg::PIN_GROUND_MOTOR_LEFT, LOW);
            left_done = true;
        }
    }
}

// Sends the whole current state to the pins
static void write_hardware()
{
    servo1.write(current_state.servo1_deg);
    servo2.write(current_state.servo2_deg);

    // Actuators -> signal 2 is always the opposite of signal 1
    if (current_state.actuator1_signal1_high)
    {
        digitalWrite(cfg::PIN_ACT1_SIGNAL1, HIGH);
        digitalWrite(cfg::PIN_ACT1_SIGNAL2, LOW);
    }
    else
    {
        digitalWrite(cfg::PIN_ACT1_SIGNAL1, LOW);
        digitalWrite(cfg::PIN_ACT1_SIGNAL2, HIGH);
    }
    if (current_state.actuator2_signal1_high)
    {
        digitalWrite(cfg::PIN_ACT2_SIGNAL1, HIGH);
        digitalWrite(cfg::PIN_ACT2_SIGNAL2, LOW);
    }
    else
    {
        digitalWrite(cfg::PIN_ACT2_SIGNAL1, LOW);
        digitalWrite(cfg::PIN_ACT2_SIGNAL2, HIGH);
    }

    // send a color when it changed
    if (current_state.led_red != written_led_red ||
        current_state.led_green != written_led_green ||
        current_state.led_blue != written_led_blue)
    {
        leds[0] = CRGB(current_state.led_red, current_state.led_green, current_state.led_blue);
        FastLED.show();
        written_led_red = current_state.led_red;
        written_led_green = current_state.led_green;
        written_led_blue = current_state.led_blue;
    }

    // Ground motors
    send_motor_pulses();
}

#endif

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
    time_ms = now_ms;
}

void hal_set_logging(bool logging)
{
    // set the logging_on var
    logging_on = logging;
}

void hal_set_ground_motor(int side, float command)
{
    // use clamp_value(command, -1.0f, 1.0f) to limit the command and then convert to pulse w/ helper func
    float lim = clamp_value(command, -1.0f, 1.0f);
    float pulse = command_to_pulse_us(lim);
    // set the pulse_left_us or pulse_right_us element for current_state based on the side

    // if () {

    // } else {

    // }
}

void hal_set_servo(int servo_number, int angle_deg)
{
    // do data validation of servo data by making sure angles are set to max if over and set to min if under
    if (angle_deg > 360)
        angle_deg = 360;
    if (angle_deg < 0)
        angle_deg = 0;
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
    // use actuator_number param
    //  if ()
    //  {
    //      current_state.actuator1_signal1_high = ;
    //  }
    //  else if ()
    //  {
    //      current_state.actuator2_signal1_high = ;
    //  }
}

void hal_set_led(int red, int green, int blue)
{
    // set the 3 led elems for current_state
}

HalState hal_get_state()
{
    return current_state;
}

void hal_arm_escs()
{
    // Sends the arming command to both ground motors
    hal_set_ground_motor(SIDE_LEFT, cfg::ARM_COMMAND);
    hal_set_ground_motor(SIDE_RIGHT, cfg::ARM_COMMAND);

#ifdef ARDUINO
    // Keep sending pulses so the motor controllers see a steady signal and arm.
    int i;
    for (i = 0; i < cfg::ARM_PULSE_COUNT; i++)
    {
        send_motor_pulses();
        delay(cfg::ARM_PULSE_DELAY_MS);
    }
#endif

    // Always finish with the motors stopped.
    hal_set_ground_motor(SIDE_LEFT, 0.0f);
    hal_set_ground_motor(SIDE_RIGHT, 0.0f);
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
#ifdef ARDUINO
    write_hardware(); // real pins first, then the optional log line
#endif
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
