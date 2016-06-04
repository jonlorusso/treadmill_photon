#include "Button.h"
#include "Pins.h"
#include "Particle.h"

Button::Button(int pin, void (*callback)(void)) : _timer(BUTTON_POLL, &Button::poll, *this) {
	_pin = pin;
	_lastRead = 0;
	_callback = callback;
	_timer.start();
}

void Button::poll(){
    if ((long)(micros() - _lastRead) >= DEBOUNCE_TIME)  {
        digitalWrite(LEDS, HIGH);
        digitalWrite(BUTTONS, LOW);
        pinMode(_pin, INPUT_PULLUP);
        if ( digitalRead(_pin) == LOW ) {
            _callback();
        }
        pinMode(_pin, OUTPUT);
        digitalWrite(BUTTONS, HIGH);
        _lastRead = micros();
    }
}
