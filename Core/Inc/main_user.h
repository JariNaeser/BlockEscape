/*
 * main_user.h
 *
 *  Created on: Dec 15, 2023
 *      Author: jarinaser
 */

#ifndef INC_MAIN_USER_H_
#define INC_MAIN_USER_H_

#include <stdint.h>

extern int startGame;

void draw_filled_square(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
void task_draw_fct( void *pvParameters );
void task_readJoystick_fct( void *pvParameters );
void displayWelcomeMessage();

#endif /* INC_MAIN_USER_H_ */
