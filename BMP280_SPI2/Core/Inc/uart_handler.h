/*
 * uart_handler.h
 *
 *  Created on: Mar 1, 2025
 *      Author: velu
 */

#ifndef INC_UART_HANDLER_H_
#define INC_UART_HANDLER_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define RX_BUFFER_SIZE 255
//extern uint8_t cmdBuffer[RX_BUFFER_SIZE]; // For processing
extern volatile uint8_t uart_led_cmd_ready;


//FP

void UART_Init(UART_HandleTypeDef *huart);
int __io_putchar(int ch);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void Handle_LED_Commands();

#endif /* INC_UART_HANDLER_H_ */
