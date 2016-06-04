#ifndef Display_h
#define Display_h

#include "Particle.h"

class Display {
public:
    Display();
    void setLedState(int ledNum, bool state);
    void setChar(int digitNum, char c, bool decimal);
    void setChars(String chars);
    void setNumber(int number); 
    void setNumber(float number); 
    void refreshLeds();
    void refreshDigits();
private:
    int _ledState;
    int _digitCodes[4];
    void _setDigitCode(int digitNum, int digitCode);
    /* void _refreshLeds(); */
    /* void _refreshDigits(); */
    /* void _refresh(); */
    void _setErrorChars();
};

#endif
