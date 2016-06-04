#include "Treadmill.h"
#include "Pins.h"
#include "Particle.h"

#define SPEED_UPDATE_FREQUENCY 100

#define INCREMENT 0.1

#define START_SPEED 0.6
#define STOP_SPEED 0.0
#define MIN_SPEED 0.6
#define MAX_SPEED 6.0

Treadmill::Treadmill() : _timer(SPEED_UPDATE_FREQUENCY,  &Treadmill::update, *this) {
    pinMode(TREADMILL, OUTPUT);
    _currentSpeed = STOP_SPEED;
    _targetSpeed = STOP_SPEED;
    _timer.start();
}

int Treadmill::getRuntimeInMillis() {
    //return (_stopTime || Time.now()) - _startTime;
    return millis() - _startTime;
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

void Treadmill::start() {
    _startTime = millis();
    // TODO record start time and add method to get time running
    // TODO record distance
    // TODO record calories 
    _targetSpeed = START_SPEED;
}

// FIXME this will lead to an abrupt stop
void Treadmill::stop() {
    _stopTime = Time.now();
    _targetSpeed = STOP_SPEED;
}

int Treadmill::_analogSpeed() {
    return (int)map((long)(_currentSpeed * 10), 0, 100, 255, 0);
}

void Treadmill::update() {
    if ( _currentSpeed < _targetSpeed ) {
        _currentSpeed = _currentSpeed + INCREMENT;
    } else {
        if ( _currentSpeed > _targetSpeed ) {
            _currentSpeed = _currentSpeed - INCREMENT;
        }
    }

    _currentSpeed = constrain(_currentSpeed, STOP_SPEED, MAX_SPEED);

//    analogWrite(TREADMILL, _analogSpeed());
}

//boolean validSpeed(int speed) {
//  return speed <= STOP_SPEED && speed >= MAX_SPEED;
//}

