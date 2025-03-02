/*
 * led_control.c
 *
 *  Created on: Mar 1, 2025
 *      Author: velu
 */


#include "led_control.h"
#include "uart_handler.h"
#include "stm32f4xx_hal.h"

void TurnOFF_all_LEDs()
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
}

void Process_LED_Commands(const char* cmd)
{
	if (cmd == NULL || strlen(cmd) == 0) {

	    printf("Error: Invalid command\r\n");
	    return;
	}
		printf("Received input: %s\r\n", cmd);
		TurnOFF_all_LEDs();

		if ( strcmp("green",cmd) == 0)
		{
			HAL_GPIO_WritePin(GPIOD, GREEN_LED_PIN, GPIO_PIN_SET);
		}
		else if ( strcmp("orange",cmd) == 0)
		{
			HAL_GPIO_WritePin(GPIOD, ORANGE_LED_PIN, GPIO_PIN_SET);
		}
		else if ( strcmp("red",cmd) == 0)
		{
			HAL_GPIO_WritePin(GPIOD, RED_LED_PIN, GPIO_PIN_SET);
		}
		else if ( strcmp("blue",cmd) == 0)
		{
			HAL_GPIO_WritePin(GPIOD, BLUE_LED_PIN, GPIO_PIN_SET);
		}
		else {
		    printf("Invalid LED command received: %s\r\n", cmd);
		}
}

