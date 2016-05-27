#ifndef Display_h
#define Display_h

#include "MyThreadController.h"
#include "MyThread.h"

#include "Pins.h"

#define SCAN_MODE   0
#define SPEED_MODE  1
#define TIME_MODE   2
#define DIST_MODE   3
#define CAL_MODE    4

typedef struct Display* Display;

void setMode(Display* display, int mode);
void setChar(Display* display, int digitNum, char c, boolean decimal);
void setChars(Display* display, String chars, boolean decimals[4]);

void _setDigitCode(Display* display, int digitNum, byte digitCode);
void _setLedState(Display* display, int ledNum, boolean state);

void _refreshLeds(Display* display);
void _refreshDigits(Display* display);
void _refresh();

#endif
