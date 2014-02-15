/*
 * GPS Navigator program on Arduino IDE
 *
 *  Created on: Feb. 15 , 2014
 *      Author: kodera2t
 */
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <UTFT.h>
#include <avr/pgmspace.h>
#include <Time.h>
///// Connection to GPS module, 52:RX pin, 50:TX pin of Mega
static const int RXPin = 52, TXPin = 50;
static const uint32_t GPSBaud = 9600;

// Declare which fonts we will be using
extern uint8_t BigFont[];

UTFT myGLCD(ITDB32S,38,39,40,41);
// map data
extern unsigned int world_map[13440] ;

TinyGPSPlus gps;

SoftwareSerial ss(RXPin, TXPin);
unsigned long blink_count2=0,blink_count=0;
int c_flag=0;
void setup()
{
  blink_count2=0;
  myGLCD.InitLCD();
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(18, 18, 149);
  myGLCD.fillScr(18, 18, 149);
  myGLCD.setColor(237,234,17);
  myGLCD.print("londitude", 5,200);
  myGLCD.print("latitude", 170,200);
  myGLCD.drawBitmap (0, 0, 160, 84, world_map,2);
  ss.begin(GPSBaud);
}

void loop()
{
  float flat, flon;
  int mark_x, mark_y;
  while(ss.available()){
    char c = ss.read();
    if(gps.encode(c)){
    printDateTime(gps.date, gps.time,5,175);
    printFloat(gps.location.lng(), gps.location.isValid(), 13, 10, 5, 220);
    printFloat(gps.location.lat(), gps.location.isValid(), 12, 10, 180, 220);
    flon=gps.location.lng();
    flat=gps.location.lat();
    if(flat>0){      
    mark_y=(90.0-flat)*(84.0/90.0);
    }else{
    mark_y=84.0-flat*(84.0/90.0);
    }

    if(flon>0){
    mark_x=14.0+flon*0.933;
    }else{
    if(flon>-15){
      mark_x=(flon+15)*0.933;
    }else{
      mark_x=182-(flon+180)*0.933;
    }
    }
   blink_count=millis()-2000*blink_count2; 
  if(blink_count>2000){
    blink_count2++;
    if(c_flag==0){
    myGLCD.setColor(237,0,0);
    c_flag=1;
  }else{
    myGLCD.setColor(0,0,237);
    c_flag=0;    
  }
    myGLCD.fillCircle(mark_x, mark_y, 4);
  }  
  myGLCD.setColor(237,234,17);
///if(gps.encode(c)) end
    }
///GPS data processing end
  }

}//end of loop


// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}


static void printFloat(float val, bool valid, int len, int prec, int x_posi, int y_posi)
{

  if (!valid)
  {
    while (len-- > 1)
      myGLCD.print("*", x_posi, y_posi);
  }
  else
  {
    myGLCD.printNumF(val, prec, x_posi, y_posi);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
    myGLCD.print(" ", x_posi, y_posi);
  }
  smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len, int x_posi, int y_posi)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  myGLCD.print(sz, x_posi, y_posi);
  smartDelay(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t, int x_posi, int y_posi)
{
  if (!d.isValid())
  {
    myGLCD.print("********** ", x_posi, y_posi);
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    myGLCD.print(sz, x_posi, y_posi);
  }
  
  if (!t.isValid())
  {
    myGLCD.print("********** ", x_posi, y_posi);
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    myGLCD.print(sz, x_posi+170, y_posi);
  }
  smartDelay(0);
}

static void printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartDelay(0);
}
