/*
 * LED_control.h
 *
 *  Created on: Mar 1, 2025
 *      Author: velu
 */

#ifndef INC_LED_CONTROL_H_
#define INC_LED_CONTROL_H_

//LED colors
#define GREEN_LED_PIN GPIO_PIN_12
#define ORANGE_LED_PIN GPIO_PIN_13
#define RED_LED_PIN GPIO_PIN_14
#define BLUE_LED_PIN GPIO_PIN_15

void TurnOFF_all_LEDs();
void Process_LED_Commands(const char* cmd);

#endif /* INC_LED_CONTROL_H_ */
