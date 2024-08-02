#define  LEANBOT_SHOW_VERSION
#include <Leanbot.h>
 
String sendCommandF(const __FlashStringHelper* cmd) {
  Serial.println();
  delay(50);                                  
  Serial.println(cmd);
 
  const int MAX_RESPONSE_LENGTH = 24 + 4;
  
  static String response;
  response.reserve(MAX_RESPONSE_LENGTH);
  response = Serial.readStringUntil('\n');
  return response;
}
 
String sendCommandPrintF(const __FlashStringHelper* cmd) {
  String response = sendCommandF(cmd);
  Serial.println(response);
  return response;
}
 
String  sendCommand2F(const __FlashStringHelper* cmd, String cmd2) {
  Serial.println();
  delay(50);                                 
  Serial.print(cmd);
  Serial.print(cmd2);
  Serial.println();
  
  const int MAX_RESPONSE_LENGTH = 24 + 4;
  
  static String response;
  response.reserve(MAX_RESPONSE_LENGTH);
  response = Serial.readStringUntil('\n');
  return response;
}
 
#define  sendCommand(cmd)        sendCommandF(F(cmd))
#define  sendCommandPrint(cmd)   sendCommandPrintF(F(cmd))
#define  sendCommand2(cmd, cmd2) sendCommand2F(F(cmd), cmd2)
 
bool checkATisOK() {
  String response = sendCommand( "AT" );
  return response.startsWith(F( "+OK" ));
}
 
void beep() {
  Leanbot.tone(440*4, 25);
  delay(100);
}
 
void beep_long() {
  while(1)  {
    beep();
  }
}
    
void return_MAC_BLE(){
  String response = sendCommand( "AT+LADDR" ); // MAC SPP
  response.remove(0, 7);                       // Remove string "AT+LADDR"
  
  Serial.println("MAC BLE: ");  
  
  Serial.print(response[0]);
  Serial.print(response[1]);
  Serial.print(":");
  Serial.print(response[2]);
  Serial.print(response[3]);
  Serial.print(":");
  Serial.print(response[4]);
  Serial.print(response[5]);
  Serial.print(":");
  Serial.print(response[6]);
 
  char Buf[11];
  response.toCharArray(Buf, 11);
  char BLE = Buf[7]+1;                // BLE MAC[7] = SPP MAC[7] +1
  Serial.print(BLE);
 
  Serial.print(":");
  Serial.print(response[8]);
  Serial.print(response[9]);
  Serial.print(":");
  Serial.print(response[10]);
  Serial.print(response[11]);
  Serial.println(); 
}


void print_MAC_BLE(String name){
  String response = sendCommand( "AT+LADDR" ); // MAC SPP
  response.remove(0, 7);                       // Remove string "AT+LADDR"
  
  Serial.println();
  Serial.print(name); 
  Serial.print("\t");
    
  Serial.print(response[0]);
  Serial.print(response[1]);
  Serial.print(":");
  Serial.print(response[2]);
  Serial.print(response[3]);
  Serial.print(":");
  Serial.print(response[4]);
  Serial.print(response[5]);
  Serial.print(":");
  Serial.print(response[6]);
 
  char Buf[11];
  response.toCharArray(Buf, 11);
  char BLE = Buf[7]+1;                // BLE MAC[7] = SPP MAC[7] +1
  Serial.print(BLE);
 
  Serial.print(":");
  Serial.print(response[8]);
  Serial.print(response[9]);
  Serial.print(":");
  Serial.print(response[10]);
  Serial.print(response[11]);
  Serial.println(); 
}

bool printAndCheckConfig_JDY_33() {
  bool isLeanbot = true;
  String response;
  
  response   = sendCommandPrint( "AT+BAUD"    );
  isLeanbot &= response.startsWith(F( "+BAUD=8" ));
  
  return_MAC_BLE();
  
  response   = sendCommandPrint( "AT+NAME"    );
  isLeanbot &= response.startsWith(F( "+NAME=Leanbot " )) ;
  return_MAC_BLE();
  
  response   = sendCommandPrint( "AT+NAMB"    );
  isLeanbot &= response.startsWith(F( "+NAMB=Leanbot " )) ; 
  isLeanbot &= response.endsWith(F( " BLE\r" )) ;                           
 
  response   = sendCommandPrint( "AT+PIN"    );
  isLeanbot &= response.startsWith(F( "+PIN=1234" )) ;
  
  response   = sendCommandPrint( "AT+ENLOG"  );
  isLeanbot &= response.startsWith(F( "+ENLOG=0" )) ;
  
  return isLeanbot;
}
 
bool setBaud115200()  {
  long Baud[] = {9600, 2400, 4800, 19200, 38400, 57600, 128000};
  for (int i = 0; i < 7; i++) { 
    Serial.begin(Baud[i]);
    if ( ! checkATisOK() ) continue;
    
    String result = sendCommand( "AT+BAUD8" ); 
    delay(10);
        
    Serial.begin(115200); 
    Serial.println();
    Serial.print( ">> Baudrate detected      : " );
    Serial.println( Baud[i] );
    
    Serial.print( ">> Baudrate set to 115200 : " );
    Serial.println( result );
 
    return true;
  }
  Serial.begin(115200); 
  return false;
}
 
bool checkJDY33()  {
  bool isJDY33 = true;
  String response;
  
  response = sendCommandPrint( "AT+VERSION" );
  isJDY33 &= response.startsWith(F( "+VERSION=JDY-33" )) ;
  
  if     ( response.startsWith(F( "+VERSION=JDY-33-V2.24" ))) { return isJDY33; } // Version 2.24, next step Config
  else if( response.startsWith(F( "+VERSION=JDY-33-V2.23" ))) { return isJDY33; } // Version 2.23, next step Config 
  else  {                                                                         // Version other, stop Config  
    Serial.println("Wrong version - DO NOT USE");
    beep_long();                                                                  // Keep beep, stop Config
  }
}
 
void waitForEnter() {
  String response;
  while( ! response.startsWith(F( "\r" )))  {   
    response = Serial.readStringUntil('\n'); 
  }
}
 
void setnewConfig_JDY_33(String name)  {
  sendCommand  ( "AT+PIN1234" );  
  sendCommand  ( "AT+ENLOG0" );  
  sendCommand2 ( "AT+NAMELeanbot " , name );        
  sendCommand2 ( "AT+NAMBLeanbot " , name + " BLE");  
}
 
String idRandom() {
  return String(random(100000, 1000000)); // id 6 Number
}
 
void setColor(CRGB Color) {       // Set color led RGB
  LbRGB.fillColor(Color);             
  LbRGB.show();                       
}
 
void setup() {
  Leanbot.begin();
  randomSeed(analogRead(0));
 
  if ( ! checkATisOK() ) {
    if ( ! setBaud115200() ) { 
      setColor(CRGB::Red);        // Red Can't find Baudrate
      Serial.println("\r\n>> STOP : Can't find Baudrate");
      beep_long();                // Keep beep, stop Config
    }
  } else {
    Serial.println();
    Serial.println(">> Baudrate detected      : 115200");
  }
 
  if ( ! checkJDY33() ) {
    setColor(CRGB::Red);         // Red not JDY-33 
    Serial.println("\r\n>> STOP : JDY-33 not found");
    beep_long();                 // Keep beep, stop Config
  }
  
  Serial.println();
  Serial.println( ">> Current Config" );
  
  bool isLeanbot = printAndCheckConfig_JDY_33();    
  
  Serial.println();
  Serial.print( ">> isLeanbot = " );
  Serial.println( isLeanbot );
 
  if ( isLeanbot ) {
    setColor(CRGB::Green);      // Green isLeanbot = 1
    beep();
//    isLeanbot = printAndCheckConfig_JDY_33();    
//    print_MAC_BLE(name);
    Serial.println();  
    Serial.println( ">> End" );
    return;
  }
 
 setColor(CRGB::Yellow);        // Yellow isLeanbot = 0
    
 String name = idRandom();
 Serial.println();
 Serial.println( ">> Send ENTER to config Leanbot " + name); 
 
 waitForEnter();     // wait receive ENTER from serial;
 
 Serial.println();  
 Serial.println(">> ENTER received ");
 
//  newConfig
  setnewConfig_JDY_33(name);
  
  Serial.println();
  Serial.println( ">> Config Leanbot Done " );
  
  Serial.println();
  Serial.println( ">> New Config " );
  
//  read config 
  isLeanbot = printAndCheckConfig_JDY_33();    
  print_MAC_BLE(name);
  Serial.println();
  Serial.print( ">> isLeanbot = " );
  Serial.println( isLeanbot );   
    
  if ( isLeanbot ) {
    setColor(CRGB::Green);     // Green isLeanbot = 1
    beep();
    Serial.println();  
    Serial.println( ">> End" );
    return;
  } else {
    setColor(CRGB::Red);        // Red not JDY-33
    Serial.println(">> Can't Config Leanbot");
    beep_long();                // Keep beep, stop Config
  }
}
 
void loop(){}
