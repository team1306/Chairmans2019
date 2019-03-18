#include <Servo.h>

int analogPin = A7;
int val = 0;
Servo cim;
const int CIM_PIN = 9;
int i = 0;

void setup() {
  Serial.begin(9600);           //  setup serial
  pinMode(analogPin, INPUT);    //  Set A7 as an input
  cim.attach(CIM_PIN);
  cim.write(59);                //  Run motor at 1/3 speed backwards
  Serial.println("Start");
  delay(2000);
}
int prev = 0;
int curr = 0;
void loop() {
  prev = curr;
  curr = analogRead(analogPin);
  Serial.println(i);
  //  val = analogRead(analogPin);  // read the input pin
  //  Serial.println(val);          // debug value
  if (curr > 500) {
    i++;
    delay(20);
    //  Serial.println(i);
  }
  if (i > 174) {
    i = 0;
    cim.write(90);
    Serial.println("Stop");
    delay(4000);
  } else {
    cim.write(59);
  }
  //delay(200);
}
