#ifndef Treadmill_h
#define Treadmill_h

#include "Particle.h"

class Treadmill {
public:
    Treadmill();
    float getCurrentSpeed();
    float getTargetSpeed();
    float incrementSpeed();
    float decrementSpeed();
    int getRuntimeInMillis();
    void start();
    void stop();
    void update();
private:
    int _startTime;
    int _stopTime;

    float _currentSpeed;
    float _targetSpeed;
    int _analogSpeed();
    Timer _timer;
};

#endif
