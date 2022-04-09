#include <Adafruit_GFX.h>    
#include <Adafruit_TFTLCD.h> 
#include <TouchScreen.h>
#include "HUSKYLENS.h"
#include "SoftwareSerial.h"


#define LCD_CS A3     // Chip Select goes to Analog 3
#define LCD_CD A2     // Command/Data goes to Analog 2
#define LCD_WR A1     // LCD Write goes to Analog 1
#define LCD_RD A0     // LCD Read goes to Analog 0
#define LCD_RESET A4  // Can alternately just connect to Arduino's reset pin

#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

// Define some TFT readable colour codes to human readable names
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define TS_MINX 70
#define TS_MINY 125
#define TS_MAXX 925
#define TS_MAXY 890

#define MINPRESSURE 10
#define MAXPRESSURE 1200

// Touch screen pins
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 37   // can be a digital pin
#define XP 36   // can be a digital pin

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);           

// Pins for Stepper Motor Driver
const int stepPin_1 = 38;
const int dirPin_1 = 39;
const int stepPin_2 = 40;
const int dirPin_2 = 41;
const int stepPin_3 = 42;
const int dirPin_3 = 43;

const int buzzerPin = 3;
const int stopPin = 7;

HUSKYLENS huskylens;
SoftwareSerial mySerial(10, 11);// RX, TX
//HUSKYLENS green line >> Pin 10; blue line >> Pin 11

int QTY = 0;
int SET = 0;

void init_Screen(){

  tft.fillScreen(CYAN);
  tft.fillRect  (40,160,80,80,BLACK);
  tft.fillRect  (360,160,80,80,BLACK);
  tft.fillRect  (160,160,160,80,GREEN);

  tft.setTextSize(4);
  tft.setTextColor(BLACK);
  tft.setCursor(185,185);
  tft.print("START");

  tft.setCursor(45,120);
  tft.print("QTY");
  tft.setCursor(365,120);
  tft.print("SET");
  tft.setCursor(60,60);
  tft.setTextSize(3);
  tft.print("Smart Resistor Cutter");
}


void setup(void) {
  // Setup for Huskylens
  Serial.begin(9600);
//  mySerial.begin(9600);
//  
//  while (!huskylens.begin(mySerial))
//  {
//    Serial.println(F("Begin failed!"));
//    Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>Serial 9600)"));
//    Serial.println(F("2.Please recheck the connection."));
//    delay(100);
//  }

  // Setup Screen
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  tft.setCursor(150,140);
  tft.setTextColor(WHITE);
  tft.setTextSize(5);
  tft.print("Welcome");
  delay(1000);
  
  init_Screen();
  tft.setTextSize(4);
  tft.setTextColor(WHITE);
  tft.setCursor(70,185);
  tft.print("0");
  tft.setCursor(390,185);
  tft.print("0");

  // Declare pins as Outputs for motor drivers
  pinMode(stepPin_1, OUTPUT);
  pinMode(dirPin_1, OUTPUT);
  pinMode(stepPin_2, OUTPUT);
  pinMode(dirPin_2, OUTPUT);
  pinMode(stepPin_3, OUTPUT);
  pinMode(dirPin_3, OUTPUT);

  pinMode(buzzerPin, OUTPUT);
  pinMode(stopPin, INPUT);
}


void loop(void) {
  
  int X, Y;
  // Read inputs from initial screen
  TSPoint p = waitTouch();
  X = p.x; Y = p.y;

  if(Y < 240 && Y > 160)
  {
    if (X < 120 && X > 40){ // Click on QTY enter section
      QTY = userScreen();
    }
    else if(X < 440 && X > 360){ // Click on SET enter section
      SET = userScreen();
    }
    else if(X < 320 && X > 160){ // Click on START
      // Start running motors
      cutScreen();
      motor(QTY, SET);

      //Reload the screen
      init_Screen();
      QTY = 0;
      SET = 0;
    }
  }
  
  tft.fillRect(40,160,80,80,BLACK);
  tft.fillRect(360,160,80,80,BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  
  if(QTY > 9){
    tft.setCursor(60,185);
    tft.print(String(QTY));
  }else{
    tft.setCursor(70,185);
    tft.print(String(QTY));
  }

  if(SET > 9){
    tft.setCursor(380,185);
    tft.print(String(SET));
  }else{
    tft.setCursor(390,185);
    tft.print(String(SET));
  }
  
}

int userScreen(){
  tft.fillScreen(CYAN);
  tft.fillRect  (0,80,160,60,BLACK);
  tft.fillRect  (160,80,160,60,BLACK);
  tft.fillRect  (320,80,160,60,BLACK);
  
  tft.fillRect  (0,140,160,60,BLACK);
  tft.fillRect  (160,140,160,60,BLACK);
  tft.fillRect  (320,140,160,60,BLACK);
  
  tft.fillRect  (0,200,160,60,BLACK);
  tft.fillRect  (160,200,160,60,BLACK);
  tft.fillRect  (320,200,160,60,BLACK);
  
  tft.fillRect  (0,260,160,60,BLACK);
  tft.fillRect  (160,260,160,60,RED);
  tft.fillRect  (320,260,160,60,GREEN);

  tft.drawLine  (0, 80, 480, 80, WHITE);
  tft.drawLine  (0, 140, 480, 140, WHITE);
  tft.drawLine  (0, 200, 480, 200, WHITE);
  tft.drawLine  (0, 260, 480, 260, WHITE);
  tft.drawLine  (160, 80, 160, 320, WHITE);
  tft.drawLine  (320, 80, 320, 320, WHITE);

  tft.setTextSize(4);
  tft.setTextColor(WHITE);
  tft.setCursor(70,100);
  tft.print("1");
  tft.setCursor(230,100);
  tft.print("2");
  tft.setCursor(390,100);
  tft.print("3");
  tft.setCursor(70,160);
  tft.print("4");
  tft.setCursor(230,160);
  tft.print("5");
  tft.setCursor(390,160);
  tft.print("6");
  tft.setCursor(70,220);
  tft.print("7");
  tft.setCursor(230,220);
  tft.print("8");
  tft.setCursor(390,220);
  tft.print("9");
  tft.setCursor(70,280);
  tft.print("0");
  tft.setCursor(210,280);
  tft.print("DEL");
  tft.setCursor(355,280);
  tft.print("ENTER");

  int X, Y;
  int result = 0;
  do{
    TSPoint p = waitTouch();
    X = p.x; 
    Y = p.y;
    result = getInput(X, Y, result);
  }while(!(Y < 320 && Y > 260 && X < 480 && X > 320));

  init_Screen();
  return result;
}


void cutScreen(){
  tft.fillRect  (0,0,480,320,GREEN);

  tft.setCursor(80,60);
  tft.setTextSize(4);
  tft.print("Set Remaining");

  tft.fillRect(340,220, 80, 80, RED);
  tft.setTextSize(3);
  tft.setTextColor(YELLOW);
  tft.setCursor(345,250);
  tft.print("STOP");
}


int getInput(int X, int Y, int previous){
  int result;

  if(Y < 140 && Y > 80 && X < 160 && X > 0){
    result = previous*10 + 1;
  } else if(Y < 140 && Y > 80 && X < 320 && X > 160){
    result = previous*10 + 2;
  } else if(Y < 140 && Y > 80 && X < 480 && X > 320){
    result = previous*10 + 3;
  } else if(Y < 200 && Y > 140 && X < 160 && X > 0){
    result = previous*10 + 4;
  } else if(Y < 200 && Y > 140 && X < 320 && X > 160){
    result = previous*10 + 5;
  } else if(Y < 200 && Y > 140 && X < 480 && X > 320){
    result = previous*10 + 6;
  } else if(Y < 260 && Y > 200 && X < 160 && X > 0){
    result = previous*10 + 7;
  } else if(Y < 260 && Y > 200 && X < 320 && X > 160){
    result = previous*10 + 8;
  } else if(Y < 260 && Y > 200 && X < 480 && X > 320){
    result = previous*10 + 9;
  } else if(Y < 320 && Y > 260 && X < 160 && X > 0){
    result = previous*10;
  } else if(Y < 320 && Y > 260 && X < 320 && X > 160){
    result = (previous - previous % 10)/10;
  } else if(Y < 320 && Y > 260 && X < 480 && X > 320){
    result = previous;
  }
  
  tft.fillRect  (0,0,480,80,CYAN);
  tft.setCursor(10,30);
  tft.print(String(result));
  
  return result;
}


TSPoint waitTouch() {
  TSPoint p;
  do {
    p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
  } while((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE));
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, 480);
  p.y = map (p.y, TS_MINY, TS_MAXY, 0, 320);

  return p; 
}

int motor(int  q, int s){
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.fillRect(180,120,100,60,CYAN);
  if(s < 10){
    tft.setCursor(215,130);
    tft.print(s);
  }else{
    tft.setCursor(205,130);
    tft.print(s);
  }
  
  for(int i = 0; i < s; i++){
    // conveyor Motion & cutter Motion
//    if(conveyorMotion(q) | cutterMotion() | CameraCheck()){
    if(conveyorMotion(q) | cutterMotion()){
      return 0;
    }
    
    tft.fillRect(180,120,100,60,CYAN);
    if(s - i - 1 < 10){
      tft.setCursor(215,130);
      tft.print(s - i - 1);
    }else{
      tft.setCursor(205,130);
      tft.print(s - i - 1);
    }
  }
  return 0;
}  

int cutterMotion(){
  digitalWrite(dirPin_2, HIGH);
  digitalWrite(dirPin_3, LOW);
  for(int x = 0; x < 75; x++){
    digitalWrite(stepPin_2, HIGH);
    digitalWrite(stepPin_3, HIGH);
    delayMicroseconds(3000);
    digitalWrite(stepPin_2, LOW);
    digitalWrite(stepPin_3, LOW);
    delayMicroseconds(3000);

    // Return back to initial position
    if(stopButtonCheck()){
      digitalWrite(dirPin_2, LOW);
      digitalWrite(dirPin_3, HIGH);
      for(int y = x + 1; y >= 0; y--){
        digitalWrite(stepPin_2, HIGH);
        digitalWrite(stepPin_3, HIGH);
        delayMicroseconds(3000);
        digitalWrite(stepPin_2, LOW);
        digitalWrite(stepPin_3, LOW);
        delayMicroseconds(3000);
      } 
      return 1;
    }
    
  }
  
  digitalWrite(dirPin_2, LOW);
  digitalWrite(dirPin_3, HIGH);
  for(int x = 0; x < 75; x++){
    digitalWrite(stepPin_2, HIGH);
    digitalWrite(stepPin_3, HIGH);
    delayMicroseconds(3000);
    digitalWrite(stepPin_2, LOW);
    digitalWrite(stepPin_3, LOW);
    delayMicroseconds(3000);

    // Return back to initial position
    if(stopButtonCheck()){
      for(int y = x + 1; y < 75; y++){
        digitalWrite(stepPin_2, HIGH);
        digitalWrite(stepPin_3, HIGH);
        delayMicroseconds(3000);
        digitalWrite(stepPin_2, LOW);
        digitalWrite(stepPin_3, LOW);
        delayMicroseconds(3000);
      } 
      return 1;
    }
  } 

  return 0;
}

int conveyorMotion(int quant){

  digitalWrite(dirPin_1, LOW);
  for(int x = 0; x < 33 * quant; x++){
    digitalWrite(stepPin_1, HIGH);;
    delayMicroseconds(3000);
    digitalWrite(stepPin_1, LOW);
    delayMicroseconds(3000);
    
    if(stopButtonCheck()){
      for(int y = x + 1; y < 33 * quant; y++){
        digitalWrite(stepPin_1, HIGH);;
        delayMicroseconds(3000);
        digitalWrite(stepPin_1, LOW);
        delayMicroseconds(3000);
      }  
      return 1;
    }
  }
  return 0;
}

int stopButtonCheck(){
  int pushed = digitalRead(stopPin);
  TSPoint p;
  if ((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE)){
    p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, 480);
    p.y = map (p.y, TS_MINY, TS_MAXY, 0, 320);
  }

  if((pushed == HIGH) | (p.x > 340 & p.x < 420 & p.y > 220 & p.y < 300)){
    tft.fillRect(0,0,480,320,RED);
    tft.setTextSize(6);
    tft.setCursor(80,140);
    tft.print(" WARNING ");
    digitalWrite(buzzerPin, HIGH);
    delay(1000); // Wait a second
    digitalWrite(buzzerPin, LOW);
    
    return 1;
  }
  return 0;
  
}


int CameraCheck(){
  if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
  else if(!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
  else if(!huskylens.available()) Serial.println(F("No block or arrow appears on the screen!"));
  else
  {
        if (huskylens.available())
        {
            HUSKYLENSResult result = huskylens.read();
            if(result.width > 15){
              tft.fillRect(0,0,480,320,RED);
              tft.setTextSize(6);
              tft.setCursor(80,140);
              tft.print(" WARNING ");
              digitalWrite(buzzerPin, HIGH);
              delay(1000); // Wait a second
              digitalWrite(buzzerPin, LOW);
              return 1;
            }
            Serial.println(result.width);
        }    
  }
  return 0;
}
