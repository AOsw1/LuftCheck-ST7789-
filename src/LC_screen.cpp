#include <Arduino.h>

#include "defs.h"


// Display ******************************************************************************/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


// FreeFonts
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeSansBoldOblique9pt7b.h>
#include <Fonts/FreeSansBoldOblique12pt7b.h>
// ******************************************************************************/

// Color definitions
/* RGB 16-bit color table definition (RG565) */
#define COLOR_BLACK          0x0000      /*   0,   0,   0 */
#define COLOR_WHITE          0xFFFF      /* 255, 255, 255 */
#define COLOR_BLUE           0x001F      /*   0,   0, 255 */
#define COLOR_GREEN          0x07E0      /*   0, 255,   0 */
#define COLOR_RED            0xF800      /* 255,   0,   0 */
#define COLOR_NAVY           0x000F      /*   0,   0, 128 */
#define COLOR_DARKBLUE       0x0011      /*   0,   0, 139 */
#define COLOR_DARKGREEN      0x03E0      /*   0, 128,   0 */
#define COLOR_DARKCYAN       0x03EF      /*   0, 128, 128 */
#define COLOR_CYAN           0x07FF      /*   0, 255, 255 */
#define COLOR_TURQUOISE      0x471A      /*  64, 224, 208 */
#define COLOR_INDIGO         0x4810      /*  75,   0, 130 */
#define COLOR_DARKRED        0x8000      /* 128,   0,   0 */
#define COLOR_OLIVE          0x7BE0      /* 128, 128,   0 */
#define COLOR_GRAY           0x8410      /* 128, 128, 128 */
#define COLOR_GREY           0x8410      /* 128, 128, 128 */
#define COLOR_SKYBLUE        0x867D      /* 135, 206, 235 */
#define COLOR_BLUEVIOLET     0x895C      /* 138,  43, 226 */
#define COLOR_LIGHTGREEN     0x9772      /* 144, 238, 144 */
#define COLOR_DARKVIOLET     0x901A      /* 148,   0, 211 */
#define COLOR_YELLOWGREEN    0x9E66      /* 154, 205,  50 */
#define COLOR_BROWN          0xA145      /* 165,  42,  42 */
#define COLOR_DARKGRAY       0x7BEF      /* 128, 128, 128 */
#define COLOR_DARKGREY       0x7BEF      /* 128, 128, 128 */
#define COLOR_SIENNA         0xA285      /* 160,  82,  45 */
#define COLOR_LIGHTBLUE      0xAEDC      /* 172, 216, 230 */
#define COLOR_GREENYELLOW    0xAFE5      /* 173, 255,  47 */
#define COLOR_SILVER         0xC618      /* 192, 192, 192 */
#define COLOR_LIGHTGRAY      0xC618      /* 192, 192, 192 */
#define COLOR_LIGHTGREY      0xC618      /* 192, 192, 192 */
#define COLOR_LIGHTCYAN      0xE7FF      /* 224, 255, 255 */
#define COLOR_VIOLET         0xEC1D      /* 238, 130, 238 */
#define COLOR_AZUR           0xF7FF      /* 240, 255, 255 */
#define COLOR_BEIGE          0xF7BB      /* 245, 245, 220 */
#define COLOR_MAGENTA        0xF81F      /* 255,   0, 255 */
#define COLOR_TOMATO         0xFB08      /* 255,  99,  71 */
#define COLOR_GOLD           0xFEA0      /* 255, 215,   0 */
#define COLOR_ORANGE         0xFD20      /* 255, 165,   0 */
#define COLOR_SNOW           0xFFDF      /* 255, 250, 250 */
#define COLOR_YELLOW         0xFFE0      /* 255, 255,   0 */



#include "WiFi.h"

#include "screen.h"
#include "Bitmaps.h"
#include "HW_Funktionen.h"

extern char TextZahl[7];
extern int myCO2;
extern int CO2mittel;
extern int CO2hoch;
#include <time.h> // time() ctime()
extern tm timeinfo;         // the structure
extern bool CO2alarm;
extern bool TestMessungen;
extern int     myCO2;
extern float   myTemp;
extern float   myHum;
extern float   myPre;

int16_t maxX=1;
int16_t maxY=1;

void StartUpTFT(void)
{

  // SPI starten
  SPI.begin(TFT_SCK, -1, TFT_MOSI);

  // Display initialisieren (2.0" ST7789 → 240x320)
  tft.init(240, 320);

  // Ausrichtung (0–3)
  tft.setRotation(1);   // Landscape

  // Hintergrund schwarz
  tft.fillScreen(ST77XX_BLACK);


    Serial.println("\n \nTFT Display:");
    Serial.print("maxX: ");
    Serial.println(tft.width());
    Serial.print("maxY: ");
    Serial.println(tft.height());
    yield();
    
    maxX=tft.width();
    maxY=tft.height();

    tft.drawRect(0,0, (maxX ) , (maxY ) , random(0xffff));
    Serial.print("Startbildschirm.. ");
    for (int i = 0; i < (maxX / 2); i = i + 1)
    {
            tft.drawRect(i, i, random(maxX/2), random(maxY/2), random(0xffff)); 
            //delay(15);
            delay(1);
    }

    yield();
    delay(500);
    tft.fillScreen(COLOR_BLACK);
    Serial.println("Fertig! ");

    
    
       
#ifdef LogoHTL
      tft.setFont(&FreeSans12pt7b);
      DisplayDrawText(60+40,75+5,"Elektronik", COLOR_YELLOW);
      DisplayDrawText(37+35,100+5,"Medizintechnik", COLOR_YELLOW);

      tft.setFont(&FreeSans12pt7b);
      DisplayDrawText(20-10,138+40," Hardwaredesign & Software", COLOR_WHITE);
      DisplayDrawText(27+0,153+55,"  Ing. Alois Oswald, BEd", COLOR_WHITE);

      #include "Logo.h"
      tft.drawBitmap(11+45, 15, startlogo, 200, 33, COLOR_BLACK ,COLOR_WHITE);
    #endif

   #ifdef  LogoAO
     tft.setFont(&FreeSans12pt7b);
      DisplayDrawText(60+40,75+5,"Elektronik", COLOR_YELLOW);
      DisplayDrawText(37+35,100+5,"Steuertungstechnik", COLOR_YELLOW);
      DisplayDrawText(37+35,125+5,"Software", COLOR_YELLOW);

      tft.setFont(&FreeSans12pt7b);
      DisplayDrawText(20-10,138+40," Hardwaredesign & Software", COLOR_WHITE);
      DisplayDrawText(27+0,153+55,"  Ing. Alois Oswald, BEd", COLOR_WHITE);

      #include "LogoAO.h"
      tft.drawBitmap(11+45, 15, startlogo, 200, 33, COLOR_BLACK ,COLOR_WHITE);
   #endif

     yield();
     delay(800/10);
     LedTest();
     pieps(150, 300);
     delay(1000/10);

     /**   Test der Wetter Icons
     tft.fillScreen(COLOR_BLACK);
     #include "Bitmaps.h"
     tft.drawBitmap(10, 10, weather01, 30, 30, COLOR_WHITE);
     tft.drawBitmap(10, 50, weather02, 30, 30, COLOR_WHITE);
     tft.drawBitmap(10, 90, weather03, 30, 30, COLOR_WHITE);

     tft.drawBitmap(50, 10, weather04, 30, 30, COLOR_WHITE);
     tft.drawBitmap(50, 50, weather09, 30, 30, COLOR_WHITE);
     tft.drawBitmap(50, 90, weather10, 30, 30, COLOR_WHITE);

     tft.drawBitmap(90, 10, weather11, 30, 30, COLOR_WHITE);
     tft.drawBitmap(90, 50, weather13, 30, 30, COLOR_WHITE);
     tft.drawBitmap(90, 90, weather50, 30, 30, COLOR_WHITE);
     delay(10000);
     delay(10000);
     delay(10000);
     delay(10000);
     */

     // Vorbereitung für nächste Ausgabe bei Setup
     tft.fillScreen(COLOR_BLACK);
     tft.drawRect(0, 0, maxX - 1, maxY - 1, COLOR_WHITE);
     
     //tft.setFont(&FreeMono12pt7b);
     tft.setFont(&FreeMono9pt7b);
     
     yield();
}

void DisplayDrawText(uint16_t x, uint16_t y, String s)
{
  //DisplayDrawText(x, y, s);
    tft.setCursor(x, y);
    tft.println(s);
}
void DisplayDrawText(uint16_t x, uint16_t y, String s, uint16_t color)
{
 //DisplayDrawText(x, y, s, color);
    tft.setCursor(x, y);
    tft.setTextColor(color);
    tft.println(s);
}




void UpdateDisplayDarkScreen(bool doUpdate)
{
  
    int Zeile;
    String Text;
    static int OldCo2=9999;
    static int OldMin=99;
    
    //tft.setBacklight(false);
    tft.setFont(&FreeMono12pt7b);
    if (doUpdate) 
    {  
      tft.fillScreen(COLOR_BLACK);
      tft.fillRect(0, 0, maxX - 1, 29, COLOR_BLACK);
    }

    Zeile=42;
    //tft.drawRect(0, 30, maxX - 1, 75, COLOR_DARKGRAY);
    if ( (myCO2 != OldCo2)  || (doUpdate) )
    {
      OldCo2=myCO2;
      tft.fillRect(5, 25, 170, 55, COLOR_BLACK);
      
      utoa (myCO2, TextZahl, 10);
  //D    tft.setFont(u8g2_font_inb16_mn);
      if      (myCO2<CO2mittel)  { tft.drawBitmap(18, Zeile-6, Smile1, 30, 30, COLOR_DARKGRAY);   DisplayDrawText(63, Zeile+15, TextZahl, COLOR_DARKGRAY);}
      else if (myCO2<CO2hoch)    { tft.drawBitmap(18, Zeile-6, Smile2, 30, 30, COLOR_DARKGRAY);   DisplayDrawText(63, Zeile+15, TextZahl, COLOR_DARKGRAY);}
      else                       { tft.drawBitmap(18, Zeile-6, Smile3, 30, 30, COLOR_DARKGRAY);   DisplayDrawText(63, Zeile+15, TextZahl, COLOR_DARKGRAY);}
    }

 //D     tft.setFont(u8g2_font_6x13_tr);
    Zeile=40;
    DisplayDrawText(177, Zeile, "ppm ", COLOR_DARKGRAY);
    Zeile=55;
    DisplayDrawText(177, Zeile, "CO2 ", COLOR_DARKGRAY);

    //Uhrzeit

    //Zeile=127;
    Zeile=100;
    //int Zeitspalte=135;
    int Zeitspalte=80;
    if (!getLocalTime(&timeinfo)) {
  //D      tft.setFont(u8g2_font_inb16_mn);
      DisplayDrawText(Zeitspalte, Zeile, "hh");
      DisplayDrawText(Zeitspalte+27, Zeile, ":");
      DisplayDrawText(Zeitspalte+42, Zeile, "mm");
    }
    else
    {
    if ( (timeinfo.tm_min != OldMin)  || (doUpdate) )
      {
        OldMin=timeinfo.tm_min;
        DisplayDrawText(Zeitspalte, Zeile, "      ");
        tft.fillRect(Zeitspalte-10, Zeile-35, 100, 50, COLOR_BLACK);
    //D      tft.setFont(u8g2_font_inb16_mn);
        if (timeinfo.tm_hour<10) { DisplayDrawText(Zeitspalte, Zeile, " " + String(timeinfo.tm_hour), COLOR_DARKGRAY); }
        else                     { DisplayDrawText(Zeitspalte, Zeile, String(timeinfo.tm_hour), COLOR_DARKGRAY); }
        DisplayDrawText(Zeitspalte+27, Zeile, ":", COLOR_DARKGRAY);
        if (timeinfo.tm_min<10) { DisplayDrawText(Zeitspalte+42, Zeile, "0" + String(timeinfo.tm_min), COLOR_DARKGRAY); }
        else                     { DisplayDrawText(Zeitspalte+42, Zeile, String(timeinfo.tm_min), COLOR_DARKGRAY); }
      }
    }
  
} // Ende DarkScreen

//====================================================================================================
//  NO WIFI ==========================================================================================
void UpdateDisplayNoWifi(bool doUpdate)
{
    int Zeile;
    String Text;
    static int OldCo2=9999;
    
    if (doUpdate) 
    {  
      tft.fillScreen(COLOR_BLACK);
      tft.fillRect(0, 0, maxX - 1, 29, COLOR_BLACK);
    }


//D      tft.setFont(&FreeMono9pt7b   );
    DisplayDrawText(37,22, "LUFTCHECK", COLOR_GOLD);

    Zeile=42;
    tft.drawRect(0, 30, maxX - 1, 45, COLOR_WHITE);
    if ( (myCO2 != OldCo2)  || (doUpdate) )
    {
      OldCo2=myCO2;
      tft.fillRect(1, 31, 170, 40, COLOR_BLACK);
      
      utoa (myCO2, TextZahl, 10);
 //D       tft.setFont(u8g2_font_inb16_mn    ); 



      int16_t BitmapX = 18;   
      if      (myCO2<CO2mittel)   { tft.drawBitmap(BitmapX, Zeile-4, Smile1, 30, 30 ,COLOR_GREEN);   
                                       DisplayDrawText(65, Zeile+17, TextZahl, COLOR_GREEN);}
      else if (myCO2<CO2hoch)     { tft.drawBitmap(BitmapX, Zeile-4, Smile2, 30, 30 ,COLOR_YELLOW);  
                                        DisplayDrawText(65, Zeile+17, TextZahl, COLOR_YELLOW);}
      else                        { tft.drawBitmap(BitmapX, Zeile-4, Smile3, 30, 30 ,COLOR_RED);      
                                        DisplayDrawText(65, Zeile+17, TextZahl, COLOR_RED);}
    }

 //D     tft.setFont(u8g2_font_6x13_tr );
    Zeile=40;
    DisplayDrawText(177, Zeile+7, "ppm ", COLOR_WHITE);
    Zeile=55;
    DisplayDrawText(177, Zeile+7, "CO2 ", COLOR_WHITE);

    if (myHum>0)
    {  //Ausgabe Temp und HUM
      tft.drawRect(0, 75, maxX - 1, 45, COLOR_WHITE);
      tft.drawRect(0, 75, maxX /2 , 45, COLOR_WHITE);
      
      Zeile=88;
      int16_t BitmapX = 8;   
      tft.drawBitmap(BitmapX, Zeile-7, Thermometer, 18, 30 ,COLOR_LIGHTBLUE);
      dtostrf (myTemp, 4, 1, TextZahl);
      Text=TextZahl; Text=Text + "    ";
      tft.fillRect(30, Zeile-5, 50, 25, COLOR_BLACK);
 //D       tft.setFont(u8g2_font_chargen_92_tr );
      DisplayDrawText(33, Zeile+10+4, Text);
 //D       tft.setFont(u8g2_font_6x13_tr);
      DisplayDrawText(83, Zeile-4+5+4, "o");
      DisplayDrawText(90, Zeile-1+5+4, "C");

      tft.drawBitmap(120, Zeile-8, Tropfen, 14, 30 ,COLOR_LIGHTBLUE);
      dtostrf (myHum, 4, 1, TextZahl);
      Text=TextZahl; Text=Text + "    ";
      tft.fillRect(140, Zeile-5, 50, 25, COLOR_BLACK);
 //D       tft.setFont(u8g2_font_chargen_92_tr);
      DisplayDrawText(142, Zeile+10+4, Text);
 //D       tft.setFont(u8g2_font_6x13_tr);
      DisplayDrawText(199, Zeile-1+8, "RH");
      DisplayDrawText(199, Zeile+12+8, "% ");
    }
    else
    { 
      //Temp. zentriert ausgeben!
      tft.drawRect(0, 75, maxX - 1, 115, COLOR_WHITE);
          
      Zeile=88;
      int einrueck=52;
      tft.drawBitmap(8+einrueck, Zeile-7, Thermometer, 18, 30, COLOR_LIGHTBLUE);
      dtostrf (myTemp, 4, 1, TextZahl);
      Text=TextZahl; Text=Text + "    ";
  //D      tft.setFont(u8g2_font_chargen_92_tr);
      DisplayDrawText(33+einrueck+10, Zeile+15, Text);
  //D      tft.setFont(u8g2_font_6x13_tr);
      DisplayDrawText(83+einrueck+15, Zeile-4+15, "o");
      DisplayDrawText(90+einrueck+15, Zeile-1+15, "C");
      
    }

    if (myPre>0)
    {
      //Luftdruck. zentriert ausgeben!
      tft.drawRect(0, 115, maxX - 1, maxY - 23, COLOR_WHITE);
      //tft.drawRect(0, 115, maxX /2 , maxY - 23, COLOR_WHITE);

      Zeile=127;
      int einrueck=52;
      dtostrf (myPre, 4, 0, TextZahl);
      Text=TextZahl; Text=Text + "    ";
      
   //D     tft.setFont(u8g2_font_chargen_92_tr);
      DisplayDrawText(28 + einrueck-10, Zeile + 12, Text);

   //D     tft.setFont(u8g2_font_6x13_tr);
      DisplayDrawText(83 + einrueck, Zeile + 12, "hPa ");
      tft.drawBitmap(7+einrueck, Zeile-1, Luftdruck, 19, 19, COLOR_LIGHTBLUE);
    }

    Zeile=161;
    
    //Wifi angabe
    tft.fillRect(1, maxY - 22, maxX,   23, COLOR_BLACK);
    tft.drawRect(0, maxY - 23, maxX-1, 22, COLOR_WHITE);
    
    //DisplayDrawText(14, Zeile+8, "LuftCheck by HTL Leonding!");
    DisplayDrawText(13, Zeile+8, WiFi.softAPSSID() );

    if (CO2alarm)
    { tft.drawBitmap(187, Zeile-5, Alarm_ein, 17, 17, COLOR_RED); }
    else
    { tft.drawBitmap(187, Zeile-5, Alarm_aus, 17, 17, COLOR_WHITE); }

  //D    tft.setFont(u8g2_font_6x13_tr);
    if (TestMessungen)  {DisplayDrawText(207, Zeile+8, "!");}
    else                {DisplayDrawText(207, Zeile+8, " ");}

}

void UpdateDisplay(bool doUpdate)
{
    int Zeile;
    int BoxX, BoxY, BoxB, BoxH;
    String Text;
    static int OldCo2=9999;
    static int OldWifiStatus=99;
    static int OldMin=99;
    
    tft.setTextSize(1);

    if (doUpdate) 
    {  
      tft.fillScreen(COLOR_BLACK);
      tft.fillRect(0, 0, maxX - 1, 29, COLOR_BLACK);
    
      tft.setFont(&FreeSansBoldOblique12pt7b   );
      DisplayDrawText(80,23, "LUFTCHECK", COLOR_GOLD);
    }

    Zeile=42;
    BoxX=0; BoxY=34; BoxB=maxX-1; BoxH=72;
    tft.drawRect(BoxX, BoxY,BoxB, BoxH, COLOR_WHITE);
    if ( (myCO2 != OldCo2)  || (doUpdate) )
    {
      OldCo2=myCO2;
      tft.fillRect(BoxX+1, BoxY+1, 170, BoxH-2, COLOR_BLACK);
      
      utoa (myCO2, TextZahl, 10);
      tft.setFont(&FreeMonoBold18pt7b);  

      int16_t BitmapX = 18; 

      Serial.println("********** UpdateDisplay*************");
      if      (myCO2<CO2mittel)   { tft.drawBitmap(BitmapX, Zeile, Smile1, 30, 30 ,COLOR_GREEN);  //ORIGINALVERSION
                                       DisplayDrawText(75, Zeile+24, TextZahl, COLOR_GREEN);}
      else if (myCO2<CO2hoch)     { tft.drawBitmap(BitmapX, Zeile, Smile2, 30, 30 ,COLOR_YELLOW);  
                                        DisplayDrawText(75, Zeile+24, TextZahl, COLOR_YELLOW);}
      else                        { tft.drawBitmap(BitmapX, Zeile, Smile3, 30, 30 ,COLOR_RED);      
                                        DisplayDrawText(75, Zeile+24, TextZahl, COLOR_RED);}
    }

    tft.setFont(&FreeSans9pt7b );
    DisplayDrawText(190, Zeile+9, "ppm ", COLOR_WHITE);
    Zeile=Zeile+15;
    DisplayDrawText(190, Zeile+9, "CO2 ", COLOR_WHITE);

    while (1)
    {
      /* code */
    }
    


    BoxX=0; BoxY=75; BoxB=maxX-1; BoxH=60;
    tft.drawRect(BoxX, BoxY, maxX - 1, BoxH, COLOR_WHITE);
    tft.drawRect(BoxX, BoxY, maxX /2 , BoxH, COLOR_WHITE);
    Zeile=88;
    int16_t BitmapX = 8;   
    tft.drawBitmap(BitmapX, Zeile+7, Thermometer, 18, 30 ,COLOR_LIGHTBLUE);
    dtostrf (myTemp, 4, 1, TextZahl);
    Text=TextZahl; Text=Text + "    ";
    tft.fillRect(30, Zeile-5, 50, 25, COLOR_BLACK);
//D      tft.setFont(u8g2_font_chargen_92_tr );
    DisplayDrawText(33, Zeile+10+4, Text);
//D      tft.setFont(u8g2_font_6x13_tr);
    DisplayDrawText(83, Zeile-4+5+4, "o");
    DisplayDrawText(90, Zeile-1+5+4, "C");

    int Zeitspalte=135;
    if (myHum>0)
    {
      tft.drawBitmap(120, Zeile-8, Tropfen, 14, 30 ,COLOR_LIGHTBLUE);
      dtostrf (myHum, 4, 1, TextZahl);
      Text=TextZahl; Text=Text + "    ";
      tft.fillRect(140, Zeile-5, 50, 25, COLOR_BLACK);
  //D      tft.setFont(u8g2_font_chargen_92_tr);
      DisplayDrawText(142, Zeile+10+4, Text);
 //D       tft.setFont(u8g2_font_6x13_tr);
      DisplayDrawText(199, Zeile-1+8, "RH");
      DisplayDrawText(199, Zeile+12+8, "% ");
    }
    else
    { 
      //Datumausgabe
 //D       tft.setFont(u8g2_font_chargen_92_tr);
      DisplayDrawText(Zeitspalte, Zeile+10, String(timeinfo.tm_mday) + ". ");
      DisplayDrawText(Zeitspalte+42, Zeile+10, String(timeinfo.tm_mon+1)  + ". " );
    }

    tft.drawRect(0, 115, maxX - 1, 39, COLOR_WHITE);
    tft.drawRect(0, 115, maxX /2 , 39, COLOR_WHITE);
    
    Zeile=127;
    if (myPre>0)
    {
      dtostrf (myPre, 4, 0, TextZahl);
      Text=TextZahl; Text=Text + "    ";
 //D       tft.setFont(u8g2_font_chargen_92_tr);
      DisplayDrawText(28, Zeile+13, Text);
 //D       tft.setFont(u8g2_font_6x13_tr);
      DisplayDrawText(83, Zeile+13, "hPa ");
      tft.drawBitmap(7, Zeile-1, Luftdruck, 19, 19 ,COLOR_LIGHTBLUE);
    }
    else
    { 
      //Datumausgabe
 //D       tft.setFont(u8g2_font_chargen_92_tr);
      DisplayDrawText(22, Zeile+13, String(timeinfo.tm_mday) + ". ");
      DisplayDrawText(22+42, Zeile+13, String(timeinfo.tm_mon+1)  + ". " );
    }


    //Uhrzeit
    if (!getLocalTime(&timeinfo)) {
 //D       tft.setFont(u8g2_font_chargen_92_tr);
      DisplayDrawText(Zeitspalte, Zeile+13, "hh");
      DisplayDrawText(Zeitspalte+27, Zeile+13, ":");
      DisplayDrawText(Zeitspalte+42, Zeile+13, "mm");
    }
    else
    {
    if ( (timeinfo.tm_min != OldMin)  || (doUpdate) )
      {
        OldMin=timeinfo.tm_min;
        int Zeitspalte=135;
        DisplayDrawText(Zeitspalte, Zeile+10, "      ");
        tft.fillRect(Zeitspalte-10, Zeile-5, 80, 25, COLOR_BLACK);
   //D       tft.setFont(u8g2_font_chargen_92_tr);
        if (timeinfo.tm_hour<10) { DisplayDrawText(Zeitspalte, Zeile+13, " " + String(timeinfo.tm_hour)); }
        else                     { DisplayDrawText(Zeitspalte, Zeile+13, String(timeinfo.tm_hour)); }
        DisplayDrawText(Zeitspalte+27, Zeile+10, ":");
        if (timeinfo.tm_min<10) { DisplayDrawText(Zeitspalte+42, Zeile+13, "0" + String(timeinfo.tm_min)); }
        else                     { DisplayDrawText(Zeitspalte+42, Zeile+13, String(timeinfo.tm_min)); }
      }
    }
    
    Zeile=161;
    if ( ( WiFi.status() != OldWifiStatus )  || (doUpdate) )
    { 
      OldWifiStatus = WiFi.status();

      tft.fillRect(1, maxY - 22, maxX - 2, 22, COLOR_BLACK);
      tft.drawRect(0, maxY - 23, maxX - 1, 23, COLOR_WHITE);
      
//D        tft.setFont(u8g2_font_6x13_tr);
      if ( WiFi.status() == WL_CONNECTED ) 
      { 
        DisplayDrawText(45, Zeile+7, "IP ");
        DisplayDrawText(75, Zeile+7, "          ");
        DisplayDrawText(75, Zeile+7, WiFi.localIP().toString() );
        tft.drawBitmap(15, Zeile-4, WifiPIC, 20, 15, COLOR_GREEN);
      }
      else
      {
        DisplayDrawText(45, Zeile+7, "Keine Wifi-Verbindung!");
        tft.drawBitmap(18, Zeile-4, WifiPIC, 20, 15, COLOR_RED);
      }
    }

    if (CO2alarm)
    { tft.drawBitmap(180, Zeile-4, Alarm_ein, 17, 17, COLOR_RED); }
    else
    { tft.drawBitmap(180, Zeile-4, Alarm_aus, 17, 17, COLOR_WHITE); }

//D      tft.setFont(u8g2_font_6x13_tr);;
    if (TestMessungen)  {DisplayDrawText(200, Zeile+7, "!");}
    else                {DisplayDrawText(200, Zeile+7, " ");}

}



String  weatherTXT1 = "xy";
double  weatherTEMP1 = 98.0;
String  weatherICON1 = "99";
String  weatherTXT2 = "xy";
double  weatherTEMP2 = 89.0;
String  weatherICON2 = "99";


void UpdateDisplay_WForcast(bool doUpdate)
{
  
    int Zeile;
    String Text;
    static int OldCo2=9999;
    static int OldWifiStatus=99;
    static int OldMin=99;
    
    if (doUpdate) 
    {  
      tft.fillScreen(COLOR_BLACK);
      tft.fillRect(0, 0, maxX - 1, 29, COLOR_BLACK);
    }


  //D    tft.setFont(u8g2_font_crox5tb_tr   );
    DisplayDrawText(37,22, "LUFTCHECK", COLOR_GOLD);

    Zeile=42;
    tft.drawRect(0, 30, maxX - 1, 45, COLOR_WHITE);
    if ( (myCO2 != OldCo2)  || (doUpdate) )
    {
      OldCo2=myCO2;
      tft.fillRect(1, 31, 170, 40, COLOR_BLACK);
      
      utoa (myCO2, TextZahl, 10);
 //D       tft.setFont(u8g2_font_inb16_mn    ); 

      int16_t BitmapX = 18;   
      if      (myCO2<CO2mittel)   { tft.drawBitmap(BitmapX, Zeile-4, Smile1, 30, 30 ,COLOR_GREEN);   
                                       DisplayDrawText(65, Zeile+17, TextZahl, COLOR_GREEN);}
      else if (myCO2<CO2hoch)     { tft.drawBitmap(BitmapX, Zeile-4, Smile2, 30, 30 ,COLOR_YELLOW);  
                                        DisplayDrawText(65, Zeile+17, TextZahl, COLOR_YELLOW);}
      else                        { tft.drawBitmap(BitmapX, Zeile-4, Smile3, 30, 30 ,COLOR_RED);      
                                        DisplayDrawText(65, Zeile+17, TextZahl, COLOR_RED);}
    }

//D      tft.setFont(u8g2_font_6x13_tr );
    Zeile=40;
    DisplayDrawText(177, Zeile+7, "ppm ", COLOR_WHITE);
    Zeile=55;
    DisplayDrawText(177, Zeile+7, "CO2 ", COLOR_WHITE);

    tft.drawRect(0, 75, maxX - 1, 41, COLOR_WHITE);
    tft.drawRect(0, 75, maxX /2 , 41, COLOR_WHITE);

    //TEMPERATUR
    Zeile=88;
    int16_t BitmapX = 8;   
    tft.drawBitmap(BitmapX, Zeile-7, Thermometer, 18, 30 ,COLOR_LIGHTBLUE);
    dtostrf (myTemp, 4, 1, TextZahl);
    Text=TextZahl; Text=Text + "    ";
    tft.fillRect(30, Zeile-5, 50, 25, COLOR_BLACK);
 //D     tft.setFont(u8g2_font_chargen_92_tr );
    DisplayDrawText(33, Zeile+10+4, Text);
 //D     tft.setFont(u8g2_font_6x13_tr);
    DisplayDrawText(83, Zeile-4+5+4, "o");
    DisplayDrawText(90, Zeile-1+5+4, "C");
    
    //Uhrzeit
    int Zeitspalte=135;
    if (!getLocalTime(&timeinfo)) {
   //D     tft.setFont(u8g2_font_chargen_92_tr);
      DisplayDrawText(Zeitspalte, Zeile+13, "hh");
      DisplayDrawText(Zeitspalte+27, Zeile+13, ":");
      DisplayDrawText(Zeitspalte+42, Zeile+13, "mm");
    }
    else
    {
    if ( (timeinfo.tm_min != OldMin)  || (doUpdate) )
      {
        OldMin=timeinfo.tm_min;
        int Zeitspalte=135;
        DisplayDrawText(Zeitspalte, Zeile+10, "      ");
        tft.fillRect(Zeitspalte-10, Zeile-5, 80, 25, COLOR_BLACK);
  //D        tft.setFont(u8g2_font_chargen_92_tr);
        if (timeinfo.tm_hour<10) { DisplayDrawText(Zeitspalte, Zeile+13, " " + String(timeinfo.tm_hour)); }
        else                     { DisplayDrawText(Zeitspalte, Zeile+13, String(timeinfo.tm_hour)); }
        DisplayDrawText(Zeitspalte+27, Zeile+10, ":");
        if (timeinfo.tm_min<10) { DisplayDrawText(Zeitspalte+42, Zeile+13, "0" + String(timeinfo.tm_min)); }
        else                     { DisplayDrawText(Zeitspalte+42, Zeile+13, String(timeinfo.tm_min)); }
      }
    }

    //Wettervorhersage
    tft.drawRect(0, 115, maxX - 1, 39, COLOR_WHITE);
    tft.drawRect(0, 115, maxX /2 , 39, COLOR_WHITE);

    Zeile=127;
 //D     tft.setFont(u8g2_font_chargen_92_tr);
    if (weatherTEMP1<10)     { DisplayDrawText(37, Zeile+12, " " + String(weatherTEMP1, 1) + "  "); }
    else                     { DisplayDrawText(37, Zeile+12, String(weatherTEMP1, 1) + "  "); }
    int tftX=10; int tftY=Zeile-7;
    switch ( weatherICON1.toInt() ) {
        case 1:
            tft.drawBitmap(tftX, tftY, weather01, 30, 30, COLOR_YELLOW);
            break;
        case 2:
            tft.drawBitmap(tftX, tftY, weather02, 30, 30, COLOR_YELLOW);
            break;
        case 3:
            tft.drawBitmap(tftX, tftY, weather03, 30, 30, COLOR_LIGHTGREY);
            break;
        case 4:
            tft.drawBitmap(tftX, tftY, weather04, 30, 30, COLOR_LIGHTGREY);
            break;
        case 9:
            tft.drawBitmap(tftX, tftY, weather09, 30, 30, COLOR_LIGHTBLUE);
            break;
        case 10:
            tft.drawBitmap(tftX, tftY, weather10, 30, 30, COLOR_BLUE);
            break;
        case 11:
            tft.drawBitmap(tftX, tftY, weather11, 30, 30, COLOR_MAGENTA);
            break;
        case 13:
            tft.drawBitmap(tftX, tftY, weather13, 30, 30, COLOR_WHITE);
            break;                
        case 50:
            tft.drawBitmap(tftX, tftY, weather50, 30, 30, COLOR_OLIVE);
            break;
        case 99:
            tft.drawBitmap(tftX, tftY, weatherxx, 30, 30, COLOR_OLIVE);
            break;
        default:
            Serial.println("\n !!!!!! UpdateDisplay_WForcast Fehler bei Auswahl des Wetter-Icons  \n");
            break;
    }

    if (weatherTEMP2>0)     { DisplayDrawText(155, Zeile+12, " " + String(weatherTEMP2, 1) + "  "); }
    else                     { DisplayDrawText(155, Zeile+12, String(weatherTEMP2, 1) + "  "); }
    tftX=118; tftY=Zeile-7;
    switch ( weatherICON2.toInt() ) {
        case 1:
            tft.drawBitmap(tftX, tftY, weather01, 30, 30, COLOR_YELLOW);
            break;
        case 2:
            tft.drawBitmap(tftX, tftY, weather02, 30, 30, COLOR_YELLOW);
            break;
        case 3:
            tft.drawBitmap(tftX, tftY, weather03, 30, 30, COLOR_LIGHTGREY);
            break;
        case 4:
            tft.drawBitmap(tftX, tftY, weather04, 30, 30, COLOR_LIGHTGREY);
            break;
        case 9:
            tft.drawBitmap(tftX, tftY, weather09, 30, 30, COLOR_LIGHTBLUE);
            break;
        case 10:
            tft.drawBitmap(tftX, tftY, weather10, 30, 30, COLOR_BLUE);
            break;
        case 11:
            tft.drawBitmap(tftX, tftY, weather11, 30, 30, COLOR_MAGENTA);
            break;
        case 13:
            tft.drawBitmap(tftX, tftY, weather13, 30, 30, COLOR_WHITE);
            break;                
        case 50:
            tft.drawBitmap(tftX, tftY, weather50, 30, 30, COLOR_OLIVE);
            break;
        case 99:
            tft.drawBitmap(tftX, tftY, weatherxx, 30, 30, COLOR_OLIVE);
            break;
        default:
            Serial.println("\n !!!!!! Fehler bei Auswahl des Wetter-Icons \n \n");
            break;
    }
   
    
    Zeile=161;
    if ( ( WiFi.status() != OldWifiStatus )  || (doUpdate) )
    { 
      OldWifiStatus = WiFi.status();

      tft.fillRect(1, maxY - 22, maxX - 2, 22, COLOR_BLACK);
      tft.drawRect(0, maxY - 23, maxX - 1, 23, COLOR_WHITE);
      
  //D      tft.setFont(u8g2_font_6x13_tr);
      if ( WiFi.status() == WL_CONNECTED ) 
      { 
        DisplayDrawText(45, Zeile+7, "IP ");
        DisplayDrawText(75, Zeile+7, "          ");
        DisplayDrawText(75, Zeile+7, WiFi.localIP().toString() );
        tft.drawBitmap(15, Zeile-4, WifiPIC, 20, 15, COLOR_GREEN);
      }
      else
      {
        DisplayDrawText(45, Zeile+7, "Keine Wifi-Verbindung!");
        tft.drawBitmap(18, Zeile-4, WifiPIC, 20, 15, COLOR_RED);
      }
    }

    if (CO2alarm)
    { tft.drawBitmap(180, Zeile-4, Alarm_ein, 17, 17, COLOR_RED); }
    else
    { tft.drawBitmap(180, Zeile-4, Alarm_aus, 17, 17, COLOR_WHITE); }

 //D     tft.setFont(u8g2_font_6x13_tr);;
    if (TestMessungen)  {DisplayDrawText(200, Zeile+7, "!");}
    else                {DisplayDrawText(200, Zeile+7, " ");}

}
