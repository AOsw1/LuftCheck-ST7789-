#include <Arduino.h>

#include "HW_Funktionen.h"
#include "defs.h"

extern int led1ChannelR;
extern int led1ChannelG;
extern int led1ChannelB;
extern int led2ChannelR;
extern int led2ChannelG;
extern int led2ChannelB;

int MaxR=170;
int MaxG=120;
int MaxB=210;

void SetLED1(int myR, int myG, int myB)
{
    if (myR>MaxR) {myR=MaxR;}
    if (myR>MaxG) {myR=MaxG;}
    if (myR>MaxB) {myR=MaxB;}
  
    ledcWrite(led1ChannelR, myR);
    ledcWrite(led1ChannelG, myG);
    ledcWrite(led1ChannelB, myB);
}

void SetLED2(int myR, int myG, int myB)
{
    if (myR>MaxR) {myR=MaxR;}
    if (myR>MaxG) {myR=MaxG;}
    if (myR>MaxB) {myR=MaxB;}

    ledcWrite(led2ChannelR, myR);
    ledcWrite(led2ChannelG, myG);
    ledcWrite(led2ChannelB, myB);
}

//#include "TFT_22_ILI9225.h"
//extern TFT_22_ILI9225 tft;

void LedTest(void)
{
    Serial.println("Test LED1 R");    //tft.drawText(10, 40, "1 R  ");
    for(int i=0; i<120; i++) { SetLED1(i, 0, 0); delay(8); }
    SetLED1(0,0,0);
    Serial.println("Test LED1 G");    //tft.drawText(10, 40, "1 G  ");
    for(int i=0; i<120; i++) { SetLED1(0, i, 0); delay(8); }
    SetLED1(0,0,0);
    Serial.println("Test LED1 B");    //tft.drawText(10, 40, "1 B  ");
    for(int i=0; i<120; i++) { SetLED1(0, 0, i); delay(8); }
    SetLED1(0,0,0);
    Serial.println("Test LED2 R");    //tft.drawText(10, 40, "2 R  ");
    for(int i=0; i<120; i++) { SetLED2(i, 0, 0); delay(8); }
    SetLED2(0,0,0);
    Serial.println("Test LED2 G");    //tft.drawText(10, 40, "2 G  ");
    for(int i=0; i<120; i++) { SetLED2(0, i, 0); delay(8); }
    SetLED2(0,0,0);
    Serial.println("Test LED2 B");    //tft.drawText(10, 40, "2 B  ");
    for(int i=0; i<120; i++) { SetLED2(0, 0, i); delay(8); }
    SetLED2(0,0,0);
}

void pieps(void) { pieps(500, 300); }

void pieps(bool Alarm)
{       
  pieps(2000, 150);
  delay(300);
  pieps(3000, 150);
  delay(300);
  pieps(2000, 150);
  delay(300); 
}

void pieps(int wh, int warten)
{
  
  Serial.println("pieps");
  for(int Piep=0; Piep<wh; Piep++)
  {
    digitalWrite(MyBUZZ, HIGH);
    delayMicroseconds(warten);
    digitalWrite(MyBUZZ, LOW);
    delayMicroseconds(warten);
  }
}

#include <Preferences.h>
extern Preferences preferences;
extern bool TestMessungen;
extern bool CO2alarm;
bool ChkTaster(bool &NextScreen)
{       
  NextScreen=false;
  if ( digitalRead(MySW1) == 0 )
  { 
    Serial.println("SW1 - 0"); 
    pieps();delay(250); pieps();
    NextScreen=true;
    return true;
  }

  if ( digitalRead(MySW2) == 0 )
  { 
    Serial.println("SW2 - 0"); 
    if (!TestMessungen)         //TestMessungen einschalten
    {
      TestMessungen=true;
      pieps();
    }
    else                        //CO2Alarm ein/aus schalten und speichern
    {
      if (CO2alarm) {CO2alarm=false;} else {CO2alarm=true;}
      pieps(500, 200); delay(229); pieps(500, 200);
      yield();
      preferences.begin("calibration", false); preferences.putInt("CO2alarm",  CO2alarm ); preferences.end();
      yield();
    } 
  }

  return false;
}

bool ChkTaster(int &ScreenNr)
{       
 
  return false;
}

extern int myCO2;
extern int CO2mittel;
extern int CO2hoch;
void LedAnzeige(bool LEDDark)
{
  if ( LEDDark )
  {
    SetLED1(0,0,0); SetLED2(0,0,0);
  }
  else
  {
    if      (myCO2<CO2mittel)   { SetLED1(0,50,0); }
    else if (myCO2<CO2hoch)     { SetLED1(50,40,0); }
    else                        { SetLED1(50,0,0); }

    if      (myCO2<CO2mittel)   { SetLED2(0,50,0); }
    else if (myCO2<CO2hoch)     { SetLED2(50,40,0); }
    else                        { SetLED2(50,0,0); }
  }
}