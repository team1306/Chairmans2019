  #include <FastLED.h>
  #include <Wire.h>
  #include "Adafruit_Trellis.h"
  
  const int nLEDS=100;//How many LED's we have
  const int segmentSize=20;//How many LED's are in each gear
  CRGB leds[nLEDS];
  const int LED_PIN = 41;
  
  const int TRELLIS_INT_PIN = A3;
  const int TRELLIS_NUM_KEYS = 16;
  Adafruit_Trellis matrix0 = Adafruit_Trellis();
  Adafruit_TrellisSet trellis = Adafruit_TrellisSet(&matrix0);
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

void setup(){
    FastLED.addLeds<NEOPIXEL,LED_PIN>(leds, nLEDS);
    
    trellis.begin(0x70);
    trellis.readSwitches();
    for(int i=0i<8;i++){
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
    
  }
  trellis.readSwitches();//Reset the state of the trellis
}
