#pragma once
#include "types.h"

// Turns drive + steer into a command for each ground motor (-1 to 1).
// Pure math: it does not touch the hardware layer.
MotorCmd mix_ground(Command cmd);
