#include <Leanbot.h>
int v = 2000;
void setup() {
  Leanbot.begin();                  // initialize Leanbot
  LbMotion.runLR(+v, +v);
  delay(500);
  LbMotion.runLR(-v, -v);
}
 
void loop()
{
  // int v = 100;
  // for(v; v <= 2000; v +=100)
  // {
  //   LbMotion.runLR(+v, +v);
  //   Serial.println(v);
  //   delay(10000);
  // }
}
//#include <Leanbot.h>
//void setup() {
//  Leanbot.begin();                  // initialize Leanbot
//}
// 
//void loop()
//{
////  int v = 100;
////  for(v; v <= 2000; v +=100)
////  {
////    LbMotion.runLR(+v, +v);
////    delay(10000);
////    Serial.println(v);
////  }
//}
