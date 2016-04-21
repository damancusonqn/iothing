#include <led.h>

// #define LED_PIN 16 // GPIO2

Led::Led()
{
}

void Led::toggle()
{
	digitalWrite(ledPIN, !digitalRead(ledPIN));
}
void Led::off()
{
	digitalWrite(ledPIN, 1);
}

void Led::on()
{
	digitalWrite(ledPIN, 0);
}

void Led::flash(uint16_t time)
{
	on();
	ledTimer.stop();
	ledTimer.initializeMs(time, TimerDelegate(&Led::off, this)).startOnce();
}

void Led::blinkSTART(uint16_t time)
{
	ledTimer.stop();
	ledTimer.initializeMs(time, TimerDelegate(&Led::toggle, this)).start();
}

void Led::blinkSTOP()
{
	ledTimer.stop();
	off();
}

void Led::init(uint8_t pin){
	ledPIN = pin;
	pinMode(ledPIN, OUTPUT);
	off();
}
