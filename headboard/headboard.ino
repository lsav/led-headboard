#include "command.h"

static const char EOL = ';';
String readline;
Command command;

void setup() {
  Serial.begin(9600);
  command.begin();
  Serial.println("SunInFace firmware finished loading");
}

void loop() {

  if (Serial.available()) {
    readline = Serial.readStringUntil(EOL);
    command.parse(readline.c_str());
  }
  command.execute();
}
