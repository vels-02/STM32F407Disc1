/*
 * BMP280_SPI.h
 *
 *  Created on: Feb 28, 2025
 *      Author: velu
 */

#ifndef INC_BMP280_SPI_H_
#define INC_BMP280_SPI_H_

#include "main.h"

// BMP280 Chip ID
#define BMP280_CHIP_ID     0x58 // Expected device ID

// BMP280 I2C/SPI Registers
#define BMP280_REG_ID            0xD0
#define BMP280_REG_CTRL_MEAS     0xF4
#define BMP280_REG_CONF_REG      0xF5
#define BMP280_REG_STATUS        0xF3
#define BMP280_REG_PRESS_MSB     0xF7
#define BMP280_REG_TEMP_MSB      0xFA
#define BMP280_REG_CALIB_DATA    0x88

// Calibration data structure for BMP280
typedef struct
{
    uint16_t dig_T1;    // Temperature calibration
    int16_t dig_T2;
    int16_t dig_T3;

    uint16_t dig_P1;    // Pressure calibration
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;
} BMP280_CalibData_t;

extern BMP280_CalibData_t calib_data;  // Instance of calibration data

// BMP280 SPI functions

uint8_t BMP280_SPI_Init(void);  // Initialize SPI for BMP280
void ReadAndCalibrate(int32_t temp_raw_data, int32_t press_raw_data);  // Read and calibrate sensor data
uint8_t BMP280_SPI_ReadDeviceID(uint8_t reg);  // Read Device ID
uint8_t BMP280_ReadRegister(uint8_t reg, uint8_t *pData, uint8_t len);  // Read data from register
HAL_StatusTypeDef BMP280_WriteRegister(uint8_t reg, uint8_t value);  // Write to register
uint8_t BMP280_ReadSensorCalibData(void);  // Read sensor calibration data
uint8_t BMP280_SPI_ReadSensorData(void);  // Read raw sensor data

float BMP280_CompensateTemperature(int32_t temp_raw_data, int32_t *t_fine);  // Compensate temperature
float BMP280_CompensatePressure(int32_t press_raw_data, int32_t t_fine);  // Compensate pressure

#endif /* INC_BMP280_SPI_H_ */
