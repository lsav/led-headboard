#ifndef HEADBOARD_LEDDRIVER_H
#define HEADBOARD_LEDDRIVER_H

#include <math.h>
#include <Arduino.h>

/**
  * Hardware layout, globals for ISR manipulation
  */
#define pin1A 9
#define pin1B 10
#define pin2 11
extern volatile uint8_t channelMSB; // MSB for using TIMER2 in 16-bit PWM
extern uint16_t channelValue; // 16 bit value for the TIMER2 controlled channel
//ISRs
extern void channelLow();
extern void channelHigh();

/**
  * Manage LED colours, transformations and PWM
  * Manage three channel PWM on the ATMega328P
  * Two channels are handled semi-automatically by TIMER1
  * A third channel needs to use TIMER2 with resolution extended
  * through use of a variable
  */
class LEDDriver {
  public:
    LEDDriver(float r=0, float g=0, float b=0);
    void setRGB(); // configure timers for current values
    void setRGB(float R, float G, float B); // configure timer for three channels
    void begin(); //configure timers
    float r, g, b;
  private:
    void rebindColours();
    const uint16_t pwmMax = 65535;
    // handle transformations to human-eye centric values
    uint16_t rMap();
    uint16_t gMap();
    uint16_t bMap();
    uint16_t visibleToAbs(float visible, int base=256);
};

#endif

  // https://www.sparkfun.com/datasheets/Components/SMD/ATMega328.pdf
