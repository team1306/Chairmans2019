#include <Servo.h>

Servo cim;

int ANALOG_PIN_NEGATIVE = A7;
int ANALOG_PIN_POSITIVE = A5;
const int CIM_PIN = 9;

boolean isHigh = false;

int currPositive = 0;
int currNegative = 0;
int i = 0;

void setup() {
  Serial.begin(9600);           //  setup serial
  pinMode(ANALOG_PIN_NEGATIVE, INPUT);   //  Set A7 as an input
  pinMode(ANALOG_PIN_POSITIVE, INPUT);   //  Set A5 as an input
  cim.attach(CIM_PIN);
  cim.write(180);                //  Run motor at full speed
  Serial.println("Start");
  delay(2000);
}

void loop() {
  int numRotations = 1;
  rotate(numRotations);
//    cim.write(180);
}

void rotate(int r){
  currPositive = analogRead(ANALOG_PIN_POSITIVE);
  currNegative = analogRead(ANALOG_PIN_NEGATIVE);
  Serial.print("Current+: ");
  Serial.println(currPositive);
  
  Serial.print("Current-: ");
  Serial.println(currNegative);
  if (currPositive > 475) {             //  If the current value is greater than 475 (peak), set isHigh to true
    isHigh = true;
    Serial.println("isHigh: ");
    Serial.println(isHigh);
  }
  if (isHigh && currNegative < 375) {   //  If there's a peak and the current value is below 375 (trough), iterate and reset isHigh to false
    i++;
    Serial.print("i ");
    Serial.println(i);
    isHigh = false;
  }

  if (i >= 174*r) {                //  Stop rotating after one revolution
    i = 0;
    cim.write(90);
    Serial.println("Stop");
    delay(4000);
  } else {
    cim.write(180);              //  Keep rotating if you haven't finished one revolution
  }

}
