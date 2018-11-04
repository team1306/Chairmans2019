  #include <FastLED.h>
  #include <Wire.h>
  #include "Adafruit_Trellis.h"

  //Leds
  const int nLEDS=100;//How many LED's we have
  const int segmentSize=20;//How many LED's are in each gear
  CRGB leds[nLEDS];
  const int LED_PIN = 41;
  CRGB current=CRGB(230,20,20);

  //Trellis
  const int TRELLIS_INT_PIN = A3;
  const int TRELLIS_NUM_KEYS = 16;
  Adafruit_Trellis matrix0 = Adafruit_Trellis();
  Adafruit_TrellisSet trellis = Adafruit_TrellisSet(&matrix0);

  //Motor
  const int timeInRotation=3000;//How many miliseconds the wheel must turn in between stages
/*
 * Arduino Pins / Connections
 * Pin 9  - Victor PWM Output
 * Pin 20 - SDA (Trellis)
 * Pin 21 - SCL (Trellis)
 * Pin 41 - Din (LED Strip)
 * Pin A3 - INT (Trellis)
 * 
 * Power
 *                                  --> 5V -> Arduino Vin
 * Robot Battery -> Breaker -> PDP -|
 *                                  -->40A -> Victor -> CIM
 */

int stage=0;

void setup(){
    FastLED.addLeds<NEOPIXEL,LED_PIN>(leds, nLEDS);
    
    trellis.begin(0x70);
    trellis.readSwitches();
    for(int i=0;i<8;i++){
      trellis.setLED(i);
    }
    trellis.setLED(15);
    trellis.setLED(12);
    //TODO: Motor
}

void loop(){
  int button=-1;//To avoid accidents,buttons activate on release. If you press a
  //button too early, simply hold it until you need it.
  for (uint8_t i=0; i<TRELLIS_NUM_KEYS; i++){
    if(trellis.justReleased(i)){
      button=i;
    }
  }
  if(!(button==-1)){
    if(button<8){
      increment(1);
    }
    if(button==12){
      increment(-1);
    }
    if(button==15){
      //reset
    }
  }
  trellis.readSwitches();//Reset the state of the trellis
}
/**
 * Moves the stage by i. if I is one it will move forward one stage (one rotation, one led group).
 * If i is negative one, it will move back one. If the stage is then greater than the last stage,
 * it will call end(), which will animate the LED's and set the gears to moving. 
 * In the normal increment, the gears will spin for a set amount of time, so we must use some timing
 * to ensure that differences in setting LED's does not change the distance rotated.
 */
void increment(int i){
  int startTime=micros();
  if(i>0){
    //set motor forward
  }else{
    //set motor backward
  }
  while(micros()<startTime+abs(i)*timeInRotation){
    
  }
  //set motor 0
}



//LED utilities
CRGB RED=CRGB(230,20,20);//Badgerbots Red color
CRGB White=CRGB(255,255,255);
/**
 * Sets a segment of leds to the crgb. A block is the LED's behind a given gear or location.
 * DOES NOT update live leds
 */
void setSegment(int i,CRGB c){
  for(int ind=i*segmentSize;ind<i*segmentSize+segmentSize;i++){
    leds[ind]=c;
  }
}

