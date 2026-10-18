#pragma once
#include <stdint.h>
#include "types.h"

// This is the ONLY layer that will ever touch pins. Today it only prints.

// Everything the hardware has been asked to do right now.
struct HalState {
    float pulse_left_us;           // left ground motor signal, 125 to 250
    float pulse_right_us;          // right ground motor signal, 125 to 250
    int   servo1_deg;              // 0 to 180
    int   servo2_deg;              // 0 to 180
    bool  actuator1_signal1_high;  // signal 2 is always the opposite of signal 1
    bool  actuator2_signal1_high;
    int   led_red;                 // 0 to 255
    int   led_green;
    int   led_blue;
};

void  hal_init();
void  hal_set_time_ms(uint32_t now_ms);       // only used to timestamp the printed lines
void  hal_set_logging(bool logging_on);

// side: SIDE_LEFT or SIDE_RIGHT.  command: -1 to 1 (0 = stopped).
void  hal_set_ground_motor(int side, float command);
// servo_number: 1 or 2.  angle_deg: 0 to 180.
void  hal_set_servo(int servo_number, int angle_deg);
// actuator_number: 1 or 2.
void  hal_set_actuator(int actuator_number, bool signal1_high);
void  hal_set_led(int red, int green, int blue);

// Call once at the end of every loop step. Sends everything to the hardware.
// Today: prints one line whenever anything changed.
void  hal_update();

HalState hal_get_state();                     // lets tests look at the current outputs

// Converts a command (-1 to 1) into a pulse length in microseconds (125 to 250).
float command_to_pulse_us(float command);
