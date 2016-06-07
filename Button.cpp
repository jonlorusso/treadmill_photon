#include "Button.h"
#include "Pins.h"
#include "Particle.h"

Button::Button(int pin, buttonType type) {
    _pin = pin;
    _lastRead = 0;
    _lastValue = false;
    type = type;
}

bool Button::poll(){
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
