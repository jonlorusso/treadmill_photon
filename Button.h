#ifndef Button_h
#define Button_h

#include "MyThreadController.h"

#define DEBOUNCE_TIME 200

typedef struct Button* Button;

int _pin;
bool _lastValue;
long _lastRead;

Button new_button(int pin, MyThreadController* threadController, void (*callback)(void));
boolean is_pressed(Button* button);

#endif
