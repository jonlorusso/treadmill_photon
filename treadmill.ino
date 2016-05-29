#include "ButtonThread.h"
#include "DisplayThread.h"
#include "TreadmillThread.h"

#include "MyThreadController.h"
#include "MyThread.h"

#include "Pins.h"

#define DEFAULT_INTERVAL 1

#define SCAN_MODE  0
#define SPEED_MODE 1
#define TIME_MODE  2
#define DIST_MODE  3
#define CAL_MODE   4

MyThreadController* _threadController;

MyThread* _countdownThread;
MyThread* _updateTextThread;
MyThread* _updateModeThread;
MyThread* _scanThread;

DisplayThread* _display;
TreadmillThread* _treadmill;
ButtonThread* _startStopButton;
ButtonThread* _incButton;
ButtonThread* _decButton;
ButtonThread* _modeButton;

int _mode, _scanMode;
boolean _starting, _running;
int _countdownCounter;

bool _isSafe() {
    return ( digitalRead( KILL_SWITCH ) == LOW );
}

void _initializeThread(MyThread* thread, void (*callback)(void), unsigned long _interval) {
    thread = new MyThread();
    thread->onRun(callback);
    thread->setInterval(_interval);
    _threadController->add(thread);
}

void setup() {
    _running = false;
    _starting = false;

    pinMode(LEDS, OUTPUT);
    pinMode(BUTTONS, OUTPUT);

    _threadController = new MyThreadController();

    _display = new DisplayThread(_threadController);
    _treadmill = new TreadmillThread(_threadController);

//    _startStopButton = new ButtonThread(START_STOP_BUTTON, _checkStartStop, _threadController);
//    _incButton = new ButtonThread(INC_BUTTON, _checkInc, _threadController);
//    _decButton = new ButtonThread(DEC_BUTTON, _checkDec, _threadController);
//    _modeButton = new ButtonThread(MODE_BUTTON, _checkMode, _threadController);

//    _updateTextThread = new MyThread();
    _initializeThread(_updateTextThread, _updateText, DEFAULT_INTERVAL);

    //_updateModeThread = new MyThread();
    _initializeThread(_updateModeThread, _updateMode, DEFAULT_INTERVAL);

    //_scanThread = new MyThread();
    _initializeThread(_scanThread, _scan, 5000); // flip mode every 2 seconds while scanning

    _countdownThread = new MyThread();
    _countdownThread->enabled = false;
    _threadController->add(_countdownThread);

//    pinMode(KILL_SWITCH, INPUT_PULLUP);
    //_killSwitch = Button(KILL_SWITCH, _threadController, _checkKill);
    pinMode(KILL_SWITCH, INPUT_PULLUP);
    attachInterrupt(KILL_SWITCH, _checkKill, CHANGE);

    pinMode(START_STOP_BUTTON, INPUT_PULLUP);
    attachInterrupt(START_STOP_BUTTON, _checkStartStop, CHANGE);
    // need interrupt

    _running = false;
}

bool _isScanning() {
    return _mode == SCAN_MODE;
}

void _scan() {
    if ( _isSafe() && _running && _isScanning() ) 
        _scanMode = (_scanMode % 4) + 1;
}

void _setSpeed() {
    float speed = _treadmill->getCurrentSpeed();

    char integer[2], decimal[2];
  
    sprintf( integer, "%d", (int)speed);
    sprintf( decimal, "%d", ((int)(speed * 10)) % 10);

    _display->setChar(0, ' ', false);
    _display->setChar(1, ' ', false);
    _display->setChar(2, integer[0], true);
    _display->setChar(3, decimal[0], false);
}

// FIXME
void _setTime() {
    int time = millis() / 1000;
    char c[2];
  
    int m = 1000;
    for ( int i = 0; i < 4; i++ ) {
        sprintf( c, "%d", time / m); 
        time = time - ( ( time / m ) * m );
        m = m / 10; 
        _display->setChar(0, c[0], false);
    } 
}

// FIXME
void _setDist() {
    _setSpeed();
}

// FIXME
void _setCal() {
    _setSpeed();
}

int _getMode() {
    return _isScanning() ? _scanMode : _mode;
}

void _updateText() {
    bool decimals[4] = { false, false, false, false };
    if ( _isSafe() && _running ) {
        if ( _getMode() == SPEED_MODE ) {
            _display->setChars("SPED", decimals);
      //      _setSpeed();
        }
        if ( _getMode() == TIME_MODE ) {
            _display->setChars("TIME", decimals);
      //      _setTime();
        }
        if ( _getMode() == DIST_MODE ) {
            _display->setChars("dISt", decimals);
      //      _setDist();
        }
        if ( _getMode() == CAL_MODE ) {
            _display->setChars("cALS", decimals);
       //     _setCal();
        }
    } else {
        if ( _isSafe() ) {
            if ( _starting ) {
                char countdown[4];
                sprintf( countdown, "%*d", 4, _countdownCounter );
                _display->setChars( countdown, decimals);
            } else {
                //_setSafeDigits();
                _display->setChars( "poop", decimals );
//                _display->setChar(0, '-', true);
//                _display->setChar(1, '-', true);
//                _display->setChar(2, '-', true);
//                _display->setChar(3, '-', true);
            }
        } else {
            bool decimals[4] = { false, false, false, false };
            _display->setChars( "SAFE", decimals );
        }
    }
}

void _updateMode() {
    for ( int i = 0; i < 5; i++ )
        _display->setLedState(i, false);

    if ( _isSafe() && _running ) {
        _display->setLedState( SCAN_MODE, _isScanning() );
        _display->setLedState( _getMode(), true );
    }
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
    if ( _countdownCounter > 1 ) {
        _countdownCounter = _countdownCounter - 1;
    } else {
        _countdownThread->enabled = false;
        _running = true;
        _starting = false;
        _mode = SPEED_MODE;
        _treadmill->start();
    }
}

void _start() {
    if ( _isSafe() && !_running && !_starting) {
        _starting = true;
        _countdownCounter = 4;
        _countdownThread->onRun(_startTreadmillCountdown);
        _countdownThread->setInterval(1000);
        _countdownThread->enabled = true;
    }
}

void _stop() {
    _starting = false;
    _countdownThread->enabled = false;
    _running = false;
    _treadmill->stop();
}

void _checkStartStop() {
    digitalWrite(LEDS, HIGH);
    if ( _isSafe() ) {
        if ( !_running && !_starting ) {
            _start();
        } else {
            _stop();
        }
    }
}

void _checkInc() {
    if ( _isSafe() && _running ) {
        _mode = SPEED_MODE;
        _treadmill->incrementSpeed();
    }
}

void _checkDec() {
    if ( _isSafe() && _running ) {
        _mode = SPEED_MODE;
        _treadmill->decrementSpeed();
    }
}

void _checkMode() {
    if ( _isSafe() && _running ) {
        _mode = ( _mode + 1 ) % 5;
    }
}

void _checkKill() {
    if ( !_isSafe() ) {
        _stop();
    }
}

