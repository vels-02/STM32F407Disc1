/*
 * util.h
 *
 *  Created on: Mar 2, 2025
 *      Author: velu
 */

#ifndef INC_UTIL_H_
#define INC_UTIL_H_

#include <stm32f4xx_hal.h>


HAL_StatusTypeDef WaitForConditionWithTimeout(volatile uint8_t *condition, uint32_t timeout_ms);


#endif /* INC_UTIL_H_ */
