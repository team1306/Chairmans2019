#include <Servo.h>

Servo cim;

int ANALOG_PIN = A7;
const int CIM_PIN = 9;

boolean isHigh = false;
int curr = 0;
int i = 0;

void setup() {
  Serial.begin(9600);           //  setup serial
  pinMode(ANALOG_PIN, INPUT);   //  Set A7 as an input
  cim.attach(CIM_PIN);
  cim.write(45);                //  Run motor at 1/2 speed backwards
  Serial.println("Start");
  delay(2000);
}

void loop() {
  curr = analogRead(ANALOG_PIN);
  Serial.print("Current: ");
  Serial.println(curr);

  if (curr > 400) {             //  If the current value is greater than 400 (peak), set isHigh to true
    isHigh = true;
  }
  if (isHigh && curr < 200) {   //  If there's a peak and the current value is below 200 (trough), iterate and reset isHigh to false
    i++;
    isHigh = false;
  }

  if (i > 174) {                //  Stop rotating after one revolution
    i = 0;
    cim.write(90);
    Serial.println("Stop");
    delay(4000);
  } else {
    cim.write(45);              //  Keep rotating if you haven't finished one revolution
  }
}
