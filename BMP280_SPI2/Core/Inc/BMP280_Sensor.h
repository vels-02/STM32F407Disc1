/*
 * BMP280_Sensor.h
 *
 *  Created on: Mar 1, 2025
 *      Author: velu
 */

#ifndef BMP280_SENSOR_H_
#define BMP280_SENSOR_H_

#include <stdint.h>

// BMP280 Chip ID
#define BMP280_CHIP_ID     0x58 // Expected device ID

#define BMP280_ADDR        	0x76  // BMP280 I2C address when SDO is connected to GND
#define BMP280_REG_ID       0xD0  // Register for Device ID

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


//Function declarations


float BMP280_CompensateTemperature(int32_t temp_raw_data, int32_t *t_fine);  // Compensate temperature
float BMP280_CompensatePressure(int32_t press_raw_data, int32_t t_fine);  // Compensate pressure
void ReadAndCalibrate(int32_t temp_raw_data, int32_t press_raw_data);  // Read and calibrate sensor data
void BMP280_ApplySensorCalibData(uint8_t *calib_raw_data);
void ProcessSensorData(uint8_t *temp_data, uint8_t *press_data);
#endif /* BMP280_SENSOR_H_ */
