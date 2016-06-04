#include "Button.h"
#include "Display.h"
#include "Treadmill.h"

#include "Pins.h"

#define NINE_MINUTES 9 * 60
#define DEFAULT_INTERVAL 1

#define SCAN_MODE  0
#define SPEED_MODE 1
#define TIME_MODE  2
#define DIST_MODE  3
#define CAL_MODE   4

Timer scanTimer(5000, _scan);
Timer countdownTimer(1, _countdown);

Button* _startStopButton = new Button(START_STOP_BUTTON, _checkStartStop);
Button* _incButton = new Button(INC_BUTTON, _checkInc);
Button* _decButton = new Button(DEC_BUTTON, _checkDec);
Button* _modeButton = new Button(MODE_BUTTON, _checkMode);

Display* _display = new Display();
Treadmill* _treadmill = new Treadmill();

enum state {
  unsafe,
  engaging,
  safe,
  starting,
  running,
  stopping
};
state _state = unsafe;

int _showClock = false;
int _scanMode = SPEED_MODE;
int _mode;
int _countdownCounter;


void setup() {
    Time.zone(0.5);
    Particle.syncTime();
    Serial.begin(9600);

    pinMode(BUTTONS, OUTPUT);
    pinMode(KILL_SWITCH, INPUT_PULLUP);

    scanTimer.start();
//    attachInterrupt(KILL_SWITCH, _checkKill, CHANGE);

    Particle.function("wifiCommand", wifiCommand);
}

int wifiCommand(String command) {
  if ( command == "CLOCK" ) 
    _showClock = !_showClock;

  if ( command == "START" )
    _checkStartStop();

  if ( command == "STOP" )
    _checkStartStop();

  if ( command == "INC" )
    _checkInc();

  if ( command == "DEC" )
    _checkDec();

  if ( command == "MODE" )
    _checkMode();

  return 1;
}

bool _isScanning() {
    return _mode == SCAN_MODE;
}

void _scan() {
    if ( _state == running && _isScanning() ) 
        _scanMode = (_scanMode % 4) + 1;
}

void _setSpeed() {
  _display->setChars(String::format( "%5.1f", _treadmill->getCurrentSpeed() ));
}

void _setTimeHourMinute(int time) {
    _display->setChars(
        String::format("%02d%s%02d", 
          Time.hour(time / 1000),
          time % 1000 > 500 ? "." : "",
          Time.minute(time / 1000)));
}

void _setTimeMinuteSecond(int time) {
    _display->setChars(
        String::format("%02d%s%02d", 
          Time.minute(time / 1000),
          time % 1000 > 500 ? "." : "",
          Time.second(time / 1000)));
}

// FIXME
void _setDist() {
  float x = _treadmill->getCurrentSpeed() + 2.3;
  _display->setNumber(x);
}

// FIXME
void _setCal() {
  float x = _treadmill->getCurrentSpeed() + 1.7;
  int y = (int)(x * 10);
  _display->setNumber(y);
}

int _getMode() {
    return _isScanning() ? _scanMode : _mode;
}

void _updateText() {
    if ( _showClock ) {
        _setTimeHourMinute((Time.local() - ( NINE_MINUTES ) ) * 1000 + ( millis() % 1000 ));
    } else if ( _state == running ) {
      if ( _getMode() == SPEED_MODE ) {
        _setSpeed();
      }
      if ( _getMode() == TIME_MODE ) {
        _setTimeMinuteSecond(_treadmill->getRuntimeInMillis());
      }
      if ( _getMode() == DIST_MODE ) {
        _setDist();
      }
      if ( _getMode() == CAL_MODE ) {
        _setCal();
      }
    } else if ( _state == starting ) {
        _display->setNumber(_countdownCounter);
    } else if ( _state == engaging ) {
        _display->setChars( "0.0.0.0." );
    } else if ( _state == safe ) {
        _display->setChars("-.-.-.-.");
    } else if ( _state == unsafe ) {
       _display->setChars( "SAFE" );
    }
}

void _updateMode() {
    for ( int i = 0; i < 5; i++ )
        _display->setLedState(i, false);

    if ( _state == running ) {
        _display->setLedState( SCAN_MODE, _isScanning() );
        _display->setLedState( _getMode(), true );
    } else if ( _state == engaging ) {
        for (int i = 0; i < 5; i++) {
            _display->setLedState(i, true);
        }
    }
}

void _assertStopped() {
    switch ( _state ) {
        case unsafe:
        case engaging:
        case safe:
        case starting:
          _treadmill->stop();
          break;
    }
}

void loop() {
    _updateMode();
    _updateText();
    _checkKill();

    _assertStopped();

    _display->refreshLeds();
    _display->refreshDigits();
}

void (*_countdownCallback)(void);
void (*_countdownCompleteCallback)(void);
void _countdown() {
      if ( _countdownCounter > 0 ) {
          _countdownCounter = _countdownCounter - 1;
          if (_countdownCallback) _countdownCallback();
      } else {
          countdownTimer.stop();
          if (_countdownCompleteCallback) _countdownCompleteCallback();
      }
}

void _setupCountdown(int count, int period, void (*onCompleteCallback)(void)) {
  _countdownCounter = count;
  _countdownCompleteCallback = onCompleteCallback;
  countdownTimer.changePeriod(period);
  countdownTimer.start();
}

void _start() {
  _treadmill->start();
  _state = running;
  _mode = SPEED_MODE;
}

void _stop() {
    _state = safe;
    _treadmill->stop();
}

void _checkStartStop() {
    if ( _state == safe ) {
        _state = starting;
        _setupCountdown(3, 1000, _start);
    } else if ( _state == running  || _state == starting ) {
        _stop();
    }
}

void _checkInc() {
    if ( _state == running ) {
        _mode = SPEED_MODE;
        _treadmill->incrementSpeed();
    }
}

void _checkDec() {
    if ( _state == running ) {
        _mode = SPEED_MODE;
        _treadmill->decrementSpeed();
    }
}

void _checkMode() {
    _mode = ( _mode + 1 ) % 5;
}

void _engage() {
  _state = safe;
}

void _checkKill() {
    if ( !_isSafe() && _state != unsafe ) {
        _stop();
        _state = unsafe;
    } else if ( _isSafe() && _state == unsafe ) {
        _state = engaging;
        _setupCountdown(5, 400, _engage);
    }
}

bool _isSafe() {
    return ( digitalRead( KILL_SWITCH ) == LOW );
}
