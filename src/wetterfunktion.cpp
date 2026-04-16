#include <Arduino.h>

#include <HTTPClient.h>
#include <Arduino_JSON.h>

#include "wetterfunktionen.h"

extern String weatherTXT1;
extern double  weatherTEMP1;
extern String weatherICON1;
extern String weatherTXT2;
extern double  weatherTEMP2;
extern String weatherICON2;
extern String serverPath1;
extern String serverPARAMETER;
extern String serverPath2;
extern String serverAPI;


//==============WETTER
String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response empfangen! ");
    //Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}


void getWeather(void)
{
  tm WetterZeittm;
  int weatherSTD=99;
  String jsonBuffer;
  
  String myserverPath = serverPath1 + serverPARAMETER + serverPath2 + serverAPI;
  Serial.print("Openweater LINK: "); Serial.println(myserverPath);

  jsonBuffer = httpGETRequest(myserverPath.c_str());
  //Serial.println(jsonBuffer);
  JSONVar myObject = JSON.parse(jsonBuffer);

  // JSON.typeof(jsonVar) can be used to get the type of the var
  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("Parsing input failed!");
    return;
  }
  
  // Serial.print("JSON object = ");
  // Serial.println(myObject);
  //https://arduinojson.org/v6/assistant/
  //https://jsonformatter.org/json-parser

  Serial.println("\n  Wetterdaten  \n");
  Serial.print("Ort: "); Serial.println(myObject["city"]["name"]);

  //Für die Ausgabe
  /*for(int i=0; i<15; i=i+1)
  {
    Serial.print(i);Serial.print(": ");
    Serial.print(myObject["list"][i]["dt"]);  
    
    time_t intWetterZeittm = myObject["list"][i]["dt"];
    localtime_r(&intWetterZeittm, &WetterZeittm);
    
    //'''DAS MUSS NOCH GECHECKT WERDEN!!!
    Serial.println("ZEIT DAS MUSS NOCH GECHECKT WERDEN!!!  ");
    
    if (WetterZeittm.tm_isdst == 1) {weatherSTD = WetterZeittm.tm_hour-2;}   //Sommerzeit DST
    else                            {weatherSTD = WetterZeittm.tm_hour-1;}    //Winterzeit

    Serial.print(" H ");
    Serial.print(weatherSTD);
    
    Serial.print(" ");
    Serial.print(myObject["list"][i]["dt_txt"]);
    Serial.print(" ");
    Serial.print(myObject["list"][i]["weather"][0]["description"]);
    Serial.print("  Temp: ");
    Serial.print(myObject["list"][i]["main"]["temp"]);
    Serial.print("  Icon: ");
    Serial.print(myObject["list"][i]["weather"][0]["icon"]);
    Serial.println("\n");
  }
  */

  //Entleeren, nur zum Test ob es Werte gibt
  weatherTXT1 = "xx";
  weatherTEMP1 = 99.0;
  weatherICON1 = "99";
  weatherTXT2 = "yy";
  weatherTEMP2 = 88.0;
  weatherICON2 = "99";

  
  //Wetterwerte für 9 und 15 auslesen
  for(int i=0; i<8; i=i+1)
  {  
    time_t intWetterZeittm = myObject["list"][i]["dt"];
    localtime_r(&intWetterZeittm, &WetterZeittm);
    
    if (WetterZeittm.tm_isdst == 1) {weatherSTD = WetterZeittm.tm_hour-2;}   //Sommerzeit DST
    else                            {weatherSTD = WetterZeittm.tm_hour-1;}    //Winterzeit

    if (weatherSTD == 9)
    {
        weatherTXT1 = myObject["list"][i]["weather"][0]["description"];
        weatherTEMP1 = myObject["list"][i]["main"]["temp"];
        weatherICON1= myObject["list"][i]["weather"][0]["icon"];
        weatherICON1 = weatherICON1.substring(0,2);

        Serial.print(" Wetter 9  ");
        Serial.print(myObject["list"][i]["dt_txt"]);
        Serial.print(" ");
        Serial.print(myObject["list"][i]["weather"][0]["description"]);
        Serial.print("  Temp: ");
        Serial.print(myObject["list"][i]["main"]["temp"]);
        Serial.print("  Temp: ");
        Serial.print(weatherTEMP1);
        Serial.print("  Icon: ");
        Serial.print(weatherICON1);
        Serial.println("");
    }
    
    if (weatherSTD == 15)
    {
        weatherTXT2 = myObject["list"][i]["weather"][0]["description"];
        weatherTEMP2 = myObject["list"][i]["main"]["temp"];
        weatherICON2= myObject["list"][i]["weather"][0]["icon"];
        weatherICON2 = weatherICON2.substring(0,2);

        Serial.print(" Wetter 15  ");
        Serial.print(myObject["list"][i]["dt_txt"]);
        Serial.print(" ");
        Serial.print(myObject["list"][i]["weather"][0]["description"]);
        Serial.print("  Temp: ");
        Serial.print(myObject["list"][i]["main"]["temp"]);
        Serial.print("  Temp: ");
        Serial.print(weatherTEMP2);
        Serial.print("  Icon: ");
        Serial.print(weatherICON2);
        Serial.println("");
    }
  }

  Serial.println("\n ");

}
