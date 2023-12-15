#include <stdio.h>
#include <stdbool.h>
#include "BSP/stm32f769i_discovery_lcd.h"	//Include file to use the LDC screen
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "main_user.h"
#include "joystick.h"

/* Private define ------------------------------------------------------------*/
#define COLOR_C 	LCD_COLOR_WHITE
#define COLOR_T 	LCD_COLOR_GRAY
#define CUBE_SIDE_LEN	20
#define DISP_X_SIZE	800
#define DISP_Y_SIZE	480

/* Private data types definition ---------------------------------------------*/


/* Public variables ----------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
static TaskHandle_t task_draw_handle;		//Touchscreen Task Handle
static TaskHandle_t task_readJoystick_handle;		//Touchscreen Task Handle
static SemaphoreHandle_t lcd_mut;			//Mutex to access Display
											// /!\In this case with only one
											//    task is useless.


/* Private function prototypes -----------------------------------------------*/
static void draw_filled_square(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
static void task_draw_fct( void *pvParameters );
static void task_readJoystick_fct( void *pvParameters );
/* Functions definition ------------------------------------------------------*/
/**
  * @brief Create the FreeRTOS objects and tasks.
  * @return true if the tasks are created, false otherwise.
  */
void freeRTOS_user_init(void){
	bool retval = true;

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
					256,							//Task stack dimension (1kB)
					NULL,							//Task parameter
					1,								//Task priority
					&task_readJoystick_handle );			//Task handle
}

static void task_readJoystick_fct( void *pvParameters ){
	BaseType_t ret;
	while(1){
		readJoystick();
		vTaskDelay(pdMS_TO_TICKS(200));
	}
}

static void task_draw_fct( void *pvParameters ){

	uint16_t x = 0;
	uint16_t y = 0;
	uint16_t gap = 10;
	BaseType_t ret;

	while(1){
		ret = xSemaphoreTake(lcd_mut, portMAX_DELAY);	//Lock Mutex
		if (ret == pdTRUE){
			draw_filled_square(x, y, CUBE_SIDE_LEN, CUBE_SIDE_LEN);

			if(x + gap + CUBE_SIDE_LEN < DISP_X_SIZE){
				x += gap + CUBE_SIDE_LEN;
			}else{
				x = 0;
				y += gap + CUBE_SIDE_LEN;
			}
			xSemaphoreGive(lcd_mut);					//Unlock Mutex
		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

static void draw_filled_square(uint16_t x, uint16_t y, uint16_t width, uint16_t height){
	BSP_LCD_SetTextColor(COLOR_C);	//Set border color
	BSP_LCD_FillRect(x, y, width, height); //Draw the circle border outline
	//BSP_LCD_DrawCircle(x, y, r);
	//BSP_LCD_SetTextColor(COLOR_T);		//Set the filling color
	//BSP_LCD_FillCircle(x, y, r-1);	//Draw the filled circle
}
