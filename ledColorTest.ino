/*
  ledColorTest.ino - Code to run on Chairmans Visual for the 2019 season
  author - Mateo Silver
*/
#include <FastLED.h>

#define NUM_LEDS 150
#define DATA_PIN 6

String input = "";
int r = 255;
int g = 255;
int b = 255;

CRGB leds[NUM_LEDS];


String midString(String str, String start, String finish){
  int locStart = str.indexOf(start);
  if (locStart==-1) return "";
  locStart += start.length();
  int locFinish = str.indexOf(finish, locStart);
  if (locFinish==-1) return "";
  return str.substring(locStart, locFinish);
}


void setup() {
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop() {
  if (Serial.available() > 0)
  {
    input = Serial.readString();

    r = midString(input, "(" , ",").toInt();
    g = midString(input, "," , ",").toInt();
    b = midString(input, "," , ")").toInt();

    for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = CRGB(r,g,b);
    }
    FastLED.show();
    delay(30);
  }
}
