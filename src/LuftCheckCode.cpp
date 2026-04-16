#include <Arduino.h>
using namespace std;

//#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include "WIFIfunktionen.h"
//#include <AsyncElegantOTA.h>

WifiFunktionen myWifi;
AsyncWebServer server(80);

#include <SPIFFS.h>

/* Configuration of NTP by https://werner.rothschopf.net/microcontroller/202103_arduino_esp32_ntp_en.htm  */
#define MY_NTP_SERVER "at.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"
#include <time.h> // time() ctime()
tm timeinfo;         // the structure
 
#include "defs.h"
#include "values.h"
#include "HW_Funktionen.h"
#include "screen.h"
#include "wetterfunktionen.h"

#include <Preferences.h>
Preferences preferences;


#include <PubSubClient.h>
//const char* mqtt_server = mqttLink;  // Add your MQTT Broker IP address
WiFiClient mqttEspClient;
PubSubClient mqttclient(mqttEspClient);
long lastMsg = 0;
char msg[50];
int value = 0;



#ifdef  SensorBME
  #include "BlueDot_BME280.h"
  BlueDot_BME280 bme1;
#endif
#ifdef  SensorSHT
  #include "SHTSensor.h"
  SHTSensor sht;
#endif

//MH-Z19 by Jonathan Dempsey
#include "MHZ19.h" 
#define RX_PIN 25                                          // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 26                                          // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)
MHZ19 myMHZ19;                                             // Constructor for library
HardwareSerial mySerial(1);                              // ESP32 create device to MH-Z19 serial

// Variables and constants
uint16_t x, y;
bool flag = false;
char TextZahl[7];
bool AlarmAktiv=false;
String espBeschreibung;

//Funktionsprototypen
String processor(const String& var);
String outputState(int output);
String output;

// Variables and constants anderer Dateien
extern String  weatherICON1;

//= Webseiten ====================================================================
#include "Webpages.h"

// Replaces placeholder with button section in your web page
String processor(const String& var)
{
  Serial.print("page processor >>"); Serial.println(var);
  String returnTXT = "\n";
  
  if(var == "BUTTONPLACEHOLDER"){
    returnTXT += "<h4>Display</h4>      <div class='display'> <label class=\"switch\">  <input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id='check1' " ;
    if (tftScreen == Display::Wetter) { returnTXT += " checked ";  }
    returnTXT +=  "><span class=\"slider\"></span></label> </div> \n";
    returnTXT += "<h4>Testmessungen</h4>    <div class='text'>    <label class=\"switch\">  <input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id='check2' ";
    if (TestMessungen) { returnTXT += " checked ";  }
    returnTXT +=   "><span class=\"slider\"></span></label> </div> \n";
    returnTXT += "<h4>Dunkles Display</h4> <div class='text'>    <label class=\"switch\">  <input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id='check3' ";
    if (tftScreen == Display::DarkScreen) { returnTXT += " checked ";  }
    returnTXT += "><span class=\"slider\"></span></label> </div> \n";
  }

  if(var == "DIAGRAMM"){
    returnTXT += "\n";
    returnTXT += "<svg id='statSvg' xmlns='http://www.w3.org/2000/svg' width=480 height=1500>";
    const int y1=50; const int y2=35;
    for (int pos=0; (pos<MerkerAnz-1); pos++)
    {
      returnTXT += "<text id=s0 x=10 y=" + String(y1+30*pos) + ">";   
      returnTXT += String(Merkliste[pos].h) + ":";
      if (Merkliste[pos].m < 10) { returnTXT += "0"; }  //führende 0 bei Minute einfügen
      returnTXT += String(Merkliste[pos].m) + "</text> ";
      returnTXT += "<text x=60 y=" + String(y1+30*pos) + ">";   
      returnTXT += String(Merkliste[pos].co2) + " ppm</text> ";
      if      (Merkliste[pos].co2>CO2hoch) { returnTXT += "<rect id=s3 x=50 y="; }
      else if (Merkliste[pos].co2>CO2mittel)  { returnTXT += "<rect id=s2 x=50 y="; }
      else                              { returnTXT += "<rect id=s1 x=50 y="; }
      returnTXT += String(y2+30*pos) + " width=" + String(Merkliste[pos].co2/4) + " height=20 rx=3 ry=3  />" ;
      returnTXT += "\n";   
    } 
    returnTXT += "<line x1=51 y1=10 x2=51 y2=1500 stroke-width=2 stroke=#808080 /> </svg>";
    returnTXT += "\n";
  }

  if(var == "DIAGRAMMtemp"){
    returnTXT += "\n";
    returnTXT += "<svg id='statSvg' xmlns='http://www.w3.org/2000/svg' width=480 height=1500>";
    const int y1=50; const int y2=35;
    for (int pos=0; (pos<MerkerAnz-1); pos++)
    {
      returnTXT += "<text id=s0 x=10 y=" + String(y1+30*pos) + ">";   
      returnTXT += String(Merkliste[pos].h) + ":";
      if (Merkliste[pos].m < 10) { returnTXT += "0"; }  //führende 0 bei Minute einfügen
      returnTXT += String(Merkliste[pos].m) + "</text> ";
      returnTXT += "<text x=60 y=" + String(y1+30*pos) + ">";   
      returnTXT += String(Merkliste[pos].temp, 1) + " &deg;C</text> ";
      returnTXT += "<rect id=s4 x=50 y="; 
      returnTXT += String(y2+30*pos) + " width=" + String(Merkliste[pos].temp*6) + " height=20 rx=3 ry=3  />" ;
      returnTXT += "\n";   
    } 
    returnTXT += "<line x1=51 y1=10 x2=51 y2=1500 stroke-width=2 stroke=#808080 /> </svg>";
    returnTXT += "\n";
  }

  if(var == "DIAGRAMMhum"){
    returnTXT += "\n";
    returnTXT += "<svg id='statSvg' xmlns='http://www.w3.org/2000/svg' width=480 height=1500>";
    const int y1=50; const int y2=35;
    for (int pos=0; (pos<MerkerAnz-1); pos++)
    {
      returnTXT += "<text id=s0 x=10 y=" + String(y1+30*pos) + ">";   
      returnTXT += String(Merkliste[pos].h) + ":";
      if (Merkliste[pos].m < 10) { returnTXT += "0"; }  //führende 0 bei Minute einfügen
      returnTXT += String(Merkliste[pos].m) + "</text> ";
      returnTXT += "<text x=60 y=" + String(y1+30*pos) + ">";   
      returnTXT += String(Merkliste[pos].hum, 1) + " &percnt; r.F.</text> ";
      returnTXT += "<rect id=s5 x=50 y="; 
      returnTXT += String(y2+30*pos) + " width=" + String(Merkliste[pos].hum*3) + " height=20 rx=3 ry=3  />" ;
      returnTXT += "\n";   
    } 
    returnTXT += "<line x1=51 y1=10 x2=51 y2=1500 stroke-width=2 stroke=#808080 /> </svg>";
    returnTXT += "\n";

    if (!myHumMessung) { returnTXT = "\n <br> \n"; }    //Wenn keine Feuchte gemessenwird KEINE Ausgabe
  }

  if(var == "MYTIME"){
    String esptime="";
    if (!getLocalTime(&timeinfo)) 
    {
      returnTXT="Failed to obtain time";
      Serial.println("processor - Failed to obtain time");
    }
    else
    {
      esptime = String(timeinfo.tm_hour);
      esptime = esptime + ":";
      if (timeinfo.tm_min < 10) { esptime = esptime + "0";} 
      esptime = esptime  + String(timeinfo.tm_min); returnTXT= esptime;
    }
  }

  if(var == "CO2alarm"){
    if (CO2alarm)   {returnTXT= "<option>ein</option><option>aus</option>";}
    else            {returnTXT= "<option>aus</option><option>ein</option>";}
  }

  if(var == "showHUM"){
    if (myHum>0.0)   {returnTXT= "\n\n&nbsp&nbsp&nbsp <img src='/Tropfen_jpg' width=35 height=70 > &nbsp &nbsp Hum: <span id='hum'>";
                      returnTXT+= myHumTXT;
                      returnTXT+= " </span> &#37; \n \n"; }
    else           {returnTXT= " <!-- Keine Hum ausgabe! --> "; }
  }

  if(var == "COUNTER"){ returnTXT += "- "; returnTXT += Webcounter++; returnTXT += " - ";   }
  if(var == "MYCO2"){ returnTXT += myCO2; returnTXT += " ";  }
  if(var == "MYTEMP"){ returnTXT = myTempTXT; }
  if(var == "MYHUM"){ returnTXT = myHumTXT;  }
  if(var == "calCO2"){ returnTXT = String(calCO2, 3);  }
  if(var == "calTemp"){ returnTXT = String(calTemp, 3); }
  if(var == "calHum"){ returnTXT = String(calHum, 3);  }
  if(var == "CO2mittel"){ returnTXT = String(CO2mittel); }
  if(var == "CO2hoch"){ returnTXT = String(CO2hoch);  }
  if(var == "weatherlink"){ returnTXT = serverPath1 + serverPARAMETER + serverPath2 + serverAPI;  }
  if(var == "weatherkey"){ returnTXT = serverAPI;  }
  if(var == "weatherparameter"){ returnTXT = serverPARAMETER;  }
  if(var == "mqttLink"){ returnTXT = mqttLink;  }
  if(var == "mqttUser"){ returnTXT = mqttUser;  }
  if(var == "mqttKW"){ returnTXT = mqttKW;  }
   if(var == "mqttRetain"){ returnTXT = mqttRetain;  }
  if(var == "mqttTopic"){ returnTXT = mqttTopic;  }
  if(var == "mqttIntervall"){ returnTXT = String(mqttIntervall); }
  
  if(var == "espHostname"){ returnTXT = espHostname;  }
  if(var == "espBeschreibung"){ returnTXT = espBeschreibung;  }

  return returnTXT;
}



//=====================================================================
//=====================================================================
void handlenotFound(AsyncWebServerRequest *request) {
  Serial.println("##WEBPAGE handlenotFound");
  request->send(404, "text/plain", "Not found on my ESP");
}

void handleesptime(AsyncWebServerRequest *request) {
  String esptime="";
  if (!getLocalTime(&timeinfo)) 
  {
    request->send(200, "text/plain", "Failed to obtain time" );
  }
  else
  {
    esptime = String(timeinfo.tm_hour);
    esptime = esptime + ":";
    esptime = esptime  + String(timeinfo.tm_min);
    request->send(200, "text/plain", esptime );
  }
}
  
bool GetAndWriteParameter(AsyncWebServerRequest *request, String Parametername, String &myValue) 
{
    bool ParameterOK=false;
    String inputMessage;
    if (request->hasArg(Parametername.c_str()))
    {
      inputMessage = request->getParam(Parametername, true)->value();
      if ( !inputMessage.isEmpty()  )
      { 
        myValue=inputMessage; myValue.trim();
        preferences.putString(Parametername.c_str(), myValue );   
        Serial.print("preferences.putString " + Parametername + ": "); Serial.println(myValue);
        ParameterOK=true;
      }
    }
    yield();
    return ParameterOK;
}

bool GetAndWriteParameter(AsyncWebServerRequest *request, String Parametername, float &myValue) 
{
    bool ParameterOK=false;
    String inputMessage;
    if (request->hasArg(Parametername.c_str()))
    {
      inputMessage = request->getParam(Parametername, true)->value();
      if ( !inputMessage.isEmpty()  )
      { 
        myValue=atof(inputMessage.c_str());
        preferences.putFloat(Parametername.c_str(),  myValue );   
        Serial.print("preferences.putFloat " + Parametername + ": "); Serial.println(inputMessage );
        ParameterOK=true;
      }
    }
    yield();
    return ParameterOK;
}

bool GetAndWriteParameter(AsyncWebServerRequest *request, String Parametername, int &myValue) 
{
    bool ParameterOK=false;
    String inputMessage;
    if (request->hasArg(Parametername.c_str()))
    {
      inputMessage = request->getParam(Parametername, true)->value();
      if ( !inputMessage.isEmpty()  )
      { 
        myValue=atoi(inputMessage.c_str());
        preferences.putInt(Parametername.c_str(),  myValue );   
        Serial.print("preferences.putInt " + Parametername + ": "); Serial.println(inputMessage );
        ParameterOK=true;
      }
    }
    yield();
    return ParameterOK;
}

bool GetAndWriteParameter(AsyncWebServerRequest *request, String Parametername, bool &myValue) 
{
    bool ParameterOK=false;
    String inputMessage;
    if (request->hasArg(Parametername.c_str()))
    {
      inputMessage = request->getParam(Parametername, true)->value();
      if ( !inputMessage.isEmpty()  )
      { 
        if (inputMessage=="ein") {myValue=true;}
        if (inputMessage=="aus") {myValue=false;}
        
        preferences.putInt(Parametername.c_str(),  myValue );   
        Serial.print("preferences.putInt(bool) " + Parametername + ": "); Serial.println(myValue );
        ParameterOK=true;
      }
    }
    yield();
    return ParameterOK;
}

void handleSetup(AsyncWebServerRequest *request) 
  {
    Serial.println("##WEBPAGE handlesetCalValues");
    
    //List all parameters (Compatibility)
    /*
    int args = request->args();
    for(int i=0;i<args;i++){
      Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
    }
    */

    preferences.begin("calibration", false);
    GetAndWriteParameter(request, "calCO2", calCO2);
    GetAndWriteParameter(request, "calTemp", calTemp);
    GetAndWriteParameter(request, "calHum", calHum);
    GetAndWriteParameter(request, "CO2mittel", CO2mittel);
    GetAndWriteParameter(request, "CO2hoch", CO2hoch);
    GetAndWriteParameter(request, "CO2alarm", CO2alarm);
    GetAndWriteParameter(request, "espBeschreibung", espBeschreibung);
    GetAndWriteParameter(request, "weatherkey", serverAPI);
    GetAndWriteParameter(request, "weatherpara", serverPARAMETER);
    GetAndWriteParameter(request, "mqttLink", mqttLink);
    GetAndWriteParameter(request, "mqttUser", mqttUser);
    GetAndWriteParameter(request, "mqttKW", mqttKW);
    GetAndWriteParameter(request, "mqttRetain", mqttRetain);
    GetAndWriteParameter(request, "mqttTopic", mqttTopic);
    GetAndWriteParameter(request, "mqttIntervall", mqttIntervall);
    
    preferences.end(); 
    Serial.println("WERTE GESCHRIEBEN");
    yield(); 
    String  HtmlPage="<html><meta name='viewport' content='width=device-width, initial-scale=1.0, user-scalable=yes'> \n";
            HtmlPage+="<style>html {font-family: Arial; display: inline-block; text-align: center;}</style> \n";
            HtmlPage+="<body><br><h4>Einstellungen gespeichert! <br> <h3><a href='/'>Startseite</a> <br> <h3><a href='/setup'>Setup</a> </body></html>";
    request->send(200, "text/html", HtmlPage);
  }

  void handleFunktionen(AsyncWebServerRequest *request) 
  {
    String mySting="";   
    Serial.println("##WEBPAGE handleFunktionen");
    
    //List all parameters (Compatibility)
    int args = request->args();
    for(int i=0;i<args;i++)
    {
      Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());

      mySting=request->argName(i).c_str();
      if (mySting == "check1")
      {
        Serial.println("check1 gefunden: DisplayModus");
        mySting=request->arg(i).c_str();
        if (mySting=="0") { tftScreen = Display::Messung; }
        if (mySting=="1") { tftScreen = Display::Wetter; }
        forceUpdate=true;
      }
      if (mySting == "check2")
      {
        Serial.println("check2 gefunden: TestMessungen");
        mySting=request->arg(i).c_str();
        if (mySting=="0") { TestMessungen = false; }
        if (mySting=="1") { TestMessungen = true; }
      }
      if (mySting == "check3")
      {
        Serial.println("check3 gefunden: DarkScreen");
        mySting=request->arg(i).c_str();
        if (mySting=="0") { tftScreen = Display::Messung; }
        if (mySting=="1") { tftScreen = Display::DarkScreen; }
        forceUpdate=true;
      }
    }
    
    yield(); 
    request->send(200, "text/html", "OK!");
  }


void printLocalTime()
{
  if ( WiFi.status() == WL_CONNECTED )
  {
    if(!getLocalTime(&timeinfo))
    {
      Serial.println("Failed to obtain time");
    }
    else
    {
      Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S");
      if (timeinfo.tm_isdst == 1) // Daylight Saving Time flag
        Serial.print("  DST");
      else
        Serial.print("  standard");
    }
  }
  else
  {
    Serial.print("Wifi not connected - Failed to obtain time  "); Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }
}


void mqtt_callback(char* topic, byte* message, unsigned int length) {
  Serial.print("mqtt-Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT
}

bool mqtt_reconnect() {
  // Loop until we're reconnected
  int mqttConnectCnt=0;
  bool ReturnVal=false;

  while ( !mqttclient.connected() && (mqttConnectCnt<3) ) 
  {
    mqttConnectCnt++;
    Serial.print(CR "Attempting MQTT connection..<"); Serial.print(mqttConnectCnt); Serial.print("> ");
    // Attempt to connect
    if (  mqttclient.connect("ESP_Client", mqttUser.c_str(), mqttKW.c_str())  ) 
    {
      Serial.println(" mqtt connected");
      mqttConnectCnt=0;
      ReturnVal=true;
      // Subscribe
      //mqttclient.subscribe("LuftCheck/output");
    } 
    else 
    {
      Serial.print(" mqtt failed, rc=");
      Serial.print(mqttclient.state());
      Serial.println(" try again in 4 seconds");
      delay(4000);
    }
  }
  return ReturnVal; 
}

#include "esp_task_wdt.h"

void waitMillis(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    yield();  // erlaubt WiFi/RTOS-Tasks während des Wartens
  }
}


// ===============================================================================================================================
// ===============================================================================================================================
void setup(void)
{
  esp_task_wdt_init(15, true); //wdt Sekunden Timeout setzen
  
  pinMode(MyLED1R, OUTPUT);
  pinMode(MyLED1G, OUTPUT);
  pinMode(MyLED1B, OUTPUT);
  pinMode(MyLED2R, OUTPUT);
  pinMode(MyLED2G, OUTPUT);
  pinMode(MyLED2B, OUTPUT);

  // Setup PWM and attach the channel to the GPIO to be controlled
  ledcSetup(led1ChannelR, freq, resolution);
  ledcSetup(led1ChannelG, freq, resolution);
  ledcSetup(led1ChannelB, freq, resolution);
  ledcAttachPin(MyLED1R, led1ChannelR);
  ledcAttachPin(MyLED1G, led1ChannelG);
  ledcAttachPin(MyLED1B, led1ChannelB);
  ledcSetup(led2ChannelR, freq, resolution);
  ledcSetup(led2ChannelG, freq, resolution);
  ledcSetup(led2ChannelB, freq, resolution);
  ledcAttachPin(MyLED2R, led2ChannelR);
  ledcAttachPin(MyLED2G, led2ChannelG);
  ledcAttachPin(MyLED2B, led2ChannelB);
  
  pinMode(MySW1, INPUT_PULLUP);
  pinMode(MySW2, INPUT_PULLUP);
  pinMode(MyBUZZ, OUTPUT);

  Serial.begin(115200);Serial.println("\n");
  Serial.println("START LUFTCHECK!!!");

  pieps();
  StartUpTFT();
  Serial.println("START tft fertig");
  pieps();
  
  //Startmeldung
  DisplayDrawText(12, 15, SWver);
  
  //BME280 Sensor
  #ifdef  SensorBME
    delay(200);
    Wire.begin(BME280SDA, BME280SCL);  //SDA,SCL
    delay(100);
    bme1.parameter.communication = 0;                    //I2C communication for Sensor 1 (bme1)
    bme1.parameter.I2CAddress = 0x76;                    //I2C Address for Sensor 1 (bme1)
    bme1.parameter.sensorMode = 0b11;                    //Setup Sensor mode for Sensor 1
    bme1.parameter.IIRfilter = 0b100;                   //IIR Filter for Sensor 1
    bme1.parameter.humidOversampling = 0b101;            //Humidity Oversampling for Sensor 1
    bme1.parameter.tempOversampling = 0b101;              //Temperature Oversampling for Sensor 1
    bme1.parameter.pressOversampling = 0b101;             //Pressure Oversampling for Sensor 1
    bme1.parameter.pressureSeaLevel = 1013.25;            //default value of 1013.25 hPa (Sensor 1)
    bme1.parameter.tempOutsideCelsius = 15;               //default value of 15°C
    bme1.parameter.tempOutsideFahrenheit = 59;            //default value of 59°F
    delay(100);
    uint8_t chipID = bme1.init();
    if (chipID == 0x60) { Serial.println(F("BME280 Sensor detected! chipID = 0x60 "));  DisplayDrawText(10, 40, "BME280 Sensor ID=0x60 "); myHumMessung=true;}
    else
    { Serial.println(F(" ! I2C Sensor found chipID = ")); Serial.println(chipID); DisplayDrawText(10, 40, "I2C Sensor ID "+ (String)chipID ); }                      
    /* BMP280 has chip-id 0x58 */
  #endif
  #ifdef  SensorSHT
    delay(200);
    Wire.begin(BME280SDA, BME280SCL);  //SDA,SCL
    delay(100);
    if (sht.init())
    { Serial.print("SHT init(): success\n");  DisplayDrawText(10, 40, "SHT30 Sensor OK! "); myHumMessung=true; }
    else
    { Serial.print("SHT init(): failed\n");   DisplayDrawText(10, 40, "! SHT30 Sensor ERROR!" );}
    sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x
  #endif


  //MHZ19 Sensor
  mySerial.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN); // (ESP32 Example) device to MH-Z19 serial start   
  myMHZ19.begin(mySerial);                                // *Serial(Stream) refence must be passed to library begin(). 
  //myMHZ19.setRange(5000);
  myMHZ19.autoCalibration(true);                              // Turn auto calibration ON (OFF autoCalibration(false))

  if(myMHZ19.errorCode == RESULT_OK)
  { Serial.println("MHZ19  Sensor OK!"); DisplayDrawText(10, 60, "MHZ19  Sensor OK!!"); }
  else
  { Serial.println("MHZ19 Sensor ERROR!"); DisplayDrawText(10, 60, "MHZ19 Sensor ERROR!"); }
  
  
  //Dateisystem
  Serial.println();
  if(!SPIFFS.begin(true)) { Serial.println("Error mounting SPIFFS"); }  
  else                    { Serial.println("Mounting SPIFFS OK!");  }

  preferences.begin("calibration", true);
  calCO2 = preferences.getFloat("calCO2", 1.0);                       Serial.printf("calCO2: %f \n", calCO2);
  #ifdef  SensorBME
    calTemp = preferences.getFloat("calTemp", 0.82);                    Serial.printf("calTemp: %f \n", calTemp);
    calHum = preferences.getFloat("calHum", 1.08);                      Serial.printf("calHum: %f \n", calHum);
  #endif
  #ifdef  SensorSHT
    calTemp = preferences.getFloat("calTemp", 0.85);                    Serial.printf("calTemp: %f \n", calTemp);
    calHum = preferences.getFloat("calHum", 1.15);                      Serial.printf("calHum: %f \n", calHum);
  #endif
  CO2mittel = preferences.getInt("CO2mittel", 800);                   Serial.printf("CO2mittel: %i \n", CO2mittel);
  CO2hoch = preferences.getInt("CO2hoch", 1500);                      Serial.printf("CO2hoch: %i \n", CO2hoch);
  CO2alarm = preferences.getInt("CO2alarm", 1);                       Serial.printf("CO2alarm: %i \n", CO2alarm);
  espBeschreibung = preferences.getString("espBeschreibung", "");     Serial.printf("espBeschreibung: %s \n", espBeschreibung.c_str() );

  mqttLink = preferences.getString("mqttLink", "broker.hivemq.com");     Serial.printf("mqttLink: %s \n", mqttLink.c_str() );
  mqttUser = preferences.getString("mqttUser", "");            Serial.printf("mqttUser: %s \n", mqttUser.c_str() );
  mqttKW = preferences.getString("mqttKW", "");             Serial.printf("mqttKW: %s \n", mqttKW.c_str() );
  mqttTopic = preferences.getString("mqttTopic", "HTLLEONDING/LuftCheck");       Serial.printf("mqttTopic: %s \n", mqttTopic.c_str() );
  mqttRetain = preferences.getString("mqttRetain", "0");            Serial.printf("mqttRetain: %s \n", mqttRetain);
  mqttIntervall = preferences.getInt("mqttIntervall", 0);            Serial.printf("mqttIntervall: %i \n", mqttIntervall);

  serverAPI = preferences.getString("weatherkey", serverAPIdefault);                  Serial.printf("serverAPI: %s \n", serverAPI.c_str() );
  serverPARAMETER = preferences.getString("weatherpara", serverPARAMETERdefault);     Serial.printf("serverPARAMETER: %s \n", serverPARAMETER.c_str() );
  Serial.print("openweathermap: "); Serial.println(serverPath1 + serverPARAMETER + serverPath2 + serverAPI);
  preferences.end();
  Serial.println();

  //Wifi
  DisplayDrawText(10, 80, "WIFI ..");
  myWifi.WifiOFF();
  if ( myWifi.connectToWifi() == WL_CONNECTED ) { DisplayDrawText(60, 80, WiFi.SSID() ); pieps(200, 300); }
  else                                          { DisplayDrawText(60, 80, WiFi.softAPSSID()); pieps(200, 200); }
  
  //Webseiten
  server.on("/SetupWifi", HTTP_GET, handleSetupWIFI);
  server.on("/SetupWifi", HTTP_POST, handlegetSetupWIFI);
  server.on("/setupwifi", HTTP_GET, handleSetupWIFI);
  server.on("/setupwifi", HTTP_POST, handlegetSetupWIFI);
  
  server.on("/temp", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(200, "text/plain", myTempTXT );   });
  server.on("/hum", HTTP_GET, [](AsyncWebServerRequest *request)  { request->send(200, "text/plain", myHumTXT );    });
  server.on("/co2", HTTP_GET, [](AsyncWebServerRequest *request)  { request->send(200, "text/plain", myCO2TXT );    });
  server.on("/esptime", HTTP_GET, handleesptime);
  
  //Bilder für Webseiten
  server.on("/Zeiger_jpg", HTTP_GET, [](AsyncWebServerRequest *request){  request->send(SPIFFS, "/Zeigermessung.jpg", "image/png");  });
  server.on("/Zeiger1_jpg", HTTP_GET, [](AsyncWebServerRequest *request){  request->send(SPIFFS, "/Zeigermessung1.jpg", "image/png");  });
  server.on("/Zeiger2_jpg", HTTP_GET, [](AsyncWebServerRequest *request){  request->send(SPIFFS, "/Zeigermessung2.jpg", "image/png");  });
  server.on("/Zeiger3_jpg", HTTP_GET, [](AsyncWebServerRequest *request){  request->send(SPIFFS, "/Zeigermessung3.jpg", "image/png");  });
  server.on("/Thermometer_jpg", HTTP_GET, [](AsyncWebServerRequest *request){  request->send(SPIFFS, "/Thermometer.jpg", "image/png");  });
  server.on("/Tropfen_jpg", HTTP_GET, [](AsyncWebServerRequest *request){  request->send(SPIFFS, "/Tropfen.jpg", "image/png");  });

  // Route for Homepage
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)     { request->send(200, "text/html", index_html, processor); });
  server.on("/diagramm", HTTP_GET, [](AsyncWebServerRequest *request)     { request->send(200, "text/html", Diagramm_html, processor); });
  server.on("/Diagramm", HTTP_GET, [](AsyncWebServerRequest *request)     { request->send(200, "text/html", Diagramm_html, processor); });

   // Route for KALIBRATION
  server.on("/setup", HTTP_GET, [](AsyncWebServerRequest *request)     { request->send(200, "text/html", SETUP_html, processor); });
  server.on("/Setup", HTTP_GET, [](AsyncWebServerRequest *request)     { request->send(200, "text/html", SETUP_html, processor); });
  server.on("/setup", HTTP_POST, handleSetup);

  // Route für weitere Funktionen
  server.on("/handleFunktionen", HTTP_GET, handleFunktionen);
  server.on("/handleFunktionen", HTTP_POST, handleFunktionen);

  // Route onNotFound
  server.onNotFound(handlenotFound);

  yield();
  //AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  yield();
  server.begin();
  Serial.println("WEBSERVER gestartet");
  DisplayDrawText(10, 100, "WEBSERVER gestartet");

  if ( WiFi.status()  == WL_CONNECTED)
  {
    //init and get the time NACH : https://werner.rothschopf.net/microcontroller/202103_arduino_esp32_ntp_en.htm   
    configTime(0, 0, MY_NTP_SERVER); // 0, 0 because we will use TZ in the next line
    setenv("TZ", MY_TZ, 1); // Set environment variable with your time zone
    tzset();

    printLocalTime();   DisplayDrawText(10, 120, "Uhrzeit ermittelt ");
    getWeather();       DisplayDrawText(10, 140, "Wetterdaten ermittelt ");
    
    //Messdaten, erste Messzeit festlegen
    MerkerMin = timeinfo.tm_min + 3; if (MerkerMin>60) { MerkerMin=0; } 
    MerkerStd = timeinfo.tm_hour;
    Serial.printf("Erste Messzeit für Merker= %i : %i" CR, MerkerStd, MerkerMin);

    //MQTT
    Serial.print(CR "mqtt starteten   ");
    mqttclient.setServer(mqttLink.c_str(), 1883);
    //mqttclient.setCallback(mqtt_callback);  //Topics zum empfangen
    if (mqtt_reconnect() )   //Mqtt verbinden
    {
      Serial.println("... mqtt gestartet");
      DisplayDrawText(10, 160, "Mqtt:" + mqttLink);
    }
    else
    {
      Serial.println("... mqtt ERROR");
      DisplayDrawText(10, 160, "mqtt ERROR " + mqttLink);
    }
  }

  delay(3500);
  //while(1);
} //ende Setup



// ===============================================================================================================================
// ===============================================================================================================================
int ScreenVorher=0;
void loop(void)
{

  //--------------- BME Sensor Messen --------------------------
  currentMillis = millis();
  if (( (currentMillis - previousMillisMessTemp) >= (interval1) ) || (loopCnt<15)  || TestMessungen)  //seltener messen, ev danach besseres Ergebnis
  { 
    previousMillisMessTemp=currentMillis;

    #ifdef  SensorBME
      myTemp= (myTemp + bme1.readTempC()*calTemp ) / 2;     //eigene Kalibrationswerte
      myHum = (myHum + bme1.readHumidity()*calHum ) / 2;     //eigene Kalibrationswerte
      myPre = (myPre + bme1.readPressure()) / 2;
      dtostrf (myHum, 4, 1, myHumTXT);
      dtostrf (myTemp, 4, 1, myTempTXT);

      Serial.print(F("BMx 280 Temp [°C]: ")); Serial.print(myTemp);
      Serial.print(F("  Hum [%]: "));  Serial.print(myHum);
      Serial.print(F("  Press [hPa]: "));  Serial.print(myPre);
      Serial.print(F("  Alt [m]: "));  Serial.print(bme1.readAltitudeMeter());
      Serial.print(F("  ID: "));  Serial.println(bme1.checkID());    //BME ist 0x66, BMP ist dez88
    #endif

    #ifdef  SensorSHT
      if (sht.readSample())
      {
        myTemp= (myTemp + sht.getTemperature()*calTemp ) / 2;     //eigene Kalibrationswerte
        myHum = (myHum + sht.getHumidity()*calHum ) / 2;          //eigene Kalibrationswerte
        myPre = 0;                                                // ?? (myPre + sht30.vpd_kPa*1000) / 2;
        dtostrf (myHum, 4, 1, myHumTXT);
        dtostrf (myTemp, 4, 1, myTempTXT);

        Serial.print(F("SHT30 Temp [°C]: ")); Serial.print(myTemp);
        Serial.print(F("  Hum [%]: "));  Serial.print(myHum);
        //Serial.print(F("  Press [hPa]: "));  Serial.print(myPre);
      }
      else
      {
        Serial.print("Error SHT3ß in readSample()\n");
      }
    #endif
  }
      

  //--------------- CO2 Messen --------------------------
  currentMillis = millis();
  if ( ((currentMillis - previousMillisMessCO2) >= (interval2) )  || (loopCnt<30) || TestMessungen)  //seltener messen, ev danach besseres Ergebnis
  {
    previousMillisMessCO2=currentMillis;

    if(myMHZ19.errorCode != RESULT_OK)
    {     Serial.println("Error found in CO2 communication "); }
    else
    {
      myCO2 = (myCO2 + myMHZ19.getCO2()*calCO2) / 2;             // Request CO2 (as ppm) //eigene Kalibrationswerte
      Serial.print("MHZ19 CO2 (ppm): ");  Serial.print(myCO2);                                
      Serial.print("  - Temperature (C): ");  Serial.print(myMHZ19.getTemperature());
      Serial.print("  - getAccuracy: "); Serial.println(myMHZ19.getAccuracy());
      utoa (myCO2, myCO2TXT, 10);
    }
  }

  //--------------- Taster --------------------------
  bool NextScreen=false;
  ChkTaster(NextScreen);
  if (NextScreen) { ++tftScreen; forceUpdate=true; Serial.printf(" -------- SCREEN: %u \r\n" , tftScreen); }

  //--------------- gelegentlich Display komplet aktualisieren 
  currentMillis = millis();
  if ( ((currentMillis - previousMillis30Min) >= (interval30Min) )  ) 
  { 
    previousMillis30Min = currentMillis; 
    forceUpdate=true;
  }
  
  //--------------- LEDs --------------------------
  LedAnzeige( (tftScreen == Display::DarkScreen) );
  
  
  //--------------- Display --------------------------
  if ( WiFi.status() != WL_CONNECTED )
  {
    //Serial.println("!!!! UpdateDisplayNoWifi(forceUpdate);");
    UpdateDisplayNoWifi(forceUpdate);
    ScreenVorher=0;
  }
  else
  {
    if (ScreenVorher==0) {ScreenVorher=1; forceUpdate=true;}  //forceUpdate wenn Wifi zwischendurch kommt
    
    //ungültige Wetterdaten? auf nächstes Display umschalten
    if ((weatherICON1 == "99") && (tftScreen==Display::Wetter))
    { ++tftScreen; }

    switch (tftScreen) { 
      case Display::Messung :
        UpdateDisplay(forceUpdate);
        break;
      case Display::Wetter :
        UpdateDisplay_WForcast(forceUpdate);
        break;
      case Display::TestScreen :
        StartUpTFT();
        break;
      case Display::DarkScreen :
        UpdateDisplayDarkScreen(forceUpdate);
        break;
    }
  }


  static int forceUpdatecnt=0;              //Sicherstellen das ein Update durchführt wird!
  if  ((forceUpdate) && (forceUpdatecnt>0))
  { forceUpdate=false; forceUpdatecnt=0; }
  if (forceUpdate) { forceUpdatecnt++; }
  

  //--------------- CO2 Alarm
  if ( myCO2<CO2hoch ) {AlarmAktiv=true;}
  if (CO2alarm && (myCO2>CO2hoch) && AlarmAktiv)
  {
      Serial.println("!!!! CO2 ALARM !!!!!!!!!!!!!");
      //zeit für den 1. Alarm
      pieps(true);
      previousMillisCO2Alarm = currentMillis;
      AlarmAktiv=false;
  }
  //Wiederholung des CO2 Alarms
  currentMillis = millis();
  if ( ((currentMillis - previousMillisCO2Alarm) >= (interval15min) )  ) 
  { 
    previousMillisCO2Alarm = currentMillis; 
    if ((CO2alarm) && (myCO2>CO2hoch) && (!AlarmAktiv))
    {
      Serial.println("!!!! CO2 ALARM wh !!!!!!!!!!!!!");
        //zeit für den 1. Alarm
        pieps(true);
        previousMillisCO2Alarm = currentMillis;
        AlarmAktiv=false;
    }
  }

  //--------------- WIFI Verbindung prüfen, TestMessungen=false;
  currentMillis = millis();
  if ( ((currentMillis - previousMillisWifi) >= (interval3) )  || (loopCnt<1) ) 
  { 
    previousMillisWifi = currentMillis; 
    Serial.println(CR "********************");
    myWifi.connectToWifi();  

    TestMessungen=false;        //Genug Testmessungen gemacht! 
    forceUpdate=true;
  }
  
  if ( WiFi.status()  == WL_CONNECTED)
  {
          yield(); 
          //--------------- Uhrzeit prüfen
          printLocalTime();
          
          //--------------- Wetterdaten aktualisieren
          currentMillis = millis();
          if ( ((currentMillis - previousMillis2Std) >= (interval2Std) )  || (loopCnt<2) )  
          {
            previousMillis2Std=currentMillis;
            if ( WiFi.status() == WL_CONNECTED ) 
                { Serial.println("!!!  Aktualisieren der Wetterdaten !!");   getWeather(); }
            forceUpdate=true;
          }
          
          //--------------- Messdaten in Merker speichen
          if(!getLocalTime(&timeinfo))    //habe ich eine Uhrzeit?
            {
              Serial.println("Failed to obtain time, Merker alle 30 min speichern");

              currentMillis = millis();
              if ( ((currentMillis - previousMillisMerker) >= (interval1min) )  )  
              {
                previousMillisMerker=currentMillis;

                Serial.printf(CR"Messdaten in Merker speichen 30min");

                //Merker umsortieren
                for (int pos=(MerkerAnz-1); pos>0; pos--)
                {
                  Merkliste[pos] = Merkliste[pos-1];
                } 
                //Merkliste[0].h = MerkerStd;
                //Merkliste[0].m = MerkerMin;
                Merkliste[0].co2 = myCO2;
                Merkliste[0].temp = myTemp;
                Merkliste[0].hum = myHum;
                Merkliste[0].pres = myPre;
              }

            }
          else
          {  
              if ( (MerkerMin == timeinfo.tm_min) && (MerkerStd==timeinfo.tm_hour) )
              {
                Serial.printf(CR"Messdaten in Merker speichen %i : %i" CR, MerkerStd, MerkerMin);

                //Merker umsortieren
                for (int pos=(MerkerAnz-1); pos>0; pos--)
                {
                  Merkliste[pos] = Merkliste[pos-1];
                } 
                Merkliste[0].h = MerkerStd;
                Merkliste[0].m = MerkerMin;
                Merkliste[0].co2 = myCO2;
                Merkliste[0].temp = myTemp;
                Merkliste[0].hum = myHum;
                Merkliste[0].pres = myPre;

                //Nächste Zeit festlegen
                if (MerkerMin<30) { MerkerMin=30; } else {  MerkerMin=0; 
                                                            MerkerStd++; if (MerkerStd>23) { MerkerStd=0; }    }
                ////MerkerMin = timeinfo.tm_min + 1; if (MerkerMin>60) { MerkerMin=0; }    // Für minütilche Messungen
                Serial.printf("Nächste Zeit für Merker speichen %i : %i" CR, MerkerStd, MerkerMin);
                
                //ZUM Test: Merker ausgeben
                /*
                Serial.println(CR);
                for (int pos=0; (pos<MerkerAnz-1); pos++)
                {
                  Serial.printf("[%i] %i : %i   co2: %i, tem: %f, hum: %f " CR, pos, Merkliste[pos].h, Merkliste[pos].m, Merkliste[pos].co2, Merkliste[pos].temp, Merkliste[pos].hum  )  ;
                } 
                Serial.println(CR);
                */
              }
          }


          //MQTT==============================================================
          if (mqttIntervall > 0)    //mqttIntervall 0 : kein mqtt
          {
            currentMillis = millis();
            if ( (currentMillis - previousMillisPublMQTT) >= (interval1min * mqttIntervall  ) )  
            {  
              previousMillisPublMQTT=currentMillis;
              Serial.println(CR "MQTT publish start: ");
              
              if ( mqtt_reconnect() )
              {
                bool mqttRetainBool = (mqttRetain.toInt() != 0);   // "0" -> false, alles andere -> true

                mqttclient.loop();
                string myTopic; myTopic.append(mqttTopic.c_str()); myTopic.append("/CO2");
                if ( mqttclient.publish( myTopic.c_str(), myCO2TXT, mqttRetainBool) ) { Serial.println("published myCO2TXT "); }
                myTopic.clear(); myTopic.append(mqttTopic.c_str()); myTopic.append("/HUM");
                if ( mqttclient.publish(myTopic.c_str(), myHumTXT, mqttRetainBool) ) { Serial.println("published myHumTXT "); }
                myTopic.clear(); myTopic.append(mqttTopic.c_str()); myTopic.append("/TEMP");
                if ( mqttclient.publish(myTopic.c_str(), myTempTXT, mqttRetainBool) ) { Serial.println("published myTempTXT "); }

                delay(50);
                String esptime = String(timeinfo.tm_mday) + "." + String(timeinfo.tm_mon) + "." + String(timeinfo.tm_year+1900) +" ";
                esptime = esptime + String(timeinfo.tm_hour) + ":";
                if (timeinfo.tm_min < 10) { esptime = esptime + "0";} 
                esptime = esptime  + String(timeinfo.tm_min); 
                myTopic.clear();
                myTopic.append(mqttTopic.c_str()); myTopic.append("/Timestamp");
                if ( mqttclient.publish(myTopic.c_str(),  esptime.c_str(), mqttRetainBool  ) ) { Serial.println("published Timestamp "); }
                String ipStr = WiFi.localIP().toString();
                myTopic.clear();
                myTopic.append(mqttTopic.c_str()); myTopic.append("/IP");
                if ( mqttclient.publish(myTopic.c_str(),  ipStr.c_str() , mqttRetainBool ) ) { Serial.println("published IP "); }
              }
              else
              { Serial.println( "  --  MQTT publish nicht durchgeführt");}
            }
          }  
          //===========================================

  }   //Loop Funktionen die nur bei Wifi ausgeführt werden


  // Zum Testen der UART
  if (Serial.available())
  {
      char Zeichen = (char)Serial.read();
      Serial.print(">>");
      Serial.println(Zeichen);     
  }


  //Verzögerungen am Anfang verkleinern, um schneller Messwerte zu erhalten
  loopCnt++;

  delay(1000);Serial.print("_ ");
  Serial.println(" ");
  yield();
  
}  //end LOOP

