#include <Arduino_FreeRTOS.h>
#include <Adafruit_GFX.h>                 //graphix library for drawing
#include <Adafruit_TFTLCD.h>              //LCD library to drive screen
#include "TouchScreen.h"                  //library for tounchscreen
#include <Wire.h>                         //I2C library
#include <radio.h>                        //Radio library
#include <TEA5767Radio.h>                     //si4703 library
#include "MCUFRIEND_kbv.h"   
#include <TEA5767.h>
#include <semphr.h>

#define YP A3                             // touchscreen - must be an analog pin, use "An" notation!
#define XM A2                             // touchscreen - must be an analog pin, use "An" notation!
#define YM 9                              // touchscreen - can be a digital pin
#define XP 8                              // touchscreen - can be a digital pin

#define TS_MINX 150                       //touchscreen - min x value
#define TS_MINY 120                       //touchscreen - min y value
#define TS_MAXX 920                       //touchscreen - max x value
#define TS_MAXY 940                       //touchscreen - max y value

TEA5767 radio;
//TEA5767Radio radio = TEA5767Radio();                             //create instance of SI4703 called radio
                                          //create instance of touchscreen called ts
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3                         //LCD Control Pins
#define LCD_CD A2                         //LCD Control Pins
#define LCD_WR A1                         //LCD Control Pins
#define LCD_RD A0                         //LCD Control Pins
                                          // optional
#define LCD_RESET A4                       //LCD Control Pins

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

                                          //create instance of TFTLCD called tft
MCUFRIEND_kbv tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);


int station=10060;                        //var to hold station
//int volume = 5;
String gear="";                           //var to hold volume
int cursor=22;
int horz;                                 //var to hold x touch
int vert;                                 //var to hold y touch
String rdsname="ON";                    //var to hold RDS name data
String rdstext="OFF";                 //var to hold RDS Text data
//String rdstime="12:12";                   //var to hold RDS Time data
int xVal; //X values from joystick
int yVal; //Y values from joystick
int buttonVal;
int bottone = 49; 
int X = A14;  //I assign the letter X to pin A1
int Y = A13;
//--------------------------
//7 SEG DISPLAY
const int segA = 26;
const int segB = 27;
const int segC = 28;
const int segD = 29;
const int segE = 30;
const int segF = 31;
const int segG = 32;
const int segH = 33;  // Decimal point or Dp
const char common = 'c';  // common cathode
bool decPt = true;  // decimal point display flag
//-------------------
//LDR
int ldr;
void Task1(void *pvParameters);
void Task2(void *pvParameters);
SemaphoreHandle_t xSemaphore;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);                    //serial comms for debug
  xSemaphore = xSemaphoreCreateBinary();
  tft.reset();                           //reset the screen
 uint16_t id=tft.readID();
  tft.begin(id);                    //start screen using chip identifier hex

  tft.setRotation(1);                    //set rotation for wide screen
  tft.fillScreen(BLACK);                 //fill screen with black (ersatz clear)
  //delay(250);
  tft.setCursor(0, 0); 
  tft.setTextColor(WHITE);               //set text color white
  tft.setTextSize(2);                    //set text size to 2 (1-6)
  tft.println("     Arduino FM Radio");  //print header to screen
  
   tft.drawRoundRect(10, 20, 300, 200, 6, WHITE); //draw screen outline
   tft.fillRoundRect(20, 30, 160, 50, 6, RED);    //draw station box
   tft.fillRoundRect(20, 90, 180, 50, 6, BLUE);   //draw volume box
    tft.fillRoundRect(210, 90, 60, 50, 6, BLUE);
   //tft.fillRoundRect(135, 90, 50, 50, 6, BLUE);   //draw mute box
   tft.fillRoundRect(20, 150,110, 50, 6, YELLOW); //draw RDS box
   tft.fillRoundRect(140, 150,110, 50, 6, YELLOW); 
   tft.fillRoundRect(190, 30, 50, 50, 6, RED);    //draw station up buton
   tft.fillRoundRect(250, 30, 50, 50, 6, RED);    //draw station down buton
   //tft.fillRoundRect(190, 90, 50, 50, 6, BLUE);   //draw volume up buton
   //tft.fillRoundRect(250, 90, 50, 50, 6, BLUE);   //draw volume down buton
   tft.fillTriangle(215, 30, 192, 67, 235, 67,WHITE); //draw up triangle for station
   tft.fillTriangle(275, 67, 252, 30, 295, 30,WHITE); //draw down triangle for station
   //tft.fillTriangle(215, 90, 192, 127, 235, 127,WHITE); //draw up triangle for volume
   //tft.fillTriangle(275, 127, 252, 90, 295, 90,WHITE);  //draw down triangle for volume
   //delay(250);
  //  tft.setCursor(152, 105);                             //put cursor in mute box
  //  tft.setTextSize(3);                                  //set text size 3
  //  tft.println("X");                                    //write a bog ol x in mute box

 // volume = constrain(volume, 1, 15);
  // radio.init();                                        //initialize radio

  // radio.setFrequency(10060);      //set band and freq
  // radio.setVolume(volume);                              //set volume
  // radio.setMono(false);                                 //set stereo
  // radio.setMute(false);  
  pinMode(X, INPUT); //Sets the analog ports used to an input
pinMode(Y, INPUT);
//pinMode(bottone,INPUT);
//---------------
//LDR
pinMode(53,OUTPUT);
pinMode(51,OUTPUT);
pinMode(A15,INPUT);
digitalWrite(segA,LOW);
digitalWrite(segB,LOW);
digitalWrite(segC,LOW);
digitalWrite(segD,LOW);
digitalWrite(segE,LOW); // DOT
digitalWrite(segF,LOW);
digitalWrite(segG,LOW);
digitalWrite(segH,LOW);

xTaskCreate(
  Radio
  ,"Radio"
  ,300
  ,NULL
  ,1
  ,NULL);
xTaskCreate(
  control
  ,"Control"
  ,128
  ,NULL
  ,2
  ,NULL);
  vTaskStartScheduler(); 

}

void loop() {
  // put your main code here, to run repeatedly:

}
void DLetter()
{
  digitalWrite(segA,HIGH);
  digitalWrite(segB,HIGH);
  digitalWrite(segC,HIGH);
  digitalWrite(segD,LOW);
  digitalWrite(segE,LOW);
  digitalWrite(segF,HIGH);
  digitalWrite(segG,HIGH);
  digitalWrite(segH,HIGH);
}
void PLetter()
{
  digitalWrite(segA,HIGH);
  digitalWrite(segB,HIGH);
  digitalWrite(segC,HIGH);
  digitalWrite(segD,HIGH);
  digitalWrite(segE,LOW);
  digitalWrite(segF,LOW);
  digitalWrite(segG,LOW);
  digitalWrite(segH,HIGH);
}
void RLetter()
{
 digitalWrite(segA,HIGH);
  digitalWrite(segB,HIGH);
  digitalWrite(segC,HIGH);
  digitalWrite(segD,HIGH);
  digitalWrite(segE,LOW);
  digitalWrite(segF,HIGH);
  digitalWrite(segG,LOW);
  digitalWrite(segH,HIGH);
}
void NLetter(){
  digitalWrite(segA,HIGH);
  digitalWrite(segB,HIGH);
  digitalWrite(segC,LOW);
  digitalWrite(segD,HIGH);
  digitalWrite(segE,HIGH);
  digitalWrite(segF,LOW);
  digitalWrite(segG,HIGH);
  digitalWrite(segH,HIGH);
}
void think(){                                           //logic for buttons
  if(horz>240 && horz<390){   
     //Serial.print("kiki");        //station up
    if(vert>576 && vert<705){
      Serial.print("nada");
      // radio.seekUp();
      // station=radio.getFrequency();
      station=station+10;
      tft.fillRoundRect(20, 30, 160, 50, 6, RED);
      radio.setFrequency(station);
    }
  }
  if(horz>240 && horz<390){ 
     //Serial.print("hooo");         //station down
    if(vert>750 && vert<870){
      station=station-10;
      // radio.seekDown();
      // station=radio.getFrequency();
      tft.fillRoundRect(20, 30, 160, 50, 6, RED);
      radio.setFrequency(station);
    }
  }
//  if(horz>538 && horz<588){           //volume up
//      if(vert>302 && vert<390){
//        volume++;
//        tft.fillRoundRect(20, 90, 110, 50, 6, BLUE);
//        radio.setVolume(volume);
//      }
//    }
  //  

   
   if(horz>690 && horz<740){           //mute
     if(vert>160 && vert<385){
       //Serial.print("hiii");
       radio.init();
       radio.setMute(false); 
       //station=10060;
       radio.setFrequency(station);
     }
   }
   if(horz>690 && horz<750){
     if(vert>460 && vert<600){
       //Serial.print("hooo");
       radio.setMute(true); 
       //station=00000;
       radio.term();     }
   }
   
}
void Radio(void *pvParameters){
  (void) pvParameters;
  TickType_t xLastWakeTime;

xLastWakeTime = xTaskGetTickCount(); 
  while(1){
xSemaphoreTake(xSemaphore, portMAX_DELAY);    
    //Serial.println("radio");
    tft.setCursor(22, 40);
 tft.setTextColor(WHITE);
 tft.setTextSize(2);
 tft.print("Station:");
 tft.setTextColor(WHITE);
 tft.print(station);                //write station
 tft.setCursor(22, 110);
 tft.setTextColor(WHITE);
 tft.setTextSize(2);
 tft.print("Current Gear:");
 tft.setCursor(212, 110);
 tft.setTextColor(WHITE);
 tft.print(gear);
 tft.setTextColor(BLACK);
 tft.setCursor(152, 170);
 tft.setTextSize(3);
 tft.print(rdstext);                //write RDS data
tft.setCursor(32, 170);
 tft.print(rdsname);
//tft.setCursor(22, 180);
 //tft.print(rdstext);
                                    //look for a touch
                                    //since we are sharing pins between display & touch
                                    //we must change directions everytime we look for a touch
   digitalWrite(13, HIGH);
   TSPoint p = ts.getPoint();       //get touch
   digitalWrite(13, LOW);
   pinMode(XM, OUTPUT);
   pinMode(YP, OUTPUT);
   
   if (p.z > ts.pressureThreshhold) { //if touch is above threshold
     
     horz=p.x;                        //send touch values to variables
     vert=p.y;
    //  Serial.print("horz: ");
    //  Serial.println(horz);
    //  Serial.print("vert: ");
    //  Serial.println(vert);
     think();                         //call think function
  }
  xSemaphoreGive(xSemaphore);
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(80));
  
 // delay(1000);
  }
  
}
void control(void *pvParameters){
  (void) pvParameters;
  TickType_t xLastWakeTime;
xLastWakeTime = xTaskGetTickCount();

  while(1){
    //xSemaphoreTake(xSemaphore, portMAX_DELAY);
    //Serial.println("ldr");
    xVal = analogRead(X); //sets the X value
yVal = analogRead(Y); //sets the Y value
Serial.println(yVal);
buttonVal = digitalRead(bottone); //sets the Y value
if (xVal<50 && yVal<600 && yVal>500){
  Serial.println("P");
  //cursor=22;
  gear=" P";  
  PLetter();
   tft.fillRoundRect(210, 90, 60, 50, 6, BLUE);
}
if (yVal<100  && xVal<600 && xVal>500){
  Serial.println("R"); 
  gear=" R";
  
   RLetter();
    tft.fillRoundRect(210, 90, 60, 50, 6, BLUE);
}
if (xVal>1000 && yVal<600 && yVal>500){
  Serial.println("N");
  
  //cursor=62;
 gear=" N";
   NLetter();
    tft.fillRoundRect(210, 90, 60, 50, 6, BLUE);
}
if (yVal>1000 && xVal<600 && xVal>500){
  Serial.println("D");
  //cursor=82;
  gear=" D";
   DLetter();
    tft.fillRoundRect(210, 90, 60, 50, 6, BLUE);
}

//Serial.println(" Y is...");
//
//Serial.println(yVal); //prints Y values
//
//Serial.println(" X is...");
//
//Serial.println(xVal); //prints X values
//Serial.println(" button is...");
//Serial.println(buttonVal);

//Serial.println("ldr:");
//Serial.println(ldr);
 ldr=analogRead(A15);
 if(ldr<100){
    digitalWrite(53,1);
    digitalWrite(51,1);
  }else if(ldr<300){
digitalWrite(53,0);
    digitalWrite(51,1);    
  }
  else {
    digitalWrite(53,0);
    digitalWrite(51,0);
  }
     xSemaphoreGive(xSemaphore);
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));

  }
}
