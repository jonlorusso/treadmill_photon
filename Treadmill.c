#include "Treadmill.h"
#include "MyThreadController.h"
#include "MyThread.h"
#include "Pins.h"
//#include "Main.h"

#define INCREMENT 0.1

#define START_SPEED 0.6

#define MIN_SPEED 0.6
#define MAX_SPEED 6.0

#define STOP 0.0

struct Treadmill {
    float _currentSpeed;
    float _targetSpeed;
    MyThread* _thread;
};

Treadmill* new_treadmill(MyThreadController* threadController,void (*callback)(void)) { 
    pinMode(TREADMILL, OUTPUT);

    Treadmill* treadmill = malloc(sizeof(Treadmill));
    treadmill->_currentSpeed = STOP;
    treadmill->_targetSpeed = STOP;

    treadmill->_thread = new Thread();
    treadmill->_thread->onRun(_refresh);
    treadmill->_thread->setInterval(1);
    threadController->add(treadmill->_thread);
}

float incrementSpeed(Treadmill* treadmill) {
    treadmill->_targetSpeed = treadmill->_targetSpeed + INCREMENT;
    return treadmill->_targetSpeed;
}

float decrementSpeed(Treadmill* treadmill) {
    treadmill->_targetSpeed = treadmill->_targetSpeed - INCREMENT;
    return treadmill->_targetSpeed;
}

void start(Treadmill* treadmill) {
    treadmill->_targetSpeed = START_SPEED;
}

// FIXME this will lead to an abrupt stop
void stop(Treadmill* treadmill) {
    treadmill->_targetSpeed = STOP;
}

int _analogSpeed(Treadmill* treadmill) {
    return (int)map((long)(treadmill->_currentSpeed * 10), 1, 100, 255, 0);
}

void _refresh() {
    if ( treadmill->_currentSpeed < treadmill->_targetSpeed ) {
        treadmill->_currentSpeed = treadmill->_currentSpeed + INCREMENT;
    } else {
        if ( treadmill->_currentSpeed > treadmill->_targetSpeed ) {
            treadmill->_currentSpeed = treadmill->_currentSpeed - INCREMENT;
        }
    }

    analogWrite(TREADMILL, _analogSpeed(treadmill));
}

//boolean validSpeed(int speed) {
//  return speed <= STOP && speed >= MAX_SPEED;
//}

