#include <button.h>


//TODO: move to config file?
#define LONG_PRESS_TIME 4 // long press time in seconds

Button::Button()
{
}

void Button::init(uint16_t _interruptPin)
{
    interruptPin = _interruptPin;

    pinMode(interruptPin, INPUT_PULLUP);
    attachInterrupt(interruptPin, Delegate<void()>(&Button::interruptHandler, this), FALLING);
    buttonTimer.initializeMs(250, TimerDelegate(&Button::checkState, this));
}

void Button::checkState()
{
    // digitalWrite(16, !digitalRead(16));

    // Serial.println("DBG:+++");
    if (digitalRead(interruptPin)){
        buttonTimer.stop();
        Serial.println("DBG:Timer stopped");
    }
    if (++downCount >= LONG_PRESS_TIME*4){ /*timer_freq =250ms*/
        Serial.println("DBG:LongPress!");
        buttonTimer.stop();
    }
}


void Button::interruptHandler() /*Falling edge*/
{
    Serial.println("fall");
    if (!buttonTimer.isStarted()){
        downCount = 0;
        Serial.println("DBG:int:start Timer");
        buttonTimer.start();
    }
}
