#ifndef ButtonThread_h
#define ButtonThread_h

#include "MyThread.h"
#include "MyThreadController.h"

class ButtonThread: public MyThread{
protected:
    int _pin;
    void (*_callback)(void);
    long _lastRead;
public:
    ButtonThread(int pin, void (*callback)(void), MyThreadController* threadController);
    void run();
};

#endif
