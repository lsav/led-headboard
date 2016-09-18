#include <Adafruit_TLC59711.h>
#include <math.h>
#include <SPI.h>

class LEDDriver {
    public:
        int parse(const char* data);
        void begin();
        LEDDriver(int deviceNumber=1, int clock=13, int data=12, float r=0, float g=0, float b=0);
    private:
        float r, g, b;
        uint16_t rMap();
        uint16_t gMap();
        uint16_t bMap();
        uint16_t visibleToAbs(float visible, int base=256);
        void rebindColours();
        int clock;
        int data;
        int deviceNumber;
        const uint16_t pwmMax = 65535;
        const int numChips = 1;  // only support single chip
        Adafruit_TLC59711 pwmChip;
        void sunrise(float fadeDuration);  // fadeDuration in seconds
        void setRGB(float targetR, float targetG, float targetB, float fadeDuration);  // fadeDuration in seconds
        void partyMode();
};
