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
		printf("Received input: %s\r\n", cmd);
		TurnOFF_all_LEDs();

		if ( strncmp("green",(char*)cmd, strlen("green")) == 0)
		{
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
		}
		else if ( strncmp("orange",(char*)cmd,strlen("orange")) == 0)
		{
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
		}
		else if ( strncmp("red",(char*)cmd,strlen("red")) == 0)
		{
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
		}
		else if ( strncmp("blue",(char*)cmd,strlen("blue")) == 0)
		{
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
		}
}

