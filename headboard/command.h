#ifndef HEADBOARD_COMMAND_H
#define HEADBOARD_COMMAND_H

#include "led-driver.h"
#include "queue.h"

// real-time tuning hax
static const unsigned int LOOP_FREQ_HZ = 275;

class Colour {
  public:
    Colour(float r=0, float g=0, float b=0);
    float r;
    float g;
    float b;
};

static const Colour LIGHTS_OFF{0, 0, 0};
static const Colour RED{0.3, 0.1, 0.0};
static const Colour ORANGE{0.55, 0.35, 0.15};
static const Colour WARM_WHITE{0.7, 0.5, 0.2};
static const Colour COOL_WHITE{1, 0.8, 0.4};


class Transition {
  public:
    Transition(const Colour& start, const Colour& target, float millisFade);
    Transition(const Transition&) = delete; // avoid accidents
    void step(Colour& current);
    bool isComplete();
    unsigned long steps;
    float dR;
    float dG;
    float dB;
};


/**
  * Parse serial data, set current command
  */
class Command {
  public:
    Command();
    void begin();
    void parse(const char* data);
    void execute();
  private:
    Colour current;
    LEDDriver lamp;
    queue<Transition>* transitions;
    void clearTransitions();
    void addTransition(const Colour& start, const Colour& target, double millisFade);
    void on();
    void off();
    void sunrise(float millisFade);
};

#endif
