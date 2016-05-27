/*
 	MyThreadController.h - Controlls a list of MyThreads with different timings

	Basicaly, what it does is to keep track of current MyThreads and run when
	necessary.

	MyThreadController is an extended class of MyThread, because of that,
	it allows you to add a MyThreadController inside another MyThreadController...

	For instructions, go to https://github.com/ivanseidel/ArduinoMyThread

	Created by Ivan Seidel Gomes, March, 2013.
	Released into the public domain.
*/

#ifndef MyThreadController_h
#define MyThreadController_h

#include "MyThread.h"
#include "inttypes.h"

#define MAX_THREADS		15

class MyThreadController: public MyThread{
protected:
	MyThread* thread[MAX_THREADS];
	int cached_size;
public:
	MyThreadController(unsigned long _interval = 0);

	// run() Method is overrided
	void run();

	// Adds a thread in the first available slot (remove first)
	// Returns if the MyThread could be added or not
	bool add(MyThread* _thread);

	// remove the thread (given the MyThread* or MyThreadID)
	void remove(int _id);
	void remove(MyThread* _thread);

	// Removes all threads
	void clear();

	// Return the quantity of MyThreads
	int size(bool cached = true);

	// Return the I MyThread on the array
	// Returns NULL if none found
	MyThread* get(int index);
};

#endif
