#ifndef HW_FUNKTIONEN_H
#define HW_IFUNKTIONEN_H
 
    void SetLED1(int myR, int myG, int myB);
    void SetLED2(int myR, int myG, int myB);
    void LedTest(void);
    void pieps(void);
    void pieps(bool Alarm);
    void pieps(int wh, int warten);
    bool ChkTaster(bool &NextScreen);
    bool ChkTaster(int &ScreenNr);
    void LedAnzeige(bool LEDDark);


#endif // HW_FUNKTIONEN_H