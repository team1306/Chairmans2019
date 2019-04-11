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
int numEffects = 10;
int currentEffect = 0;

#define FORWARD 0
#define BACKWARD 1
#define SLOW 250
#define MEDIUM 50
#define FAST 5

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, nLEDS);

  pinMode(LIMIT_SWITCH_PIN, INPUT);

  Serial.println("Attaching CIM as Servo");
  cim.attach(CIM_PIN);
  cim.write(90);                //  Stop motor to start
}

void loop() {
  checkIfPushed();

  effect(currentEffect);
  currentEffect++;

  if (currentEffect == numEffects) {
    currentEffect = 0;
  }
}

void effect(int currentEffect) {
  switch (currentEffect) {
    case 0:
      checkIfPushed();
      setSectionColorCRGB(1, CRGB::Red);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(2, CRGB::Red);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(3, CRGB::Red);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(4, CRGB::Red);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(5, CRGB::Red);
      delay(500);

      checkIfPushed();
      break;
    case 1:
      checkIfPushed();
      setSectionColorCRGB(1, CRGB::Orange);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(2, CRGB::Orange);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(3, CRGB::Orange);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(4, CRGB::Orange);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(5, CRGB::Orange);
      delay(500);

      checkIfPushed();
      break;
    case 2:
      checkIfPushed();
      setSectionColorCRGB(1, CRGB::Yellow);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(2, CRGB::Yellow);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(3, CRGB::Yellow);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(4, CRGB::Yellow);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(5, CRGB::Yellow);
      delay(500);

      checkIfPushed();
      break;
    case 3:
      checkIfPushed();
      setSectionColorCRGB(1, CRGB::Green);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(2, CRGB::Green);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(3, CRGB::Green);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(4, CRGB::Green);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(5, CRGB::Green);
      delay(500);

      checkIfPushed();
      break;
    case 4:
      checkIfPushed();
      setSectionColorCRGB(1, CRGB::Blue);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(2, CRGB::Blue);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(3, CRGB::Blue);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(4, CRGB::Blue);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(5, CRGB::Blue);
      delay(500);

      checkIfPushed();
      break;
    case 5:
      checkIfPushed();
      setSectionColorCRGB(1, CRGB::Violet);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(2, CRGB::Violet);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(3, CRGB::Violet);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(4, CRGB::Violet);
      delay(500);

      checkIfPushed();
      setSectionColorCRGB(5, CRGB::Violet);
      delay(500);

      checkIfPushed();
      break;
    case 6:
      checkIfPushed();
      rainbow(3, FAST, FORWARD);
      checkIfPushed();

      break;
    default:
      break;
  }
}

void allColorRGB(int r, int g, int b) {
  for (int i = 0; i < nLEDS; i++) {
    leds[i].red   = r;
    leds[i].green = g;
    leds[i].blue  = b;
  }
  FastLED.show();
}

void allColorCRGB(CRGB c) {
  for (int i = 0; i < nLEDS; i++) {
    setLEDs(i, c);
  }
  FastLED.show();
}

void setSectionColorRGB(int section, int r, int g, int b) {
  for (int i = 0 + (segmentSize * section) - segmentSize; i < segmentSize * section; i++) {
    leds[i].red   = r;
    leds[i].green = g;
    leds[i].blue  = b;
  }
  FastLED.show();
}

void setSectionColorCRGB(int section, CRGB c) {
  for (int i = 0 + (segmentSize * section) - segmentSize; i < segmentSize * section; i++) {
    setLEDs(i, c);
  }
  FastLED.show();
}


void checkIfPushed() {
  if (digitalRead(LIMIT_SWITCH_PIN) == LOW) {
    cim.write(180);
    delay(1000);
    cim.write(90);
  }else{
    cim.write(90);
  }
}

void setLEDs(int index, CRGB value) {
  leds[index] = value;
}

void rainbow(int cycles, int speed, int direction) { // TODO direction
  for (int j = 0; j < 256 * cycles; j++) {
    for (int i = 0; i < nLEDS; i++) {
      if (direction == FORWARD) {
        setLEDs(i, Wheel(((i * 256 / nLEDS) + j) % 255));
      }
      else {
        setLEDs(i, Wheel(((i * 256 / nLEDS) + j) % 255));
      }
    }
    FastLED.show();
    delay(speed);
  }
}

CRGB Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return CRGB(1 * (WheelPos * 3) / 4, 1 * (255 - WheelPos * 3) / 4, 0);
  }
  else if (WheelPos < 170) {
    WheelPos -= 85;
    return CRGB(1 * (255 - WheelPos * 3) / 4, 0, 1 * (WheelPos * 3) / 4);
  }
  else {
    WheelPos -= 170;
    return CRGB(0, 1 * (WheelPos * 3) / 4, 1 * (255 - WheelPos * 3) / 4);
  }
}
