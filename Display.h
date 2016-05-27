#ifndef Display_h
#define Display_h

#include "MyThreadController.h"
#include "MyThread.h"

#include "Pins.h"

#define SCAN_MODE   0
#define SPEED_MODE  1
#define TIME_MODE   2
#define DIST_MODE   3
#define CAL_MODE    4

class Display{
    public:
        Display(MyThreadController* threadController);

};

void _initialize();

void setMode(int mode);
void setChar(int digitNum, char c, boolean decimal);
void setChars(String chars, boolean decimals[4]);

void _setDigitCode(int digitNum, byte digitCode);
void _setLedState(int ledNum, boolean state);

void _refreshLeds();
void _refreshDigits();
void _refresh();

#endif
