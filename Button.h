#ifndef Button_h
#define Button_h

#include "Pins.h"

#define DEBOUNCE_TIME 200000

enum buttonType { bNONE, bSTART_STOP, bINC, bDEC, bMODE };

class Button {
public:
	Button(int pin, buttonType type);
	bool poll();
        buttonType type;
private:
	int _pin;
	long _lastRead;
	bool _lastValue;
};

#endif
