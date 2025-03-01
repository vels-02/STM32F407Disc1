/*
 * uart_handler.c
 *
 *  Created on: Mar 1, 2025
 *      Author: velu
 */

#include "uart_handler.h"
#include "led_control.h"

uint8_t rxBuffer[RX_BUFFER_SIZE];
volatile uint8_t rx_index = 0;
volatile uint8_t uart_led_cmd_ready;
uint8_t cmdBuffer[RX_BUFFER_SIZE];

extern UART_HandleTypeDef huart2;

// Redirect printf to UART2
int __io_putchar(int ch) {
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

void UART_Init(UART_HandleTypeDef *huart)
{
	//Uart intialized by HAL, just enable interrupt
	 HAL_UART_Receive_IT(&huart2, (uint8_t*)&rxBuffer[rx_index], 1); // Start receiving UART data in interrupt mode

}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        // Ensure index stays within buffer limits
        if (rx_index >= RX_BUFFER_SIZE) rx_index = 0;

        rxBuffer[rx_index] = huart->Instance->DR;

        if (rxBuffer[rx_index] == '\r' || rxBuffer[rx_index] == '\n') {
            rxBuffer[rx_index] = '\0';  // Null-terminate
            if (rx_index > 0) {        // Only print if there's actual content
               // printf("Received input: %s\r\n", rxBuffer);
            	strcpy((char*)cmdBuffer, (char*)rxBuffer);
            	uart_led_cmd_ready =1;
            } else {
                printf("Received empty input\r\n");  // Debugging empty input
            }
            rx_index = 0;  // Reset for the next message

            memset((uint8_t*)rxBuffer,0,RX_BUFFER_SIZE);
        } else {
            rx_index++;    // Move to the next position
        }

        // Start receiving next byte
        HAL_UART_Receive_IT(&huart2, (uint8_t *)&rxBuffer[rx_index], 1);
    }
}

void Handle_LED_Commands()
{
	if (uart_led_cmd_ready)
	{
		Process_LED_Commands((const char *)cmdBuffer);
		//clear the buffer & flag
		memset((uint8_t*)cmdBuffer,0,RX_BUFFER_SIZE);
		uart_led_cmd_ready = 0;
	}
}
