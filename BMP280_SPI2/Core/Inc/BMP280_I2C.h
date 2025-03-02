/*
 * BMP280_I2C.h
 *
 *  Created on: Mar 1, 2025
 *      Author: velu
 */

#ifndef INC_BMP280_I2C_H_
#define INC_BMP280_I2C_H_


#include "stm32f4xx_hal.h"



//Function declaration to read device id
uint8_t BMP280_I2C_ReadDeviceID(void);
HAL_StatusTypeDef BMP280_I2C_WriteRegister(uint8_t reg, uint8_t value);
uint8_t BMP280_I2C_ReadRegister(uint8_t reg, uint8_t *pData, uint8_t len);
uint8_t BMP280_I2C_Init(void);
uint8_t BMP280_I2C_ReadSensorData(void);
uint8_t BMP280_I2C_ReadSensorCalibData(void);

#endif /* INC_BMP280_I2C_H_ */
