#include "script.h"
#include "config.h"

// EDIT THIS TABLE to change what the vehicle does.
// Each row is: how long (ms), drive, steer, mode.
static const Step DEFAULT_SCRIPT[] = {
    // { , , , MODE_DRIVING },   // forward
    // { , , , MODE_DRIVING },   // turn in place (slowly, steer gain is small)
    // { , , , MODE_DRIVING },   // forward
    // { , , , MODE_DRIVING }    // stop
};
static const int DEFAULT_COUNT = sizeof(DEFAULT_SCRIPT) / sizeof(DEFAULT_SCRIPT[0]);

Command script_get_from(const Step *steps, int count, uint32_t t_ms)
{
    Command result;
    result.drive = 0.0f;
    result.steer = 0.0f;
    result.mode = MODE_LANDED;

    // Right after boot: stay still in landed mode.
    if (t_ms < cfg::START_DELAY_MS)
    {
        return result;
    }

    uint32_t time_into_script = t_ms - cfg::START_DELAY_MS;
    int i;
    for (i = 0; i < count; i++)
    {
        //     - If time_into_script is less than steps[i].duration_ms, we are in this step:
        //         copy drive, steer and mode from steps[i] into result, then return result
        //     - Otherwise this step is over: subtract steps[i].duration_ms from
        //         time intoo script and go on to the next step
    }

    // Script is finished: stay still, and if count is more than 0 stay in the last step's mode.

    return result;
}

Command script_get(uint32_t t_ms)
{
    return script_get_from(DEFAULT_SCRIPT, DEFAULT_COUNT, t_ms);
}

bool script_done(uint32_t t_ms)
{
    uint32_t total = cfg::START_DELAY_MS;
    int i;
    for (i = 0; i < DEFAULT_COUNT; i++)
    {
        // total =
        // update the total using the properties of DEFAULT_SCRIPT at i
    }

    return false; // should return when t_ms is bigger or equal
}

// ---------------- smoothing ----------------
static float previous_drive = 0.0f;
static float previous_steer = 0.0f;

void filter_reset()
{
    // reset the settings for drive & steer to 0
}

Command filter_command(Command raw)
{
    float b = cfg::CMD_FILTER_B;
    Command result;
    result.drive = (1.0f - b) * previous_drive + b * raw.drive;
    result.steer = (1.0f - b) * previous_steer + b * raw.steer;
    result.mode = raw.mode;
    previous_drive = result.drive;
    previous_steer = result.steer;
    return result;
}
