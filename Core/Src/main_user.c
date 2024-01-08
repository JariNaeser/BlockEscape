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
#define MOVE_INTERVAL 700
#define SPAWN_PERCENTAGE 5
#define X_AXIS_CENTER X_AXIS_ELEMENTS / 2
#define Y_AXIS_CENTER Y_AXIS_ELEMENTS / 2

/* Private data types definition ---------------------------------------------*/


/* Public variables ----------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

float speed = 1;
enum DifficultyLevel difficultyLevel = HIGH;
enum Direction currentDirection;
int score = 0;
struct Node* queue;
int startGame;
bool showInitialDelay = true;
bool gameFinished = false;

static TaskHandle_t task_draw_handle;
static TaskHandle_t task_readJoystick_handle;
static SemaphoreHandle_t lcd_mut;


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
	queue = initQueue();

	lcd_mut = xSemaphoreCreateMutex();			//Create mutex (LCD access)
	if(lcd_mut == NULL)
		retval = false;

	retval &= xTaskCreate(task_draw_fct, "Task draw", 256, NULL, 2, &task_draw_handle); // Lower priority
	retval &= xTaskCreate(task_readJoystick_fct, "Task read joystick", 256, NULL, 1, &task_readJoystick_handle); // Higher priority

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

	Node* tmp = queue->next;

	while(tmp != NULL){
		Node* next = tmp->next;

		if(newDirection == DOWN){
			// Remove blocks
			if(tmp->x == X_AXIS_CENTER && tmp->y == Y_AXIS_CENTER + 1){
				gameFinished = true;
				return;
			}else if(tmp->y == 0){
				removeNode(tmp);
			}else{
				// Move blocks
				tmp->y -= 1;
			}
		}else if(newDirection == UP){
			// Remove blocks
			if(tmp->x == X_AXIS_CENTER && tmp->y == Y_AXIS_CENTER - 1){
				gameFinished = true;
				return;
			}else if(tmp->y == X_AXIS_ELEMENTS - 1){
				removeNode(tmp);
			}else{
				// Move blocks
				tmp->y += 1;
			}
		}else if(newDirection == LEFT){
			// Remove blocks
			if(tmp->x == X_AXIS_CENTER - 1 && tmp->y == Y_AXIS_CENTER){
				gameFinished = true;
				return;
			}else if(tmp->x == X_AXIS_ELEMENTS - 1){
				removeNode(tmp);
			}else{
				// Move blocks
				tmp->x += 1;
			}
		}else if(newDirection == RIGHT){
			// Remove blocks
			if(tmp->x == X_AXIS_CENTER + 1 && tmp->y == Y_AXIS_CENTER){
				gameFinished = true;
				return;
			}else if(tmp->x == 0){
				removeNode(tmp);
			}else{
				// Move blocks
				tmp->x -= 1;
			}
		}

		tmp = next;
	}

	// Generate new blocks
	if(newDirection == DOWN){
		for(int x = 0; x < X_AXIS_ELEMENTS; x++){
			if(getRandomInt(100) <= SPAWN_PERCENTAGE){
				addNode(queue, createNode(true, x, Y_AXIS_ELEMENTS - 1));
			}
		}
	}else if(newDirection == UP){
		for(int x = 0; x < X_AXIS_ELEMENTS; x++){
			if(getRandomInt(100) <= SPAWN_PERCENTAGE){
				addNode(queue, createNode(true, x, 0));
			}
		}
	}else if(newDirection == LEFT){
		for(int y = 0; y < X_AXIS_ELEMENTS; y++){
			if(getRandomInt(100) <= SPAWN_PERCENTAGE){
				addNode(queue, createNode(true, 0, y));
			}
		}
	}else if(newDirection == RIGHT){
		for(int y = 0; y < X_AXIS_ELEMENTS; y++){
			if(getRandomInt(100) <= SPAWN_PERCENTAGE){
				addNode(queue, createNode(true, X_AXIS_ELEMENTS - 1, y));
			}
		}
	}
}

void computeInitialGrid(){
	for(int i = 0; i < Y_AXIS_ELEMENTS; i++){
		for(int j = 0; j < X_AXIS_ELEMENTS; j++){
			if(getRandomInt(100) <= SPAWN_PERCENTAGE){
				// Add new node
				addNode(queue, createNode(true, j, i));
			}
		}
	}

	// Free central block
	Node* tmp = queue->next;

	while(tmp != NULL){
		if(tmp->x == X_AXIS_ELEMENTS && tmp->y == Y_AXIS_ELEMENTS){
			// Remove node
			removeNode(tmp);
		}
		tmp = tmp->next;
	}
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

			BSP_LCD_Clear(LCD_COLOR_BLACK);

			// Draw bricks
			Node* tmp = queue->next;

			while(tmp != NULL){

				if(tmp->isPixel){
					draw_filled_square(tmp->x * CUBE_SIDE_LEN, tmp->y * CUBE_SIDE_LEN, CUBE_SIDE_LEN, CUBE_SIDE_LEN, LCD_COLOR_WHITE);
				}

				tmp = tmp->next;
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
			if(difficultyLevel == MEDIUM){
				speed += 0.03;
			}else if(difficultyLevel == HIGH){
				speed += 0.045;
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

	// Empty queue
	Node* tmp = queue->next;

	while(tmp != NULL){
		Node* next = tmp->next;
		removeNode(tmp);
		tmp = next;
	}

	queue->next = NULL;

	// Fill initial grid
	computeInitialGrid();

	// Wait until new game gets started
	while(!startGame) vTaskDelay(pdMS_TO_TICKS(200));

}
