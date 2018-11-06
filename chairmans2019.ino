/*
* chairmans2019.ino - Code to run on Chairmans Visual for the 2019 season
* authors - Egan Johnson, Mateo Silver
*
* Arduino Pins / Connections in use
* Pin 9  - Victor PWM Output
* Pin 20 - SDA (Trellis)
* Pin 21 - SCL (Trellis)
* Pin 41 - Din (LED Strip)
* Pin A3 - INT (Trellis)
*
* Power
*                               ┌─> 5V─>Arduino Vin
*   Robot Battery─>Breaker─>PDP─┤
*                               └─>40A─>Victor─>CIM
*
*    Trellis Buttons
*    +---+---+---+---+
*    | > | > | > | > |      > is go to next step
*    +---+---+---+---+      < is go back a step
*    | > | > | > | > |      - is reset
*    +---+---+---+---+
*    |   |   |   |   |
*    +---+---+---+---+
*    | < |   |   | - |      *command triggers on button release
*    +---+---+---+---+
*
*/

#include <FastLED.h>
#include <Wire.h>
#include "Adafruit_Trellis.h"

//Leds
const int nLEDS=100;//How many LED's we have
const int segmentSize=20;//How many LED's are in each gear/segment
const int nSegments=5;//How many segments are in the whole thing
CRGB leds[nLEDS];
const int LED_PIN = 41;
CRGB current=CRGB(230,20,20);
CRGB past=CRGB(255,255,255);
CRGB future=CRGB(0,0,0);

//Trellis
const int TRELLIS_INT_PIN = A3;
const int TRELLIS_NUM_KEYS = 16;
Adafruit_Trellis matrix0 = Adafruit_Trellis();
Adafruit_TrellisSet trellis = Adafruit_TrellisSet(&matrix0);

//Motor
const int timeInRotation=3000;//How many miliseconds the wheel must turn in between stages

int stage = 0;

void setup(){
FastLED.addLeds<NEOPIXEL,LED_PIN>(leds, nLEDS);

trellis.begin(0x70);
trellis.readSwitches();

trellisBootLEDs(); //cool boot animation from last year

//turn on LEDS that have actions
for(int i=0;i<8;i++){
  trellis.setLED(i);
}
trellis.setLED(15);
trellis.setLED(12);
//TODO: Motor
}

void loop(){
    //To avoid accidents,buttons activate on release.
    //If you press a button too early, simply hold it until you need it.
    int button=-1;

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
            increment(-stage);   //this should work, right? -mateo
                                //Yes, but its called stage and is changed in-methods - Egan
        }
    }
        trellis.readSwitches();//Reset the state of the trellis. Am i doing trellis right? -Egan
}
/**
* Moves the stage by i. if I is one it will move forward one stage (one rotation, one led group).
* If i is negative one, it will move back one. If the stage is then greater than the last stage,
* it will call end(), which will animate the LED's and set the gears to moving.
* In the normal increment, the gears will spin for a set amount of time, so we must use some timing
* to ensure that differences in setting LED's does not change the distance rotated.
*/
void increment(int i){
    stage+=i;
    CRGB originalValues[nSegments];
    CRGB endValues[nSegments];
    for(int seg=0;seg<nSegments;seg++){
      originalValues[seg]=getSegment(seg);
      if(seg<stage){
        endValues[seg]=past;
      }
      else if(seg==stage){
        endValues[seg]=current;
      }
      else{
        endValues[seg]=future;
      }
    }
    int startTime=micros();
    if(i>0){
        //set motor forward
    }else{
        //set motor backward
    }
    int duration=abs(i)*timeInRotation
    while(micros()<startTime+duration){
      t=micros();//Initialize time for uniform brighness
      for(int seg=0;seg<nSegments;seg++){
        //set each to the blend of its original and end values with the more time giving more weight to the end color.
        //The abs() is there to ensure that the delay in the two micros() calls above does not cause a negative value.
        setSegment(blendColor(endValues[seg],originalValues[seg],t,abs(startTime+duration-startTime));
      }
      FastLED.show()
    }
    //set motor 0
}



//LED utilities
CRGB RED=CRGB(230,20,20);//Badgerbots Red color
CRGB WHITE=CRGB(255,255,255);
/**
* Sets a segment of leds to the crgb. A block is the LED's behind a given gear or location.
* DOES NOT update live leds
*/
void setSegment(int i,CRGB c){
    for(int ind=i*segmentSize;ind<i*segmentSize+segmentSize;i++){
        leds[ind]=c;
    }
}
void setSegment(int i,CRGB c,CRGB destination[]){
    for(int ind=i*segmentSize;ind<i*segmentSize+segmentSize;i++){
        destination[ind]=c;
    }
}
/**
 * Gets the value for a segment
 */
CRGB getSegment(int i){
  return leds[i*segmentSize];
}
CRGB getSegment(int i, CRGB source[]){
  return source[i*segmentSize];
}
/**
 * Finds the blend of colors c1 and c2. The ratio of c1 to c2 is determined by alpha1 to alpha2.
 * Alpha 1 and Alpha2 must be positive, but do not have to have a certain sum.
 */
CRGB blendColors(CRGB c1, CRGB c2, double alpha1, double alpha2){
  double sum=alpha1+alpha2;
  alpha1=alpha1/sum;
  alpha2=alpha2/sum;
  CRGB combined=CRGB(0,0,0);
  for(int i=0;i<3;i++){
    combined[i]=(byte)(c1[i]*alpha1+c2[i]*alpha2);
  }
 return combined; 
}
//Trellis utilities
/**
 * Boot LEDs on trelis
 */
void trellisBootLEDs() {
  for (uint8_t i=0; i<TRELLIS_NUM_KEYS; i++) {
    trellis.setLED(i);
    trellis.writeDisplay();
    delay(50);
  }
  for (uint8_t i=0; i<TRELLIS_NUM_KEYS; i++) {
    trellis.clrLED(i);
    trellis.writeDisplay();
    delay(50);
  }
}
