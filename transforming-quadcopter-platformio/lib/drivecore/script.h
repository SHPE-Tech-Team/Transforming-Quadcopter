#pragma once
#include <stdint.h>
#include "types.h"

// One row of the script.
struct Step {
    uint32_t duration_ms;
    float    drive;
    float    steer;
    Mode     mode;
};

// Works with any table of steps (this is what the tests use).
// t_ms is the time since boot.
Command script_get_from(const Step* steps, int count, uint32_t t_ms);

// Uses the default table inside script.cpp.
Command script_get(uint32_t t_ms);
bool    script_done(uint32_t t_ms);

// Smooths drive and steer so the vehicle does not jerk. The mode passes straight through.
void    filter_reset();
Command filter_command(Command raw);
