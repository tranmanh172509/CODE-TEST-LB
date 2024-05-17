#include <Arduino_APDS9960.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!APDS.begin()) {
    Serial.println("Error initializing APDS-9960 sensor.");
  }
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  delay(100);
}

void loop() 
{
   /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  // check if a color reading is available
  while (! APDS.colorAvailable()) {
    delay(5);
  }
  int ar, ag, ab;
  int proximity;
  float AX, AY, AZ, GX, GY, GZ;
  float soundValue;
  // read the color
  
  if (APDS.proximityAvailable()) {
    
    proximity = APDS.readProximity();
    APDS.readColor(ar, ag, ab);
    AX = a.acceleration.x*2 ;
    AY = a.acceleration.y*2 + 7.5;
    AZ = a.acceleration.z - 2;
    GX = g.gyro.x*2 + 2;
    GY = g.gyro.y*2 + 4;
    GZ = g.gyro.z*2 + 6;
    soundValue = analogRead(A6);
    
    ar = map(ar, 0, 255, 15, 25);
    ag = map(ag, 0, 255, 20, 30);
    ab = map(ab, 0, 255, 25, 35);

    soundValue = map(soundValue, 0, 900, 0, 100);
    proximity = map(proximity, 0, 255, 30, 40);
  
    // print the values
    Serial.print("r:");
    Serial.print(ar);
    Serial.print(",");
    
    Serial.print("g:");
    Serial.print(ag);
    Serial.print(",");
    
    Serial.print("b:");
    Serial.print(ab);
    Serial.print(",");
    
    Serial.print("kc:");
    Serial.print(proximity);
    Serial.print(",");

    Serial.print("AX:");
    Serial.print(AX);
    Serial.print(",");
    
    Serial.print("AY:");
    Serial.print(AY);
    Serial.print(",");
    
    Serial.print("AZ:");
    Serial.print(AZ);
    Serial.print(", ");
    
    Serial.print("GX:");
    Serial.print(GX);
    Serial.print(",");
    
    Serial.print("GY:");
    Serial.print(GY);
    Serial.print(",");
    
    Serial.print("GZ:");
    Serial.print(GZ);
    Serial.print(",");

    Serial.println(soundValue);
    
  }
}
  
