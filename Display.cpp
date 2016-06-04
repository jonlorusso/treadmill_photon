#include "Display.h"

#include "Pins.h"
#include "math.h"
#include "Particle.h"

#define DISPLAY_REFRESH_FREQUENCY 1
#define LED_DELAY 3000
#define DIGIT_DELAY 2000

//static const int ledMasks[] = { 0b00010000, 0b00001000, 0b00000100, 0b00000010, 0b00000001 };
static const int ledMasks[] = { 16, 8, 4, 2, 1 };

static const int ledPins[] = { SCAN_LED, SPEED_LED, TIME_LED, DIST_LED, CAL_LED };
static const int digitPins[] = { DIGIT1, DIGIT2, DIGIT3, DIGIT4 };
static const int segmentPins[] = { TOP, TOPRIGHT, BOTTOMRIGHT, BOTTOM, BOTTOMLEFT, TOPLEFT, MIDDLE, DECIMAL };

static const int digitCodeMap[] = {
    0b00000000, 0b10000110, 0b00100010, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
    0b00000010, 0b00011101, 0b00001111, 0b01000000, 0b01000000, 0b10000000, 0b01000000, 
    0b10000000, 0b01010010, 0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 
    0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111, 0b00000000, 0b00000000, 
    0b00011000, 0b01001000, 0b00001100, 0b10100111, 0b00000000, 0b01110111, 0b01111111, 
    0b00111001, 0b00111111, 0b01111001, 0b01110001, 0b01111101, 0b01110110, 0b00110000, 
    0b00011110, 0b01110110, 0b00111000, 0b01001111, 0b00110111, 0b00111111, 0b01110011, 
    0b10111111, 0b01010000, 0b01101101, 0b00000111, 0b00111110, 0b01111110, 0b01001111, 
    0b01110110, 0b01100110, 0b01011011, 0b00111001, 0b01100100, 0b00001111, 0b00100011, 
    0b00001000, 0b00100000, 0b01110111, 0b01111100, 0b01011000, 0b01011110, 
    0b01111001, 0b01110001, 0b01101111, 0b01110100, 0b00010000, 0b00011110, 0b01110110, 
    0b00110000, 0b01001111, 0b01010100, 0b01011100, 0b01110011, 0b01100111, 0b01010000, 
    0b01101101, 0b01000100, 0b00011100, 0b00011100, 0b01001111, 0b01110110, 0b01101110, 
    0b01011011, 0b00011101, 0b00110000, 0b00001111, 0b01000000 
};

//Display::Display() : _timer(DISPLAY_REFRESH_FREQUENCY, &Display::refresh, *this) {
Display::Display() {
    _ledState = 0;
    pinMode(LEDS, OUTPUT);
    for ( int i = 0; i < 4; i++ ) {
        _digitCodes[i] = 0;
        pinMode(digitPins[i], OUTPUT);
    }
    for ( int i = 0; i < 8; i++ )
        pinMode(segmentPins[i], OUTPUT);

    /* _timer.start(); */
}

void Display::setLedState(int ledNum, bool state) {
    _ledState = state ? ( _ledState | ledMasks[ledNum] ) : ( _ledState & ~ledMasks[ledNum] ) ;
}

void Display::setChar(int digitNum, char c, bool decimal) {
    int digitCode = digitCodeMap[ c - 32 ];
//    digitCode = decimal ? digitCode | 0b10000000 : digitCode;
    digitCode = decimal ? digitCode | 128 : digitCode;
    _setDigitCode(digitNum, digitCode);
}

void Display::setChars(String chars) {
    unsigned int i = 0;
    int digit = 0;
    while ( i < chars.length() ) {
        char c = chars.charAt(i);
        i = i + 1;

        if ( c == '.' ) {
            setChar(digit, ' ', true);
        } else if ( (i + 1) <= chars.length() ) {
            char d = chars.charAt(i);
            if ( d == '.' ) {
                setChar(digit, c, true);
                i = i + 1;
            } else {
                setChar(digit, c, false);
            }
        } else {
            setChar(digit, c, false);
        }

        digit++;
    }
}

/**
void Display::setChars(String chars, bool decimals[4]) {
    for (int i = 0; i < 4; i++) {
         setChar(i, chars.charAt(i), decimals[i]);
    }
}
**/

void Display::_setErrorChars() {
    setChars(" Err");
}

void Display::setNumber(int number) {
    if ( number > 9999 ) {
        _setErrorChars();
        return;
    }

    setChars(String::format("%4d", number));
}

void Display::setNumber(float number) {
    if ( number > 9999 ) {
        _setErrorChars();
        return;
    }

    String integerPart = String((int)number);
    float decimal = number - (int)number;

    int decimalDigits = (int)(pow(10, (3 - integerPart.length())) * decimal);
    String decimalPart = String(decimalDigits);

    Serial.println(decimalPart);
    integerPart.concat(".");
    integerPart.concat(decimalPart);

    setChars(integerPart);
}

void Display::_setDigitCode(int digitNum, int digitCode) {
    _digitCodes[digitNum] = digitCode;
}

void Display::refreshLeds() {
    digitalWrite(LEDS, LOW);
    for (int i = 0; i < 5; i++) {
        if ( _ledState & ledMasks[i] ) {
	  digitalWrite(ledPins[i], LOW);
	  delayMicroseconds(LED_DELAY);
	  digitalWrite(ledPins[i], HIGH);
        }
    }
}

void Display::refreshDigits() {
    digitalWrite(LEDS, HIGH);
    for (int i = 0; i < 4; i++) {
        int digitPin = digitPins[i];
        digitalWrite(digitPin, LOW);
        for (int segmentNum=0 ; segmentNum < 8 ; segmentNum++) {
            if (_digitCodes[i] & (1 << segmentNum)) {
                digitalWrite(segmentPins[segmentNum], LOW);
            }
        }
        delayMicroseconds(DIGIT_DELAY);
        for (int segmentNum=0 ; segmentNum < 8 ; segmentNum++) {
            digitalWrite(segmentPins[segmentNum], HIGH);
        }
       digitalWrite(digitPin, HIGH);
    }
}
