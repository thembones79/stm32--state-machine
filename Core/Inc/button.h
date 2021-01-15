/*
 * button.h
 *
 *  Created on: Jan 3, 2021
 *      Author: themb
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

typedef enum
{
	IDLE = 0,
	DEBOUNCE, // 1
	PRESSED, // 2
	REPEAT, // 3
	RELEASE // 4
} BUTTON_STATE;

typedef struct button_struct
{
	BUTTON_STATE 	State;
	GPIO_TypeDef* 	GpioPort;
	uint16_t 		GpioPin;
	uint32_t		LastTick;
	uint32_t		Timer;
	uint32_t		TimerIdle;
	uint32_t		TimerDebounce;
	uint32_t		TimerPressed;
	uint32_t		TimerRepeat;
	uint32_t		TimerRelease;
	void(*ButtonPress)(void);
	void(*ButtonLongPress)(void);
	void(*ButtonRepeatPress)(void);
	void(*ButtonRelease)(void);
} TButton;

void ButtonInitKey(TButton* Key, GPIO_TypeDef* GpioPort, uint16_t GpioPin, uint32_t TimerIdle,
					uint32_t TimerDebounce, uint32_t TimerPressed, uint32_t TimerRepeat, uint32_t TimerRelease);

void ButtonRegisterPressCallback(TButton* Key, void* Callback);
void ButtonRegisterLongPressCallback(TButton* Key, void* Callback);
void ButtonRegisterRepeatPressCallback(TButton* Key, void* Callback);
void ButtonRegisterReleaseCallback(TButton* Key, void* Callback);

void ButtonTask(TButton* Key);

#endif /* INC_BUTTON_H_ */
