#include "MyThread.h"
#include "MyThreadController.h"
#include "ButtonThread.h"
#include "Pins.h"

#define DEBOUNCE_TIME 200000

ButtonThread::ButtonThread(int pin, void (*callback)(void), MyThreadController* threadController): MyThread() {
    _pin = pin;
    _lastRead = 0;
    _callback = callback;

    //enabled = false;
    //pinMode(pin, INPUT);
    //digitalWrite(pin, HIGH);
    threadController->add(this);
}

/**
    bool shouldRun(long time){
        if (digitalRead(pin) == LOW){
            enabled = true;
            _lastButtonPushed = (time ? time : millis());
        }

        return Thread::shouldRun(time);
    }
**/

void ButtonThread::run(){
    if ((long)(micros() - _lastRead) >= DEBOUNCE_TIME)  {
        digitalWrite(LEDS, HIGH);
        digitalWrite(BUTTONS, LOW);
        pinMode(_pin, INPUT_PULLUP);
        if ( digitalRead(_pin) == LOW ) {
            _callback();
        }
        pinMode(_pin, OUTPUT);
        digitalWrite(BUTTONS, HIGH);
        _lastRead = micros();
    }

    MyThread::run();
}

/**
ButtonThread btn1Thread(BTN1, 3000);
ButtonThread btn2Thread = ButtonThread(BTN2, 5000);
ButtonThread* btn3Thread;

void btn1Callback(){
    Serial.print("BTN 1 Thread: ");
    Serial.println(millis() - btn1Thread._lastButtonPushed);
}

void btn2Callback(){
    digitalWrite(LED, btn2Thread.enabled ? HIGH : LOW);
}

void btn3Callback(){
    Serial.println("BTN 3 Thread");
}

void setup(){
    Serial.begin(9600);
    pinMode(LED, OUTPUT);

    btn1Thread.onRun(btn1Callback);
    btn1Thread.setInterval(100); 

    btn2Thread.onRun(btn2Callback);
    btn2Thread.setInterval(200);

    btn3Thread = new ButtonThread(BTN3, 4000);
    btn3Thread->onRun(btn3Callback);
    btn3Thread->setInterval(100);

    controll.add(&btn1Thread);
    controll.add(&btn2Thread);
    controll.add(btn3Thread);
}

void loop() {
    controll.run();
}
**/
