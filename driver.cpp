#include "driver.h"
#include "assert.h"

using namespace std;

uint16_t LEDDriver::visibleToAbs(float visible) {
  // convert desired visible brightness to absolute brightness value
  double power = visible/pwmMax;
  double multiplier = pwmMax/(expBase-1);
  uint16_t ret = pwmMax - (uint16_t)(multiplier*(pow(expBase, power) - 1));
  return ret;
}

void LEDDriver::parse(String data) {
    //tokenize data into command, data and duration
    //int fade = stoi(data);
    if(data == "on") {
      Serial.println("On!!");
      setRGB(0, 0, 0, 2);
    } else if(data == "off") {
      setRGB(pwmMax, pwmMax, pwmMax, 2);
      Serial.println("Off!!");
    } else if(data == "sunrise") {
      sunrise(20);
    }
}

void LEDDriver::sunrise(int fadeDuration) {
  
  // integer divisibility issues
  if(fadeDuration % 5 != 0)
    fadeDuration += fadeDuration % 5;
  
  //assume off, but turn off to be safe first  
  setRGB(0, 0, 0, 3);
  
  int t1 = fadeDuration/5;
  int t2 = 2*t1;
  
  //setRGB(65535, 61937, 57568, fadeDuration); //warm white eh?
  
  setRGB(30000, 35000, 10000, t1); //a red dawn rises
  setRGB(55000, 45000, 25000, t2); //yellow it out...
  setRGB(65535, 50000, 35000, t2); // lighten a ton
  
  }

void LEDDriver::partyMode() {}


LEDDriver::LEDDriver(int deviceNumber, int clock, int data, uint16_t r, uint16_t g, uint16_t b) :
    deviceNumber(deviceNumber), clock(clock), data(data), linR(r), linG(g), linB(b), pwmChip(numChips, clock, data) {}

void LEDDriver::begin() {
    pwmChip.begin();
    pwmChip.write();
    pwmChip.setLED(deviceNumber, visibleToAbs(linR), visibleToAbs(linG), visibleToAbs(linB));
    pwmChip.write();
}

void LEDDriver::stringifyFloats(float r, float g, float b, char* output) {

	//stringify the floats #arduinoproblems
    char rStr[20];
    char gStr[20];
    char bStr[20];
    dtostrf(r, 4, 7, rStr);
    dtostrf(g, 4, 7, gStr);
    dtostrf(b, 4, 7, bStr);
    sprintf(output, "R: %s, G: %s, B: %s", rStr, gStr, bStr);
}
	
void LEDDriver::setRGB(uint16_t r, uint16_t g, uint16_t b, int fadeDuration) {

    Serial.println(("Starting setRGB -- R: %u, G: %u, B: %u", r, g, b));
    if(fadeDuration == 0) {
      linR = r;
      linG = g;
      linB = b;
      pwmChip.setLED(deviceNumber, visibleToAbs(linR), visibleToAbs(linG), visibleToAbs(linB));
      pwmChip.write();
    } else {
      long unsigned int millisecs = fadeDuration*1000/6; //hack!  
      float dR = (r - linR)/millisecs;
      float dG = (g - linG)/millisecs;
      float dB = (b - linB)/millisecs;
  	
  	  //advance along linear scale while setting LEDs according to transform
      for(int i=0; i < millisecs; i++) {
          linR += dR;
          linG += dG;
          linB += dB;
          pwmChip.setLED(deviceNumber, visibleToAbs(linR), visibleToAbs(linG), visibleToAbs(linB));
          pwmChip.write();
          delay(1);
      }
    }
	    
  	//counter long term drift in floats
  	if(linR < 0)
  		linR = 0;
  	if(linG < 0)
  		linG = 0;
  	if(linB < 0)
  		linB = 0;
    if(linR > pwmMax)
      linR = pwmMax;
    if(linG > pwmMax)
      linG = pwmMax;
    if(linB > pwmMax)
      linB = pwmMax;
    Serial.println("Ending setRGB");
}
