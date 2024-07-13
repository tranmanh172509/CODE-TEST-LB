/*
  Chương trình lựa chọn thứ tự cho đèn Led RGB và lưu vào bộ nhớ EEPROM
 
  Các bước thực hiện:
    1. Mở và nạp chương trình
    2. Sử dụng nút chạm TB2A để thay đổi thứ tự của các đèn Led
    3. Quan sát hiệu ứng: nếu các Led xoay theo chiều kim đồng hồ thì nghĩa là đã đúng thứ tự
    4. Chạm đồng thời 2 nút TB1A và TB1B để lưu thứ tự hiện tại vào bộ nhớ EEPROM
*/
 
#include <Leanbot.h>
 
 
/*------------------------------------------------------------------*/
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
 
byte effectCounter = 0;
byte colorCounter = 0;
 
byte chainType = 2;
 
/*------------------------------------------------------------------*/
void setup() {
  Leanbot.begin();
 
  setChainType(chainType);
}
 
 
/*------------------------------------------------------------------*/
void setChainType(int chainType) {
  Serial.print("chainType: ");
  Serial.println(chainType);
  LbRGB.setChainType(chainType);
}
 
 
/*------------------------------------------------------------------*/
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
 
 
/*------------------------------------------------------------------*/
void beep(int repeats) {
  while (repeats--) {
    Leanbot.tone(440*4, 25);
    delay(50);
  }
}
 
void beep_long() {
  Leanbot.tone(130*1, 3000);
}
 
 
/*------------------------------------------------------------------*/
void loop() {
  if (LbTouch.readBits() == TB1A + TB1B) {  // write to EEPROM by touching both TB1A and TB1B
    Serial.println(F("Start writting to EEPROM..."));
 
    setChainType(chainType);
 
    LbRGB.clear();
    LbRGB.fillColor(CRGB::Green, ledEffect[0]);
    LbRGB.show();
 
    beep_long();
    LbHwConfig.writeRGBChainData(chainType);
  }
 
  if (LbTouch.readBits() == TB2A) {
    beep(1);
 
    effectCounter = 0;  // reset effect
    colorCounter = 0;
 
    chainType++;
    if (chainType > 2) {
      chainType = 1;
    }
    setChainType(chainType);
 
    while (LbTouch.readBits() != 0);  // wait until release
  }
 
  displayChain();
}
