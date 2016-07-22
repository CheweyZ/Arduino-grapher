#include "cactus_io_DHT22.h"
#define DHT22_PIN A4
DHT22 dht(DHT22_PIN);

#include <Time.h>
int time = 0;
time_t t;

#include <SPI.h>          // f.k. for Arduino-1.5.2
#include "Adafruit_GFX.h"// Hardware-specific library
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

//#include <stdint.h>
#include "TouchScreen.h"

#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif
 
// These are the pins for the shield!
#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin
uint8_t SwapXY = 0;

uint16_t TS_LEFT = 920;
uint16_t TS_RT  = 150;
uint16_t TS_TOP = 940;
uint16_t TS_BOT = 120;
 
#define MINPRESSURE 10
#define MAXPRESSURE 1000
 
// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
int openspace=0;
int graphfill=0;
int graphspot=0;
int history[]={0,0,0,0,0,0,0,0,0,0,0,0};
TSPoint tp;
int currentview=1;
int lastfill;
int spaceabove;
//DH22 operating 0-100H -40~80C

//USER settings Below :Leave All Settings Above


int linecol=GREEN;
int humidcol=WHITE;
int tempcol=WHITE;
int humidwarn=YELLOW;
int tempwarn=YELLOW;
int humidhigh=RED;
int temphigh=RED;

/*int warntemp=26;
int hightemp=35;*/
int warntemp=22;
int hightemp=25;

int warnhumid=60;
int highhumid=80;

//End Of User Settings

void setup() {
  Serial.begin(9600);
  dht.begin();

  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setRotation(1);
  setTime(22,59,55,07,14,17);
}
int oldsecond=0;
void loop() {
  if (oldsecond!=second()){//to avoid constant flicker and using delay() cause interupt and clock halts so within 1 cycle is out of sync so 
    //this method alows a update to occur every second and cauing no system hault
  dht.readHumidity();
  dht.readTemperature();
  int myWidth = tft.width();
  int myHeight = tft.height();
  //tft.fillRect(0, 0, myWidth, myHeight/4, BLACK);
  tft.setTextColor(WHITE,BLACK); 
    tft.setCursor((myWidth*6/8)/3, myHeight/8);
  if (second()==0){
    tft.fillRect(0, 0, myWidth, myHeight/4, BLACK);//as wierd stuff happens if this is not here the placement of the second jumps one and leaves the 9 from 59
    // above the line fills to make the refresh as seamless as possible
    
  }
    tft.print(second());
  
  tft.drawFastHLine(0, myHeight/4, myWidth, linecol);
  tft.drawFastVLine(myWidth/3, 0, myHeight/4, linecol);
  tft.drawFastVLine((myWidth/3)*2, 0, myHeight/4, linecol);
  //tft.drawFastHLine(0, (myHeight*7)/8, myWidth, linecol);  works use for weeks
  
  if (currentview==1){  
  tft.drawFastHLine(0, (myHeight-12), myWidth, linecol);
  tft.setCursor(0, (myHeight-10));
  tft.setTextSize(1);
  tft.print("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21");
  tft.setTextSize(2);
  }
//  tft.drawFastVLine();
  //Serial.println(myWidth/3);
  //Serial.println(minute());

  
  tft.setCursor((myWidth/8)/3, myHeight/8);
  tft.print(hour());
  tft.setCursor((myWidth*3.5/8)/3, myHeight/8);
  tft.print(minute());

  if (dht.humidity>highhumid){
    tft.setTextColor(humidhigh,BLACK); 
  }else if (dht.humidity>warnhumid){
    tft.setTextColor(humidwarn,BLACK); 
  }else{
    tft.setTextColor(humidcol,BLACK); 
  }  
  //tft.setTextColor(humidcol,BLACK); 
  tft.setCursor((myWidth/3)+20, myHeight/8);
  tft.print(dht.humidity);
  if (dht.temperature_C>hightemp){
    tft.setTextColor(temphigh,BLACK);
  }else if (dht.temperature_C>warntemp){
    tft.setTextColor(tempwarn,BLACK);
  }else{
    tft.setTextColor(tempcol,BLACK);
  } 
  tft.setCursor(((2*myWidth)/3)+20, myHeight/8);
  tft.print(dht.temperature_C);

  openspace=myHeight-12-myHeight/4;
  
  fillhumid(openspace,myWidth,myHeight);
  filltemp(openspace,myWidth,myHeight);

  


  

    uint16_t xpos, ypos;  //screen coordinates
    tp = ts.getPoint();   //tp.x, tp.y are ADC values

    // if sharing pins, you'll need to fix the directions of the touchscreen pins
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    pinMode(XP, OUTPUT);
    pinMode(YM, OUTPUT);
    //    digitalWrite(XM, HIGH);
    //    digitalWrite(YP, HIGH);
    // we have some minimum pressure we consider 'valid'
    // pressure of 0 means no pressing!

    /*if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) {//cords dont matter just need a touch value
        // is controller wired for Landscape ? or are we oriented in Landscape?
        //if (SwapXY != (Orientation & 1)) SWAP(tp.x, tp.y);
        // scale from 0->1023 to tft.width  i.e. left = 0, rt = width
        // most mcufriend have touch (with icons) that extends below the TFT
        // screens without icons need to reserve a space for "erase"
        // scale the ADC values from ts.getPoint() to screen values e.g. 0-239
        xpos = map(tp.x, TS_LEFT, TS_RT, 0, tft.width());
        ypos = map(tp.y, TS_TOP, TS_BOT, 0, tft.height());
        Serial.println(ypos);
    }*/
    if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) {
      Serial.println(tp.z);
    }
  
  
    
  //Serial.println(openspace);
  //Serial.println(myHeight); //240
  //Serial.println(myWidth); //320
  oldsecond=second();
  }  
  //delay(3000);

}

void fillhumid(int openspace,int myWidth,int myHeight){
  graphfill=map(dht.humidity,0,99.9,0,openspace);
  if (hour()<=10){
  graphspot=map(hour()-1,0,9,0,myWidth/3);
  //tft.fillRect(graphspot,lastfill,10,-(openspace-lastfill),RED);
  tft.fillRect(graphspot,spaceabove,10,(myHeight/4),BLACK);
  tft.fillRect(graphspot, (myHeight-12), 10, -graphfill, GREEN);
  
//  lastfill=-graphfill;
//  spaceabove=(myHeight-12-graphfill);
  
  }else{
      graphspot=map(hour(),11,24,(myWidth/3),myWidth);
      graphspot+=30;
     //tft.fillRect(graphspot,lastfill,10,-(openspace-lastfill),RED);
     // tft.fillRect(graphspot,spaceabove,10,(myHeight/4),BLACK);
      tft.fillRect(graphspot, (myHeight-12), 10, -graphfill, GREEN);
  }
    lastfill=-graphfill;
  spaceabove=(myHeight-12-graphfill);

 // tft.fillRect((myWidth*(hour()-1.5))/24, (myHeight-12), 10, -graphfill, YELLOW);
}

void filltemp(int openspace,int myWidth,int myHeight){
  graphfill=map(dht.temperature_C,-40,80,0,openspace);
  if (hour()<=10){
  graphspot=map(hour()-1,0,9,0,myWidth/3);
  tft.fillRect(graphspot, (myHeight-12), 10, -graphfill, BLUE);
  }
}


void cleanuppins(){
 
   // Clean up pin modes for LCD
   pinMode(XM, OUTPUT);
   digitalWrite(XM, LOW);
   pinMode(YP, OUTPUT);
   digitalWrite(YP, HIGH);
   pinMode(YM, OUTPUT);
   digitalWrite(YM, LOW);
   pinMode(XP, OUTPUT);
   digitalWrite(XP, HIGH);
}
