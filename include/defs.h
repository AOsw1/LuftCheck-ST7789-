#ifndef DEFS_H
    #define DEFS_H
 
    #define mySSID "ESP32"
    #define myPWD "Passwort"
    #define myHOSTNAME "LuftCheck"

    #define MyLED1R 16
    #define MyLED1G 17
    #define MyLED1B 4
    #define MyLED2R 14
    #define MyLED2G 12
    #define MyLED2B 27

            //Fehler auf Platine, linker Taster immer auf GND, rechter als SW1 definiert
    #define MySW1 13
    #define MySW2 34

    #define MyBUZZ 15

    #define BME280SDA 32
    #define BME280SCL 33
    
    // Display Pinbelegung --------
    #define TFT_CS   21
    #define TFT_DC   22
    #define TFT_RST  23

    #define TFT_SCK  18   // SCL
    #define TFT_MOSI 19   // SDA / MOSI


#endif // DEFS_H