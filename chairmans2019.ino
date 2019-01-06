/*
  chairmans2019.ino - Code to run on Chairmans Visual for the 2019 season
  authors - Egan Johnson, Mateo Silver

  Arduino Pins / Connections in use
  Pin 9  - Victor PWM Output
  Pin 20 - SDA (Trellis)
  Pin 21 - SCL (Trellis)
  Pin 41 - Din (LED Strip)
  Pin A2 - INT (Trellis)

  Power                                                   ┌──>Trellis 5V
                                ┌─>5V──>Breadboard 5V Rail┼──>LED Strip 5V
    Robot Battery─>Breaker─>PDP─┤                         └──>Arduino Vin
                                └─>40A─>Victor─>CIM

     Trellis Buttons
     +---+---+---+---+
     | > | > | > | > |      > is go to next step
     +---+---+---+---+      < is go back a step
     | > | > | > | > |      - is reset
     +---+---+---+---+
     |   |   |   |   |
     +---+---+---+---+
     | < |   |   | - |      *command triggers on button release
     +---+---+---+---+

*/

#include <FastLED.h>
#include <Wire.h>
#include "Adafruit_Trellis.h"
#include <Servo.h>

#define debug

//Leds
const int nLEDS = 22; //How many LED's we have
const int segmentSize = 20; //How many LED's are in each gear/segment
const int nSegments = 5; //How many segments are in the whole thing
const float topMotorSpeed=0.25;
const int stepDurations[]={100,100,100,100,100};
CRGB leds[nLEDS];
const int LED_PIN = 42;
CRGB current = CRGB(230, 20, 20);
CRGB past = CRGB(255, 255, 255);
CRGB future = CRGB(0, 0, 0);

//Trellis
const int TRELLIS_INT_PIN = A2;
const int TRELLIS_NUM_KEYS = 16;
Adafruit_Trellis matrix0 = Adafruit_Trellis();
Adafruit_TrellisSet trellis = Adafruit_TrellisSet(&matrix0);

//Motor
const uint32_t period = 3 * 1000; //How many seconds (3) the wheel must turn in between stages
Servo cim;
const int CIM_PIN = 9;
int stage = 0;

void setup() {
#ifdef debug
  Serial.begin(9600);
  Serial.println("Beginning led operations");
#endif
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, nLEDS);
  for (int i = 0; i < nLEDS; i++) {
    leds[i] = current;
#ifdef debug
    String message = "|Setting led ";
    message.concat(i);
    Serial.println(message);
#endif
  }
  FastLED.show();
#ifdef debug
  Serial.println("Ending LED operations");
  Serial.println("Beginning Trellis");
#endif
  trellis.begin(0x74);
  trellis.readSwitches();
  trellisBootLEDs();
#ifdef debug
  Serial.println("|Setting Trellis LED's");
#endif
  for (int i = 0; i < 8; i++) {
#ifdef debug
    String message = "||Setting trellis led ";
    message.concat(i);
    Serial.println(message);
#endif
    trellis.setLED(i);
  }
#ifdef debug
  Serial.println("||Setting trellis led 15");
  Serial.println("||Setting trellis led 12");
#endif
  trellis.setLED(15);
  trellis.setLED(12);
  cim.attach(CIM_PIN);
}

void loop() {
#ifdef debug
  Serial.print("Looping");
#endif
  //To avoid accidents,buttons activate on release.
  //If you press a button too early, simply hold it until you need it.
  int button = -1;
  #ifdef debug
  Serial.print("button: "+button);
  #endif

  for (uint8_t i = 0; i < TRELLIS_NUM_KEYS; i++) {
  if (trellis.justPressed(i)) {    
    #ifdef debug
    Serial.print("Button v"); Serial.println(i);
    #endif
    trellis.setLED(i);
  } 
  if (trellis.justReleased(i)) {
    #ifdef debug
    Serial.print("Button ^"); Serial.println(i);
    #endif
    trellis.clrLED(i);
          button = i;
  }
#ifdef debug
      Serial.println("Button pressed- Button " + i);
#endif
    
  }
  if (!(button == -1)) {
    if (button < 8) {
      increment(1);
    }
    if (button == 12) {
      increment(-1);
    }
    if (button == 15) {
      increment(-stage);   //this should work, right? -mateo
      //Yes, but its called stage and is changed in-methods - Egan
    }
  }
  trellis.readSwitches();//Reset the state of the trellis. Am i doing trellis right? -Egan
}
/**
  Moves the stage by i. if I is one it will move forward one stage (one rotation, one led group).
  If i is negative one, it will move back one. If the stage is then greater than the last stage,
  it will call end(), which will animate the LED's and set the gears to moving.
  In the normal increment, the gears will spin for a set amount of time, so we must use some timing
  to ensure that differences in setting LED's does not change the distance rotated.
*/
void increment(int i) {
  #ifdef debug
        Serial.print("increment fx");
  #endif
  stage += i;
  CRGB originalValues[nSegments];
  CRGB endValues[nSegments];

  //copy segment values into original and decide new segment
  for (int seg = 0; seg < nSegments; seg++) {
    originalValues[seg] = getSegment(seg);
    if (seg < stage) {
      endValues[seg] = past;
    }
    else if (seg == stage) {
      endValues[seg] = current;
    }
    else {
      endValues[seg] = future;
    }
  }
  int startTime = micros();
  int speed=0;
  if (i > 0) {
    speed=topMotorSpeed;
  } else {
    speed=-topMotorSpeed;
  }
  setMotor(speed);
  int duration = sumRange(stepDurations,i,stage-i);
  while (micros() < startTime + duration) {
    int t = micros(); //Initialize time for uniform brighness
    for (int seg = 0; seg < nSegments; seg++) {
      //set each to the blend of its original and end values with the more time giving more weight to the end color.
      //The abs() is there to ensure that the delay in the two micros() calls above does not cause a negative value.
      setSegment(seg, blendColors(endValues[seg], originalValues[seg], t, abs(startTime + duration - startTime)));
    }
    FastLED.show();
  }
  setMotor(-speed);
  delay(micros()-(startTime + duration));//backtrack ammount duration overshot by
  setMotor(0);
  
}


//LED utilities
CRGB RED = CRGB(230, 20, 20); //Badgerbots Red color
CRGB WHITE = CRGB(255, 255, 255);
/**
  Sets a segment of leds to the crgb. A block is the LED's behind a given gear or location.
  DOES NOT update live leds
*/
void setSegment(int i, CRGB c) {
  for (int ind = i * segmentSize; ind < i * segmentSize + segmentSize; i++) {
    leds[ind] = c;
  }
}
void setSegment(int i, CRGB c, CRGB destination[]) {
  for (int ind = i * segmentSize; ind < i * segmentSize + segmentSize; i++) {
    destination[ind] = c;
  }
}
/**
   Gets the value for a segment
*/
CRGB getSegment(int i) {
  return leds[i * segmentSize];
}
CRGB getSegment(int i, CRGB source[]) {
  return source[i * segmentSize];
}
/**
   Finds the blend of colors c1 and c2. The ratio of c1 to c2 is determined by alpha1 to alpha2.
   Alpha 1 and Alpha2 must be positive, but do not have to have a certain sum.
*/
CRGB blendColors(CRGB c1, CRGB c2, double alpha1, double alpha2) {
  double sum = alpha1 + alpha2;
  alpha1 = alpha1 / sum;
  alpha2 = alpha2 / sum;
  CRGB combined = CRGB(0, 0, 0);
  for (int i = 0; i < 3; i++) {
    combined[i] = (byte)(c1[i] * alpha1 + c2[i] * alpha2);
  }
  return combined;
}
//Trellis utilities
/**
   Boot LEDs on trelis
*/
void trellisBootLEDs() {
  for (uint8_t i = 0; i < TRELLIS_NUM_KEYS; i++) {
    trellis.setLED(i);
    trellis.writeDisplay();
    delay(50);
  }
  for (uint8_t i = 0; i < TRELLIS_NUM_KEYS; i++) {
    trellis.clrLED(i);
    trellis.writeDisplay();
    delay(50);
  }
}

//CIM Utilities
/**
 * Rotates CIM x number of time segments
 */
void rotateMotorFinite(int direction, int steps){
          Serial.print("rotating motor for duration");
    if(direction == 1){
        cim.write(135);  
        delay(period * steps);
        cim.write(85);//included to counter momentum of motor
        delay(10);
        cim.write(90);
        Serial.print("cim 135");
    }
    else if(direction == -1){
        cim.write(45);
        delay(period * steps);
        cim.write(95);
        delay(10);
        cim.write(90);
        Serial.print("cim 45");
    }
}
/**
 * Sets the motor to the direction indicated by the parameter.
 * Lack of blocking 'delay' calls enables synchronization with leds
 * float value of 1 sets to full forward, and float value of -1 sets to full backward.
 */
void setMotor(float value){
  cim.write(90+int(90*value));
}

/**
 * finds the sum of the array values in parameter1 between paramater2 and paramter3.
 * param 2 and 3 represent array indecies, but their order does not matter.
 */
int sumRange(int arr[],int ind1,int ind2){
  int lower=min(ind1,ind2);
  int upper=max(ind1,ind2);
  int sum=0;
  for(int i=lower; i<upper;i++){
    sum+=arr[i];
  }
  return sum;
 }

