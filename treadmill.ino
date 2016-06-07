#include "Button.h"
#include "Display.h"
#include "Treadmill.h"

#include "Pins.h"

#define NINE_MINUTES 9 * 60

// FIXME change this to an enum
#define SCAN_MODE  0
#define SPEED_MODE 1
#define TIME_MODE  2
#define DIST_MODE  3
#define CAL_MODE   4

Timer recordButtonTimer(5, _recordButtonPress);
Timer scanTimer(5000, _scan);
Timer countdownTimer(1, _countdown);

Button _buttons[] = {
  Button(START_STOP_BUTTON, bSTART_STOP),
  Button(INC_BUTTON, bINC),
  Button(DEC_BUTTON, bDEC),
  Button(MODE_BUTTON, bMODE),
};

Display* _display = new Display();
Treadmill* _treadmill = new Treadmill();

enum state { UNSAFE, ENGAGING, SAFE, STARTING, RUNNING, STOPPING };
state _state = UNSAFE;

enum clockMode { OFF, TIME, DATE, WEATHER };
clockMode _clockMode = OFF;

int _buttonPress = bNONE;
int _scanMode = SPEED_MODE;
int _mode;
int _countdownCounter;

String temp = " n/a";

void (*_countdownCallback)(void);
void (*_countdownCompleteCallback)(void);

void setup() {
    Time.zone(0.5);
    Particle.syncTime();
    Serial.begin(9600);

    Particle.subscribe("weather", _storeWeather);

    pinMode(BUTTONS, OUTPUT);
    pinMode(KILL_SWITCH, INPUT_PULLUP);

    recordButtonTimer.start();
    scanTimer.start();
//    attachInterrupt(KILL_SWITCH, _checkKill, CHANGE);

    Particle.function("wifiCommand", wifiCommand);
}

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

void _storeWeather(const char *event, const char *data) {
  Serial.println("_storeWeather called with..");
  Serial.println(data);
  String weatherData = String(data);
  int start = weatherData.indexOf("degrees_f");

  if ( start > -1 ) {
    int end = weatherData.indexOf(",", start);
    temp = weatherData.substring(start + 12, end - 5);
    Serial.println(temp);
  }
}

void _setWeather() {
  _display->setChars(temp);
}

int wifiCommand(String command) {
  if ( command == "WEATHER" )
    _clockMode = ( _clockMode == WEATHER ) ? OFF : WEATHER;

  if ( command == "TIME" ) 
    _clockMode = ( _clockMode == TIME ) ? OFF : TIME;

  if ( command == "DATE" ) 
    _clockMode = ( _clockMode == DATE ) ? OFF : DATE;

  if ( command == "START" ) {
    _state = STARTING;
    _setupCountdown(3, 1000, _start);
  }

  if ( command == "STOP" )
    _stop();

  if ( _state == RUNNING ) {
    if ( command == "INC" ) {
      _mode = SPEED_MODE;
      _treadmill->incrementSpeed();
    } else if ( command == "DEC" ) {
      _mode = SPEED_MODE;
      _treadmill->incrementSpeed();
    } else if ( command == "MODE" ) {
      _mode = ( _mode + 1 ) % 5;
    }
  }

  return 1;
}

bool _isScanning() {
    return _mode == SCAN_MODE;
}

void _scan() {
    if ( _state == RUNNING && _isScanning() ) 
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

void _setDate(int time) {
  _display->setChars( String::format("%02d.%02d", Time.month(time), Time.day(time)));
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
    if ( _clockMode == TIME ) {
      _setTimeHourMinute((Time.local() - ( NINE_MINUTES ) ) * 1000 + ( millis() % 1000 ));
    } else if ( _clockMode == DATE ) {
      _setDate(Time.local());
    } else if ( _clockMode == WEATHER ) {
      _setWeather();
    } else if ( _state == RUNNING ) {
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
    } else if ( _state == STARTING ) {
        _display->setNumber(_countdownCounter);
    } else if ( _state == ENGAGING ) {
        _display->setChars( "0.0.0.0." );
    } else if ( _state == SAFE ) {
        _display->setChars("-.-.-.-.");
    } else if ( _state == UNSAFE ) {
       _display->setChars( "SAFE" );
    }
}

void _updateMode() {
    for ( int i = 0; i < 5; i++ )
        _display->setLedState(i, false);

    if ( _state == RUNNING ) {
        _display->setLedState( SCAN_MODE, _isScanning() );
        _display->setLedState( _getMode(), true );
    } else if ( _state == ENGAGING ) {
        for (int i = 0; i < 5; i++) {
            _display->setLedState(i, true);
        }
    }
}

void _assertStopped() {
    switch ( _state ) {
        case UNSAFE:
        case ENGAGING:
        case SAFE:
        case STARTING:
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

void _start() {
  _treadmill->start();
  _state = RUNNING;
  _mode = SPEED_MODE;
}

void _stop() {
    _state = SAFE;
    _treadmill->stop();
}

void _recordButtonPress() {
  _buttonPress = bNONE;
  for ( unsigned int i = 0; i < sizeof _buttons; i++ ) {
    if ( _buttons[i].poll() ) {
      _buttonPress = _buttons[i].type;
      break;
    }
  }
}

void _buttonDispatch() {
  if ( _buttonPress == bSTART_STOP ) {
        if ( _state == SAFE ) {
            _state = STARTING;
            _setupCountdown(3, 1000, _start);
        } else if ( _state == RUNNING  || _state == STARTING ) {
            _stop();
        }
  } else if ( _buttonPress == bINC ) {
      if ( _state == RUNNING ) {
          _mode = SPEED_MODE;
          _treadmill->incrementSpeed();
      }
  } else if ( _buttonPress == bDEC ) {
      if ( _state == RUNNING ) {
          _mode = SPEED_MODE;
          _treadmill->decrementSpeed();
      }
  } else if ( _buttonPress == bMODE ) {
      _mode = ( _mode + 1 ) % 5;
  }
  _buttonPress = NULL;
}

void _engage() {
  _state = SAFE;
}

void _checkKill() {
    if ( !_isSafe() && _state != UNSAFE ) {
        _stop();
        _state = UNSAFE;
    } else if ( _isSafe() && _state == UNSAFE ) {
        _state = ENGAGING;
        _setupCountdown(5, 400, _engage);
    }
}

bool _isSafe() {
    return ( digitalRead( KILL_SWITCH ) == LOW );
}
