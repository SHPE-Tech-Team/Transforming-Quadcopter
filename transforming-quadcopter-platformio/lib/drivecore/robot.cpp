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

    hal_update();
}

void robot_kill()
{
    armed = false;
    kill_latched = true;
}

bool robot_is_armed()
{
    // just return status of the relevant variable
}

void robot_step(uint32_t now_ms)
{
    hal_set_time_ms(now_ms);

    // arm once the start delay is over (unless either type of kill was requested)
    // if ( && && now_ms >= cfg::START_DELAY_MS) {

    // }

    Command raw = script_get(now_ms);
    Command cmd = filter_command(raw);
    MotorCmd motors = mix_ground(cmd);

    // apply the mode here from the cmd by calling func and using param

    if (mode_allows_ground_drive(cmd.mode) == false)
    {
        motors.left = 0.0f;
        motors.right = 0.0f;
    }

    // add safety cut here. If disarmed, the ground motors are forced to stopped.

    // Step 6: send everything out
    hal_set_ground_motor(SIDE_RIGHT, motors.right);
    hal_set_ground_motor(SIDE_LEFT, motors.left);
    hal_update();

    // TODO later (flight, later): the four flight motors will be commanded here.
}
