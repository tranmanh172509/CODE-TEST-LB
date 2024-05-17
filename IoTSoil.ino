#include "Leanbot.h"
#include "LeanbotIoT.h"


// using F() macro to lower Arduino SRAM usage
#define  WIFI_SSID      F( "DTT-SANXUAT" )
#define  WIFI_PASSWORD  F( "68686868" )
 
#define  IOT_ASSET_ID   F( "33khwmcNaidrPJ2oZpcgde" )
#define  IOT_REALM      F( "master" )
//  Asset      https://iot-qa.pythaverse.space/manager/#/assets/false/33khwmcNaidrPJ2oZpcgde
//  Dashboard  https://iot-qa.pythaverse.space/manager/#/insight/false/???
 
 
auto iotSoilTension = LbIoT.Asset.attributeInt(F( "soilTensionMeasured" ));
 
#include <U8g2lib.h>
#include <Wire.h>
 
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0);   // SH1106 for 1.3" OLED module

void setup() {
  Leanbot.begin();
  LbSPISerial.begin();      // initialize serial for ESP module: baud=57600 at pin 11 and 12
  LbSPISerial.println();    // abort any previous spurious partial command text to esp
  WiFi.init(LbSPISerial);   // init WiFi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  printMacAddress();        // get & print MAC address
  
  u8g2.setBusClock( 400000 );                // max I2C speed
  u8g2.begin();
  u8g2.setFont( u8g2_font_luBS08_tr );        // OLED: choose a suitable font
}
 
void printMacAddress() {
  uint8_t mac[6];

  WiFi.macAddress(mac);
  Serial.print("Wifi MAC: ");
  
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

void loop() {
  if ( LbIoT.Wifi.status() != WL_CONNECTED ) {
    if ( LbIoT.Wifi.begin( WIFI_SSID, WIFI_PASSWORD ) < 0 ) {
      u8g2.print(F( "No WiFi" ));
      return;  // retry
    }
  }
 
 
  if ( LbIoT.Asset.status() != ESTABLISHED ) {
    if ( LbIoT.Asset.begin( IOT_ASSET_ID, IOT_REALM ) < 0 ) {
      return;  // retry
    }
 
    auto iotNotes = LbIoT.Asset.attribute(F( "notes" ));
    Serial.print( "Asset: " );
    Serial.println( iotNotes.read() );      // print Asset info
  }
 
 
  // read sensor value
  int sensorValue = analogRead(A0);
  Serial.print( "\n\n\n" "######   Send the \"sensorValue = " );
  Serial.print(sensorValue);
  Serial.print( "\" to server   ######" "\n\n" );
  iotSoilTension.write( sensorValue );
  OLED_Display(sensorValue);
  delay(2500);                           // small delay to wait for response from server
  LbIoT.Asset.flushResponse(NULL);
  checkResponseStatus();
 
 
  Serial.print( "\n\n\n" "######   Read the value from server   ######" "\n\n" );
  int value = iotSoilTension.read();
  checkResponseStatus();
  Serial.print( iotSoilTension.name() );
  Serial.print(" = ");
  Serial.println(value);
  delay(2500);
}
 
 
/*
  https://developer.mozilla.org/en-US/docs/Web/HTTP/Status#client_error_responses
  - Informational responses (100 – 199)
  - Successful responses    (200 – 299)
  - Redirection messages    (300 – 399)
  - Client error responses  (400 – 499)
  - Server error responses  (500 – 599)
 
  Common status:
  - HTTP/1.1 200 OK       : The request succeeded
  - HTTP/1.1 403 Forbidden: Incorrect "attributeId" or "Access public write" is not configured
*/
void checkResponseStatus() {
  int responseStatus = LbIoT.Asset.responseStatus();
  LbIoT.decodeStatus( responseStatus );
}

void OLED_Display(int sensorValue) {
  for ( u8g2.firstPage() ; u8g2.nextPage() ; ) {      // OLED buffer drawing loop
    u8g2.setCursor(35, 16);
    u8g2.print(F( "Moisture" ));
 
    u8g2.setCursor(50, 28);
    u8g2.print(sensorValue);
 
    u8g2.setCursor(8, 60);
    u8g2.print(WIFI_SSID);
  }
}
