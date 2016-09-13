#include <Adafruit_TLC59711.h>
#include "driver.h"
#include <SPI.h>

const int EOL = 46;  // period
String readline = "";
LEDDriver driver = LEDDriver();

void setup() {
  Serial.begin(9600);
  Serial.println("Headboard PWM Test");
  driver.begin();
}

void loop() {

  driver.setRGB(1, 1, 1, 1);
  delay(1000);
  driver.setRGB(0, 0, 0, 1);
  delay(1000);
  //driver.setRGB(1, 0, 0, 1);
  //driver.setRGB(0, 1, 0, 1);
  //driver.setRGB(0, 0, 1, 1);
  //delay(1000);
  //driver.sunrise(10);

// */
//  while(Serial.available()) {
//    readline = Serial.readStringUntil(EOL);
//    if(driver.parse(readline) == -1)
//      Serial.println("BAD COMMAND");
//  }
}
