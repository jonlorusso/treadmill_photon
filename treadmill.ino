#include "Button.h"
#include "Display.h"
#include "Treadmill.h"
#include "MyThreadController.h"
#include "MyThread.h"

#define SCAN_MODE  0
#define SPEED_MODE 1
#define TIME_MODE  2
#define DIST_MODE  3
#define CAL_MODE   4

MyThreadController* _threadController;
Display* _display;
Treadmill* _treadmill;

MyThread* _countdownThread;
MyThread* _startStopButtonThread;
MyThread* _incButtonThread;
MyThread* _decButtonThread;
MyThread* _modeButtonThread;

Button* _startStopButton;
Button* _incButton;
Button* _decButton;
Button* _modeButton;

int _mode;
boolean _safe;
boolean _running;
int _countdownCounter;

void setup() {
    _threadController = new MyThreadController();
    _display = new Display(myThreadController);
    _treadmill = new Treadmill(myThreadController);

    _countdownThread = new MyThread();
    _countdownThread->enabled = false;
    _threadController->add(_countdownThread);

    _startStopButton = Button(START_STOP_BUTTON);
    _initializeThread(_startStopButtonThread, _checkStartStop, 1);

    _incButton = Button(INC_BUTTON);
    _initializeThread(_incButtonThread, _checkInc, 1);

    _decButton = Button(DEC_BUTTON);
    _initializeThread(_decButtonThread, _checkDec, 1);

    _modeButton = Button(MODE_BUTTON);
    _initializeThread(_modeButtonThread, _checkMode, 1);

    //_killSwitch = Button(KILL_SWITCH, myThreadController, _checkKill);

    _safe = false;
    _running = false;

    pinMode(KILL_SWITCH, INPUT_PULLUP);
}

void loop() {
    _threadController->run();
}


void _setSafeDigits() {
    _display->setChar(0, '-', true);
    _display->setChar(1, '-', true);
    _display->setChar(2, '-', true);
    _display->setChar(3, '-', true);
}

void _startTreadmillCountdown() {
    if ( _countdownCounter > 0 ) {
        _countdownCounter = _countdownCounter - 1;
    } else {
        _treadmill->start();
    }
}

void _start() {
    if (!_running) {
        if (digitalRead( KILL_SWITCH ) == LOW) {
	    _countdownThread->onRun((void (*)())_startTreadmillCountdown);
            _countdownThread->setInterval(1000);
            _countdownCounter = 3;
            _countdownThread->enabled = true;
        }
    }
}

void _stop() {
    if (_safe) {
        _setSafeDigits();
    } else {
        _treadmill->stop();
    }
}

void _checkStartStop() {
    if (_safe) {
        if ( _startStopButton->isPressed() ) {
            _running ? _stop() : _start();
        }
    }
}

void _checkInc() {
    if ( _safe && _running ) {
        if ( _incButton->isPressed() ) {
             _mode = SPEED_MODE;
            _display->setMode(SPEED_MODE);
            _treadmill->incrementSpeed();
         }
    }
}

void _checkDec() {
    if ( _safe && _running ) {
        if ( _decButton->isPressed() ) {
            _mode = SPEED_MODE;
            _display->setMode(SPEED_MODE);
            _treadmill->decrementSpeed();
        }
    }
}

void _checkMode() {
    if (_safe && _running) {
        if ( _modeButton->isPressed() ) {
            _mode = ( _mode + 1 ) % 5;
            _display->setMode(SPEED_MODE);
            _treadmill->decrementSpeed();
        }
    }
}

void _checkKill() {
//    _safe = _killSwitch.isPressed();
//
//    if ( ! _safe ) {
//        _
//    } else {
//
//    }
}

void _initializeThread(MyThread* thread, void (*callback)(void), unsigned long _interval) {
  thread->onRun(callback);
  thread->setInterval(_interval);
  _threadController.add(thread);
}

