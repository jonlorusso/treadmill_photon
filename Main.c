#include "Main.h"
#include "MyThreadController.h"

threadController = new MyThreadController();
display = new_display(threadController);
treadmill = new_treadmill(threadController);

startStopButton = new_button(threadController, _checkStartStop);
incButton = new_button(threadController, _checkInc);
decButton = new_button(threadController, _checkDec);
modeButton = new_button(threadController, _checkMode);
