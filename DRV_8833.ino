#define IN1    3
#define IN2    2

//    Out1 - GND  USB
//    Out2 - VBus USB
//    VCC  - 5V   Nano
//    GND  - GND  Nano

void setup() {
  Serial.begin(115200);
}

void LbMotor_setPower(byte power) {
//  pinMode(IN1, OUTPUT);
  pinMode(IN2, INPUT_PULLUP);
 
  analogWrite(IN1, 255-255);      // motor slow  // D3
  delay(20);                      // 20 chay ok, -> 15 thi khong chay
  analogWrite(IN1, 255-power);    // motor slow
}

void loop() {

 // for (int speed = 255; speed >= 150; speed-=5) {
 //   Serial.println(speed);
    LbMotor_setPower(150);
    delay(5* 1000);
    LbMotor_setPower(0);
    delay(3 * 1000);
 // }
}
