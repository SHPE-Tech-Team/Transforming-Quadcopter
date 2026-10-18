#pragma once
#include "types.h"

// Sets the servos, the two linear actuators and the LED for the given mode.
void mode_apply(Mode mode);

// The ground motors may only run in driving mode.
bool mode_allows_ground_drive(Mode mode);
