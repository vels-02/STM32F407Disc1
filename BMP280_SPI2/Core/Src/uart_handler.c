/*
 * uart_handler.c
 *
 *  Created on: Mar 1, 2025
 *      Author: velu
 */

#include "uart_handler.h"
#include "led_control.h"


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

uint8_t rxBuffer[RX_BUFFER_SIZE];
volatile uint8_t rx_index = 0;
//volatile uint8_t uart_led_cmd_ready;


extern UART_HandleTypeDef huart2;
extern QueueHandle_t uartQueue;


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
            if (rx_index > 0) {
            	 uint8_t tempBuffer[RX_BUFFER_SIZE];
            	// Only print if there's actual content
               // printf("Received input: %s\r\n", rxBuffer);
            	strcpy((char *)tempBuffer, (char *)rxBuffer);

            	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            	BaseType_t result = xQueueSendFromISR(uartQueue, tempBuffer, &xHigherPriorityTaskWoken);

            	if (result == errQUEUE_FULL) {
            	    printf("Queue send failed: Queue full!\r\n");
            	} else if (result != pdTRUE) {
            	    printf("Queue send failed: Unknown error!\r\n");
            	}
            	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
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
