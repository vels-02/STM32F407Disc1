/*
 * util.c
 *
 *  Created on: Mar 2, 2025
 *      Author: velu
 */

#include "util.h"


HAL_StatusTypeDef WaitForConditionWithTimeout(volatile uint8_t *condition, uint32_t timeout_ms) {

    uint32_t startTick = HAL_GetTick();  // Store the current system time

    while (!(*condition)) {  // Wait for the condition (e.g., rxWriteComplete)
        if (HAL_GetTick() - startTick >= timeout_ms) {
            // Timeout occurred
            return HAL_TIMEOUT;  // Use HAL_TIMEOUT instead of HAL_OK
        }
        HAL_Delay(1);  // Optional: Add a small delay to prevent busy-waiting
    }

    return HAL_OK;  // Condition met before timeout
}


