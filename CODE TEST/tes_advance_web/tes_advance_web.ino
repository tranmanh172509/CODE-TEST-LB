#include <Leanbot.h>
#include <Arduino_APDS9960.h>
 
#include "MPU6050.h"
MPU6050 MPUSensor;
 
const int delayMs = 40;
 
void setup() {
  Leanbot.begin();
  Wire.begin();        
  
  MPUSensor.initialize();
  if ( MPUSensor.testConnection() ) {
    waitUntilNextBLESend();
    Serial.println(F( "Init MPU6050 ok." ));
  } else {
    waitUntilNextBLESend();
    Serial.println(F( "Init MPU6050 error." ));
  }
 
  if (APDS.begin()) {
    waitUntilNextBLESend();
    Serial.println(F( "Init APDS9960 ok." ));
  } else {
    waitUntilNextBLESend();
    Serial.println(F( "Init APDS9960 error." ));
  }
}
 
 
void loop() {
  MAX4466_Print();
  APDS9960_Print();
  MPU6050_Print();
}
 
void  APDS9960_Print(){
  APDS.proximityAvailable();
 
  if (APDS.gestureAvailable()) {
    int gesture = APDS.readGesture();
    switch (gesture) {
      case GESTURE_UP:
        waitUntilNextBLESend();
        Serial.println(F( "APDS9960  gesture DOWN" ));
        break;
 
      case GESTURE_DOWN:
        waitUntilNextBLESend();
        Serial.println(F( "APDS9960  gesture UP" ));
        break;
 
      case GESTURE_LEFT:
        waitUntilNextBLESend();
        Serial.println(F( "APDS9960  gesture RIGHT" ));
        break;
 
      case GESTURE_RIGHT:
        waitUntilNextBLESend();
        Serial.println(F( "APDS9960  gesture LEFT" ));
        break;
 
      default:
        break;
    }
  }
 
  if (APDS.colorAvailable()) {
    int r, g, b, c;
    APDS.readColor(r, g, b, c);
    waitUntilNextBLESend();
 
    Serial.print(F( "APDS9960  RGB " ));
    Serial_printtb(r);
    Serial_printtb(g);
    Serial_printtb(b);
    Serial.println(c);
  }
}
 
void MAX4466_Print() {
  const int N_SAMPLES = 175;
 
  long sum  = 0, ssum = 0;
 
  for (int i = 0; i < N_SAMPLES; i++) {
    long soundSample = analogRead(A6) - 512;
    sum  += soundSample;
    ssum += soundSample * soundSample;
  }
 
  long Mean = 512 + sum / N_SAMPLES;
  long Var  = (ssum - ((sum*sum) / N_SAMPLES)) / N_SAMPLES;
  Serial.print(F("MAX4466   Mean "));
  Serial.print(Mean);
  Serial.print(F(" Variance "));
  Serial.println(Var);
}
 
void MPU6050_Print(){
  if(MPUSensor.testConnection()){
    int ax, ay, az;
    int gx, gy, gz;
 
    MPUSensor.getAcceleration(&ax, &ay, &az);
    MPUSensor.getRotation(&gx, &gy, &gz);
    waitUntilNextBLESend();
 
    Serial.print(F( "MPU6050   Axyz " ));
    Serial_printtb(ax);
    Serial_printtb(ay);
    Serial_printtb(az);
    Serial.print("Gxyz ");
    Serial_printtb(gx);
    Serial_printtb(gy);
    Serial.println(gz);
  }
}
 
void Serial_printtb(int a) {
  Serial.print(a);
  Serial.print('\t');
}
 
void waitUntilNextBLESend() {
  static int nextSendTime = 0;
  while ((int)(nextSendTime - millis()) >= 0);
  nextSendTime = millis() + delayMs;
}
