#include <button.h>


//TODO: move to config file?
#define LONG_PRESS_TIME 4 // long press time in seconds

Button::Button()
{
}

void Button::init(uint16_t _interruptPin, LongPressDelegate callback)
{
    interruptPin = _interruptPin;
    ButtonDelegate = callback;

    pinMode(interruptPin, INPUT_PULLUP);
    attachInterrupt(interruptPin, Delegate<void()>(&Button::interruptHandler, this), FALLING);
    buttonTimer.initializeMs(250, TimerDelegate(&Button::checkState, this));
}

void Button::checkState()
{
    if (digitalRead(interruptPin)){
        buttonTimer.stop();
    }
    if (++downCount >= LONG_PRESS_TIME*4){ /*timer_freq =250ms*/
        buttonTimer.stop();
        ButtonDelegate();
    }
}


void Button::interruptHandler() /*Falling edge*/
{
    if (!buttonTimer.isStarted()){
        downCount = 0;
        buttonTimer.start();
    }
}
