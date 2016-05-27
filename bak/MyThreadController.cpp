#include "MyThread.h"
#include "MyThreadController.h"

MyThreadController::MyThreadController(unsigned long _interval): MyThread(){
	cached_size = 0;

	clear();
	setInterval(_interval);

	#ifdef USE_THREAD_NAMES
		// Overrides name
		MyThreadName = "MyThreadController ";
		MyThreadName = MyThreadName + MyThreadID;
	#endif
}

/*
	MyThreadController run() (cool stuf)
*/
void MyThreadController::run(){
	// Run this thread before
	if(_onRun != NULL)
		_onRun();

	unsigned long time = millis();
	int checks = 0;
	for(int i = 0; i < MAX_THREADS && checks <= cached_size; i++){
		// Object exists? Is enabled? Timeout exceeded?
		if(thread[i]){
			checks++;
			if(thread[i]->shouldRun(time)){
				thread[i]->run();
			}
		}
	}

	// MyThreadController extends MyThread, so we should flag as runned thread
	runned();
}


/*
	List controller (boring part)
*/
bool MyThreadController::add(MyThread* _thread){
	// Check if the MyThread already exists on the array
	for(int i = 0; i < MAX_THREADS; i++){
		if(thread[i] != NULL && thread[i]->MyThreadID == _thread->MyThreadID)
			return true;
	}

	// Find an empty slot
	for(int i = 0; i < MAX_THREADS; i++){
		if(!thread[i]){
			// Found a empty slot, now add MyThread
			thread[i] = _thread;
			cached_size++;
			return true;
		}
	}

	// Array is full
	return false;
}

void MyThreadController::remove(int id){
	// Find MyThreads with the id, and removes
	bool found = false;
	for(int i = 0; i < MAX_THREADS; i++){
		if(thread[i]->MyThreadID == id){
			thread[i] = NULL;
			cached_size--;
			return;
		}
	}
}

void MyThreadController::remove(MyThread* _thread){
	remove(_thread->MyThreadID);
}

void MyThreadController::clear(){
	for(int i = 0; i < MAX_THREADS; i++){
		thread[i] = NULL;
	}
	cached_size = 0;
}

int MyThreadController::size(bool cached){
	if(cached)
		return cached_size;

	int size = 0;
	for(int i = 0; i < MAX_THREADS; i++){
		if(thread[i])
			size++;
	}
	cached_size = size;

	return cached_size;
}

MyThread* MyThreadController::get(int index){
	int pos = -1;
	for(int i = 0; i < MAX_THREADS; i++){
		if(thread[i] != NULL){
			pos++;

			if(pos == index)
				return thread[i];
		}
	}

	return NULL;
}
