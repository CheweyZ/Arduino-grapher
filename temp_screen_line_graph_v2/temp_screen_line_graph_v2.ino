#include "cactus_io_DHT22.h"
#define DHT22_PIN A4
DHT22 dht(DHT22_PIN);

#include <Time.h>
int time = 0;
time_t t;

#include <SD.h>
File masterfile;
#include <SPI.h>          // f.k. for Arduino-1.5.2
#include "Adafruit_GFX.h"// Hardware-specific library
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

//#include <stdint.h>
//___________________________________
#include "TouchScreen.h"
//____________________________________
/*#if defined(__SAM3X8E__)
  #undef __FlashStringHelper::F(string_literal)
  #define F(string_literal) string_literal
  #endif
  //http://www.xcluma.com/how-to-use-2.4inch-tft-lcd-mcufriend
*/

/*#include <stdio.h>
  uint16_t ID;
  uint8_t hh, mm, ss;
  uint8_t conv2d(const char* p)
  {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9') v = *p - '0';
  return 10 * v + *++p - '0';
  }*/

// These are the pins for the shield!
#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin
//uint8_t SwapXY = 0;

uint16_t TS_LEFT = 920;
uint16_t TS_RT  = 150;
uint16_t TS_TOP = 940;
uint16_t TS_BOT = 120;
/*
#define MINPRESSURE 10
#define MAXPRESSURE 1000*/



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
double history[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//TSPoint tp;
int oldsecond = 1;
int currentview = 1;
//int currentvalue = 1;
int howmanyaccross = 15;
int multiplier = 1;
int holder;
String holder2;
int holder3;
int spacetilltop;
//uint8_t buffer[2];
//DH22 operating 0-100H -40~80C

//USER settings Below :Leave All Settings Above


int linecol = GREEN;
int humidcol = WHITE;
int tempcol = WHITE;
int humidwarn = YELLOW;
int tempwarn = YELLOW;
int humidhigh = RED;
int temphigh = RED;

int warntemp = 26;
int hightemp = 35;
/*int warntemp = 22;
  int hightemp = 25;*/

int warnhumid = 60;
int highhumid = 80;

//End Of User Settings

void setup() {
  Serial.begin(9600);
  dht.begin();

  SD.begin(10);
  /*Serial.print("Initializing SD card...");

    if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
    }
    Serial.println("initialization done.");*/
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.fillScreen(BLACK);
  //tft.setTextSize(2);
  tft.setRotation(1);
  setTime(22, 14, 55, 07, 14, 17);
  /*  hh = conv2d(__TIME__);
    mm = conv2d(__TIME__ + 3);
    ss = conv2d(__TIME__ + 6);
    //setTime(hh, mm, ss, 07, 14, 17);*/
  //if (!SD.exists("settings.txt")) {
  masterfile = SD.open("settings.txt");
  masterfile.close();
  masterfile = SD.open("monthlog.txt");
  masterfile.close();
  /* masterfile = SD.open("daylog.txt");
    masterfile.close();*/
  /* masterfile = SD.open("logh.txt");
    masterfile.close();
     masterfile = SD.open("logt.txt");
    masterfile.close();*/
  masterfile = SD.open("testh.txt");
  masterfile.close();
  masterfile = SD.open("testt.txt");
  masterfile.close();
  // }

  //grapher();
}

void loop() {
  //tft.drawLine(x1, y1, x2, y2, color);
  if (oldsecond != second()) { //to avoid constant flicker and using delay() cause interupt and clock halts so within 1 cycle is out of sync so
    //this method alows a update to occur every second and cauing no system hault
    dht.readHumidity();
    dht.readTemperature();
    int myWidth = tft.width();
    int myHeight = tft.height();
    //tft.fillRect(0, 0, myWidth, myHeight/4, BLACK);
    tft.setTextColor(WHITE, BLACK);
    tft.setCursor((myWidth * 6 / 8) / 3, myHeight / 8);
    if (second() == 0) {
      tft.fillRect(0, 0, myWidth, myHeight / 4, BLACK); //as wierd stuff happens if this is not here the placement of the second jumps one and leaves the 9 from 59
      // above the line fills to make the refresh as seamless as possible

    }
    tft.print(second());

    tft.drawFastHLine(0, myHeight / 4, myWidth, linecol);
    tft.drawFastVLine(myWidth / 3, 0, myHeight / 4, linecol);
    tft.drawFastVLine((myWidth / 3) * 2, 0, myHeight / 4, linecol);
    //tft.drawFastHLine(0, (myHeight*7)/8, myWidth, linecol);  works use for weeks

    if (currentview == 1) {
      tft.drawFastHLine(0, (myHeight - 12), myWidth, linecol);
      howmanyaccross = 15;
      multiplier = 15;
    } else if (currentview == 2) {
      tft.setCursor(0, (myHeight - 15));
      tft.drawFastHLine(0, (myHeight * 7) / 8, myWidth, linecol);
      howmanyaccross = 7;
      multiplier = 1;
    }
    //currentvalue = 0;
    tft.setTextSize(1);
    for (int currentvalue = 0; currentvalue <= howmanyaccross; currentvalue++) {
      //tft.drawFastVLine(((myWidth * currentvalue) / howmanyaccross), (myHeight) / 4, myHeight, linecol);
      tft.setCursor(((myWidth * currentvalue) / howmanyaccross) - howmanyaccross - 5, (myHeight - 10));
      tft.println(currentvalue * multiplier);
      //currentvalue++;
    }
    tft.setTextSize(2);



    //  tft.drawFastVLine();
    //Serial.println(myWidth/3);
    //Serial.println(minute());


    if (((minute() % 15) == 0) && (second() == 0)) {
      masterfile = SD.open("testh.txt", FILE_WRITE);
      //masterfile.seek(0);
      masterfile.println(int(dht.humidity));
      // Serial.println(int(dht.humidity));
      //masterfile.flush();
      masterfile.close();
      masterfile = SD.open("testt.txt", FILE_WRITE);
      //masterfile.seek(0);
      masterfile.println(int(dht.temperature_C));
      //masterfile.flush();
      masterfile.close();

      //masterfile.seek(4);
      // if (masterfile.available()) {

      grapher();
      //shifter("testh");
    } else if ((minute() == 59) && (second() == 0)) {
      masterfile = SD.open("a.txt");
      while (masterfile.available()) {
        holder2=masterfile.readStringUntil('\n');
        holder=holder2.toInt();
      }}




      tft.setCursor((myWidth / 8) / 3, myHeight / 8);
      tft.print(hour());
      tft.setCursor((myWidth * 3.5 / 8) / 3, myHeight / 8);
      tft.print(minute());

      if (dht.humidity > highhumid) {
        tft.setTextColor(humidhigh, BLACK);
      } else if (dht.humidity > warnhumid) {
        tft.setTextColor(humidwarn, BLACK);
      } else {
        tft.setTextColor(humidcol, BLACK);
      }
      //tft.setTextColor(humidcol,BLACK);
      tft.setCursor((myWidth / 3) + 20, myHeight / 8);
      tft.print(dht.humidity);
      if (dht.temperature_C > hightemp) {
        tft.setTextColor(temphigh, BLACK);
      } else if (dht.temperature_C > warntemp) {
        tft.setTextColor(tempwarn, BLACK);
      } else {
        tft.setTextColor(tempcol, BLACK);
      }
      tft.setCursor(((2 * myWidth) / 3) + 20, myHeight / 8);
      tft.print(dht.temperature_C);

      // openspace = myHeight - 12 - myHeight / 4;

      //  fillhumid(openspace,myWidth,myHeight);
      //  filltemp(openspace,myWidth,myHeight);

      if (second() == 30) {
        currentview += 1;
        if (currentview == 3) {
          currentview -= 2;
        }
        grapher();
      }



      //Serial.println(openspace);
      //Serial.println(myHeight); //240
      //Serial.println(myWidth); //320
      oldsecond = second();
    }
    //delay(3000);

  }

  void grapher() {
    tft.fillScreen(BLACK);
    if (currentview == 1) {
      shifter("testh", "logh");
      shifter("testt", "logt");
      outgraph("logh", 15, BLUE);
      outgraph("logt", 15, GREEN);
    }
  }



  void outgraph(String fileto, int howmany, int col) {
    int myWidth = tft.width();
    int myHeight = tft.height();
    int drawpointx = 0;
    int drawpointy = 0;
    masterfile = SD.open(String(fileto + ".txt"));
    for (int upto = 0; upto <= howmany; upto++) {
      //Serial.println(masterfile.read());
      //masterfile.seek(upto);
      //holder=masterfile.read(buffer,sizeof(buffer));
      holder2 = masterfile.readStringUntil('\n');
      holder = holder2.toInt();
      // masterfile.read();
      //holder=20;
      // Serial.println(holder);
      // Serial.write(masterfile.read());
      // if (currentview == 1) {
      //tft.drawLine(x1, y1, x2, y2, color);
      spacetilltop = map(holder, 0, 100, myHeight - 15, myHeight / 4);
      //tft.drawLine((myWidth * (upto/2) / howmanyaccross),(((myWidth * (upto/2)) / howmanyaccross)*2),spacetilltop,spacetilltop,BLUE);
      // tft.drawLine(drawpointx, drawpointy, (myWidth * (upto / 2) / howmany), spacetilltop, col);
      tft.drawLine(drawpointx, drawpointy, ((myWidth * upto) / howmany), spacetilltop, col);
      // drawpointx = (myWidth * (upto / 2) / howmany);
      drawpointx = (myWidth * upto) / howmany;
      drawpointy = spacetilltop;
      //}
      if (!masterfile.available()) {
        break;
      }
    }
    masterfile.close();
  }




  void shifter(String fileto, String newfile) {
    SD.remove(String(newfile + ".txt"));
    int temp = 0;
    int temp2 = 0;
    //Serial.println("it");
    masterfile = SD.open(String(fileto + ".txt"));
    //masterfile.seek(20);
    while (masterfile.available()) {
      masterfile.readStringUntil('\n');
      temp++;
      //Serial.println("works");
    }
    masterfile.close();


    // masterfile = SD.open(String(fileto + ".txt"));
    for (int count = 0; count < 16; count++) {
      masterfile = SD.open(String(fileto + ".txt"));
      temp2 = 0;
      while (masterfile.available()) {
        if (temp2 == (temp - count + 1)) {
          holder2 = masterfile.readStringUntil('\n');
          holder = holder2.toInt();
          //Serial.println("it");
          history[count] = holder;
          //Serial.println(history[count]);
          //Serial.println("works");
        }
        masterfile.readStringUntil('\n');
        temp2++;
      }
      masterfile.close();
      //history[temp]=holder;
      //Serial.println(history[temp]);
      //temp++;

    }
    //masterfile.close();
    /*
      Serial.println("_");
      Serial.println(history[0]);
      Serial.println(history[15]);
      Serial.println(history[18]);
    */
    //Serial.println(sizeof(history));
    //Serial.println(history[50]);

    masterfile = SD.open(String(newfile + ".txt"), FILE_WRITE);
    for (int c = 0; c < 15; c++) {

      masterfile.println(history[c]);
      //Serial.println(history[c]);
      masterfile.flush();

    }
    masterfile.close();


  }


  /*
    masterfile = SD.open("testh.txt", FILE_WRITE);
    //masterfile.seek(0);
    masterfile.println(int(dht.humidity));
  */


  /*void error() {
    tft.setCursor(tft.height() / 2, (tft.width() / 2));
    tft.print("THE END IS NAY");
    }

    void cleanuppins() {

    // Clean up pin modes for LCD
    pinMode(XM, OUTPUT);
    digitalWrite(XM, LOW);
    pinMode(YP, OUTPUT);
    digitalWrite(YP, HIGH);
    pinMode(YM, OUTPUT);
    digitalWrite(YM, LOW);
    pinMode(XP, OUTPUT);
    digitalWrite(XP, HIGH);
    }*/
