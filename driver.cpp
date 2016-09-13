#include "driver.h"

bool isFloat(const char* token) {
  
  float f;
  return(sscanf(token, "%f", &f) != EOF);
}

uint16_t LEDDriver::visibleToAbs(float visible, int base) {
  // convert desired visible brightness to absolute brightness value
  // visible is given as a float between 0 and 1
  // control must be inverted, since hardware is active low "by design"
  //return (uint16_t)(pwmMax*(1-visible));
  return (pwmMax - (uint16_t)((pwmMax/(base-1))*(pow(base, visible) - 1)));
}

uint16_t LEDDriver::rMap() {return visibleToAbs(r, 8);}
uint16_t LEDDriver::gMap() {return visibleToAbs(g, 8);}
uint16_t LEDDriver::bMap() {return visibleToAbs(b, 8);}

//Insure that floating point errors don't accumulate
void LEDDriver::rebindColours() {
	
	if(r < 0)
		r = 0;
	if(r > 1)
		r = 1;
	if(g < 0)
		g = 0;
	if(g > 1)
		g = 1;
	if(b < 0)
		b = 0;
	if(b > 1)
		b = 1;
}

int LEDDriver::parse(String data) {
    // tokenize data into command, data and duration
    // run the command
    // return -1 on error
    if(data.length() == 0) {
      return -1;
    }

    // single word commands
    if(data == "on") {
      sunrise(1); // go to default warm white quickly
    } else if(data == "off")  {
      setRGB(0, 0, 0, 1);
    } else if(data == "party") {
      partyMode();
    } else {
      // multipart commands - split on whitespace
      // "set r g b [t]" and "sunrise [t]"
      String tokens[10];
      int numTokens = 0;
      for(int i = 0; i < data.length(); i++) {
        if(sscanf(data.c_str(), "%s", tokens[i].c_str()) == EOF)
          return -1;
        numTokens++;
      }
      for(int i=1; i < numTokens; i++) {
        if(!isFloat(tokens[i].c_str()))
          return -1;
      }
      if((tokens[0] == "set")) {
        //set
        if(numTokens == 5) {
          setRGB(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()), atoi(tokens[4].c_str()));
        } else if(numTokens == 4) {
          setRGB(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()), 0);
        }
      } else if((numTokens == 2) && (tokens[0] == "sunrise")) {
          sunrise(atof(tokens[1].c_str()));
      } else
        return -1;
    }
    
    return 0;
}

void LEDDriver::sunrise(float fadeDuration) {
  
  if(r + g + b != 0) // start from off only
    setRGB(0, 0, 0, 1);
  
  setRGB(1, .6, .8, fadeDuration); //warm white eh?
  }

void LEDDriver::partyMode() {}


LEDDriver::LEDDriver(int deviceNumber, int clock, int data, float r, float g, float b) :
  deviceNumber(deviceNumber), r(r), g(g), b(b), clock(clock), data(data),
  pwmChip(numChips, clock, data) {
    
    rebindColours();
    /*
    for(uint16_t i=0; i<pwmMax; i++) {
      float vis = i/pwmMax;
      rMap[i] = visibleToAbs(vis, 16);
      gMap[i] = visibleToAbs(vis, 16);
      bMap[i] = visibleToAbs(vis, 16);
    }
    */
}

void LEDDriver::begin() {
  rebindColours();
  pwmChip.begin();
  pwmChip.setLED(deviceNumber, rMap(), gMap(), bMap());
  pwmChip.write();
}

void LEDDriver::setRGB(float targetR, float targetG, float targetB, float fadeDuration) {

    //targetR = (uint16_t)(pwmMax*targetR);
    //targetG = (uint16_t)(pwmMax*targetG);
    //targetB = (uint16_t)(pwmMax*targetB);
    if(fadeDuration == 0) {
		r = targetR;
		g = targetG;
		b = targetB;
		pwmChip.setLED(deviceNumber, rMap(), gMap(), bMap());
    //pwmChip.setLED(deviceNumber, rMap[r], gMap[g], bMap[b]);
		pwmChip.write();
    } else {
      double steps = fadeDuration*1000;
      float dR = (targetR - r)/steps;
      float dG = (targetG - g)/steps;
      float dB = (targetB - b)/steps;
      
  	
  	  //advance along linear scale while setting LEDs according to transform
      //SPI writes take about 1.15 ms with simple calculations
      //exponential calculations drive that up substantially to 2.05ms per write
      //adding a lookup table for the exponential values will not be much worse than simple calcs, if at all
      unsigned long us = micros();
      for(int i=0; i < steps; i++, r+=dR, g+=dG, b+=dB) {
      //for(int i=0; i < steps; i++) {
        //pwmChip.setLED(deviceNumber, pwmMax*(1-r), pwmMax*(1-g), pwmMax*(1-b));
        pwmChip.setLED(deviceNumber, rMap(), gMap(), bMap());
        //pwmChip.setLED(deviceNumber, 0, 0, 0);
        pwmChip.write();
      }
      unsigned long dus = micros()-us;
      Serial.println("  time to complete " + String((steps), 1) + " steps: " + String((dus/1000000.0), 2) + " s");
      Serial.println("  per write(): " + String((dus/steps), 1) + " us");
    }
	rebindColours();
}
