#include "Button.h"
#include "Pins.h"
#include "MyThreadController.h"

struct Button {
    int _pin;
    bool _lastValue;
    long _lastRead;
    MyThread* _thread;
    void (*callback)(void);
};

Button* new_button(int pin, MyThreadController* threadController,void (*callback)(void)) { 
    Button* button = malloc(sizeof(Button));
    button->_pin = pin;
    button->_lastValue = false;
    button->_lastRead = 0;
    button->_callback = callback;
}

boolean is_pressed(Button* button);
    if ((long)(micros() - button->_lastRead) >= DEBOUNCE_TIME)  {
        digitalWrite(LEDS, HIGH);
        digitalWrite(BUTTONS, LOW);
        pinMode(button->_pin, INPUT_PULLUP);
	button->_lastValue = ( digitalRead(button->_pin) == LOW );
        pinMode(button->_pin, OUTPUT);
        digitalWrite(BUTTONS, HIGH);
        button->_lastRead = micros();
    }
    return button->_lastValue;
}
