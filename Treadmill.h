#ifndef Treadmill_h
#define Treadmill_h

#include "MyThreadController.h"

#define SCAN_MODE  0
#define SPEED_MODE 1
#define TIME_MODE  2
#define DIST_MODE  3
#define CAL_MODE   4

typedef struct Treadmill* Treadmill;

float incrementSpeed(Treadmill* treadmill);
float decrementSpeed(Treadmill* treadmill);

int _analogSpeed(Treadmill* treadmill);
void start(Treadmill* treadmill);
void stop(Treadmill* treadmill);

void _refresh();

#endif
