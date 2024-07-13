/*
  Chương trình lựa chọn chiều quay động cơ và lưu vào bộ nhớ EEPROM
 
  Các bước thực hiện:
    1. Mở và nạp chương trình
    2. Sử dụng nút chạm TB2A để thay đổi chiều quay của động cơ
    3. Quan sát hiệu ứng: nếu Leanbot tiến về phía trước thì nghĩa là đã đúng chiều quay
    4. Sử dụng nút chạm TB2B để dừng Leanbot
    5. Chạm đồng thời 2 nút TB1A và TB1B để lưu chiều quay hiện tại vào bộ nhớ EEPROM
*/
 
#include <Leanbot.h>
 
 
/*------------------------------------------------------------------*/
void setup() {
  Leanbot.begin();
}
 
/*------------------------------------------------------------------*/
void loop() {
  static byte motionDirection = MOTION_DIRECTION_FORWARD;
 
  if (LbTouch.readBits() == TB2A) {
    beep(1);
 
    static bool loadConfig = false;
    if ( ! loadConfig ) {
      loadConfig = true;
      LbHwConfig.readMotionDirectionData(motionDirection);
    } else {
      motionDirection = (motionDirection == MOTION_DIRECTION_FORWARD) ? MOTION_DIRECTION_BACKWARD : MOTION_DIRECTION_FORWARD;
    }
 
    Serial.print("motionDirection: 0x");
    Serial.println(motionDirection, HEX);
    LbStepper_setConfig(motionDirection == MOTION_DIRECTION_BACKWARD);
 
    LbMotion.runLR(+400, +400);         // move forward
    while (LbTouch.readBits() != 0);    // wait until release
  }
 
 
  if (LbTouch.readBits() == TB2B) {
    beep(1);
    LbMotion.stopAndWait();
    while (LbTouch.readBits() != 0);    // wait until release
  }
 
 
  if (LbTouch.readBits() == TB1A + TB1B) {  // write to EEPROM by touching both TB1A and TB1B
    Serial.println(F("Start writting to EEPROM..."));
 
    LbMotion.stopAndWait();
 
    beep_long();
    LbHwConfig.writeMotionDirectionData(motionDirection);
  }
}
 
/*------------------------------------------------------------------*/
void beep(int repeats) {
  while (repeats--) {
    Leanbot.tone(440*4, 25);
    delay(50);
  }
}
 
void beep_long() {
  Leanbot.tone(130*1, 2000);
}
 
/*------------------------------------------------------------------*/
