#ifndef HEADBOARD_COMMAND_H
#define HEADBOARD_COMMAND_H

#include "led-driver.h"

// TODO: this needs tuning
static const unsigned long LOOP_FREQ_KHZ = 120;
static const unsigned int ONE_SECOND_MILLIS = 1000;

class State {
  public:
    State();
    State(double r, double g, double b);
    double r;
    double g;
    double b;
    State* next = nullptr;
    double dR;
    double dG;
    double dB;
    unsigned long steps;
    void setTransition(const State& target, unsigned long millisFade);
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
    void sunrise(unsigned long fadeMillis);
};

#endif
