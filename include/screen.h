#ifndef SCREEN_H
#define SCREEN_H
 
    void StartUpTFT(void);
    void UpdateDisplayDarkScreen(bool doUpdate);
    void UpdateDisplayNoWifi(bool doUpdate);
    void UpdateDisplay(bool doUpdate);
    void UpdateDisplay_WForcast(bool doUpdate);
    void DisplayDrawText(uint16_t x, uint16_t y, String s);
    void DisplayDrawText(uint16_t x, uint16_t y, String s, uint16_t color);
    //uint16_t  drawText(uint16_t x, uint16_t y, STRING s, uint16_t color = COLOR_WHITE);

#endif // SCREEN_H