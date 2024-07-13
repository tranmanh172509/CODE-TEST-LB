/*
  Chương trình tìm góc căn chỉnh cho 2 cánh tay ở vị trí chuẩn và lưu vào bộ nhớ EEPROM
    0°  : vuông góc với mặt đất (open)
    90° : song song với mặt đất (close)
 
  Các bước thực hiện:
    1. Mở và nạp chương trìnhn
    
    2. Mở cửa sổ Serial Monitor (chế độ "Newline")
    3. Sử dụng 4 nút chạm để điều chỉnh 2 cánh tay tới vị trí chuẩn 0°
       (TB1A, TB1B: tăng góc quay, TB2A, TB2B: giảm góc quay)
    4. Bấm phím 'n' để tạm thời lưu lại góc quay hiện tại của 2 cánh tay (deg00L và deg00R)
    5. Lặp lại bước 3 và 4 để tìm giá trị với vị trí chuẩn 90° (deg90L và deg90R)
    6. Bấm phím 's' để ghi 4 giá trị (deg00L, deg90L, deg00R, deg90R) vào bộ nhớ EEPROM
    7. Bấm phím 'y' để xác nhận ghi dữ liệu
*/
 
#include <Leanbot.h>
 
/*------------------------------------------------------------------*/
 
struct sLbGripperCalibration {
  int deg00L;
  int deg90L;
  int deg00R;
  int deg90R;
};
 
sLbGripperCalibration calibData;
 
int degL = 0;
int degR = 0;
 
int bmpDegree00 = BITMAP(ledF, ledE, ledB, ledC);
int bmpDegree90 = BITMAP(ledA, ledO, ledD);
 
 
/*------------------------------------------------------------------*/
void setup() {
  Leanbot.begin();
 
  LbGripper.setCalibration(0, 90, 0, 90);  // đưa về trạng thái mặc định
 
  CalibrateAndSaveToEEPROM();
}
 
void loop() {}
 
/*------------------------------------------------------------------*/
void CalibrateAndSaveToEEPROM() {
  CalibrateOpenPosition();                 // căn chỉnh góc 0°
  delay(500);
 
  CalibrateClosePosition();                // căn chỉnh góc 90°
  delay(500);
 
  PrintCalibrationResult();
  delay(500);
 
  WriteCalibrationResultToEEPROM();
}
 
void CalibrateOpenPosition() {
  Serial.println();
  Serial.println(F("Touch TB1A/TB2A/TB1B/TB2B to adjust the grippers to 0° position"));
  Serial.println(F("  Press 'n' to save and move to the next step"));
 
  degL = degR = 0;
  LbGripper.writeLR(degL, degR);
 
  do {
    checkTouchAndCalibrate();
    blinkLed(CRGB::Blue, bmpDegree00);
  } while ( !receiveCommand('n') );
 
  calibData.deg00L = degL;
  calibData.deg00R = degR;
}
 
void CalibrateClosePosition() {
  Serial.println();
  Serial.println(F("Touch TB1A/TB2A/TB1B/TB2B to adjust the grippers to 90° position"));
  Serial.println(F("  Press 'n' to save and move to the next step"));
 
  degL = degR = 90;
  LbGripper.writeLR(degL, degR);
 
  do {
    checkTouchAndCalibrate();
    blinkLed(CRGB::White, bmpDegree90);
  } while ( !receiveCommand('n') );
 
  calibData.deg90L = degL;
  calibData.deg90R = degR;
}
 
void WriteCalibrationResultToEEPROM() {
  Serial.println();
  Serial.println(F("Press 's' to write the calibration result to EEPROM"));
  beep(3);
  while ( !receiveCommand('s') );
 
  Serial.println(F("Do you want to write to EEPROM? (y/n)"));
  if ( waitConfirm('y') ) {
    Serial.println(F("Start writting to EEPROM..."));
    LbHwConfig.writeGripperCalibrationData(calibData.deg00L, calibData.deg90L, calibData.deg00R, calibData.deg90R);
  } else {
    Serial.println(F("Gripper calibration finished without saving to EEPROM"));
  }
}
 
/*------------------------------------------------------------------*/
void checkTouchAndCalibrate() {
  int touchBits = LbTouch.readBits();
  if (touchBits == 0) {  // không có nút nào được chạm
    return;
  }
 
  if (touchBits & TB1A) { degL++; }
  if (touchBits & TB1B) { degR++; }
 
  if (touchBits & TB2A) { degL--; }
  if (touchBits & TB2B) { degR--; }
 
  LbGripper.writeLR(degL, degR);
 
  PrintCurrentDegree();
  beep(1);                        // Beep mỗi khi tăng/giảm 1°
}
 
/*------------------------------------------------------------------*/
void PrintCurrentDegree() {
  // góc quay hiện tại
  Serial.print(F("degL = "));
  Serial.print(degL);
  Serial.print(F("\t"));
  Serial.print(F("degR = "));
  Serial.print(degR);
  Serial.println();
}
 
void PrintCalibrationResult() {
  Serial.println();
  Serial.print(F("Calibration finished:\n"));
  Serial.print(F("  LbGripper.setCalibration("));
 
  Serial.print(calibData.deg00L);
  Serial.print(F(", "));
 
  Serial.print(calibData.deg90L);
  Serial.print(F(", "));
 
  Serial.print(calibData.deg00R);
  Serial.print(F(", "));
  
  Serial.print(calibData.deg90R);
  Serial.print(F(");\n"));
}
 
/*------------------------------------------------------------------*/
void blinkLed(CRGB blinkColor, int bmpLeds) {
  LbRGB.fillColor(blinkColor, bmpLeds);
  LbRGB.show();
  delay(10);
 
  LbRGB.clear();
  LbRGB.show();
  delay(90);
}
 
/*------------------------------------------------------------------*/
void beep(int repeats) {
  while (repeats--) {
    Leanbot.tone(440*4, 25);
    delay(50);
  }
}
 
void beep_err() {
  Leanbot.tone(130*1, 500);
  delay(500);
}
 
/*------------------------------------------------------------------*/
bool receiveCommand(char cmd) {
  if (Serial.available()) {
    String str = Serial.readStringUntil('\n');
    if (str.charAt(0) == cmd) {
      beep(1);
      return true;
    }
 
    beep_err();
  }
 
  return false;
}
 
bool waitConfirm(char cmd) {
  while ( !Serial.available() );
  return receiveCommand(cmd);
}
 
/*------------------------------------------------------------------*/
