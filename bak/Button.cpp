#include "Button.h"
#include "Pins.h"

#define DEBOUNCE_TIME 200

Button::Button(int pin) {
    _pin = pin;
    _lastValue = false;
    _lastRead = 0;
}

boolean Button::isPressed() {
    if ((long)(micros() - _lastRead) >= DEBOUNCE_TIME)  {
        digitalWrite(LEDS, HIGH);
        digitalWrite(BUTTONS, LOW);
        pinMode(_pin, INPUT_PULLUP);
	_lastValue = ( digitalRead(_pin) == LOW );
        pinMode(_pin, OUTPUT);
        digitalWrite(BUTTONS, HIGH);
        _lastRead = micros();
    }
    return _lastValue;
}
