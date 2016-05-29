#ifndef TreadmillThread_h
#define TreadmillThread_h

#include "MyThread.h"
#include "MyThreadController.h"

#define SCAN_MODE  0
#define SPEED_MODE 1
#define TIME_MODE  2
#define DIST_MODE  3
#define CAL_MODE   4

class TreadmillThread: public MyThread{
protected:
    float _currentSpeed;
    float _targetSpeed;
    int _analogSpeed();
public:
    TreadmillThread(MyThreadController* threadController);
    float getCurrentSpeed();
    float getTargetSpeed();
    float incrementSpeed();
    float decrementSpeed();
    void start();
    void stop();
    void run();
};

#endif
