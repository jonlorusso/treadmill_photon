#ifndef DisplayThread_h
#define DisplayThread_h

#include "MyThreadController.h"
#include "MyThread.h"

class DisplayThread: public MyThread{
protected:
    byte _ledState;
    byte _digitCodes[4];
    void _setDigitCode(int digitNum, byte digitCode);
    void _refreshLeds();
    void _refreshDigits();
public:
    DisplayThread(MyThreadController* threadController);
    void setLedState(int ledNum, boolean state);
    void setChar(int digitNum, char c, boolean decimal);
    void setChars(String chars, boolean decimals[4]);
    void run();
};

#endif
