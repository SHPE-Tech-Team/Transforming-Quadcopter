#include "config.h"
#include "robot.h"

#ifdef ARDUINO
#include <Arduino.h>
#include "types.h"
#include "motor_hal.h"
#include "script.h"
#include "mixer.h"
#include "mode_control.h"

// If no key is received for this long, the vehicle stops
const uint32_t MANUAL_TIMEOUT_MS = 4000;

enum RunMode
{
  RUN_MANUAL,
  RUN_SCRIPT
};

static RunMode run_mode = RUN_MANUAL;
static uint32_t script_start_ms = 0;

// Manual control state
static bool manual_armed = false;
static Mode manual_mode = MODE_LANDED;
static float manual_speed = 0.4f;
static float manual_drive = 0.0f;
static float manual_steer = 0.0f;
static uint32_t last_command_ms = 0;

static void print_help()
{
  Serial.println("---- live control ----");
  Serial.println("a arm   k KILL (a to re-arm)");
  Serial.println("d driving mode   p landed mode   (servos MOVE on a mode change)");
  Serial.println("f forward  b backward  l left  r right  s stop");
  Serial.println("1-9 speed for f and b");
  Serial.println("g run script   m back to manual   h help");
  Serial.println("Vehicle stops if no key arrives for 2 seconds.");
  Serial.println("Start: disarmed, landed. Press a, then d, then f.");
  Serial.println("----------------------");
}

static void handle_key(char key, uint32_t now_ms)
{
  if (key == 'k')
  {
    // Emergency stop
    manual_armed = false;
    manual_drive = 0.0f;
    manual_steer = 0.0f;
    if (run_mode == RUN_SCRIPT)
    {
      robot_kill(); // this one stays latched until the next reboot
      run_mode = RUN_MANUAL;
      Serial.println("KILLED (script). Reboot the Teensy to run the script again.");
    }
    else
    {
      Serial.println("KILLED. Type a to arm again.");
    }
  }
  else if (key == 'a')
  {
    manual_armed = true;
    last_command_ms = now_ms;
    Serial.println("armed");
  }
  else if (key == 'd')
  {
    manual_mode = MODE_DRIVING;
    Serial.println("driving mode");
  }
  else if (key == 'p')
  {
    manual_mode = MODE_LANDED;
    manual_drive = 0.0f;
    manual_steer = 0.0f;
    Serial.println("landed mode");
  }
  else if (key == 'f')
  {
    manual_drive = manual_speed;
    manual_steer = 0.0f;
    last_command_ms = now_ms;
  }
  else if (key == 'b')
  {
    manual_drive = -manual_speed;
    manual_steer = 0.0f;
    last_command_ms = now_ms;
  }
  else if (key == 'l')
  {
    manual_drive = 0.0f;
    manual_steer = -1.0f;
    last_command_ms = now_ms;
  }
  else if (key == 'r')
  {
    manual_drive = 0.0f;
    manual_steer = 1.0f;
    last_command_ms = now_ms;
  }
  else if (key == 's')
  {
    manual_drive = 0.0f;
    manual_steer = 0.0f;
    last_command_ms = now_ms;
  }
  else if (key >= '1' && key <= '9')
  {
    manual_speed = (key - '0') / 10.0f;
    Serial.print("speed ");
    Serial.println(manual_speed);
  }
  else if (key == 'g')
  {
    run_mode = RUN_SCRIPT;
    script_start_ms = now_ms;
    filter_reset();
    Serial.println("running script (k to stop, m for manual)");
  }
  else if (key == 'm')
  {
    run_mode = RUN_MANUAL;
    manual_drive = 0.0f;
    manual_steer = 0.0f;
    filter_reset();
    Serial.println("manual control");
  }
  else if (key == 'h' || key == '?')
  {
    print_help();
  }
  // Any other key not listed is ignored
}

// One pass of the control loop for manual control
// It follows the same steps as robot_step() in robot.cpp
static void manual_step(uint32_t now_ms)
{
  hal_set_time_ms(now_ms);

  float drive = manual_drive;
  float steer = manual_steer;

  // timeout: no recent key means stop
  if (now_ms - last_command_ms > MANUAL_TIMEOUT_MS)
  {
    drive = 0.0f;
    steer = 0.0f;
  }

  Command raw;
  raw.drive = drive;
  raw.steer = steer;
  raw.mode = manual_mode;
  Command cmd = filter_command(raw);

  MotorCmd motors = mix_ground(cmd);

  mode_apply(cmd.mode);
  if (mode_allows_ground_drive(cmd.mode) == false)
  {
    motors.left = 0.0f;
    motors.right = 0.0f;
  }

  // Safety cut: not armed means the wheels stay stopped.
  if (manual_armed == false)
  {
    motors.left = 0.0f;
    motors.right = 0.0f;
  }

  hal_set_ground_motor(SIDE_RIGHT, motors.right);
  hal_set_ground_motor(SIDE_LEFT, motors.left);
  hal_update();
}

void setup()
{
  Serial.begin(115200);
  robot_init();
  print_help();
}

void loop()
{
  uint32_t now_ms = millis();

  // Read every key that has arrived since the last loop.
  while (Serial.available() > 0)
  {
    char key = Serial.read();
    handle_key(key, now_ms);
  }

  if (run_mode == RUN_SCRIPT)
  {
    robot_step(now_ms - script_start_ms); // the script starts its own clock at 0
  }
  else
  {
    manual_step(now_ms);
  }

  delay(cfg::LOOP_PERIOD_MS);
}

#else
int main()
{
  robot_init();
  uint32_t t;
  for (t = 0; t <= 12000; t = t + cfg::LOOP_PERIOD_MS)
  {
    robot_step(t);
  }
  return 0;
}
#endif

// Teensy BUILD
//  Open the monitor with:  pio device monitor -e teensy40
//  Type one letter at a time (no Enter needed). Type h to see this list again.
//
//  Safety
//    a   arm the motors (they will NOT move until you arm)
//    k   KILL: stop everything right now (type a to arm again)
//
//  Mode (the lifting servos and actuators MOVE when you change mode!)
//    d   driving mode (the wheels only run in driving mode)
//    p   landed mode ("park")
//
//  Driving (each key sets the whole command)
//    f   forward          b   backward
//    l   turn left        r   turn right
//    s   stop
//    1 to 9   speed for f and b (1 = slowest, 9 = fastest). Starts at 4.
//
//  Script
//    g   run the hardcoded script in script.cpp from the start
//    m   go back to manual control (stops the vehicle)
//
//  The vehicle stops by itself if no key arrives for MANUAL_TIMEOUT_MS
//  Keep a key repeating (hold it down) to keep moving