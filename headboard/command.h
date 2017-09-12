#ifndef HEADBOARD_COMMAND_H
#define HEADBOARD_COMMAND_H

#include "led-driver.h"

 // real-time tuning hax
static const unsigned int LOOP_FREQ_HZ = 275;

class State {
  public:
    State();
    State(float r, float g, float b);
    float r;
    float g;
    float b;
    State* next = nullptr;
    float dR;
    float dG;
    float dB;
    unsigned long steps;
    void setTransition(const State& target, float millisFade);
    void nextState();
    bool isComplete();
    void step();
};

/**
  * Parse serial data, set current command
  */
class Command {
  public:
    Command();
    void begin();
    int parse(const char* data);
    void execute();
    State current;
    State lightsOff;
    State red;
    State orange;
    State warmWhite;
    State coolWhite;
  private:
    LEDDriver lamp;
    void on();
    void off();
    void sunrise(float fadeMillis);
};

#endif
