#pragma once
#include <stdint.h>

// Vehicle modes. The numbers are fixed. Do not change them.
enum Mode {
    MODE_DRIVING = 1,
    MODE_LANDED  = 2,
    MODE_FLYING  = 3
};

enum Side {
    SIDE_LEFT,
    SIDE_RIGHT
};

// What the "pilot" asks for (a script today, a radio later).
struct Command {
    float drive;   // -1 to 1.  + is forward, - is backward
    float steer;   // -1 to 1.  + is turn right, - is turn left
    Mode  mode;    // which mode the vehicle should be in
};

// Mixed output for the two ground motors. -1 to 1 (sign = direction)
struct MotorCmd {
    float left;
    float right;
};
