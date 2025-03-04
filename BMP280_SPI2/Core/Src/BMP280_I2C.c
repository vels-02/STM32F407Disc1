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
#include "util.h"


extern I2C_HandleTypeDef hi2c1; // Make sure the I2C handler is declared elsewhere

uint8_t deviceID = 0;
uint8_t temp_data[3];
uint8_t press_data[3];
uint8_t calib_raw_data[24];

HAL_StatusTypeDef status;
volatile uint8_t rxReadComplete = 0;
volatile uint8_t rxWriteComplete = 0;

// Callback for I2C error
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C1) {
        // Get error flags
    	uint32_t error = HAL_I2C_GetError(hi2c);

        if (error != HAL_I2C_ERROR_NONE) {
            // Log or handle error here
            printf("I2C error occurred: 0x%08lX\r\n", error);

            // Optionally reset the I2C or take other corrective actions
            HAL_I2C_DeInit(hi2c);
            HAL_I2C_Init(hi2c);
        }
    }
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

// Generic I2C read register function (Interrupt mode)
uint8_t BMP280_I2C_ReadRegister_IT(uint8_t reg, uint8_t *pData, uint8_t len) {
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read_IT(&hi2c1, BMP280_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, pData, len);
    if (status != HAL_OK) {
        printf("I2C read error: %d\r\n", status);
        return 0;
    }
    return 1;
}


// Callback for I2C read complete
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C1) {
    	rxReadComplete = 1;
        //printf("I2C read complete\r\n");
    }
}

// Callback for I2C write complete
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C1) {
    	rxWriteComplete = 1;
        //printf("I2C write complete\r\n");
    }
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

// Generic I2C write register function (Interrupt mode)
HAL_StatusTypeDef BMP280_I2C_WriteRegister_IT(uint8_t reg, uint8_t value) {
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write_IT(&hi2c1, BMP280_ADDR << 1, reg, I2C_MEMADD_SIZE_8BIT, &value, 1);
    if (status != HAL_OK) {
        printf("HAL I2C Write error (reg: 0x%02X, value: 0x%02X): %d \r\n", reg, value, status);
    }
    return status;
}


uint8_t BMP280_I2C_ReadSensorCalibData(void) {

	uint8_t status = BMP280_I2C_ReadRegister_IT(BMP280_REG_CALIB_DATA, calib_raw_data, 24);
    if (!status) {
        printf("Error Reading calibration data\r\n");
    }
	if (WaitForConditionWithTimeout(&rxReadComplete, 1000) != HAL_OK) {
		printf("I2C read timeout occurred\r\n");
		return HAL_ERROR;
	}
    rxReadComplete = 0;
    BMP280_ApplySensorCalibData(calib_raw_data);

    return 1;
}
uint8_t BMP280_I2C_Init(void) {

    printf("BMP280 I2C Test\r\n");

	status = BMP280_I2C_ReadRegister_IT(BMP280_REG_ID, &deviceID, 1);
	if (!status) {
		printf("Error Reading temperature sensor data \r\n");
	}
	if (WaitForConditionWithTimeout(&rxReadComplete, 1000) != HAL_OK) {
		printf("I2C read timeout occurred\r\n");
		rxReadComplete = 0;
		return HAL_ERROR;
	}
	rxReadComplete = 0;

    if (deviceID != BMP280_CHIP_ID) {
        printf("BMP280 not found! Device ID: 0x%02X\r\n", deviceID);
        return HAL_ERROR;
    } else {
        printf("I2C BMP280 found! Device ID: 0x%02X\r\n", deviceID);
    }

    if (BMP280_I2C_WriteRegister(BMP280_REG_CTRL_MEAS, 0x27) != HAL_OK) {
        printf("Failed to configure BMP280.\r\n");
        return HAL_ERROR;
    }
	/*if (WaitForConditionWithTimeout(&rxWriteComplete, 1000) != HAL_OK) {
		printf("I2C write timeout occurred\r\n");
		rxWriteComplete = 0;
		return HAL_ERROR;
	}*/
	rxWriteComplete = 0;


    if (BMP280_I2C_WriteRegister(BMP280_REG_CONF_REG, 0xA0) != HAL_OK) {
        printf("Failed to configure BMP280.\r\n");
        return HAL_ERROR;
    }
	/*if (WaitForConditionWithTimeout(&rxWriteComplete, 1000) != HAL_OK) {
		printf("I2C write timeout occurred\r\n");
		rxWriteComplete = 0;
		return HAL_ERROR;
	}*/
    rxWriteComplete = 0;

    BMP280_I2C_ReadSensorCalibData();

    return deviceID;
}

uint8_t BMP280_I2C_ReadSensorData(void) {

    uint8_t status;

    status = BMP280_I2C_ReadRegister_IT(BMP280_REG_TEMP_MSB, temp_data, 3);
    if (!status) {
        printf("Error Reading temperature sensor data \r\n");
    }
	if (WaitForConditionWithTimeout(&rxReadComplete, 1000) != HAL_OK) {
		printf("I2C write timeout occurred\r\n");
		rxReadComplete = 0;
		return HAL_ERROR;
	}
	rxReadComplete = 0;
    status = BMP280_I2C_ReadRegister_IT(BMP280_REG_PRESS_MSB, press_data, 3);
    if (!status) {
        printf("Error Reading pressure sensor data \r\n");
    }
	if (WaitForConditionWithTimeout(&rxReadComplete, 1000) != HAL_OK) {
		printf("I2C write timeout occurred\r\n");
		rxReadComplete = 0;
		return HAL_ERROR;
	}
    rxReadComplete = 0;

    ProcessSensorData(temp_data, press_data);

    return 1;
}

