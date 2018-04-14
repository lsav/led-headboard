#include "command.h"

//#define HEADBOARD_COMMAND_DEBUG
#ifdef HEADBOARD_COMMAND_DEBUG
  #define ASM_DEBUG(s) Serial.print(s)
#else
  #define ASM_DEBUG(s)
#endif

Colour::Colour(float r, float g, float b) : r(r), g(g), b(b) {}

Transition::Transition(const Colour& start, const Colour& target,
    float millisFade) {

  unsigned long timeSteps = (unsigned long)((millisFade * LOOP_FREQ_HZ) / 1000);
  steps = max(timeSteps, 1);
  dR = (target.r - start.r) / steps;
  dG = (target.g - start.g) / steps;
  dB = (target.b - start.b) / steps;
  ASM_DEBUG("Transition with step count: ");
  ASM_DEBUG(steps);
  ASM_DEBUG("\n");
}

bool Transition::isComplete() {
  return steps == 0;
}

void Transition::step(Colour& current) {
  ASM_DEBUG("Stepping with step number: ");
  ASM_DEBUG(steps);
  ASM_DEBUG("\n");
  if (steps != 0) {
    current.r += dR;
    current.g += dG;
    current.b += dB;
    steps -= 1;
  }
}

Command::Command() : lamp(), current(0, 0, 0) {
  transitions = new queue<Transition>();
}

void Command::begin() {
  lamp.begin();
  off();
}

/**
  * tokenize data into command, data and duration
  * set the state to the new command
  */
void Command::parse(const char* data) {

  if (data[0] == '/0') {
    ASM_DEBUG("Received null string\n");
    return;
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
      return;
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
    ASM_DEBUG("Command: sunrise\n");
    ASM_DEBUG("t:");
    ASM_DEBUG(tokens[1]);
    ASM_DEBUG("\n");
    sunrise(atof(tokens[1]));
  } else {
    return;
  }
}

/**
  * Progress with currently executing command
  */
void Command::execute() {

  if (transitions->empty()) {
    ASM_DEBUG("no change required\n");
    return;
  }

  Transition* t = transitions->front();
  if (t == NULL) {
    ASM_DEBUG("Got a null transition!\n");
  }
  t->step(current);

  if (t->isComplete()) {
    transitions->pop();
  }
  delete t;

  lamp.setRGB(current.r, current.g, current.b);
  ASM_DEBUG("Executing current transition - queue size: ");
  ASM_DEBUG(transitions->size());
  ASM_DEBUG("\n");
}

void Command::clearTransitions() {
  ASM_DEBUG("Clearing queue\n");
  while (!transitions->empty()) {
    Transition* t = transitions->front();
    transitions->pop();
    delete t;
    ASM_DEBUG("Clearing queue - current size: ");
    ASM_DEBUG(transitions->size());
    ASM_DEBUG("\n");
  }
}

void Command::addTransition(const Colour& start, const Colour& target, double millisFade) {
  ASM_DEBUG("Adding transition with fade length: ");
  ASM_DEBUG(millisFade);
  ASM_DEBUG("\n");
  Transition* t = new Transition(start, target, (float) millisFade);
  transitions->enqueue(t);
  ASM_DEBUG("Adding transition - current queue size: ");
  ASM_DEBUG(transitions->size());
  ASM_DEBUG("\n");
}

void Command::on() {
  clearTransitions();
  addTransition((const Colour&) current, COOL_WHITE, 1000.0);
}

void Command::off() {
  clearTransitions();
  addTransition((const Colour&) current, LIGHTS_OFF, 500.0);
}

void Command::sunrise(float millisFade) {
  clearTransitions();
  addTransition((const Colour&) current, RED, 0.3 * millisFade);
  addTransition(RED, ORANGE, 0.3 * millisFade);
  addTransition(ORANGE, WARM_WHITE, 0.3 * millisFade);
  addTransition(WARM_WHITE, COOL_WHITE, 0.1 * millisFade);
}
