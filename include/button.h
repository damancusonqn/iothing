#ifndef BUTTON_H
#define BUTTON_H

#include <SmingCore/SmingCore.h>

typedef Delegate<void()> LongPressDelegate; ///< Handler function for the long press signal

class Button
{
public:
	Button();

	void init(uint16_t, LongPressDelegate);
	void checkState();
private:

	void interruptHandler();

	uint16_t interruptPin;
	uint8_t downCount;

	LongPressDelegate ButtonDelegate;
	Timer buttonTimer;
};

#endif //#ifndef
