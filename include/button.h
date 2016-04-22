#ifndef BUTTON_H
#define BUTTON_H

#include <SmingCore/SmingCore.h>

class Button
{
public:
	Button();

	void init(uint16_t);
	void checkState();
private:

	void interruptHandler();

	uint16_t interruptPin;
	uint8_t downCount;

	Timer buttonTimer;
};

#endif //#ifndef
