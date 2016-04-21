#ifndef LED_H
#define LED_H

#include <SmingCore/SmingCore.h>

class Led
{
public:
	Led();

	void init(uint8_t Pin=16); /*onboard LED for NodeMCU: GPIO2 = 16*/

	void blinkSTART(uint16_t time=1000);
	void blinkSTOP();
	void flash(uint16_t time=250);

	void off();
	void on();
	void toggle();

private:
	uint8_t ledPIN;
	Timer ledTimer;
	// Timer ledTimerOnce;
};

#endif //#ifndef LED_H
