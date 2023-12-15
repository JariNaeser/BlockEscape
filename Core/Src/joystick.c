#include <stdint.h>
#include "adc.h"
#include "joystick.h"

uint16_t readValueX = 0;
uint16_t readValueY = 0;

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

void readJoystick(){

	 // Enable ADC for joystick
	HAL_ADC_Start(&hadc1);
	HAL_ADC_Start(&hadc2);

	HAL_ADC_PollForConversion(&hadc1,1000);
	readValueX = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_PollForConversion(&hadc2,1000);
    readValueY = HAL_ADC_GetValue(&hadc2);
}
