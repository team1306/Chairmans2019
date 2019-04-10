#include <FastLED.h>
#include <Servo.h>

//LEDs
const int segmentSize = 33; // How many LED's are in each gear/segment
const int nSegments = 5;    // How many segments are in the whole thing
const int nLEDS = segmentSize * nSegments;   // How many LED's we have
const int LED_PIN = 42;
CRGB leds[nLEDS];

//SWITCH
const int LIMIT_SWITCH_PIN = 2;

//MOTOR
Servo cim;
const int CIM_PIN = 9;

//EFFECTS
int effectNum = 10;

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, nLEDS);

  pinMode(LIMIT_SWITCH_PIN, INPUT);

  Serial.println("Attaching CIM as Servo");
  cim.attach(CIM_PIN);
  cim.write(90);                //  Stop motor to start
}

void loop() {
  if (digitalRead(LIMIT_SWITCH_PIN) == LOW) {
    cim.write(180);
    delay(1000);
    cim.write(90);
  }

  for (int i = 0; i < effectNum; i++) {
    effect(effectNum);
  }
}

void effect(int effectNum) {
  switch (effectNum) {
    case 0:
      allColor(255, 0, 0);
      break;
    case 1:
      allColor(0, 255, 0);
      break;
    case 2:
      allColor(0, 0, 255);
      break;
    default:
      break;
  }
}

void allColor(int r, int g, int b) {
  for (int i = 0; i < nLEDS; i++) {
    leds[i].red   = r;
    leds[i].green = g;
    leds[i].blue  = b;
  }
  FastLED.show();
}
