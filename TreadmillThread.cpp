#include "TreadmillThread.h"
#include "MyThreadController.h"
#include "MyThread.h"
#include "Pins.h"

#define INCREMENT 0.1

#define START_SPEED 0.6
#define STOP_SPEED 0.0
#define MIN_SPEED 0.6
#define MAX_SPEED 6.0

TreadmillThread::TreadmillThread(MyThreadController* threadController): MyThread() {
    pinMode(TREADMILL, OUTPUT);

    _currentSpeed = STOP_SPEED;
    _targetSpeed = STOP_SPEED;

    //_thread->setInterval(100); change to this?
    threadController->add(this);
}

float TreadmillThread::getCurrentSpeed() {
    return _currentSpeed;
}

float TreadmillThread::getTargetSpeed() {
    return _targetSpeed;
}

float TreadmillThread::incrementSpeed() {
    _targetSpeed = _targetSpeed + INCREMENT;
    return _targetSpeed;
}

float TreadmillThread::decrementSpeed() {
    _targetSpeed = _targetSpeed - INCREMENT;
    return _targetSpeed;
}

void TreadmillThread::start() {
    _targetSpeed = START_SPEED;
}

// FIXME this will lead to an abrupt stop
void TreadmillThread::stop() {
    _targetSpeed = STOP_SPEED;
}

int TreadmillThread::_analogSpeed() {
    return (int)map((long)(_currentSpeed * 10), 0, 100, 255, 0);
}

void TreadmillThread::run() {
    if ( _currentSpeed < _targetSpeed ) {
        _currentSpeed = _currentSpeed + INCREMENT;
    } else {
        if ( _currentSpeed > _targetSpeed ) {
            _currentSpeed = _currentSpeed - INCREMENT;
        }
    }

    analogWrite(TREADMILL, _analogSpeed());

    MyThread::run();
}

//boolean validSpeed(int speed) {
//  return speed <= STOP_SPEED && speed >= MAX_SPEED;
//}

