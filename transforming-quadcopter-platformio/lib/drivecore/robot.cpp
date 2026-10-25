#include "robot.h"
#include "types.h"
#include "config.h"
#include "motor_hal.h"
#include "script.h"
#include "mixer.h"
#include "mode_control.h"

static bool armed = false;
static bool kill_latched = false;

void robot_init()
{
    hal_init();
    filter_reset();
    armed = false;
    kill_latched = false;

    // Apply landed mode, stop both ground motors
    hal_set_ground_motor(SIDE_RIGHT, 0.0f);
    hal_set_ground_motor(SIDE_LEFT, 0.0f);

    hal_update();

}

void robot_kill()
{
    armed = false;
    kill_latched = true;
}

bool robot_is_armed()
{
    return armed;
}

void robot_step(uint32_t now_ms)
{
    hal_set_time_ms(now_ms);

    // arm once the start delay is over (unless either type of kill was requested)
    //if motors dont have permission, no one pressed emergency stop, and we have waited long enough, give motors PERMISSION to move
    if ( armed == false && kill_latched == false && now_ms >= cfg::START_DELAY_MS) {
        armed = true;
    }
    //Look at the robot’s instruction list and find what it should do at this time. raw contains drive, steer, and mode
    Command raw = script_get(now_ms);
    //cmd filters raw and makes it more smooth. for ex. instead of demanding fast speed, move toward speed slowly.
    Command cmd = filter_command(raw);
    
    MotorCmd motors = mix_ground(cmd);

    // apply the mode here from the cmd by calling func and using param
    mode_apply(cmd.mode);
    if (mode_allows_ground_drive(cmd.mode) == false)
    {
        motors.left = 0.0f;
        motors.right = 0.0f;
    }

    // add safety cut here. If disarmed, the ground motors are forced to stopped.
    if (armed == false) {
        motors.left = 0.0f;
        motors.right = 0.0f;
    }
    // Step 6: send everything out
    hal_set_ground_motor(SIDE_RIGHT, motors.right);
    hal_set_ground_motor(SIDE_LEFT, motors.left);
    hal_update();

    // TODO later (flight, later): the four flight motors will be commanded here.
}
