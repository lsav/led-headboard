#ifndef HEADBOARD_COMMAND_H
#define HEADBOARD_COMMAND_H

#include <math.h>
#include "led-driver.h"

/**
  * Parse serial data, issue commands
  */
class Command {
  public:
    int parse(const char* data);
    void Command();
    void begin();
  private:
    LEDDriver lamp;
    void Command::fadeRGB(float targetR, float targetG, float targetB,
      float fadeDuration);
    void sunrise(float fadeDuration);  // fadeDuration in seconds
    void partyMode();
}

#endif
