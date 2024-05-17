#include "Leanbot.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;
#include <U8g2lib.h>

U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0);   // SH1106 for 1.3" OLED module

void setup() {
  Leanbot.begin();
  pinMode(A1, INPUT);
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  u8g2.setBusClock( 100000 );                // max I2C speed
  u8g2.begin();
  u8g2.setFont( u8g2_font_luBS08_tr );        // OLED: choose a suitable font
}

void loop() {
  OLED_Display();
}

void OLED_Display() 
{
  u8g2.firstPage();
  do 
  {
    int rcwl = analogRead(A1);
 //   int sensorValue = analogRead(A0);
  //for ( u8g2.firstPage() ; u8g2.nextPage() ; ) {      // OLED buffer drawing loop
    u8g2.setCursor(0, 16);
    u8g2.print(F( "ND:" ));    
    u8g2.setCursor(25, 16);
    u8g2.print(bme.readTemperature());

    u8g2.setCursor(70, 16);
    u8g2.print(F( "DA: " ));    
    u8g2.setCursor(90, 16);
    u8g2.print(bme.readHumidity());
 
    u8g2.setCursor(0, 28);
    u8g2.print(bme.readPressure() / 100.0F);
    u8g2.setCursor(55, 28);
    u8g2.print(F( "P" ));  
      
    u8g2.setCursor(70, 28);
    u8g2.print(F( "|" ));
    u8g2.setCursor(75, 28);
    u8g2.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    u8g2.setCursor(115, 28);
    u8g2.print(F( "m" )); 

    u8g2.setCursor(0, 42);
    u8g2.print(F( "CD:" ));
    u8g2.setCursor(25, 42);
    u8g2.print(rcwl);  
//
//    u8g2.setCursor(70, 42);
//    u8g2.print(sensorValue);

    
    } while ( u8g2.nextPage() );
    delay(500);
}
