#include "led-driver.h"
#include "command.h"

const char EOL = ';';
String readline = "";
Command command = Command();

void setup() {
  Serial.begin(9600);
  Serial.println("Headboard PWM Test"); // ASM debug
  command.begin();
}

void loop() {

  while (Serial.available()) {
    readline = Serial.readStringUntil(EOL);
    if(command.parse(readline.c_str()) == -1) {
      Serial.println("\nBAD COMMAND");
    } else {
      Serial.println("\nGOOD COMMAND YAY");
    }
  }
}
