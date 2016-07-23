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


// These are the pins for the shield!
#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin
//uint8_t SwapXY = 0;
/*
  uint16_t TS_LEFT = 920;
  uint16_t TS_RT  = 150;
  uint16_t TS_TOP = 940;
  uint16_t TS_BOT = 120;

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
//DH22 operating 0-100H -40~80C

//USER settings Below :Leave All Settings Above


int linecol = GREEN;
int humidcol = WHITE;
int tempcol = WHITE;
int humidwarn = YELLOW;
int tempwarn = YELLOW;
int humidhigh = RED;
int temphigh = RED;

/*
int warntemp = 26;
int hightemp = 35;

int warnhumid = 60;
int highhumid = 80;
*/

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
  setTime(22, 59, 55, 07, 14, 17);
  /*  hh = conv2d(__TIME__);
    mm = conv2d(__TIME__ + 3);
    ss = conv2d(__TIME__ + 6);
    //setTime(hh, mm, ss, 07, 14, 17);*/

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
    /* tft.setCursor((myWidth * 6 / 8) / 3, myHeight / 8);
      if (second() == 0) {
       tft.fillRect(0, 0, myWidth, myHeight / 4, BLACK); //as wierd stuff happens if this is not here the placement of the second jumps one and leaves the 9 from 59
       // above the line fills to make the refresh as seamless as possible

      }
      tft.print(second());*/

    tft.drawFastHLine(0, myHeight / 4, myWidth, linecol);
    tft.drawFastVLine(myWidth / 3, 0, myHeight / 4, linecol);
    tft.drawFastVLine((myWidth / 3) * 2, 0, myHeight / 4, linecol);
    //tft.drawFastHLine(0, (myHeight*7)/8, myWidth, linecol);  works use for weeks

    if (currentview == 1) {
      // tft.drawFastHLine(0, (myHeight - 12), myWidth, linecol);
      howmanyaccross = 15;
      multiplier = 15;
    } else if (currentview == 2) {
      // tft.setCursor(0, (myHeight - 15));
      // tft.drawFastHLine(0, (myHeight * 7) / 8, myWidth, linecol);
      howmanyaccross = 7;
      multiplier = 1;
    }
    tft.drawFastHLine(0, (myHeight - 12), myWidth, linecol);
    //currentvalue = 0;
    tft.setTextSize(1);
    for (int currentvalue = 0; currentvalue <= howmanyaccross; currentvalue++) {
      //tft.drawFastVLine(((myWidth * currentvalue) / howmanyaccross), (myHeight) / 4, myHeight, linecol);
      tft.setCursor(((myWidth * currentvalue) / howmanyaccross) - howmanyaccross - 5, (myHeight - 10));
      tft.println(currentvalue * multiplier);
      //currentvalue++;
    }
    tft.setTextSize(2);


    if (((minute() % 15) == 0) && (second() == 0)) {

      writer("hh", dht.humidity);
      writer("ht", dht.temperature_C);

      grapher();
    }
    if ((hour() == 23) && (minute() == 00)&&(second() == 00)) {
      weekavg("h");
      weekavg("t");
      grapher();
    }




    tft.setCursor((myWidth / 8) / 3, myHeight / 8);
    tft.print(hour());
    tft.setCursor((myWidth * 3.5 / 8) / 3, myHeight / 8);
    tft.print(minute());

     /*if (dht.humidity > highhumid) {
       tft.setTextColor(humidhigh, BLACK);
      } else if (dht.humidity > warnhumid) {
       tft.setTextColor(humidwarn, BLACK);
      } else {*/
    tft.setTextColor(humidcol, BLACK);
    //}
    //tft.setTextColor(humidcol,BLACK);
    tft.setCursor((myWidth / 3) + 20, myHeight / 8);
    tft.print(dht.humidity);
    /*if (dht.temperature_C > hightemp) {
      tft.setTextColor(temphigh, BLACK);
      } else if (dht.temperature_C > warntemp) {
      tft.setTextColor(tempwarn, BLACK);
      } else {*/
    tft.setTextColor(tempcol, BLACK);
    // }
    tft.setCursor(((2 * myWidth) / 3) + 20, myHeight / 8);
    tft.print(dht.temperature_C);

    if (second() == 15) {
      currentview += 1;
      if (currentview == 3) {
        currentview -= 2;
      }
      grapher();
    }
    oldsecond = second();
  }
}

void grapher() {
  tft.fillScreen(BLACK);
  if (currentview == 1) {
    shifter("hh", "h");
    shifter("ht", "t");
    outgraph("h", 15, BLUE);
    outgraph("t", 15, GREEN);
  } else {
    shifter("wh", "wlh");
    shifter("wt", "wlt");
    outgraph("wlh", 7, BLUE);
    outgraph("wlt", 7, GREEN);
  }
}


void writer(String fileto, float data) {
  masterfile = SD.open(String(fileto + ".csv"), FILE_WRITE);
  masterfile.println(data);
  masterfile.close();
}


void weekavg(String fileto) {
  holder3 = 0;
  holder = 0;
  masterfile = SD.open(String(fileto + ".csv"));
  while (masterfile.available()) {
    holder2 = masterfile.readStringUntil('\n');
    holder += holder2.toInt();
    holder3++;
  }
  masterfile.close();
  writer((String("w" + fileto)), (holder / 15));
}



void outgraph(String fileto, int howmany, int col) {
  int myWidth = tft.width();
  int myHeight = tft.height();
  int drawpointx = 0;
  int drawpointy = 0;
  masterfile = SD.open(String(fileto + ".csv"));
  for (int upto = 0; upto <= howmany; upto++) {
    holder2 = masterfile.readStringUntil('\n');
    holder = holder2.toInt();
    spacetilltop = map(holder, 0, 100, myHeight - 15, myHeight / 4);
    tft.drawLine(drawpointx, drawpointy, ((myWidth * upto) / howmany), spacetilltop, col);
    drawpointx = (myWidth * upto) / howmany;
    drawpointy = spacetilltop;
    if (!masterfile.available()) {
      break;
    }
  }
  masterfile.close();
}




void shifter(String fileto, String newfile) {
  SD.remove(String(newfile + ".csv"));
  int temp = 0;
  int temp2 = 0;
  //Serial.println("it");
  masterfile = SD.open(String(fileto + ".csv"));
  //masterfile.seek(20);
  while (masterfile.available()) {
    masterfile.readStringUntil('\n');
    temp++;
    //Serial.println("works");
  }
  masterfile.close();


  for (int count = 0; count < 16; count++) {
    masterfile = SD.open(String(fileto + ".csv"));
    temp2 = 0;
    while (masterfile.available()) {
      if (temp2 == (temp - count)) {
        holder2 = masterfile.readStringUntil('\n');
        holder = holder2.toInt();
        history[count] = holder;
      }
      masterfile.readStringUntil('\n');
      temp2++;
    }
    masterfile.close();

  }
  masterfile.close();


  masterfile = SD.open(String(newfile + ".csv"), FILE_WRITE);
  for (int c = 0; c < 15; c++) {
    masterfile.println(history[c]);

  }
  masterfile.close();
}

