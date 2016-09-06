#include <Adafruit_TLC59711.h>
#include <math.h>
#include <SPI.h>


class LEDDriver {
  public:
    void parse(String data);
    void sunrise(int fadeDuration); //fadeDuration in seconds
    void setRGB(uint16_t r, uint16_t g, uint16_t b, int fadeDuration); //fadeDuration in seconds
    void partyMode();
    void begin();
    LEDDriver(int deviceNumber=1, int clock=13, int data=12, uint16_t r=65535, uint16_t g=65535, uint16_t b=65535);
  private:
    void stringifyFloats(float r, float g, float b, char* output);
    uint16_t visibleToAbs(float visible);
    // current values on linear scale
    float linR;
    float linG;
    float linB;
    int clock;
    int data;
    int deviceNumber;
    uint16_t pwmMax = 65535;
    int expBase = 256;
    //int expBase = 2;
    const int numChips = 1; // only support single chip
    Adafruit_TLC59711 pwmChip;
};
