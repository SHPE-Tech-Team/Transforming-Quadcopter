#pragma once
#include <stdint.h>

// ALL tunable numbers live here. No hardcodes anywhere else.
namespace cfg
{

    // Ground driving
    const float SPEED_LIMIT = 1.0f;  // 1.0 = full speed. Use 0.3 for the very first ground test!
    const float STEER_GAIN = 0.2f;   // how strongly steer changes the two wheels
    const float DEADBAND = 0.08f;    // motor commands smaller than this become 0 (stopped)
    const bool INVERT_LEFT = false;  // set true if the left wheel spins the wrong way
    const bool INVERT_RIGHT = false; // set true if the right wheel spins the wrong way

    // Command smoothing
    const float CMD_FILTER_B = 0.7f; // 0 to 1. Lower = smoother and slower, higher = faster

    // Motor signal range (microseconds)
    // A command of -1 gives 250, 0 gives 187.5 (stopped), +1 gives 125.
    const float PULSE_MIN_US = 125.0f;
    const float PULSE_MAX_US = 250.0f;

    // Timing
    const uint32_t LOOP_PERIOD_MS = 10;   // time between loop steps
    const uint32_t START_DELAY_MS = 2000; // vehicle stays disarmed and still this long after boot

    // Servo angles (degrees, 0 to 180) for each mode
    const int MODE1_SERVO1 = 177; // driving
    const int MODE1_SERVO2 = 180;
    const int MODE2_SERVO1 = 2; // landed
    const int MODE2_SERVO2 = 2;
    const int MODE3_SERVO1 = 2; // flying
    const int MODE3_SERVO2 = 2;
}
