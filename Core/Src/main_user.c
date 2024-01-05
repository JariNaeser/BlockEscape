#include "main_user.h"
#include <stdio.h>
#include <stdbool.h>
#include "BSP/stm32f769i_discovery_lcd.h"	//Include file to use the LDC screen
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "main_user.h"
#include "joystick.h"
#include "direction.h"
#include "difficultyLevel.h"
#include "node.h"
#include <stdlib.h>

/* Private define ------------------------------------------------------------*/
#define COLOR_W 	LCD_COLOR_WHITE
#define COLOR_G 	LCD_COLOR_GRAY
#define CUBE_SIDE_LEN	32
#define DISP_X_SIZE	800
#define DISP_Y_SIZE	480
#define X_AXIS_ELEMENTS DISP_X_SIZE / CUBE_SIDE_LEN
#define Y_AXIS_ELEMENTS DISP_Y_SIZE / CUBE_SIDE_LEN
#define MOVE_INTERVAL 500
#define SPAWN_PERCENTAGE 4
#define X_AXIS_CENTER X_AXIS_ELEMENTS / 2
#define Y_AXIS_CENTER Y_AXIS_ELEMENTS / 2

/* Private data types definition ---------------------------------------------*/


/* Public variables ----------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

float speed = 1;
enum DifficultyLevel difficultyLevel = LOW;
enum Direction currentDirection;
int score = 0;
//struct Node* queue;
bool matrix[Y_AXIS_ELEMENTS][X_AXIS_ELEMENTS];
int startGame;
bool showInitialDelay = true;
bool gameFinished = false;

static TaskHandle_t task_draw_handle;		//Touchscreen Task Handle
static TaskHandle_t task_readJoystick_handle;		//Touchscreen Task Handle
static SemaphoreHandle_t lcd_mut;			//Mutex to access Display
											// /!\In this case with only one
											//    task is useless.


/* Private function prototypes -----------------------------------------------*/

/* Functions definition ------------------------------------------------------*/
/**
  * @brief Create the FreeRTOS objects and tasks.
  * @return true if the tasks are created, false otherwise.
  */
void freeRTOS_user_init(void){
	bool retval = true;

	// Setup list
	currentDirection = UP;
	startGame = 0;
	//queue = initQueue();

	lcd_mut = xSemaphoreCreateMutex();			//Create mutex (LCD access)
	if(lcd_mut == NULL)
		retval = false;

	retval &= xTaskCreate( task_draw_fct,		//Task function
				"Task draw",					//Task function comment
				256,							//Task stack dimension (1kB)
				NULL,							//Task parameter
				1,								//Task priority
				&task_draw_handle );			//Task handle

	retval &= xTaskCreate( task_readJoystick_fct,		//Task function
				"Task read joystick",					//Task function comment
				256,									//Task stack dimension (1kB)
				NULL,									//Task parameter
				1,										//Task priority
				&task_readJoystick_handle );			//Task handle

	// Fill initial grid
	computeInitialGrid();

	// Print welcome message
	displayWelcomeMessage();
}

void task_readJoystick_fct( void *pvParameters ){

	while(1){
		// Wait until game gets started
		while(!startGame) vTaskDelay(pdMS_TO_TICKS(50));

		// Wait until initial delay is over
		while(showInitialDelay) vTaskDelay(pdMS_TO_TICKS(10));

		while(!gameFinished){
			enum Direction newDirection = readJoystick();

			if(newDirection != IDLE) currentDirection = newDirection;

			move_and_compute_new_grid(currentDirection);
			vTaskDelay(pdMS_TO_TICKS(MOVE_INTERVAL));
		}

		vTaskDelay(pdMS_TO_TICKS(200));
	}

}

void move_and_compute_new_grid(enum Direction newDirection){

	if(newDirection == DOWN){
		// Check for collision
		if(matrix[Y_AXIS_CENTER + 1][X_AXIS_CENTER]){
			gameFinished = true;
		}else{
			// Shift elements up
			for(int i = 0; i < Y_AXIS_ELEMENTS - 1; i++){
				for(int j = 0; j < X_AXIS_ELEMENTS; j++){
					matrix[i][j] = matrix[i + 1][j];
				}
			}

			// Insert new elements
			for(int j = 0; j < X_AXIS_ELEMENTS; j++){
				matrix[Y_AXIS_ELEMENTS - 1][j] = getRandomInt(100) <= SPAWN_PERCENTAGE;
			}
		}
	}else if(newDirection == UP){

		// Check for collision
		if(matrix[Y_AXIS_CENTER - 1][X_AXIS_CENTER]){
			gameFinished = true;
		}else{
			// Shift all cells DOWN
			for(int i = Y_AXIS_ELEMENTS - 1; i > 0; i--){
				for(int j = 0; j < X_AXIS_ELEMENTS; j++){
					matrix[i][j] = matrix[i - 1][j];
				}
			}

			// Insert new elements
			for(int j = 0; j < X_AXIS_ELEMENTS; j++){
				matrix[0][j] = getRandomInt(100) <= SPAWN_PERCENTAGE;
			}
		}

	}else if(newDirection == LEFT){

		// Check for collision
		if(matrix[Y_AXIS_CENTER][X_AXIS_CENTER - 1]){
			gameFinished = true;
		}else{
			// Shift all cells RIGHT
			for(int i = 0; i < Y_AXIS_ELEMENTS; i++){
				for(int j = X_AXIS_ELEMENTS - 1; j > 0; j--){
					matrix[i][j] = 	matrix[i][j - 1];
				}
			}

			for(int i = 0; i < Y_AXIS_ELEMENTS; i++){
				matrix[i][0] = getRandomInt(100) <= SPAWN_PERCENTAGE;
			}
		}

	}else if(newDirection == RIGHT){

		// Check for collision
		if(matrix[Y_AXIS_CENTER][X_AXIS_CENTER + 1]){
			gameFinished = true;
		}else{
			// Shift all cells LEFT
			for(int i = 0; i < Y_AXIS_ELEMENTS; i++){
				for(int j = 0; j < X_AXIS_ELEMENTS - 1; j++){
					matrix[i][j] = matrix[i][j + 1];
				}
			}

			// Insert new elements
			for(int i = 0; i < Y_AXIS_ELEMENTS; i++){
				matrix[i][X_AXIS_ELEMENTS - 1] = getRandomInt(100) <= SPAWN_PERCENTAGE;
			}
		}
	}
}

void computeInitialGrid(){
	for(int i = 0; i < Y_AXIS_ELEMENTS; i++){
		for(int j = 0; j < X_AXIS_ELEMENTS; j++){
			matrix[i][j] = getRandomInt(100) <= SPAWN_PERCENTAGE;
		}
	}

	// Free central block
	matrix[Y_AXIS_ELEMENTS][X_AXIS_ELEMENTS] = false;
}

int getRandomInt(int max){
	// returns from 1 to max
	return (rand() % max) + 1;
}

void task_draw_fct( void *pvParameters ){

	// Wait until game gets started
	while(!startGame) vTaskDelay(pdMS_TO_TICKS(50));

	while(1){
		if(gameFinished){
			displayGameEndMessage();
		}else{
			// Draw bricks
			for(int i = 0; i < Y_AXIS_ELEMENTS; i++){
				for(int j = 0; j < X_AXIS_ELEMENTS; j++){
					draw_filled_square(
							j * CUBE_SIDE_LEN,
							i * CUBE_SIDE_LEN,
							CUBE_SIDE_LEN,
							CUBE_SIDE_LEN,
							matrix[i][j] ? LCD_COLOR_WHITE : LCD_COLOR_BLACK
					);
				}
			}

			// Draw player
			draw_filled_square(X_AXIS_CENTER * CUBE_SIDE_LEN, Y_AXIS_CENTER * CUBE_SIDE_LEN, CUBE_SIDE_LEN, CUBE_SIDE_LEN, LCD_COLOR_ORANGE);

			if(showInitialDelay){
				// Wait for 3 seconds
				vTaskDelay(pdMS_TO_TICKS(2000));
				showInitialDelay = false;
			}

			score++;

			char buffer[14];
			sprintf(buffer, "Score: %d", score);

			// Draw score
			BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
			BSP_LCD_SetFont(&Font20);
			BSP_LCD_DisplayStringAt(0, DISP_Y_SIZE - 20, buffer, LEFT_MODE);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

			// Delay
			vTaskDelay(pdMS_TO_TICKS(MOVE_INTERVAL/speed));

			// Enhance speed
			if(difficultyLevel == LOW){
				speed += 0.01;
			}else if(difficultyLevel == MEDIUM){
				speed += 0.02;
			}else if(difficultyLevel == HIGH){
				speed += 0.03;
			}
		}
	}
}

void draw_filled_square(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
	BSP_LCD_SetTextColor(color);
	BSP_LCD_FillRect(x, y, width, height);
}

void displayWelcomeMessage(){
	BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_DisplayStringAt(0, DISP_Y_SIZE / 2 - 20, "Welcome to BlockEscape", CENTER_MODE);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font20);
	BSP_LCD_DisplayStringAt(0, DISP_Y_SIZE / 2 + 20, "To start a new game press the [user] button", CENTER_MODE);
	BSP_LCD_SetFont(&Font24);
}

void displayGameEndMessage(){

	vTaskDelay(pdMS_TO_TICKS(500));

	BSP_LCD_Clear(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
	BSP_LCD_SetFont(&Font24);

	char buffer[32];
	sprintf(buffer, "You lost! Your score was: %d", score);

	BSP_LCD_DisplayStringAt(0, DISP_Y_SIZE / 2 - 20, buffer, CENTER_MODE);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font20);
	BSP_LCD_DisplayStringAt(0, DISP_Y_SIZE / 2 + 20, "To start a new game press the [user] button", CENTER_MODE);
	BSP_LCD_SetFont(&Font24);

	// Reset variables
	speed = 1;
	startGame = 0;
	currentDirection = UP;
	showInitialDelay = true;
	gameFinished = false;
	score = 0;

	// Fill initial grid
	computeInitialGrid();

	// Wait until new game gets started
	while(!startGame) vTaskDelay(pdMS_TO_TICKS(200));

}
