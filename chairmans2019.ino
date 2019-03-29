
/*
  chairmans2019.ino - Code to run on Chairmans Visual for the 2019 season
  authors - Egan Johnson, Mateo Silver, dAVID

  Arduino Pins / Connections in use
  Pin 9  - Victor PWM Output
  Pin 20 - SDA (Trellis)
  Pin 21 - SCL (Trellis)
  Pin 42 - Din (LED Strip)
  Pin A2 - INT (Trellis)
  Pin A7 - Seat motor encoder

  Power                                                       ┌──>Trellis 5V
                                  ┌─>5V──>Breadboard 5V Rail┼──>LED Strip 5V
    Robot Battery─>Breaker─>PDP─┤                           └──>Arduino Vin
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

#include "Adafruit_Trellis.h"
#include <FastLED.h>
#include <Servo.h>
#include <Wire.h>

#define debug

// Leds
const int segmentSize = 33; // How many LED's are in each gear/segment
const int nSegments = 5;    // How many segments are in the whole thing
const int nLEDS = segmentSize * nSegments;   // How many LED's we have
CRGB leds[nLEDS];           // Cmt missing?
const int LED_PIN = 42;
CRGB current = CRGB(255, 0, 0); // changed from (230, 20, 20) bc it was too unsaturated
CRGB past = CRGB(155, 155, 155);
CRGB future = CRGB(0, 0, 0);

// CIM
const float topMotorSpeed = 1.0;
const unsigned long stepDurations[] = {1000, 1000, 1000, 1000, 1000};
const int period = 1000; // unused
bool isSpinning = false;

// Trellis
const int TRELLIS_INT_PIN = A2;
const int TRELLIS_NUM_KEYS = 16;
Adafruit_Trellis matrix0 = Adafruit_Trellis();
Adafruit_TrellisSet trellis = Adafruit_TrellisSet(&matrix0);

// Motor
Servo cim;
const int CIM_PIN = 9;
int stage = 0;
int ANALOG_PIN = A7;

int ANALOG_PIN_NEGATIVE = A7;
int ANALOG_PIN_POSITIVE = A5;
int PULSES_PER_ROTATION = 179;

boolean posOrNeg = false; //true is positive, false is negative
boolean completedRotation = false;

int currPositive = 0;
int currNegative = 0;
int currDifference = 0;
int armatureRotations = 174;
double amtToRotate = 1; //spin 1 rotation per step
int m = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Beginning led operations");
  Serial.println("Debugging mode enabled");
  Serial.println("Debugging mode disabled");
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, nLEDS);
  for (int i = 0; i < nLEDS; i++) {
    leds[i] = current;
    String message = "|Setting led ";
    message.concat(i);
    Serial.println(message);
  }
  FastLED.show();
  Serial.println("Ending LED operations");
  Serial.println("Beginning Trellis");
  trellis.begin(0x74);
  trellis.readSwitches();
  trellisBootLEDs();
  Serial.println("|Setting Trellis LED's");
  for (int i = 0; i < 8; i++) {
    String message = "||Setting trellis led ";
    message.concat(i);
    Serial.println(message);
    trellis.setLED(i);
  }
  Serial.println("||Setting trellis led 15");
  Serial.println("||Setting trellis led 12");
  trellis.setLED(15);
  trellis.setLED(12);

  Serial.println("Attaching CIM as Servo");
  pinMode(ANALOG_PIN_NEGATIVE, INPUT);   //  Set A7 as an input
  pinMode(ANALOG_PIN_POSITIVE, INPUT);   //  Set A5 as an input
  cim.attach(CIM_PIN);
  cim.write(90);                //  Run motor at full speed
  Serial.println("Start");

}

void loop() {
  // Serial.print("Looping");
  // To avoid accidents,buttons activate on release.
  // If you press a button too early, simply hold it until you need it.
  int button = -1;
  Serial.print("button: ");
  Serial.println(button);

  trellis.readSwitches();

  // Turn a pressable button off then on when pressed
  for (int i = 0; i < TRELLIS_NUM_KEYS; i++) {
    if (trellis.isKeyPressed(i) && (i < 7 || i == 12 || i == 15)) {
      button = i;
      trellis.clrLED(i);
      delay(200);
      trellis.setLED(i);
      Serial.print("Button pressed - ");
      Serial.println(i);
    }
  }

  if (button != -1)
  {
    cim.write(180);
  }
  else
  {
    cim.write(90);
  }
  for (uint8_t i = 0; i < numKeys; i++) {
    // if it was pressed, turn it on
    if (trellis.justPressed(i)) {
      Serial.print("v"); Serial.println(i);
//      trellis.setLED(i);
      increment(1);
    }
    // if it was released, turn it off
    if (trellis.justReleased(i)) {
      Serial.print("^"); Serial.println(i);
//      trellis.clrLED(i);
    }
  }


  // Reset the state of the trellis. Am i doing trellis right? -Egan
  // readSwitches() returns a boolean depending on if there's been a change in
  // the state of the trellis since the last call
}
/**
  Moves the stage by i. if I is one it will move forward one stage (one
  rotation, one led group). If i is negative one, it will move back one. If the
  stage is then greater than the last stage, it will call end(), which will
  animate the LED's and set the gears to moving. In the normal increment, the
  gears will spin for a set amount of time, so we must use some timing to ensure
  that differences in setting LED's does not change the distance rotated.
*/
void increment(int i) {
  Serial.print("increment(");
  Serial.print(i);
  Serial.println(") called");
  stage += i;
  CRGB originalValues[nSegments];
  CRGB endValues[nSegments];

  // copy segment values into original and decide new segment
  for (int seg = 0; seg < nSegments; seg++) {
    originalValues[seg] = getSegment(seg);
    if (seg < stage) {
      endValues[nSegments - 1 - seg] = past;
    } else if (seg == stage) {
      endValues[nSegments - 1 - seg] = current;
    } else {
      endValues[nSegments - 1 - seg] = future;
    }
  }
  Serial.println("LED Destinations calculated");
  unsigned long startTime = millis();
  double speed = 0.1;
  /*
    if (i > 0) {
    speed = topMotorSpeed;
    } else {
    speed = -topMotorSpeed;
    }
  */
  //Serial.print("Speed: ");
  //Serial.println(speed);
  //setMotor(speed);
  unsigned long duration = sumRange(stepDurations, stage - i, stage);
  Serial.print("Duration: ");
  Serial.println(duration);
  Serial.print("Start time: ");
  Serial.println(startTime);
  Serial.print("End time: ");
  Serial.println(startTime + duration);
  while (millis() < startTime + duration) {
    unsigned long mills = min(millis(), startTime + duration);
    // int t = millis(); // Initialize time for uniform brighness
    Serial.print("Averaging LEDS. millis:");
    Serial.println(millis());
    for (int seg = 0; seg < nSegments; seg++) {
      // set each to the blend of its original and end values with the more time
      // giving more weight to the end color. The abs() is there to ensure that
      // the delay in the two millis() calls above does not cause a negative
      // value.
      setSegment(seg, blendColors(originalValues[seg], endValues[seg], startTime + duration - mills, mills - startTime));
      //setSegment(seg, endValues[seg]);
    }
    FastLED.show();
  }
  Serial.println("Duration Done");
  //setMotor(-speed);
  Serial.print("backtranking ");
  Serial.println(-(millis() - (startTime + duration)));
  // delay(-(millis()-(startTime + duration)));//backtrack ammount duration
  // overshot by
  //setMotor(0);
  for (int seg = 0; seg < nSegments; seg++) {
    // set each to the blend of its original and end values with the more time
    // giving more weight to the end color. The abs() is there to ensure that
    // the delay in the two millis() calls above does not cause a negative
    // value.
    setSegment(seg, endValues[seg]);
    //setSegment(seg, endValues[seg]);
  }
  FastLED.show();
}

// LED utilities
CRGB RED = CRGB(230, 20, 20);     // Badgerbots Red color
CRGB WHITE = CRGB(255, 255, 255); // white
/**
  Sets a segment of leds to the crgb. A block is the LED's behind a given gear
  or location. DOES NOT update live leds
*/
void setSegment(int i, CRGB c) {
  setSegment(i, c, leds);
}
void setSegment(int i, CRGB c, CRGB destination[]) {
  for (int ind = i * segmentSize; ind < i * segmentSize + segmentSize; ind++) {
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
   Finds the blend of colors c1 and c2. The ratio of c1 to c2 is determined by
   alpha1 to alpha2. Alpha 1 and Alpha2 must be positive, but do not have to
   have a certain sum.
*/
CRGB blendColors(CRGB c1, CRGB c2, unsigned long alpha1, unsigned long alpha2) {
  unsigned long sum = alpha1 + alpha2;
  double prop1 = (0.0 + alpha1) / sum;
  double prop2 = (0.0 + alpha2) / sum;
  Serial.println("_________________________________");
  Serial.print(sum);
  Serial.print(" , ");
  Serial.println(prop2);
  Serial.println("_________________________________");
  CRGB combined = CRGB(0, 0, 0);
  for (int i = 0; i < 3; i++) {
    combined[i] = max(0, min(255, (c1[i] * prop1 + c2[i] * prop2)));
  }
  return combined;
}
// Trellis utilities
/**
   Boot LEDs on trellis

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

// CIM Utilities
/**
   Rotates CIM x number of time segments
*/
void rotateMotorFinite(int direction, int steps) {
  Serial.print("rotating motor for duration");
  if (direction == 1) {
    cim.write(135);
    delay(period * steps);
    cim.write(85); // included to counter momentum of motor
    delay(10);
    cim.write(90);
    Serial.print("cim 135");
  } else if (direction == -1) {
    cim.write(45);
    delay(period * steps);
    cim.write(95);
    delay(10);
    cim.write(90);
    Serial.print("cim 45");
  }
}
/**
   Sets the motor to the direction indicated by the parameter.
   Lack of blocking 'delay' calls enables synchronization with leds
   float value of 1 sets to full forward, and float value of -1 sets to full
   backward. hi
*/
void setMotor(float value) {
  Serial.print("Setting Motor to ");
  int val = 90 + (int)(90 * value);
  Serial.println(val);
  cim.write(90 + (int)(90 * value));
}

/**
   Finds the sum of the array values in parameter1 between paramater2 and
   paramter3. param 2 and 3 represent array indicies, but their order does not
   matter.
*/
unsigned long sumRange(const unsigned long arr[], int ind1, int ind2) {
  int lower = min(ind1, ind2);
  int upper = max(ind1, ind2);
  int sum = 0;
  for (int i = lower; i < upper; i++) {
    sum += arr[i];
  }
  return sum;
}


void rotate(double r) {
  int endCount = PULSES_PER_ROTATION * r;
  posOrNeg = false;
  m = 0;
  Serial.println("Enter rotate");
  cim.write(180);
  while (m / 2 <= endCount) {
    //cim.write(180);
    currPositive = analogRead(ANALOG_PIN_POSITIVE);
    currNegative = analogRead(ANALOG_PIN_NEGATIVE);
    currDifference = (currPositive - currNegative);

    Serial.println(currDifference);

    if (currDifference > 0) {
      if (!posOrNeg) {
        m++;
      }
      posOrNeg = true;
    }
    else if (currDifference < 0) {
      if (posOrNeg) {
        m++;
      }
      posOrNeg = false;
    }
    //Serial.println(m);

  }

  cim.write(90);
  completedRotation = true;
  Serial.println("Stop");
  //  delay(4000);

}

void spin(bool &isSpinning) {
  if (isSpinning) {
    cim.write(45);
    isSpinning = !isSpinning;
  }
  else if (!isSpinning) {
    cim.write(90);
    isSpinning = !isSpinning;
  }
}
