#include "mixer.h"
#include "config.h"
#include "mathutil.h"

MotorCmd mix_ground(Command cmd)
{
    // keep the inputs inside -1 to 1
    float drive = clamp_value(cmd.drive, -1.0f, 1.0f);
    float steer = clamp_value(cmd.steer, -1.0f, 1.0f);

    // steering speeds up one wheel and slows the other using the same value away from drive
    float steer_part = steer * cfg::STEER_GAIN;
    float right = 0.0f; // chnage
    float left = 0.0f;  // change

    // tiny commands (use abs value) to either become zero so the motors do not hum or creep
    // use the DEADBAND defined in cfg

    right = clamp_value(right, -1.0f, 1.0f) * cfg::SPEED_LIMIT;
    left = clamp_value(left, -1.0f, 1.0f) * cfg::SPEED_LIMIT;

    // flip either wheel that spins the wrong way
    // if (cfg::INVERT_RIGHT) {
    // }
    // if (cfg::INVERT_LEFT) {
    // }

    MotorCmd result;
    result.left = left;
    result.right = right;
    return result;
}
