#ifndef Treadmill_h
#define Treadmill_h

#include "MyThreadController.h"

#define SCAN_MODE  0
#define SPEED_MODE 1
#define TIME_MODE  2
#define DIST_MODE  3
#define CAL_MODE   4

class Treadmill{
	public:
	    Treadmill(MyThreadController* threadController);
            float getCurrentSpeed();
            float getTargetSpeed();
            float incrementSpeed();
            float decrementSpeed();
};

void initialize();
void _refresh();
int _analogSpeed();
void start();
void stop();

#endif
