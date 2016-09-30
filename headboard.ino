#include "led-driver.h"
#include "command.h"

const char EOL = ';';
String readline = "";
LEDDriver driver = LEDDriver();

void setup() {
    Serial.begin(9600);
    Serial.println("Headboard PWM Test"); // ASM debug
    driver.begin();
}

void loop() {

    while (Serial.available()) {
        readline = Serial.readStringUntil(EOL);
        if(driver.parse(readline.c_str()) == -1) {
            Serial.println("BAD COMMAND");
        } else {
            Serial.println("GOOD COMMAND YAY");
        }
    }

}
