#pragma once
#include <stdint.h>

void robot_init();
void robot_step(uint32_t now_ms);   // one pass through the control loop

// Emergency stop. Once called, the vehicle stays disarmed until the next reboot.
void robot_kill();
bool robot_is_armed();
