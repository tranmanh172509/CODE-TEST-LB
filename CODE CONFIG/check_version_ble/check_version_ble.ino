#include <Leanbot.h>
#include <string.h>


String sendCommandF(const __FlashStringHelper* cmd) {
  Serial.println();
  delay(50);                                  ///+++++ 
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

String  sendCommand2F(const __FlashStringHelper* cmd, String cmd2) {          ////
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


void return_MAC(){
 String response = sendCommand( "AT+LADDR" );
 response.remove(0, 7);
 // Serial.println(response); // MAC SPP
  
  String var0 = response;
  response.remove(0,2);
  String var1 = response;
  response.remove(0,2);
  String var2 = response;
  response.remove(0,2);
  String var3 = response;
  response.remove(0,2);
  String var4 = response;
  response.remove(0,2);
  String var5 = response;
  
  var0.remove(2, 11);
  var1.remove(2, 10);
  var2.remove(2, 10);
  var3.remove(2, 10);
  var4.remove(2, 10); 
//  var5.remove(2, 10);

// MAC SPP
  // Serial.print("MAC SPP: ");
  // Serial.print(var0);
  // Serial.print( ":");
  // Serial.print(var1);
  // Serial.print(":");
  // Serial.print(var2);
  // Serial.print(":");
  // Serial.print(var3);
  // Serial.print(":");
  // Serial.print(var4); 
  // Serial.print(":"); 
  // Serial.println(var5); 
  // Serial.println(); 
  
  String var6 = var3;
  var6.remove(1, 2);
  var3.remove(0, 1);
  char Buf[2];
  var3.toCharArray(Buf, 2);
  char BLE = Buf[0]+1;

// MAC BLE
  Serial.println("MAC BLE: ");

  Serial.print(var0);
  Serial.print(":");
  Serial.print(var1);
  Serial.print(":");
  Serial.print(var2);
  Serial.print(":");
  Serial.print(var6);
  Serial.print(BLE);
  Serial.print(":");
  Serial.print(var4); 
  Serial.print(":"); 
  Serial.println(var5); 
 
}

bool printAndCheckConfig_JDY_33() {
  bool isLeanbot = true;
  String response;
  
  response   = sendCommandPrint( "AT+BAUD"    );
  isLeanbot &= response.startsWith(F( "+BAUD=8" ));

  return_MAC();
  
  response   = sendCommandPrint( "AT+NAME"    );
  isLeanbot &= response.startsWith(F( "+NAME=Leanbot " )) ;
  
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
  
  return isJDY33;
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
  // return String(random(1000, 10000));  // ID 4 Number
  return String(random(100000, 1000000)); // ID 6 Number
}

bool checkNumber (String name){
  if ( name.length() != 6) return false;
  for (int i = 0; i < 6; i++) {   
     if( ! isDigit(name.charAt(i)) ) return false; 
  }
  return true; 
}


String waitResponse() {
  while( ! Serial.available() );
  static String response; 
  response = Serial.readStringUntil('\n'); 
  response.trim();
  return response; 
}


String getName() {
 
  while(true){
    Serial.println();
    Serial.println( ">> Send new name is 6 Number" ); 
    
    String name = waitResponse();                               

    if( name.length() == 0 ) return name = idRandom();
   
    if( checkNumber(name) ){
      return name;
    } else {
      Serial.println();
      Serial.println( "Name is wrong, resend" );
    }
  }
}

void Led_Error() {
  LbRGB.fillColor(CRGB::Red);       // cài đặt màu sắc cho toàn bộ các đèn
  LbRGB.show();                       // cập nhật trạng thái đèn
}
 
void Led_true(){
  LbRGB.fillColor(CRGB::Green);       // cài đặt màu sắc cho toàn bộ các đèn
  LbRGB.show();                       // cập nhật trạng thái đèn
}

void setup() {
  Leanbot.begin();
  randomSeed(analogRead(0));

  if ( ! checkATisOK() ) {
    if ( ! setBaud115200() ) { 
      Led_Error();       //colorRGB(CRGB::Red);     // Red Can't find Baudrate
      Serial.println("\r\n>> STOP : Can't find Baudrate");
      return; 
    }
  } else {
    Serial.println();
    Serial.println(">> Baudrate detected      : 115200");
  }

  if ( ! checkJDY33() ) {
    Led_Error(); //colorRGB(CRGB::Red);     // Red not JDY-33 
    Serial.println("\r\n>> STOP : JDY-33 not found");
    return;
  }
  
  
  Serial.println();
  Serial.println( ">> Current Config" );
  
  bool isLeanbot = printAndCheckConfig_JDY_33();    
  
  
  Serial.println();
  Serial.print( ">> isLeanbot = " );
  Serial.println( isLeanbot );

  if ( isLeanbot ) {
    Led_true();
    //colorRGB(CRGB::Green);   // Green isLeanbot = 1
    Serial.println();  
    Serial.println( ">> End" );
    return;
  }

  LbRGB.fillColor(CRGB::Yellow);       // cài đặt màu sắc cho toàn bộ các đèn
  LbRGB.show();                       // cập nhật trạng thái đèn
  //colorRGB(CRGB::Yellow);      // Yellow isLeanbot = 0

 String name = idRandom();
 Serial.println();
 Serial.println( ">> Send ENTER to config Leanbot " + name); 
 
 waitForEnter();     // wait receive ENTER from serial;

 Serial.println();  
 Serial.println(">> ENTER received ");

  name = getName();
  Serial.println();
  Serial.println( ">> Config LeanBot " + name);
  
//  newConfig
  setnewConfig_JDY_33(name);
  Serial.println();
  Serial.println( ">> Config Leanbot Done " );
  
  Serial.println();
  Serial.println( ">> New Config " );
  
//  read config 
  isLeanbot = printAndCheckConfig_JDY_33();    
  Serial.println();
  Serial.print( ">> isLeanbot = " );
  Serial.println( isLeanbot );   
    
  if ( isLeanbot ) {
    Led_true();
    //colorRGB(CRGB::Green);     // Green isLeanbot = 1
    Serial.println();  
    Serial.println( ">> End" );
    return;
  } else {
      Led_Error(); 
      //colorRGB(CRGB::Red);    // Red not JDY-33
    Serial.println(">> Can't Config Leanbot");
  }
  
  Serial.println();  
  Serial.println( ">> End" );
}

void loop(){}
