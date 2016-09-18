#include "driver.h"

// convert desired visible brightness to absolute brightness value
// visible is given as a float between 0 and 1
// control must be inverted, since hardware is active low "by design"
// return (uint16_t)(pwmMax*(1-visible));
uint16_t LEDDriver::visibleToAbs(float visible, int base) {
    return (pwmMax - (uint16_t)((pwmMax/(base-1))*(pow(base, visible) - 1)));
}

// this LED string is really really blue for some reason, need to compensate
// these numbers seem to give the nicest looking transitions

uint16_t LEDDriver::rMap() {return visibleToAbs(r, 10);}
uint16_t LEDDriver::gMap() {return visibleToAbs(g*.92, 10);}
uint16_t LEDDriver::bMap() {return visibleToAbs(b*.76, 8);}

// ensure that floating point errors don't accumulate
void LEDDriver::rebindColours() {
    if (r < 0)
		r = 0;
	if (r > 1)
		r = 1;
	if (g < 0)
		g = 0;
	if (g > 1)
		g = 1;
	if (b < 0)
		b = 0;
	if (b > 1)
		b = 1;
}

// tokenize data into command, data and duration
// run the command
// return -1 on error
int LEDDriver::parse(const char* data) {

    if (data[0] == '/0')
        return -1;
    const int MAX_TOKENS = 5;
    const char SEP = ' ';
    char input_buffer[256];
    int i = 0;
  
    // prevent overflow from strcpy
    char ch = '1';
    while (ch != NULL) {
        ch = data[i];
        i++;
    if (i > 255) {
        return -1;
        }
    }
  
    char* command;
    strcpy(input_buffer, data);
    char* tokens[MAX_TOKENS];
    int numTokens = 1;
    tokens[0] = strtok(input_buffer, &SEP);
  
    // break data into several smaller strings in place
    for (int i = 1; i<MAX_TOKENS; i++) {
        // find the first instance of SEP
        command = strtok(NULL, &SEP);
        if(command == NULL) {
            break;
        }
        tokens[i] = command;
        numTokens++;
    }
  
    // debug via Arduino Serial Monitor
    for (int i=0; i < numTokens; i++) {
        Serial.println(("Token: %s", tokens[i]));
    }

    // branch off on tokens[0]
    if (strcmp(tokens[0], "lumos") == 0) {
        setRGB (1, .9, .68, 1); // go to default warm white quickly
    } else if (strcmp(tokens[0], "nox") == 0) {
        setRGB (0, 0, 0, 1);
    } else if (strcmp(tokens[0], "rgb") == 0) { // debug via ASM
        Serial.print("r: ");
        Serial.print(r);
        Serial.print(", g: ");
        Serial.print(g);
        Serial.print(", b: ");
        Serial.print(b);
        Serial.println();
    } else if (strcmp(tokens[0], "party") == 0) {
        partyMode();
    } else if (strcmp(tokens[0], "set") == 0) {
        // set r g b [t]
        if (numTokens == 5) {
            setRGB(atof(tokens[1]), atof(tokens[2]), atof(tokens[3]),
                atof(tokens[4]));
        } else if (numTokens == 4) {
            setRGB(atof(tokens[1]), atof(tokens[2]), atof(tokens[3]), 0);
        } else
            return -1;
    } else if ((strcmp(tokens[0], "sunrise") == 0) && (numTokens == 2)) {
        // sunrise t
        sunrise(atof(tokens[1]));
    } else {
        return -1;
    }
    
    return 0;
}

void LEDDriver::sunrise(float fadeDuration) {

    // hacks ¯\_(ツ)_/¯
    setRGB(0.31, 0.31, 0.31, 1);
    fadeDuration *= 0.75;

    // enforce some key colour transitions for prettier sunrise
    setRGB(0.42, 0.4, 0.3, fadeDuration*0.3); // orange-red at the 30% mark
    setRGB(0.59, 0.45, 0.4, fadeDuration*0.3); // orange-yellow at the 60% mark
    setRGB(0.82, 0.6, 0.5, fadeDuration*0.3); // almost there warm white
    setRGB(1, 0.88, 0.62, fadeDuration*0.1); // final cooler white
}

void LEDDriver::partyMode() {}

LEDDriver::LEDDriver(int deviceNumber, int clock, int data, float r, float g,
    float b) :
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

void LEDDriver::setRGB(float targetR, float targetG, float targetB,
    float fadeDuration) {

    if (fadeDuration == 0) {
        // instant colour change
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

	// advance along linear scale while setting LEDs according to transform
    // SPI writes take about 1.15 ms with simple calculations
    // exponential calculations drive that up substantially to 2.05 ms per write
    unsigned long us = micros();
    for (int i=0; i < steps; i++, r+=dR, g+=dG, b+=dB) {
        pwmChip.setLED(deviceNumber, rMap(), gMap(), bMap());
        pwmChip.write();
        delay(8); // total for 1 cycle ends up being a little over 10 ms
    }
    
    // time check (for debugging via ASM)
    /*
    unsigned long dus = micros()-us;
    Serial.println("  time to complete " + String((steps), 1) +
        " steps: " + String((dus/1000000.0), 2) + " s");
    Serial.println("  per write(): " + String((dus/steps), 1) + " us");
    */
  }
  
	rebindColours();
}
