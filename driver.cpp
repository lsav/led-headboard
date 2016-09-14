#include "driver.h"

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

int LEDDriver::parse(const char* data) {
  // tokenize data into command, data and duration
  // run the command
  // return -1 on error
  
  if(data[0] == '/0')
    return -1;
  const int MAX_TOKENS = 5;
  const char SEP = ' ';
  char input_buffer[256];
  int i = 0;
  //prevent overflow from strcpy
  char ch = '1';
  while(ch != NULL) {
    ch = data[i];
    i++;
    if(i > 255) {
      return -1;
    }
  }
  char* command;
  strcpy(input_buffer, data);
  char* tokens[MAX_TOKENS];
  int numTokens = 1;
  tokens[0] = strtok(input_buffer, &SEP);
  // break data into several smaller strings in place
  for(int i = 1; i<MAX_TOKENS; i++) {
    //find the first instance of SEP
    command = strtok(NULL, &SEP);
    if(command == NULL) {
      break;
    }
    tokens[i] = command;
    numTokens++;
  }

  for(int i=0; i < numTokens; i++) {
    Serial.println(("Token: %s", tokens[i]));
  }
  
  // branch off on tokens[0]
  if(strcmp(tokens[0], "on") == 0) {
    sunrise(1); // go to default warm white quickly
  } else if(strcmp(tokens[0], "off") == 0){
    setRGB(0, 0, 0, 1);
  } else if(strcmp(tokens[0], "party") == 0) {
    partyMode();
  } else if(strcmp(tokens[0], "set") == 0) {
    // set r g b [t]
    if(numTokens == 5) {
      setRGB(atof(tokens[1]), atof(tokens[2]), atof(tokens[3]), atof(tokens[4]));
    } else if(numTokens == 4) {
      setRGB(atof(tokens[1]), atof(tokens[2]), atof(tokens[3]), 0);
    } else
      return -1;
  } else if((strcmp(tokens[0], "sunrise") == 0) && (numTokens == 2)) {
    // sunrise t
    sunrise(atof(tokens[1]));
  } else {
    return -1;
  }
  return 0;
}

void LEDDriver::sunrise(float fadeDuration) {
  
  if(r + g + b != 0) // start from off only
    setRGB(0, 0, 0, 1);
  
  setRGB(1, .8, .7, fadeDuration); //warm white eh?
  }

void LEDDriver::partyMode() {}


LEDDriver::LEDDriver(int deviceNumber, int clock, int data, float r, float g, float b) :
  deviceNumber(deviceNumber), r(r), g(g), b(b), clock(clock), data(data),
  pwmChip(numChips, clock, data) {
    
    rebindColours();
}

void LEDDriver::begin() {
  rebindColours();
  pwmChip.begin();
  pwmChip.setLED(deviceNumber, rMap(), gMap(), bMap());
  pwmChip.write();
}

void LEDDriver::setRGB(float targetR, float targetG, float targetB, float fadeDuration) {

    if(fadeDuration == 0) {
  		r = targetR;
  		g = targetG;
  		b = targetB;
  		pwmChip.setLED(deviceNumber, rMap(), gMap(), bMap());
  		pwmChip.write();
    } else {
      double steps = fadeDuration*100;
      float dR = (targetR - r)/steps;
      float dG = (targetG - g)/steps;
      float dB = (targetB - b)/steps;
      
  	  //advance along linear scale while setting LEDs according to transform
      //SPI writes take about 1.15 ms with simple calculations
      //exponential calculations drive that up substantially to 2.05ms per write
      unsigned long us = micros();
      for(int i=0; i < steps; i++, r+=dR, g+=dG, b+=dB) {
        //pwmChip.setLED(deviceNumber, pwmMax*(1-r), pwmMax*(1-g), pwmMax*(1-b));
        pwmChip.setLED(deviceNumber, rMap(), gMap(), bMap());
        //pwmChip.setLED(deviceNumber, 0, 0, 0);
        pwmChip.write();
        delay(8); //approximates the 10ms period
      }
      unsigned long dus = micros()-us;
      Serial.println("  time to complete " + String((steps), 1) + " steps: " + String((dus/1000000.0), 2) + " s");
      Serial.println("  per write(): " + String((dus/steps), 1) + " us"); //should be around 10ms
    }
	rebindColours();
}
