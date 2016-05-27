#ifndef Button_h
#define Button_h

#include "MyThread.h"
#include "MyThreadController.h"

class Button{
protected:
    int _pin;
    boolean _lastValue;
    long _lastRead;
public:
    Button(int pin);
    boolean isPressed();
};

#endif
