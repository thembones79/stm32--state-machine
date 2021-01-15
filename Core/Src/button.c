/*
 * button.c
 *
 *  Created on: Jan 3, 2021
 *      Author: themb
 */
#include "main.h"
#include "button.h"

TButton BlueKey;

void ButtonInitKey(TButton* Key, GPIO_TypeDef* GpioPort, uint16_t GpioPin, uint32_t TimerIdle,
					uint32_t TimerDebounce, uint32_t TimerPressed, uint32_t TimerRepeat, uint32_t TimerRelease)
{
	Key->GpioPort = GpioPort;
	Key->GpioPin = GpioPin;
	Key->State = IDLE;
	Key->Timer = 0;
	Key->TimerIdle = TimerIdle;
	Key->TimerDebounce = TimerDebounce;
	Key->TimerPressed = TimerPressed;
	Key->TimerRepeat = TimerRepeat;
	Key->TimerRelease = TimerRelease;
}


void ButtonRegisterPressCallback(TButton* Key, void* Callback)
{
	Key->ButtonPress = Callback;
}

void ButtonRegisterLongPressCallback(TButton* Key, void* Callback)
{
	Key->ButtonLongPress = Callback;
}

void ButtonRegisterRepeatPressCallback(TButton* Key, void* Callback)
{
	Key->ButtonRepeatPress = Callback;
}

void ButtonRegisterReleaseCallback(TButton* Key, void* Callback)
{
	Key->ButtonRelease = Callback;
}

void ButtonIdleRoutine(TButton* Key)
{
	if((HAL_GetTick() - Key->LastTick ) > Key->TimerIdle)
	{
		if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(Key->GpioPort, Key->GpioPin))
		{
			Key->State = DEBOUNCE;
			Key->Timer = Key->TimerDebounce;
		}
		else
		{
			Key->State = IDLE;
			Key->Timer = Key->TimerIdle;
		}

		Key->LastTick = HAL_GetTick();
	}
}

void ButtonDebounceRoutine(TButton* Key)
{
	if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(Key->GpioPort, Key->GpioPin))
	{
		if((HAL_GetTick() - Key->LastTick) > Key->Timer)
		{
			if(Key->ButtonPress != NULL)
			{
				Key->ButtonPress();
			}

			Key->State = PRESSED;
			Key->Timer = Key->TimerPressed;

			Key->LastTick = HAL_GetTick();
		}
	}
	else
	{
		Key->State = IDLE;
		Key->Timer = Key->TimerIdle;
	}
}

void ButtonPressedRoutine(TButton* Key)
{
	if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(Key->GpioPort, Key->GpioPin))
	{
		if((HAL_GetTick() - Key->LastTick) > Key->Timer)
		{
			if(Key->ButtonLongPress != NULL)
			{
				Key->ButtonLongPress();
			}

			Key->State = REPEAT;
			Key->Timer = Key->TimerRepeat;

			Key->LastTick = HAL_GetTick();
		}
	}
	else
	{
		Key->State = RELEASE;
		Key->Timer = Key->TimerRelease;
	}
}

void ButtonRepeatRoutine(TButton* Key)
{
	if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(Key->GpioPort, Key->GpioPin))
	{
		if((HAL_GetTick() - Key->LastTick) > Key->Timer)
		{
			if(Key->ButtonRepeatPress != NULL)
			{
				Key->ButtonRepeatPress();
			}

			Key->LastTick = HAL_GetTick();
		}
	}
	else
	{
		Key->State = RELEASE;
		Key->Timer = Key->TimerRelease;
	}
}

void ButtonReleaseRoutine(TButton* Key)
{
	if(GPIO_PIN_SET == HAL_GPIO_ReadPin(Key->GpioPort, Key->GpioPin))
	{
		if((HAL_GetTick() - Key->LastTick) > Key->Timer)
		{
			if(Key->ButtonRelease != NULL)
			{
				Key->ButtonRelease();
			}

			Key->State = IDLE;
			Key->Timer = Key->TimerIdle;

			Key->LastTick = HAL_GetTick();
		}
	}
	else
	{
		Key->State = IDLE;
		Key->Timer = Key->TimerIdle;
	}
}

void ButtonTask(TButton* Key)
{
	switch(Key->State)
	{
		case IDLE:
			ButtonIdleRoutine(Key);
			break;

		case DEBOUNCE:
			ButtonDebounceRoutine(Key);
			break;

		case PRESSED:
			ButtonPressedRoutine(Key);
			break;

		case REPEAT:
			ButtonRepeatRoutine(Key);
			break;

		case RELEASE:
			ButtonReleaseRoutine(Key);
			break;

		default:
			break;
	}
}
