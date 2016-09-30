#include "led-driver.h"

LEDDriver::LEDDriver(float r, float g, float b) : r(r), g(g), b(b) {
    rebindColours();
}

/**
  * Convert desired visible brightness to absolute brightness value
  * Visible is given as a float between 0 and 1
  */
uint16_t LEDDriver::visibleToAbs(float visible, int base) {
  return (uint16_t)((pwmMax/(base-1))*(pow(base, visible) - 1));
}

/**
  * This LED string is really really blue for some reason, need to compensate
  * These numbers seem to give the nicest looking transitions
  */
uint16_t LEDDriver::rMap() {return visibleToAbs(r, 10);}
uint16_t LEDDriver::gMap() {return visibleToAbs(g*.92, 10);}
uint16_t LEDDriver::bMap() {return visibleToAbs(b*.76, 8);}

/**
  * Bind colours to 0 < x < 1 to avoid floating point error accumulation
  */
void LEDDriver::rebindColours() {
  r = (r < 0 ? 0 : r);
  r = (r > 1 ? 1 : r);
  g = (g < 0 ? 0 : g);
  g = (g > 1 ? 1 : g);
  b = (b < 0 ? 0 : b);
  b = (b > 1 ? 1 : b);
}

/**
  * Configure pins and timers for PWM
  *
  * Configure Timer1
  * Timer1 is a 16 bit timer with two OCRs
  * This will be able to run two colour channels for PWM
  *
  * Configure Timer2
  * Timer2 is an 8-bit timer with two OCRs
  * This will run a third colour channels, but only at 8 bit resolution
  * Using a volatile counter in memory, extend the 8-bit resolution to 16
  * by incrementing the counter on each overflow interrupt
  */

void LEDDriver::begin() {
    rebindColours();
    pinMode(pin1A, OUTPUT);
    pinMode(pin1B, OUTPUT);
    pinMode(pin2, OUTPUT);
    channelMSB = 0;

    // Attach interrupts?
    /** Interrupt vectors
      * 8 0x000E TIMER2 COMPA Timer/Counter2 Compare Match A
      * 10 0x0012 TIMER2 OVF Timer/Counter2 Overflow

      * dynamic_attachInterrupt(TIM2_COMPA, channelLow);
      * dynamic_attachInterrupt(TIM2_OVF, channelHigh);
      */

    setRGB();
}

/**
  * Configure timer PWM values for stored colours
  */
void LEDDriver::setRGB() {

  //TODO: implementation needed
  OCR1A = rMap();
  OCR1B = gMap();
  channelValue = bMap();
}

/**
  * Store provided colours
  * Configure timer PWM values
  */
void LEDDriver::setRGB(uint16_t R, uint16_t G, uint16_t B) {

  r = R;
  g = G;
  b = B;
  rebindColours();
  setRGB();
}

/**
  * ISR for TIM2_COMPA
  * Set global PWM channel low if (channelMSB << 8 | OCR2A) >= channelValue
  */
void channelLow() {}

/**
  * ISR for TIM2_OVF
  * Set global PWM channel high if channelMSB == 0
  * Increment global timer2 MSB tracker
  */
void channelHigh() {}
