#include "driver.h"

using namespace std;

uint16_t LEDDriver::visibleToAbs(float visible, int base) {
  // convert desired visible brightness to absolute brightness value
  return (pwmMax - (uint16_t)(visible));
  //double power = (visible)/pwmMax;
  //return (pwmMax - (uint16_t)((pwmMax/(base-1))*(pow(base, power) - 1)));
}
//Luma conversion for RGB used in HDTV signal processing
//Y = 0.2126 R + 0.7152 G + 0.0722 B

uint16_t LEDDriver::rMap() {
	
	return visibleToAbs(r, 4);
}

uint16_t LEDDriver::gMap() {
	
	return visibleToAbs(g, 4);
}

uint16_t LEDDriver::bMap() {
	
	return visibleToAbs(b, 4);
}

//Insure that floating point errors don't accumulate
void LEDDriver::rebindColours() {
	
	if(r < 0)
		r = 0;
	if(r > pwmMax)
		r = pwmMax;
	if(g < 0)
		g = 0;
	if(g > pwmMax)
		g = pwmMax;
	if(b < 0)
		b = 0;
	if(b > pwmMax)
		b = pwmMax;
}

void LEDDriver::parse(String data) {
    //tokenize data into command, data and duration
    //int fade = stoi(data);
    if(data == "on") {
	  setRGB(pwmMax, pwmMax, pwmMax, 2);
    } else if(data == "off") {
      setRGB(0, 0, 0, 2);
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
  
  setRGB(65535, 61937, 57568, fadeDuration); //warm white eh?
  //setRGB(30000, 35000, 10000, t1); //a red dawn rises
  //setRGB(55000, 45000, 25000, t2); //yellow it out...
  //setRGB(65535, 50000, 35000, t2); // lighten a ton
  
  }

void LEDDriver::partyMode() {}


LEDDriver::LEDDriver(int deviceNumber, int clock, int data, float r, float g, float b) :
    deviceNumber(deviceNumber), r(r), g(g), b(b), clock(clock), data(data),
	pwmChip(numChips, clock, data) {}

void LEDDriver::begin() {
    pwmChip.begin();
    pwmChip.write();
	rebindColours();
    pwmChip.setLED(deviceNumber, rMap(), gMap(), bMap());
    pwmChip.write();
}

void LEDDriver::setRGB(float targetR, float targetG, float targetB, int fadeDuration) {

    //Serial.println("Starting setRGB");
    if(fadeDuration == 0) {
		r = targetR;
		g = targetG;
		b = targetB;
		pwmChip.setLED(deviceNumber, rMap(), gMap(), bMap());
		pwmChip.write();
    } else {
      double millisecs = fadeDuration*1000;
      float dR = (targetR - r)/millisecs;
      float dG = (targetG - g)/millisecs;
      float dB = (targetB - b)/millisecs;
  	
  	  //advance along linear scale while setting LEDs according to transform
      for(int i=0; i < millisecs; i++, r+=dR, g+=dG, b+=dB) {
          pwmChip.setLED(deviceNumber, rMap(), gMap(), bMap());
          pwmChip.write();
          delay(1);
      }
    }
	rebindColours();
    //Serial.println("Ending setRGB");
}
