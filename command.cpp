#include "command.h"
#include <Arduino.h>

#define HEADBOARD_COMMAND_DEBUG
#ifdef HEADBOARD_COMMAND_DEBUG
  #define ASM_DEBUG(s) Serial.print(s)
#else
  #define ASM_DEBUG(s)
#endif

Command::Command() : lamp() {}

void Command::begin() {
  lamp.begin();
  lamp.setRGB(0,0,0);
}

/**
  * Tokenize data into command, data and duration
  * run the command
  * return -1 on error
  */
int Command::parse(const char* data) {

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
  for(int i = 1; i<MAX_TOKENS; i++) {
    command = strtok(NULL, &SEP);
    if(command == NULL) {
      break;
    }
    tokens[i] = command;
    numTokens++;
  }

  for(int i=0; i < numTokens; i++) {
    ASM_DEBUG(("Token: %s\n", tokens[i]));
  }

  if(strcmp(tokens[0], "lumos") == 0) {
    ASM_DEBUG("Lumos!\n");
    fadeRGB(1, .9, .68, 1); // go to default warm white quickly
  } else if (strcmp(tokens[0], "nox") == 0) {
    ASM_DEBUG("Nox!\n");
    fadeRGB(0, 0, 0, 1);
  } else if(strcmp(tokens[0], "rgb") == 0) {
    ASM_DEBUG("r: ");
    ASM_DEBUG(lamp.r);
    ASM_DEBUG(", g: ");
    ASM_DEBUG(lamp.g);
    ASM_DEBUG(", b: ");
    ASM_DEBUG(lamp.b);
    ASM_DEBUG("\n");
  } else if (strcmp(tokens[0], "party") == 0) {
    partyMode();
  } else if (strcmp(tokens[0], "set") == 0) {
      // set r g b [t]
    if (numTokens == 5) {
      fadeRGB(atof(tokens[1]), atof(tokens[2]), atof(tokens[3]), atof(tokens[4]));
    } else if (numTokens == 4) {
      fadeRGB(atof(tokens[1]), atof(tokens[2]), atof(tokens[3]), 0);
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

void Command::sunrise(float fadeDuration) {

  //FIXME: probably don't need the hacks with hardware v3, investigate
  // hacks ¯\_(ツ)_/¯
  fadeRGB(0.31, 0.31, 0.31, 1);
  fadeDuration *= 0.75;

  // enforce some key colour transitions for prettier sunrise
  fadeRGB(0.42, 0.4, 0.3, fadeDuration*0.3); // orange-red at the 30% mark
  fadeRGB(0.59, 0.45, 0.4, fadeDuration*0.3); // orange-yellow at the 60% mark
  fadeRGB(0.82, 0.6, 0.5, fadeDuration*0.3); // almost there warm white
  fadeRGB(1, 0.88, 0.62, fadeDuration*0.1); // final cooler white
}

void Command::partyMode() {}

void Command::fadeRGB(float r, float g, float b, float fadeDuration) {
  if (fadeDuration == 0) {
    lamp.setRGB(r, g, b);
  } else {
    double steps = fadeDuration*100;
    float dR = (r - lamp.r)/steps;
    float dG = (g - lamp.g)/steps;
    float dB = (b - lamp.b)/steps;

    unsigned long us = micros();
    for (int i=0; i < steps; i++, lamp.r+=dR, lamp.g+=dG, lamp.b+=dB) {
      lamp.setRGB();
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
}
