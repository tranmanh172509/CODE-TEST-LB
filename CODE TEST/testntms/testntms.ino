// *** Please change Leanbot IDE SERVER to   ide-server-qa.leanbot.space
//   1. Click on SERVER: ONLINE button (green button at bottom right conner)
//   2. Fill in Server Address text box (next to + ADD SERVER button at top right)
//      with   ide-server-qa.leanbot.space
//   3. Click on  + ADD SERVER button
//   4. Switch back to CODE tab, ready to compile, upload code
 
 
/*
  Optical Heart Rate Detection (PBA Algorithm) using the MAX30105 Breakout
  By: Nathan Seidle @ SparkFun Electronics
  Date: October 2nd, 2016
  https://github.com/sparkfun/MAX30105_Breakout
 
  This is a demo to show the reading of heart rate or beats per minute (BPM) using
  a Penpheral Beat Amplitude (PBA) algorithm.
 
  It is best to attach the sensor to your finger using a rubber band or other tightening
  device. Humans are generally bad at applying constant pressure to a thing. When you
  press your finger against the sensor it varies enough to cause the blood in your
  finger to flow differently which causes the sensor readings to go wonky.
 
  Hardware Connections (Breakoutboard to Arduino):
  -5V = 5V (3.3V is allowed)
  -GND = GND
  -SDA = A4 (or SDA)
  -SCL = A5 (or SCL)
  -INT = Not connected
 
  The MAX30105 Breakout can handle 5V or 3.3V I2C logic. We recommend powering the board with 5V
  but it will also run at 3.3V.
*/
 
#include <Wire.h>
#include "MAX30105.h"
 
#include "heartRate.h"
 
MAX30105 particleSensor;
 
#define  FINGER_THRESHOLD    50000
 
#define  RATE_SIZE  8   // Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE];  // Array of heart rates
byte rateSpot = 0;
long lastBeat = 0;      // Time at which the last beat occurred
 
byte  beatCnt = 0;
float beatsPerMinute;
int   beatAvg;
 
#define  BeepTone        (440*1)
#define  BeepDuration    (50)
 
/*---------------------------------------------------*/
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
 
#define SCREEN_WIDTH 128   // OLED display width, in pixels
#define SCREEN_HEIGHT 64   //64 // OLED display height, in pixels
 
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
#define OLED_RESET       -1  // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
 
Adafruit_SH1106 display(OLED_RESET);
 
int xPos = 0;
 
/*---------------------------------------------------*/
void setup() {
  Serial.begin(1000000);
 
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SH1106_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.cp437(true);
  display.clearDisplay();
 
  // startup screen
  do {
    drawLine(&xPos);
  } while (xPos != 0);
  display_data(0, 0);
  delay(1000);
 
 
  // Initialize sensor
  if (!particleSensor.begin(Wire, 400000)) { //Use default I2C port, 400kHz speed
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }
  particleSensor.setup();                      // Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A);   //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0x00); //Turn off Green LED
 
  Serial.println(F("Place your index finger on the sensor with steady pressure."));
}
 
 
/*---------------------------------------------------*/
void loop() {
  #if 0  // log sampling time per loop
  static uint32_t tsLastLoop = 0;
  uint32_t tsNow = millis();
  Serial.print(F("SampleTime="));
  Serial.println(tsNow - tsLastLoop);
  tsLastLoop = tsNow;
  #endif
 
  long irValue = particleSensor.getIR();
 
  if ( checkForBeat(irValue) && (irValue >= FINGER_THRESHOLD) ) {   // We sensed a beat!
 
    Serial.println(F("We sensed a beat!"));
 
    long delta = millis() - lastBeat;
    lastBeat   = millis();
 
    tone(11, BeepTone, BeepDuration);
 
    beatCnt++;
    beatsPerMinute = 60 / (delta / 1000.0);
 
    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute;  // Store this reading in the array
      rateSpot         %= RATE_SIZE;             // Wrap variable
 
      // Take average of readings
      beatAvg = 0;
      byte x;
      for (x=0 ; x<RATE_SIZE ; x++) {
        beatAvg += rates[x];
        if (rates[x] == 0) { break; }  // out of data
      }
      beatAvg /= x;  // average of current available samples (max = RATE_SIZE)
 
      Serial.print(F("beatsPerMinute=")); Serial.print(beatsPerMinute);
      Serial.print(F(", beatAvg="));      Serial.print(beatAvg);
      Serial.print(F(", beatSamples="));  Serial.print(x);
      Serial.println();
 
      //Wire.setClock(1000000);
      {
        display_data(beatCnt, beatAvg);
        heart_beat(&xPos);
      }
      //Wire.setClock(400000);
    }
  }
 
//  Serial.print(F("IR="));      Serial.print(irValue);
//  Serial.print(F(", Beat="));  Serial.print(beatCnt);
//  Serial.print(F(", BPM="));   Serial.print(beatsPerMinute);
//  Serial.print(F(", Avg="));   Serial.print(beatAvg);
 
  if (irValue < FINGER_THRESHOLD) {
//    Serial.print(F(" No finger?"));
    sensorProcessIdle();
  }
//  Serial.println();
 
  //Wire.setClock(1000000);
  {
    drawLine(&xPos);
  }
  //Wire.setClock(400000);
}
 
 
/*---------------------------------------------------*/
void drawLine(int *x_pos) {
  // Draw a single pixel in white
  display.drawPixel(*x_pos, 8, WHITE);
  display.drawPixel((*x_pos)++, 8, WHITE);
  display.drawPixel((*x_pos)++, 8, WHITE);
  display.drawPixel((*x_pos)++, 8, WHITE);
  display.drawPixel((*x_pos), 8, BLACK);  // -----
  //Serial.println(*x_pos);
  display.fillRect(*x_pos, 0, 31, 16, BLACK);
 
//  display.display();  // workaround: avoid impact to sampling rate
 
  //delay(1);
  if (*x_pos >= SCREEN_WIDTH) {
    *x_pos = 0;
  }
}
 
 
/*---------------------------------------------------*/
void display_data(int beat, int bpm) {
 
 
  display.fillRect(0, 18, 127, 15, BLACK);
 
  // Display "bpm"
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 18);
  display.print("BPM ");
  display.setTextSize(2);
  display.print(bpm);
//  display.display();
 
  // Display "beat"
  display.setTextSize(1);
  //display.setTextColor(WHITE);
  display.setCursor(64, 18);
  display.print("Beat ");
  display.setTextSize(2);
  display.print(beat);
 
  display.display();
}
 
 
/*---------------------------------------------------*/
void heart_beat(int *x_pos) {
  /************************************************/
  //display.clearDisplay();
  display.fillRect(*x_pos, 0, 30, 15, BLACK);
  // Draw a single pixel in white
  display.drawPixel(*x_pos + 0, 8, WHITE);
  display.drawPixel(*x_pos + 1, 8, WHITE);
  display.drawPixel(*x_pos + 2, 8, WHITE);
  display.drawPixel(*x_pos + 3, 8, WHITE);
  display.drawPixel(*x_pos + 4, 8, BLACK); // -----
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 5, 7, WHITE);
  display.drawPixel(*x_pos + 6, 6, WHITE);
  display.drawPixel(*x_pos + 7, 7, WHITE); // .~.
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 8, 8, WHITE);
  display.drawPixel(*x_pos + 9, 8, WHITE); // --
  //display.display();
  //delay(1);
  /******************************************/
  display.drawPixel(*x_pos + 10, 8, WHITE);
  display.drawPixel(*x_pos + 10, 9, WHITE);
  display.drawPixel(*x_pos + 11, 10, WHITE);
  display.drawPixel(*x_pos + 11, 11, WHITE);
  //display.display();
  //delay(1);
  /******************************************/
  display.drawPixel(*x_pos + 12, 10, WHITE);
  display.drawPixel(*x_pos + 12, 9, WHITE);
  display.drawPixel(*x_pos + 12, 8, WHITE);
  display.drawPixel(*x_pos + 12, 7, WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 13, 6, WHITE);
  display.drawPixel(*x_pos + 13, 5, WHITE);
  display.drawPixel(*x_pos + 13, 4, WHITE);
  display.drawPixel(*x_pos + 13, 3, WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 14, 2, WHITE);
  display.drawPixel(*x_pos + 14, 1, WHITE);
  display.drawPixel(*x_pos + 14, 0, WHITE);
  display.drawPixel(*x_pos + 14, 0, WHITE);
  //display.display();
  //delay(1);
  /******************************************/
  display.drawPixel(*x_pos + 15, 0, WHITE);
  display.drawPixel(*x_pos + 15, 1, WHITE);
  display.drawPixel(*x_pos + 15, 2, WHITE);
  display.drawPixel(*x_pos + 15, 3, WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 15, 4, WHITE);
  display.drawPixel(*x_pos + 15, 5, WHITE);
  display.drawPixel(*x_pos + 16, 6, WHITE);
  display.drawPixel(*x_pos + 16, 7, WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 16, 8, WHITE);
  display.drawPixel(*x_pos + 16, 9, WHITE);
  display.drawPixel(*x_pos + 16, 10, WHITE);
  display.drawPixel(*x_pos + 16, 11, WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 17, 12, WHITE);
  display.drawPixel(*x_pos + 17, 13, WHITE);
  display.drawPixel(*x_pos + 17, 14, WHITE);
  display.drawPixel(*x_pos + 17, 15, WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 18, 15, WHITE);
  display.drawPixel(*x_pos + 18, 14, WHITE);
  display.drawPixel(*x_pos + 18, 13, WHITE);
  display.drawPixel(*x_pos + 18, 12, WHITE);
  //display.display();
  //delay(1);
  display.drawPixel(*x_pos + 19, 11, WHITE);
  display.drawPixel(*x_pos + 19, 10, WHITE);
  display.drawPixel(*x_pos + 19, 9, WHITE);
  display.drawPixel(*x_pos + 19, 8, WHITE);
  //display.display();
  //delay(1);
  /****************************************************/
  display.drawPixel(*x_pos + 20, 8, WHITE);
  display.drawPixel(*x_pos + 21, 8, WHITE);
  //display.display();
  //delay(1);
  /****************************************************/
  display.drawPixel(*x_pos + 22, 7, WHITE);
  display.drawPixel(*x_pos + 23, 6, WHITE);
  display.drawPixel(*x_pos + 24, 6, WHITE);
  display.drawPixel(*x_pos + 25, 7, WHITE);
  //display.display();
  //delay(1);
  /************************************************/
  display.drawPixel(*x_pos + 26, 8, WHITE);
  display.drawPixel(*x_pos + 27, 8, WHITE);
  display.drawPixel(*x_pos + 28, 8, WHITE);
  display.drawPixel(*x_pos + 29, 8, WHITE);
  display.drawPixel(*x_pos + 30, 8, WHITE); // -----
  *x_pos = *x_pos + 30;
  display.display();
  delay(1);
}
 
 
/*---------------------------------------------------*/
#define MAX30102_LED_OFF     (0x00)  // 0.0mA - Led off
#define MAX30102_LED_ON      (0x1F)  // 6.4mA - Presence detection of ~8 inch
 
void sensorProcessIdle() {
  static uint8_t   powerLevel = MAX30102_LED_OFF;
  static uint32_t  prevMs = 0;
 
  uint32_t nowMs = millis();
 
  if ((nowMs - prevMs) > 500) {
    prevMs = nowMs;
    powerLevel = (powerLevel == MAX30102_LED_OFF) ? (MAX30102_LED_ON) : (MAX30102_LED_OFF);
 
    particleSensor.setPulseAmplitudeIR(powerLevel);
    particleSensor.clearFIFO();
 
    // clear all data:
    beatAvg = 0;
    beatCnt = 0;
    beatsPerMinute = 0;
    rateSpot = 0;
    for (byte i=0 ; i<RATE_SIZE ; i++) { rates[i] = 0; }
    lastBeat = millis();
 
//    Serial.print(F("Blink: "));
//    Serial.println(powerLevel, HEX);
  }
}
 
/*---------------------------------------------------*/
