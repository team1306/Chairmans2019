#include <Servo.h>

Servo cim;

int ANALOG_PIN_NEGATIVE = A7;
int ANALOG_PIN_POSITIVE = A5;
int PULSES_PER_ROTATION=176;
const int CIM_PIN = 9;

boolean posOrNeg = false; //true is positive, false is negative
boolean completedRotation = false;

int currPositive = 0;
int currNegative = 0;
int currDifference = 0;
int armatureRotations = 174;

int i = 0;

void setup() {
  Serial.begin(9600);           //  setup serial
  pinMode(ANALOG_PIN_NEGATIVE, INPUT);   //  Set A7 as an input
  pinMode(ANALOG_PIN_POSITIVE, INPUT);   //  Set A5 as an input
  cim.attach(CIM_PIN);
  cim.write(180);                //  Run motor at full speed
  Serial.println("Start");
  delay(2000);

  int numRotations = 1;
  rotate(numRotations);
  //    cim.write(180);

}

void loop() {

}

void rotate(int r) {
  int endCount=PULSES_PER_ROTATION*r;
  while (i/2 >= armatureRotations*r) {
    cim.write(180);
    currPositive = analogRead(ANALOG_PIN_POSITIVE);
    currNegative = analogRead(ANALOG_PIN_NEGATIVE);
    currDifference = (currPositive - currNegative);

    if(currDifference > 0){
      if(!posOrNeg){
        i++;
      }
      posOrNeg = true;
    }
    else if (currDifference < 0){
      if(posOrNeg){
        i++;
      }
      posOrNeg = false;
    }
Serial.println(i);

  }
    
        //  Stop rotating after r revolutions
      i = 0;
      cim.write(90);
      completedRotation = true;
      Serial.println("Stop");
      delay(4000);
    
}
