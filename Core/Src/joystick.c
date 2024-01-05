#include <direction.h>
#include <stdint.h>
#include "adc.h"
#include "joystick.h"

// Margin for the joystick readings.
// Joystick goes from 0 - 4096 -> range: 800 -> 3296
#define MARGIN 800
#define maxValue 4096

uint16_t readValueX = 0;
uint16_t readValueY = 0;

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

enum Direction readJoystick(){

	// A0 BLU -> VRx
	// A1 BIANCO -> VRy

	// Enable ADC for joystick
	HAL_ADC_Start(&hadc1);
	HAL_ADC_Start(&hadc2);

	HAL_ADC_PollForConversion(&hadc1,1000);
	readValueX = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_PollForConversion(&hadc2,1000);
    readValueY = HAL_ADC_GetValue(&hadc2);

    // Check direction
    if(readValueX <= MARGIN)
    	return LEFT;
    else if(readValueX >= maxValue - MARGIN)
    	return RIGHT;
    else if(readValueY <= MARGIN)
    	return UP;
    else if(readValueY >= maxValue - MARGIN)
    	return DOWN;

    return IDLE;
}
