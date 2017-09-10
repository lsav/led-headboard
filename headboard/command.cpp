#include "command.h"

#define HEADBOARD_COMMAND_DEBUG
#ifdef HEADBOARD_COMMAND_DEBUG
  #define ASM_DEBUG(s) Serial.print(s)
#else
  #define ASM_DEBUG(s)
#endif

State::State() : r(0), g(0), b(0) {}
State::State(double r, double g, double b) : r(r), g(g), b(b) {}

Command::Command() : lamp(), current(0, 0, 0), lightsOff(0, 0, 0),
  red(0.3, 0.1, 0.0), orange(0.55, 0.35, 0.15), warmWhite(0.7, 0.5, 0.2),
  coolWhite(1, 0.8, 0.4) {}

void Command::begin() {
  lamp.begin();
  off();
}

/**
  * tokenize data into command, data and duration
  * set the state to the new command
  * return 0 if new command
  * return -1 if not a command
  */
int Command::parse(const char* data) {

  if (data[0] == '/0') {
    ASM_DEBUG("Received null string\n");
    return -1;
  }
  const int MAX_TOKENS = 2;
  const char SEP = ' ';
  char input_buffer[256];
  int i = 0;

  // prevent overflow from strcpy
  char ch = '1';
  while (ch != NULL) {
    ch = data[i];
    i++;
    if (i > 255) {
      ASM_DEBUG("Error: command too large\n");
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
    ASM_DEBUG("Tokens:\n");
    ASM_DEBUG(tokens[i]);
    ASM_DEBUG("\n");
  }

  if(strcmp(tokens[0], "lumos") == 0) {
    ASM_DEBUG("Command: lumos\n");
    on();
  } else if (strcmp(tokens[0], "nox") == 0) {
    ASM_DEBUG("Command: nox\n");
    off();
  } else if(strcmp(tokens[0], "rgb") == 0) {
    ASM_DEBUG("Command: rgb\n");
    ASM_DEBUG("r: ");
    ASM_DEBUG(current.r);
    ASM_DEBUG(", g: ");
    ASM_DEBUG(current.g);
    ASM_DEBUG(", b: ");
    ASM_DEBUG(current.b);
    ASM_DEBUG("\n");
  } else if ((strcmp(tokens[0], "sunrise") == 0) && (numTokens == 2)) {
    // sunrise t
    ASM_DEBUG("Command: sunrise");
    sunrise(atol(tokens[1]));
  } else {
    return -1;
  }
  return 0;
}

/**
  * Progress with currently executing command
  */
void Command::execute() {
  if (current.isComplete()) {
    current.nextState();
  } else {
    current.step();
  }

  lamp.setRGB(current.r, current.g, current.b);
}

void Command::on() {
  current.setTransition(coolWhite, ONE_SECOND_MILLIS);
  lightsOff.next = &lightsOff;
}

void Command::off() {
  current.setTransition(lightsOff, ONE_SECOND_MILLIS);
}

void Command::sunrise(unsigned long fadeMillis) {

  ASM_DEBUG("\n");
  ASM_DEBUG(fadeMillis);
  ASM_DEBUG((unsigned long) 0.3*fadeMillis);
  ASM_DEBUG("\n");
  
  current.setTransition(red, (unsigned long) 0.3*fadeMillis);
  red.setTransition(orange, (unsigned long) 0.3*fadeMillis);
  orange.setTransition(warmWhite, (unsigned long) 0.3*fadeMillis);
  warmWhite.setTransition(coolWhite, (unsigned long) 0.1*fadeMillis);
}

/**
  * Calculate and set the state transition parameters
  */
void State::setTransition(const State& target, unsigned long millisFade) {
  next = &target;
  steps = max(millisFade * LOOP_FREQ_KHZ, 1);
  dR = (target.r - r) / steps;
  dG = (target.g - g) / steps;
  dB = (target.b - b) / steps;
}

/**
  * Copy target related information from next state
  */
void State::nextState() {
  ASM_DEBUG("next state\n");
  if (next == nullptr) {
    return;
  }
  steps = next->steps;
  dR = next->dR;
  dG = next->dG;
  dB = next->dB;
  next = next->next;
}

bool State::isComplete() {
  return (steps == 0 || (r == next->r && g == next->g && b == next->b));
}

void State::step() {
  r += dR;
  g += dG;
  b += dB;
  steps -= 1;
  ASM_DEBUG(steps);
  ASM_DEBUG("\n");
}
