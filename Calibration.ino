// web calibration: https://lesysang1322002.github.io/Leanbot-BLE-Calibration/calibration

#include <Leanbot.h>
 
int stepperDir = MOTION_DIRECTION_FORWARD;
 
int ledMode = 2;
 
const int speed = 200;
const int timeDelayMs = 500;
const int timeStopLeanbot = 5000;
 
struct sLbGripperCalibration {
  int deg00L;
  int deg90L;
  int deg00R;
  int deg90R;
};
 
byte ledEffect[] = {
  BITMAP(ledA, ledO, ledD),
  BITMAP(ledO, ledA),
  BITMAP(ledO, ledF),
  BITMAP(ledO, ledE),
  BITMAP(ledO, ledD),
  BITMAP(ledO, ledC),
  BITMAP(ledO, ledB),
  BITMAP(ledO, ledA),
  BITMAP(ledA, ledO, ledD),
};
 
CRGB ledColor[] = {
  CRGB::Red,
  CRGB::Yellow,
  CRGB::Green,
  CRGB::Cyan,
  CRGB::Blue,
  CRGB::Magenta,
};
 
sLbGripperCalibration old;
sLbGripperCalibration newData;
 
int degL = 0;
int degR = 0;
 
byte oldLedMode;
byte oldStepperDir;
 
bool checkNextStep = true;
bool checkSaveToEEPROM = false;
 
void setup() {
  Leanbot.begin();
  LbHwConfig.read_220822 (old.deg00L, old.deg90L, old.deg00R, old.deg90R, oldLedMode, oldStepperDir);
  PrintOldCalibData();
}
 
void loop() {
  serial_checkCommand();
  if(checkSaveToEEPROM){
    if(LbTouch.read(TB1A) && LbTouch.read(TB1B)){
      beep(3);
      Serial.println("TB1A + TB1B touched. Calibration settings saved. Calibration Done.");
      LbHwConfig.write_220822(newData.deg00L, newData.deg90L, newData.deg00R, newData.deg90R, ledMode, stepperDir);
    }
  }
}
 
void serial_checkCommand() {
  if(Serial.available()<=0) return 0;
  static String commandBuffer = "";
  while (Serial.available() > 0) {
    char incomingChar = Serial.read();
    if (incomingChar == '\n') {
      handleCommand(commandBuffer);
      commandBuffer = "";
    } else {
      commandBuffer += incomingChar;
    }
  }
}
 
void handleCommand(String command) {
  if (command == "Step1"){
    checkNextStep = true;
    CalibrateOpenPosition();                
  }
  else if(command == "Step2"){
    if(checkNextStep){
    newData.deg00L = degL;
    newData.deg00R = degR;
    }
    CalibrateClosePosition();                
  }
  else if(command == "Step3"){
    checkNextStep = false;
    newData.deg90L = degL;
    newData.deg90R = degR;
    PrintCalibrationResult();
  }
  else if(command == "Step4"){
    WriteCalibrationResultToEEPROM();
  }
  else if(command == "Grippers"){
    LbRGB.clear();
    LbRGB.show();
    StopLeanbot();
  }
  else if(command == "RGB+Step"){
    PrintOldRGBStep();  
  }
  else{
    if(command[0] == 'L'){
    String StrdegL = "";
    String StrdegR = "";
    size_t i = 3;
    while(command[i] != ' '){
      StrdegL += command[i];
      i++;
    }
    while(i<command.length()){
      StrdegR += command[i];
      i++;
    }
    degL = atoi(StrdegL.c_str());
    degR = atoi(StrdegR.c_str());
    
    LbGripper.writeLR(degL, degR);
    PrintCurrentDegree();
    }
    else if(command[0] == 'R'){
      String strModeLed = command.substring(4,5);
      String strStepDir = command.substring(11,14);
      ledMode = atoi(strModeLed.c_str());
      stepperDir = atoi(strStepDir.c_str());
      PrintNewRGBStep();
      setStepDir(stepperDir);
      setLedMode(ledMode);
    }
  }
  beep(1);
}
 
 
/*---------------------------------- Grippers--------------------------------*/
 
void CalibrateOpenPosition() {
  Serial.print(F("OpenPosition"));
  Serial.println();
  LbGripper.setCalibration(0, 90, 0, 90);
}
 
void CalibrateClosePosition() {
  Serial.print(F("ClosePosition"));
  Serial.println();
  Leanbot: LbGripper.setCalibration(0, 90, 0, 90);
}
 
void PrintCalibrationResult() {
  PrintNewCaliData();
  LbGripper.setCalibration(newData.deg00L, newData.deg90L, newData.deg00R, newData.deg90R);
  while(Serial.available()<=0){
    for(int i = 0; i <= 5; i++){
      LbMotion.runLR(+speed, +speed);
      LbRGB.clear();
      LbRGB.fillColor(ledColor[i], ledEffect[i+1]);
      LbRGB.show();
      LbGripper.moveToLR(i * 18, i * 18, timeDelayMs);
    }
    StopLeanbot();
    delay(2000);
    for(int i = 5; i >= 0; i--){
      LbMotion.runLR(-speed, -speed);
      LbRGB.clear();
      LbRGB.fillColor(ledColor[i], ledEffect[i+1]);
      LbRGB.show();
      LbGripper.moveToLR(i * 18, i * 18, timeDelayMs);
    }
    StopLeanbot();
    delay(2000);
  }
}
 
void PrintNewCaliData() {
  Serial.print(F("SetCalibration("));
  Serial.print(newData.deg00L);
  Serial.print(F(", "));
 
  Serial.print(newData.deg90L);
  Serial.print(F(", "));
 
  Serial.print(newData.deg00R);
  Serial.print(F(", "));
  
  Serial.print(newData.deg90R);
  Serial.print(F(")"));
  Serial.println();
}
 
void PrintOldCalibData() {
  Serial.print(F("GetCalibration("));
  Serial.print(old.deg00L);
  Serial.print(F(", "));
 
  Serial.print(old.deg90L);
  Serial.print(F(", "));
 
  Serial.print(old.deg00R);
  Serial.print(F(", "));
  
  Serial.print(old.deg90R);
  Serial.print(F(")"));
  Serial.println();
}
 
void PrintCurrentDegree() {
  Serial.print(F("degL = "));
  Serial.print(degL);
  Serial.print(F("\t"));
  Serial.print(F("degR = "));
  Serial.print(degR);
  Serial.println();
}
 
void WriteCalibrationResultToEEPROM() {
  Serial.print(F("Touch TB1A + TB1B to permanently save calibration settings"));
  Serial.println();
  checkSaveToEEPROM = true;
}
 
/*---------------------------------- RGB Leds + Steppers --------------------------------*/
 
void PrintOldRGBStep(){
  Serial.print("OldLedMode: ");
  Serial.print(oldLedMode);
  Serial.print("\t");
  Serial.print("OldStepperDir: ");
  Serial.println(oldStepperDir);
}
 
void PrintNewRGBStep(){
  Serial.print("NewLedMode: ");
  Serial.print(ledMode);
  Serial.print("\t");
  Serial.print("NewStepperDir: ");
  Serial.println(stepperDir);
} 
 
byte effectCounter = 0;
byte colorCounter = 0;
 
void setLedMode(int LedMode) {
  LbRGB.setChainType(ledMode);
  unsigned long timeStart = millis();
  bool check = false;
  while(Serial.available()<=0){
  displayChain();
  unsigned long timeNow = millis();
  if (timeNow - timeStart >= timeStopLeanbot && !check) {
    StopLeanbot();
    check = true;
  }
  }
}
 
void displayChain() {
  static unsigned long tStart = -1000;
  unsigned long tNow = millis();
  if (tNow - tStart > 300) {
    tStart = tNow;
 
    LbRGB.clear();
    LbRGB.fillColor(ledColor[colorCounter], ledEffect[effectCounter]);
    LbRGB.show();
 
    effectCounter++;
    if ( effectCounter >= (sizeof(ledEffect) / sizeof(byte)) ) {
      effectCounter = 0;
    }
 
    colorCounter++;
    if ( colorCounter >= (sizeof(ledColor) / sizeof(CRGB)) ) {
      colorCounter = 0;
    }
  }
}
 
void setStepDir(int StepDir){
  LbStepper_setConfig(StepDir == MOTION_DIRECTION_BACKWARD);
  LbMotion.runLR(+speed, +speed);
}
 
void StopLeanbot(){
  LbMotion.runLR(0, 0);
}
 
/*-----------------------------------------------------------------------*/
 
void beep(int repeats) {
  while (repeats--) {
    Leanbot.tone(440*4, 25);
    delay(50);
  }
}
