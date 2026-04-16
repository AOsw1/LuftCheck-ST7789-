/** 
 *  Definitionen
 */
#define CR "\r\n"
#define SWver "LuftCheck v1.2 202601"

// setting PWM properties
const int freq = 5000;
const int resolution = 8;
int led1ChannelR = 0;
int led1ChannelG = 1;
int led1ChannelB = 2;
int led2ChannelR = 3;
int led2ChannelG = 4;
int led2ChannelB = 5;

// Global Variables
unsigned long currentMillis = 0;
unsigned long previousMillisWifi = 0;
unsigned long previousMillisCO2Alarm = 0;
unsigned long previousMillisMessTemp = 0;
unsigned long previousMillisMessCO2 = 0;
unsigned long previousMillisPublMQTT = 0;
unsigned long previousMillis1min = 0;
unsigned long previousMillis30Min = 0;
unsigned long previousMillis2Std = 0;
unsigned long previousMillis4Std = 0;
unsigned long previousMillisMerker = 0;
unsigned long interval1 = 60000*5;          //5 min
unsigned long interval2 = 60000*3;         //3 min
unsigned long interval3 = 60000*20;         //20 min
unsigned long interval1min = 60000*1;       //1 min
unsigned long interval10min = 60000*10;     //10 min
unsigned long interval15min = 60000*15;     //15 min
unsigned long interval20Min = 60000*20;     //20 min
unsigned long interval30Min = 60000*30;     //30 min
unsigned long interval2Std = 60000*60*2;    //2Std
unsigned long interval4Std = 60000*60*4;    //4Std

int     myCO2;
float   myTemp;
float   myHum;
float   myPre;
bool    myHumMessung=false;
char myTempTXT[7];
char myHumTXT[7];
char myCO2TXT[7];
int Webcounter=1;
unsigned int loopCnt=0;
bool TestMessungen=false;
bool Weckfunktion=false;

//Kalibirierung der Messwerte
float calCO2; 
float calTemp; 
float calHum;

//CO2 Alarmwerte  (siehe https://www.cik-solutions.com/anwendungen/co2-im-innenraum/)
int CO2mittel;
int CO2hoch;
bool CO2alarm;

//MQTT Werte
String mqttLink;
String mqttUser;
String mqttKW;
String mqttTopic;
String mqttRetain;
int  mqttIntervall;

//Merker für Statistik
struct Merker {
  int h=0;
  int m=0;
  int co2=0;
  float temp=0.0;
  float hum=0.0;
  float pres=0.0;
};
 
const int MerkerAnz=48;
Merker Merkliste[MerkerAnz];
int MerkerStd=0;
int MerkerMin=0;


bool forceUpdate=true;
enum Display {Messung=1, Wetter=2, DarkScreen=3, TestScreen=4};
Display& operator++(Display& d)
{
  return d = (d == Display::TestScreen) ? Display::Messung : static_cast<Display>(static_cast<int>(d)+1);
}
Display tftScreen=Display::Messung;

  String serverPath1 = "http://api.openweathermap.org/data/2.5/forecast?q=";      //"http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
  String serverPath2 = "&lang=de&units=metric&appid=";
  const String serverAPIdefault = "51bf8620cec5a06121764d5196b1a93f";
  String serverAPI = "";
  const String serverPARAMETERdefault = "Leonding,AT";
  String serverPARAMETER = "";