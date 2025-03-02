/*
 * BMP280_I2C.c
 *
 *  Created on: Mar 1, 2025
 *      Author: velu
 */

/*
 * I2C1 - GPIO Pin Details
 * PB6 - SCL
 * PB7 - SDA
 */

#include "BMP280_Sensor.h"
#include "BMP280_I2C.h"
#include <stdio.h>


extern I2C_HandleTypeDef hi2c1; // Make sure the I2C handler is declared elsewhere


uint8_t BMP280_I2C_ReadDeviceID(void) {

    uint8_t deviceID = 0;
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Read(&hi2c1, BMP280_ADDR << 1, BMP280_REG_ID, I2C_MEMADD_SIZE_8BIT, &deviceID, 1, 1000);
    if (status != HAL_OK) {
        printf("HAL I2C Read error: %d \r\n", status);
        return 0;
    }

    return deviceID;
}

//Generic I2C read register function
uint8_t BMP280_I2C_ReadRegister(uint8_t reg, uint8_t *pData, uint8_t len) {

	  HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, BMP280_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, pData, len, 1000);
	    if (status != HAL_OK) {
	        printf("I2C read error: %d\r\n", status);
	        return 0;  // Return error if read fails
	    }
	    return 1;
}
//Generic I2C write register function
HAL_StatusTypeDef BMP280_I2C_WriteRegister(uint8_t reg, uint8_t value) {

    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1, BMP280_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 1000);
    if (status != HAL_OK) {
        // Error handling for write failure
        printf("HAL I2C Write error (reg: 0x%02X, value: 0x%02X): %d \r\n", reg, value, status);
        return status;  // Return the error code to propagate it to the caller
    }
    return status;
}

uint8_t BMP280_I2C_ReadSensorCalibData(void) {

    uint8_t calib_raw_data[24];
    uint8_t status = BMP280_I2C_ReadRegister(BMP280_REG_CALIB_DATA, calib_raw_data, 24);
    if (!status) {
        printf("Error Reading calibration data\r\n");
    }

    BMP280_ApplySensorCalibData(calib_raw_data);

    return 1;
}

uint8_t BMP280_I2C_Init(void) {

    printf("BMP280 I2C Test\r\n");

    uint8_t chip_id = BMP280_I2C_ReadDeviceID();
    if (chip_id != BMP280_CHIP_ID) {
        printf("BMP280 not found! Device ID: 0x%02X\r\n", chip_id);
    } else {
        printf("I2C BMP280 found! Device ID: 0x%02X\r\n", chip_id);
    }

    if (chip_id != BMP280_CHIP_ID) {
        printf("BMP280 not detected\n");
        return 0;
    }

    if (BMP280_I2C_WriteRegister(BMP280_REG_CTRL_MEAS, 0x27) != HAL_OK) {
        printf("Failed to configure BMP280.\r\n");
        return 0;
    }

    if (BMP280_I2C_WriteRegister(BMP280_REG_CONF_REG, 0xA0) != HAL_OK) {
        printf("Failed to configure BMP280.\r\n");
        return 0;
    }

    BMP280_I2C_ReadSensorCalibData();

    return chip_id;
}

uint8_t BMP280_I2C_ReadSensorData(void) {
    uint8_t temp_data[3];
    uint8_t press_data[3];
    uint8_t status;

    status = BMP280_I2C_ReadRegister(BMP280_REG_TEMP_MSB, temp_data, 3);
    if (!status) {
        printf("Error Reading temperature sensor data \r\n");
    }

    status = BMP280_I2C_ReadRegister(BMP280_REG_PRESS_MSB, press_data, 3);
    if (!status) {
        printf("Error Reading pressure sensor data \r\n");
    }

    ProcessSensorData(temp_data, press_data);

    return 1;
}

