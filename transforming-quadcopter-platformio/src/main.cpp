#include "config.h"
#include "robot.h"

#ifdef ARDUINO
// Placeholder for when hardware work starts.
#include <Arduino.h>
void setup()
{
  Serial.begin(115200);
  robot_init();
}
void loop()
{
  robot_step(millis());
  delay(cfg::LOOP_PERIOD_MS);
}
#else
// Native build: run on a fake clock so a 12 s script finishes instantly.
int main()
{
  robot_init();
  for (uint32_t t = 0; t <= 12000; t += cfg::LOOP_PERIOD_MS)
  {
    robot_step(t);
  }
  return 0;
}
#endif