#ifndef Button_h
#define Button_h

#include "Pins.h"
#include "Particle.h"

#define DEBOUNCE_TIME 200000
#define BUTTON_POLL 10

class Button {
public:
	Button(int pin, void (*callback)(void));
	void poll();
private:
	int _pin;
	void (*_callback)(void);
	long _lastRead;
	Timer _timer;
};

#endif
