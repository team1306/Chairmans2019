#include <FastLED.h>
#include <Servo.h>

void setup() {
  // put your setup code here, to run once:
  int limitSwitchPin = 2;
  pinMode(limitSwitchPin, INPUT); 
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(limitSwitchPin) == LOW){
      cim.write(180);
      delay(1000);
      cim.write(90);
  }
}
