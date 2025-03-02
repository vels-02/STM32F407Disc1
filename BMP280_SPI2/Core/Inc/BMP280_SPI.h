/*
 * BMP280_SPI.h
 *
 *  Created on: Feb 28, 2025
 *      Author: velu
 */

#ifndef INC_BMP280_SPI_H_
#define INC_BMP280_SPI_H_

#include "main.h"

// BMP280 SPI functions

uint8_t BMP280_SPI_Init(void);  // Initialize SPI for BMP280

uint8_t BMP280_SPI_ReadDeviceID(uint8_t reg);  // Read Device ID
uint8_t BMP280_ReadRegister(uint8_t reg, uint8_t *pData, uint8_t len);  // Read data from register
HAL_StatusTypeDef BMP280_WriteRegister(uint8_t reg, uint8_t value);  // Write to register
uint8_t BMP280_ReadSensorCalibData(void);  // Read sensor calibration data
uint8_t BMP280_SPI_ReadSensorData(void);  // Read raw sensor data

#endif /* INC_BMP280_SPI_H_ */
