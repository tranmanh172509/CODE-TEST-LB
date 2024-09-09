#include <Leanbot.h>
#include <anyrtttl.h>
 
String command;
byte lineValue;
const int V_MIN = 350;
const int V_MAX = 2000;
const int V_MEAN = 1000;
const int Threshold = 100;
const int displayTime = 500;
bool checkDemofromWeb = false;
 
void setup() {
  Leanbot.begin();
  LbIRLine.setThreshold(Threshold, Threshold, Threshold, Threshold);
}
 
void loop() {
  if (!checkDemofromWeb) lbTouch_check();
  serial_checkCommand();
  printAllSensors();
  LbDelay(100);
}
 
/*******************************************************************************
 Serial Command Handling
*******************************************************************************/
 
void serial_checkCommand(){
  if (Serial.available() <= 0) return;
  char cmd = Serial.read();
 
  switch (cmd) {
    case 'F':  Leanbot_setSpeed(+4, +4);        break;    // Forward
    case 'B':  Leanbot_setSpeed(-4, -4);        break;    // Backward
    case 'L':  Leanbot_setSpeed(-4, +4);        break;    // Left
    case 'R':  Leanbot_setSpeed(+4, -4);        break;    // Right
    case 'G':  Leanbot_setSpeed(+2, +4);        break;    // Forward Left
    case 'I':  Leanbot_setSpeed(+4, +2);        break;    // Forward Right
    case 'H':  Leanbot_setSpeed(-2, -4);        break;    // Back Left
    case 'J':  Leanbot_setSpeed(-4, -2);        break;    // Back Right
    case 'S':  Leanbot_setSpeed( 0,  0);        break;    // Stop 
    case 'q':  Leanbot_SetVelocity(10);         break;
    case '0' ... '9':
                Leanbot_SetVelocity(cmd - '0'); break;
    case 'W':  ledOn();                         break;
    case 'w':  ledOff();                        break;
    case 'U':  ;                                break;    // back light on
    case 'u':  ;                                break;    // back light off
    case 'V':  hornOn();                        break;
    case 'v':  hornOff();                       break;
    case 'X':  LbGripper.close();               break;
    case 'x':  LbGripper.open();                break;
    case '.':  serial_handleCommandLine();      break;    // All Command Lines starts with '.'
    default:                                    break;
  }
}
 
void serial_handleCommandLine(){
  command = Serial.readStringUntil('\n');
 
  if      (command == F("Motion"))              MotionDemo();
  else if (command.startsWith(F("Buzzer")))     BuzzerDemo();
  else if (command == F("RGBLeds"))             RGBLedsDemo();
  else if (command == F("Objectfollow"))        ObjectfollowDemo();
  else if (command == F("StraightMotion"))      StraightMotionDemo();
  else if (command.startsWith(F("Gripper")))    GripperDemo();
  else if (command.startsWith(F("LineFollow"))) LineFollowDemo();
  else if (command == F("IRLine"))              IRLineManualCalibration();
  else if (command == F("RemoteControl"))       checkDemofromWeb = true;
 
  Serial.println(command);                      // Report Finish of Demo action
  LbDelay(100);
}
 
/*******************************************************************************
 Sensor Printing Functions
*******************************************************************************/
 
void printAllSensors() {
  printTouch();
  Serial.print(F(" - "));
  printIR();
  Serial.print(F(" - "));
  printPing();
  Serial.print(F(" - "));
  printGripper();
  Serial.println();
}
 
void printTouch() {
  const int touchPins[] = {TB1A, TB1B, TB2A, TB2B};
  Serial.print(F("TB "));
  for (int i = 0; i < 4; i++) {
    Serial.print(LbTouch.read(touchPins[i]) == HIGH ? F("1") : F("0"));
  }
}
 
void printIR(){
  Serial.print(F("IR "));                      
  LbIRLine.print(LbIRLine.read(100));
  Serial.print(F(" "));
 
  int irValues[8] = {ir6L, ir4L, ir2L, ir0L, ir1R, ir3R, ir5R, ir7R};
  int RawValues[8];
  for (int i = 0; i < 8; i++) {
    RawValues[i] = map(LbIRArray.read(irValues[i]), 0, 768, 0, 255);
    Serial.print(RawValues[i]);
    if (i < 7) Serial.print(F(" ")); 
  }
}
 
void printPing(){
  int distance = Leanbot.pingCm(); 
  Serial.print(distance);           
  Serial.print(F(" cm"));
}
 
void printGripper(){
  Serial.print(F("GR "));
  int angleL = LbGripper.readL();
  int angleR = LbGripper.readR();
  Serial.print(angleL);
  Serial.print(F(" "));
  Serial.print(angleR);
}
 
/*******************************************************************************
 Demo Functions
*******************************************************************************/
 
void MotionDemo(){
  LbMotion.runLRrpm(30, 30);
  LbMotion.waitDistanceMm(100);
  LbMotion.runLRrpm(-30, -30);
  LbMotion.waitDistanceMm(100);
  LbMotion.runLRrpm(30, -30);
  LbMotion.waitRotationDeg(90);
  LbMotion.runLRrpm(-30, 30);
  LbMotion.waitRotationDeg(90);
  LbMotion.stopAndWait();
}
 
void BuzzerDemo(){
  const char* RTTTLnotes = command.substring(7).c_str();
  anyrtttl::blocking::play(LBPIN_BUZZER, RTTTLnotes );
}
 
void RGBLedsDemo(){
  RGBLedsShow(0x555555, BITMAP( ledO, ledA, ledF, ledE, ledD, ledC, ledB));
  RGBLedsShow(0xFF0000, BITMAP( ledO, ledA                              ));
  RGBLedsShow(0xFFFF00, BITMAP( ledO, ledA, ledF                        ));
  RGBLedsShow(0x00FF00, BITMAP( ledO, ledA, ledF, ledE                  ));
  RGBLedsShow(0x00FFFF, BITMAP(       ledA, ledF, ledE, ledD            ));
  RGBLedsShow(0x0000FF, BITMAP(       ledA, ledF, ledE, ledD, ledC      ));
  RGBLedsShow(0xFF00FF, BITMAP(       ledA, ledF, ledE, ledD, ledC, ledB));
  for (int i = 0; i < 4; i++) {
    RGBLedsShowRandom();
  }
}
 
void LineFollowDemo(){
  int a = 1;
    while(a)
    {
      printAllSensors();
      runFollowLine();
      if(lineValue == 0b0000 || lineValue == 0b1111) a = 0;
    }
    if(a==0)
    Leanbot_Stop();


}
 
void GripperDemo(){
  if(command.length() == 7){
    printAllSensors();
    LbGripper.close();
    printAllSensors();
    LbDelay(1000);
    LbGripper.open();
    printAllSensors();
    LbDelay(50);
  } 
  else GripperTest();
}
 
void ObjectfollowDemo(){
  int d = Leanbot.pingCm();
  const int limit = 20;
  const int offset = 2;
 
  while ((d != 0) && (d < 50)) {
    if (d > limit + offset)      LbMotion.runLR(V_MAX, V_MAX);  
    else if (d < limit - offset) LbMotion.runLR(-V_MAX, -V_MAX);
    printAllSensors();
    LbDelay(50);
    d = Leanbot.pingCm();
  }
 
  LbMotion.runLR(0, 0); 
}
 
void StraightMotionDemo(){
  LbMotion.runLR( +2000, +2000 );    
  LbMotion.waitDistanceMm( 1000 );      
  LbDelay(100);
  LbMotion.runLR( -2000, -2000 );       
  LbMotion.waitDistanceMm( -980 );      
  LbMotion.runLR(0, 0);
}
 
/*******************************************************************************
 Leanbot_Touch_Check
******************************************************************************/
 
void lbTouch_check() {
  if (LbTouch.onPress(TB1A)) {
    if (LbMotion.isMoving())      Leanbot_setSpeed(0, 0);
    else {
      delay(100);
      if (LbTouch.onPress(TB1B))  Leanbot_setSpeed(+2, +2);
      else                        Leanbot_setSpeed(+0, +2);
    }
    beepSingle();
    delay(50);
    return;
  }
 
  if (LbTouch.onPress(TB1B)) {
    if (LbMotion.isMoving())      Leanbot_setSpeed(0, 0);
    else {
      delay(100);
      if (LbTouch.onPress(TB1A))  Leanbot_setSpeed(+2, +2);
      else                        Leanbot_setSpeed(+2, +0);
    }
    beepSingle();
    delay(50);
    return;
  }
  if (LbTouch.read(TB2A) && LbTouch.read(TB2B)) {
    beepSingle();
    LbGripper_Toggle();
  }
}
 
/*******************************************************************************
 Leanbot_Motion
*******************************************************************************/
 
int velocity  = V_MAX / 1;
int motorLDir = 0;
int motorRDir = 0;
 
void Leanbot_run() {
  LbMotion.runLR(velocity * motorLDir / 4, velocity * motorRDir / 4);
}
 
void Leanbot_setSpeed(int speedL, int speedR) {
  motorLDir = speedL;
  motorRDir = speedR;
  Leanbot_run();
}
 
void Leanbot_SetVelocity(int speed) {
  velocity = map(speed, 0, 10, V_MIN, V_MAX);
  Leanbot_run();
}
 
/*******************************************************************************
 Leanbot_Beep
*******************************************************************************/
 
#define BEEP_FREQ   1500
 
void hornOn() {
  Leanbot.tone(BEEP_FREQ, 5000);      // play sound at 1500Hz for 5000ms
}
 
void hornOff() {
  Leanbot.noTone();
}
 
void beepSingle() {
  Leanbot.tone(BEEP_FREQ, 50);        // play sound at 1500Hz for 50ms
}
 
/*******************************************************************************
 Leanbot_RGB
*******************************************************************************/
 
void ledOn() {
  LbRGB.fillColor(CRGB::White);       // white setting for all lights
  LbRGB.show();                       // light status update
}
 
void ledOff() {
  LbRGB.fillColor(CRGB::Black);       // black setting for all lights
  LbRGB.show();                       // light status update
}
 
void RGBLedsShow(long color, byte shape) {
  LbRGB.clear();
  LbRGB.fillColor(color, shape);
  LbRGB.show();
  LbDelay(displayTime);
}
 
void RGBLedsShowRandom() {
  LbRGB[ledO] = CRGB(random8(), random8(), random8());
  LbRGB[ledA] = CRGB(random8(), random8(), random8());
  LbRGB[ledF] = CRGB(random8(), random8(), random8());
  LbRGB[ledE] = CRGB(random8(), random8(), random8());
  LbRGB[ledD] = CRGB(random8(), random8(), random8());
  LbRGB[ledC] = CRGB(random8(), random8(), random8());
  LbRGB[ledB] = CRGB(random8(), random8(), random8());
  LbRGB.show();
  LbDelay(displayTime);
}
 
/*******************************************************************************
 Leanbot_IRLine
*******************************************************************************/
 
int threshold(int value, int thresholdValue) {
  return (value > thresholdValue) ? 1 : 0;
}
 
void IRLineManualCalibration(){
  LbIRLine.doManualCalibration(TB1A);
}
 
void splitThreshold(){
  int thresholds[4];
  for (int i = 0; i < 4; i++) {
  int startIndex = 11 + (i * 4);  // 11, 15, 19, 23
  thresholds[i] = map(command.substring(startIndex, startIndex + 3).toInt(), 0, 255, 0, 768);
  }
  LbIRLine.setThreshold(thresholds[0], thresholds[1], thresholds[2], thresholds[3]);
}
 
void runFollowLine() {
  for (int i = 0; i < 10; i++) {
    lineValue = LbIRLine.read();
    LbIRLine.displayOnRGB(CRGB::DarkCyan);
 
    switch (lineValue) {
      case 0b0100:
      case 0b1110:
        LbMotion.runLR(0, +V_MEAN);
        break;
      case 0b1100:
      case 0b1000:
        LbMotion.runLR(-V_MEAN, +V_MEAN);
        break;
      case 0b0010:
      case 0b0111:
        LbMotion.runLR(+V_MEAN, 0);
        break;
      case 0b0011:
      case 0b0001:
        LbMotion.runLR(+V_MEAN, -V_MEAN);
        break;

      default:
        LbMotion.runLR(+V_MEAN, +V_MEAN);
    }
 
    if (LbIRLine.isBlackDetected())
      break;
 
    LbMotion.waitDistanceMm(1);
  }
}

void Leanbot_Stop() {
  LbMotion.runLR(0,0);
}
 
/*******************************************************************************
 Leanbot_Gripper
*******************************************************************************/
 
void LbGripper_Toggle() {
  static bool GripperIsClose = false;
 
  if (GripperIsClose) {
    LbGripper.open();
    GripperIsClose = false;
  } else {
    LbGripper.close();
    GripperIsClose = true;
  }
}
 
void GripperTest() {
  int angleL  = command.substring(10, 13).toInt();
  int angleR  = command.substring(14, 17).toInt();
  LbGripper.moveToLR(angleL, angleR, 500);
}
