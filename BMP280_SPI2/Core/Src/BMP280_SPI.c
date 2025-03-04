/*
 * BMP280_SPI.c
 *
 *  Created on: Feb 28, 2025
 *      Author: velu
 */

#include "BMP280_SPI.h"
#include <stdio.h>
#include "BMP280_Sensor.h"

/*
 * Pin details
 *
 * SPI2
 * PB12 - NSS
 * PB13 - CLK
 * PB14 - MISO
 * PB15 - MOSI
 */

// *******************************************************
//                      Defines
// *******************************************************

#define BMP280_CS_HIGH()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)
#define BMP280_CS_LOW()     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)

extern SPI_HandleTypeDef hspi2;

volatile uint8_t rxCompleteFlag = 0;  // Flag to indicate completion of reception

void SPI2_IRQHandler(void) {
    // Call the HAL SPI IRQ handler
    HAL_SPI_IRQHandler(&hspi2);
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI2) {
        // Set the flag to indicate that reception is complete
        rxCompleteFlag = 1;
    }
}

uint8_t BMP280_SPI_ReadDeviceID(uint8_t reg) {
    uint8_t txData[2];
    uint8_t rxData; // Local variable for single-byte read

    txData[0] = reg | 0x80;  // Read command: MSB set

    BMP280_CS_LOW();
    // Start the transmission and reception in interrupt mode
    HAL_SPI_Transmit_IT(&hspi2, &txData[0], 1);  // Transmit the address byte
    while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY) {}  // Wait for completion

    // Start reception of the data
    HAL_SPI_Receive_IT(&hspi2, &rxData, 1);  // Receive the data byte

    // Wait until data reception is complete
    while (!rxCompleteFlag);  // rxCompleteFlag will be set when ISR completes reception

    BMP280_CS_HIGH();

    return rxData;
}

uint8_t BMP280_ReadRegister(uint8_t reg, uint8_t *pData, uint8_t len) {
    uint8_t txData = reg | 0x80; // Set read bit

    BMP280_CS_LOW();
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi2, &txData, 1, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        printf("HAL Transmit error: %d \r\n", status);
        BMP280_CS_HIGH();
        return 0; // Return 0 on error
    }

    status = HAL_SPI_Receive(&hspi2, pData, len, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        printf("HAL Receive error: %d \r\n", status);
        BMP280_CS_HIGH();
        return 0; // Return 0 on error
    }

    BMP280_CS_HIGH();
    return 1;
}

HAL_StatusTypeDef BMP280_WriteRegister(uint8_t reg, uint8_t value) {
    uint8_t txData[2] = {reg & 0x7F, value}; // Mask register address for write operation

    BMP280_CS_LOW();

    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi2, txData, 2, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        printf("HAL Transmit error: %d \r\n", status);
    }
    BMP280_CS_HIGH();

    return status;
}

uint8_t BMP280_SPI_Init(void) {
    printf("BMP280 SPI Test\r\n");

    uint8_t chip_id = BMP280_SPI_ReadDeviceID(BMP280_REG_ID);
    printf("Velu BMP280 Chip ID: 0x%X\r\n", chip_id);

    if (chip_id != BMP280_CHIP_ID) {
        printf("BMP280 SPI not detected\r\n");
        return 0;
    }

    // Set BMP280 to Normal Mode with 1x oversampling
    if (BMP280_WriteRegister(BMP280_REG_CTRL_MEAS, 0x27) != HAL_OK) {
        printf("Failed to configure BMP280.\r\n");
        return 0;
    }

    // Set configuration register (no filter, 1000 ms standby time)
    if (BMP280_WriteRegister(BMP280_REG_CONF_REG, 0xA0) != HAL_OK) {
        printf("Failed to configure BMP280.\r\n");
        return 0;
    }

    BMP280_ReadSensorCalibData();

    return 1;
}

uint8_t BMP280_ReadSensorCalibData(void) {
    uint8_t calib_raw_data[24];
    uint8_t status;

    status = BMP280_ReadRegister(BMP280_REG_CALIB_DATA, calib_raw_data, 24); // read 24 bytes of data
    if (!status) {
        printf("Error Reading temp sensor data \r\n");
    }

    BMP280_ApplySensorCalibData(calib_raw_data);

    return 1;
}

uint8_t BMP280_SPI_ReadSensorData(void) {
    uint8_t temp_data[3];
    uint8_t press_data[3];
    uint8_t status;

    status = BMP280_ReadRegister(BMP280_REG_TEMP_MSB, temp_data, 3); // read 3 bytes of data
    if (!status) {
        printf("Error Reading temperature sensor data \r\n");
    }

    status = BMP280_ReadRegister(BMP280_REG_PRESS_MSB, press_data, 3); // read 3 bytes of data
    if (!status) {
        printf("Error Reading pressure sensor data \r\n");
    }

    ProcessSensorData(temp_data, press_data);

    return 1;
}
