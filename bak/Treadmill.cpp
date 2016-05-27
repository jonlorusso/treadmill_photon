#include "Treadmill.h"
#include "MyThreadController.h"
#include "MyThread.h"
#include "Pins.h"

#define INCREMENT 0.1

#define START_SPEED 0.6

#define MIN_SPEED 0.6
#define MAX_SPEED 6.0

#define STOP 0.0

static MyThreadController* _threadController;
static MyThread* _thread;

static float _currentSpeed;
static float _targetSpeed;

void initialize() {
    pinMode(TREADMILL, OUTPUT);

    _currentSpeed = STOP;
    _targetSpeed = STOP;

    _thread = new MyThread();
    _thread->onRun(_refresh);
    _thread->setInterval(100);
    _threadController->add(_thread);
}

Treadmill::Treadmill(MyThreadController* threadController) {
    _threadController = threadController;
    initialize();
}

float Treadmill::getCurrentSpeed() {
    return _currentSpeed;
}

float Treadmill::getTargetSpeed() {
    return _targetSpeed;
}

float Treadmill::incrementSpeed() {
    _targetSpeed = _targetSpeed + INCREMENT;
    return _targetSpeed;
}

float Treadmill::decrementSpeed() {
    _targetSpeed = _targetSpeed - INCREMENT;
    return _targetSpeed;
}

void start() {
    _targetSpeed = START_SPEED;
}

// FIXME this will lead to an abrupt stop
void stop() {
    _targetSpeed = STOP;
}

int _analogSpeed() {
    return (int)map((long)(_currentSpeed * 10), 1, 100, 255, 0);
}

void _refresh() {
    if ( _currentSpeed < _targetSpeed ) {
        _currentSpeed = _currentSpeed + INCREMENT;
    } else {
        if ( _currentSpeed > _targetSpeed ) {
            _currentSpeed = _currentSpeed - INCREMENT;
        }
    }

    analogWrite(TREADMILL, _analogSpeed());
}

//boolean validSpeed(int speed) {
//  return speed <= STOP && speed >= MAX_SPEED;
//}

