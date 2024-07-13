#include <Leanbot.h>
int v = 2000;
 
void setup() {
  Leanbot.begin();                  // initialize Leanbot
  
}
 
void loop() {
  // LbMission.begin(F("ms1.1"));      // start mission ms1.1
 
  // LbMotion.runLR(+1000, +1000);     // go straight forward with speed 1000 steps/s
  // LbMotion.waitDistanceMm(+100);    //   for 100 mm distance
 
  // LbMotion.runLR(-1000, -1000);     // go straight backward with speed 1000 steps/s
  // LbMotion.waitDistanceMm(-100);    //   for 100 mm distance
 
  // LbMission.end();                  // stop, finish mission

  // for (int speed = 1400; speed <= 2000; speed += 100) {
  //   Leanbot.tone(1600, 50);
  //   Serial.println(speed);
  //   LbMotion.runLR(+speed, +speed);
  //   LbDelay(2000); 

  // }

  // while (true);

  LbMotion.runLR(+v, +v);
  LbDelay(3000); 
  LbMotion.runLR(-v, -v);
  LbDelay(3000);



}
