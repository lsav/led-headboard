#include <Adafruit_TLC59711.h>
#include "driver.h"
#include <SPI.h>

const int EOL = 46;  // period
String readline = "";
//Adafruit_TLC59711 tlc = Adafruit_TLC59711(1, 13,12);
LEDDriver driver = LEDDriver();

void setup() {
  Serial.begin(9600);
  Serial.println("Headboard PWM Test");
  //tlc.begin();
  //tlc.write();
  driver.begin();
}

void loop() {
///*
  //driver.setRGB(0, 0, 0, 3); // full off
  //driver.setRGB(0, 0, 65535, 3); // full blue
  //delay(2000);
  
  //delay(2000);
  //driver.setRGB(65535, 0, 0, 3); // full red
  //delay(2000);
  //driver.setRGB(65535, 65535, 65535, 3); // full on
  //delay(2000);
  driver.sunrise(30);
  delay(2000);
  //delay(5000);
  
  
  //delay(2000);
  //driver.setRGB(65535, 65535, 65535, 3); // full off
  //delay(2000);
  //driver.setRGB(65535, 0, 65535, 3); // full green
  //delay(2000);
  /*
  driver.setRGB(65535, 65535, 65535, 3); // full off
  delay(2000);
  driver.setRGB(65535, 65535, 0, 3); // full blue
  delay(2000);
  driver.setRGB(65535, 65535, 65535, 3); // full off
  delay(2000);
  driver.setRGB(0, 0, 65535, 3); // full yellow
  delay(2000);
  driver.setRGB(65535, 65535, 65535, 3); // full off
  delay(2000);
  driver.setRGB(0, 0, 0, 3); // full white
  delay(2000);
  driver.setRGB(65535, 65535, 65535, 3); // full off
  delay(2000);
  driver.setRGB(0, 20000, 65535, 3); // orange?
// */
//  while(Serial.available()) {
//    readline = Serial.readStringUntil(EOL);
//    driver.parse(readline);
//  }
}
