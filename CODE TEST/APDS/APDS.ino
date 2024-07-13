
#include <Arduino_APDS9960.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!APDS.begin()) {
    Serial.println("Error initializing APDS-9960 sensor.");
  }
}

void loop() {
  // check if a color reading is available
  while (! APDS.colorAvailable()) {
    delay(5);
  }
  int r, g, b;
  int proximity;
  // read the color
  
  if (APDS.proximityAvailable()) {
    
    
    proximity = APDS.readProximity();

    APDS.readColor(r, g, b);
    r = map(r, 0, 255, 0, 10);
    g = map(g, 0, 255, 10, 20);
    b = map(b, 0, 255, 20, 30);
    proximity = map(proximity, 0, 255, 30, 40);
  
    // print the values
    Serial.print("r = ");
    Serial.print(r);
    Serial.print(",");
    
    Serial.print("g = ");
    Serial.print(g);
    Serial.print(",");
    
    Serial.print("b = ");
    Serial.print(b);
    Serial.print(",");
    
    Serial.print("kc = ");
    Serial.print(proximity);
    Serial.println("");
  }
  
  

  // wait a bit before reading again
  delay(100);
}
