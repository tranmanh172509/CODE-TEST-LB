#include <Leanbot.h>
#define IN1    3
#define IN2    2
int sensorPin = A6;    // select the input pin for the potentiometer
float sensorValue;
int dir = +1;
void setup() {
 Leanbot.begin();
 Serial.begin(115200);
}
void Leanbot_DCMotor_setPower(byte power) {
//  pinMode(IN1, OUTPUT);
  pinMode(IN2, INPUT_PULLUP);
 
  analogWrite(IN1, 255-255);      // motor slow  // D3
  delay(30);                      // 20 chay ok, -> 15 thi khong chay
  analogWrite(IN1, 255-power);    // motor slow
}
void loop() {
  // read the value from the sensor:
   if ( LbTouch.onPress(TB2A) || LbTouch.onPress(TB2B) ) {  
     dir = -dir;                         // TB2A or TB2B to switch direction between +1 and -1
   }
  //analogReference(16);
  sensorValue = analogRead(sensorPin);
  Serial.print(sensorValue);
  Serial.print("=>");
  float k = map(sensorValue,0,1023,0,255);
  Serial.println(k);
  Leanbot_DCMotor_setPower( dir * k );
  delay(100);
}
