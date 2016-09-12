#include <Adafruit_TLC59711.h>
#include "driver.h"
#include <SPI.h>

const int EOL = 46;  // period
String readline = "";
LEDDriver driver = LEDDriver();

void setup() {
  //Serial.begin(9600);
  //Serial.println("Headboard PWM Test");
  driver.begin();
}

void loop() {

  //driver.sunrise(10);
  driver.setRGB(65535, 65535, 65535, 2);
  driver.setRGB(65535, 0, 0, 2);
  driver.setRGB(0, 65535, 0, 2);
  driver.setRGB(0, 0, 65535, 2);
  delay(1000);
  //driver.sunrise(10);
  delay(1000);
  driver.setRGB(0,0,0,2);

// */
//  while(Serial.available()) {
//    readline = Serial.readStringUntil(EOL);
//    driver.parse(readline);
//  }
}
