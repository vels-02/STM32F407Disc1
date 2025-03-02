/*
 * BMP280_SPI.c
 *
 *  Created on: Feb 28, 2025
 *      Author: velu
 */

#include "BMP280_SPI.h"
#include <stdio.h>

// *******************************************************
//                      Defines
// *******************************************************

#define BMP280_CS_HIGH()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)
#define BMP280_CS_LOW()     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)


BMP280_CalibData_t calib_data;

extern SPI_HandleTypeDef hspi2;

uint8_t BMP280_SPI_ReadDeviceID(uint8_t reg) {
    uint8_t txData[2];
    uint8_t rxData[2];

    txData[0] = reg | 0x80;  // Read command: MSB set
    txData[1] = 0xFF;        // Dummy byte

    BMP280_CS_LOW();
    HAL_SPI_Transmit(&hspi2, &txData[0], 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(&hspi2, &rxData[1], 1, HAL_MAX_DELAY);
    BMP280_CS_HIGH();

    return rxData[1];
}

uint8_t BMP280_ReadRegister(uint8_t reg, uint8_t *pData, uint8_t len)
{
    uint8_t txData = reg | 0x80; // Set read bit

    BMP280_CS_LOW();
    HAL_Delay(1); // Short delay to ensure chip readiness

    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi2, &txData, 1, HAL_MAX_DELAY);
    if(status != HAL_OK)
    {
        printf("HAL Transmit error: %d \r\n", status);
        BMP280_CS_HIGH();
        return 0; // Return 0 on error
    }

    status = HAL_SPI_Receive(&hspi2, pData, len, HAL_MAX_DELAY);
    if(status != HAL_OK)
	{
	 printf("HAL /Recieve error: %d \r\n", status);
	 BMP280_CS_HIGH();
	 return 0; // Return 0 on error
	}


    BMP280_CS_HIGH();
    HAL_Delay(1); // Short delay after deselecting
    return 1;
}

HAL_StatusTypeDef BMP280_WriteRegister(uint8_t reg, uint8_t value)
{
    uint8_t txData[2] = {reg & 0x7F, value}; // Mask register address for write operation

    BMP280_CS_LOW();
    HAL_Delay(1); // Short delay to ensure chip readiness

    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi2, txData, 2, HAL_MAX_DELAY);
    if(status != HAL_OK)
    {
        printf("HAL Transmit error: %d \r\n", status);
    }
    BMP280_CS_HIGH();

    return status;
}

uint8_t BMP280_SPI_Init(void)
{
	printf("BMP280 SPI Test\r\n");

	uint8_t chip_id = BMP280_SPI_ReadDeviceID(BMP280_REG_ID);
	printf("Velu BMP280 Chip ID: 0x%X\r\n", chip_id);

	if (chip_id != BMP280_CHIP_ID)
	{
		printf("BMP280 not detected\n");
		return 0;
	}

    // Set BMP280 to Normal Mode with 1x oversampling
    if (BMP280_WriteRegister(BMP280_REG_CTRL_MEAS, 0x27) != HAL_OK)
    {
        printf("Failed to configure BMP280.\r\n");
        return 0;
    }

    // Set configuration register (no filter, 1000 ms standby time)
    if (BMP280_WriteRegister(BMP280_REG_CONF_REG, 0xA0) != HAL_OK)
    {
        printf("Failed to configure BMP280.\r\n");
        return 0;
    }

    return 1;

}

uint8_t BMP280_ReadSensorCalibData(void)
{
	uint8_t calib_raw_data[24];
	uint8_t status;

	status = BMP280_ReadRegister(BMP280_REG_CALIB_DATA, calib_raw_data, 24); // read 24 bytes of data
	if ( !status)
	{
		printf("Error Reading temp sensor data \r\n");
	}

	calib_data.dig_T1 = (uint16_t)(calib_raw_data[0] | (calib_raw_data[1] << 8));
	calib_data.dig_T2 = (int16_t)(calib_raw_data[2] | (calib_raw_data[3] << 8));
	calib_data.dig_T3 = (int16_t)(calib_raw_data[4] | (calib_raw_data[5] << 8));

	// Pressure calibration
	calib_data.dig_P1 = (uint16_t)(calib_raw_data[6] | (calib_raw_data[7] << 8));
	calib_data.dig_P2 = (int16_t)(calib_raw_data[8]  | (calib_raw_data[9] << 8));
	calib_data.dig_P3 = (int16_t)(calib_raw_data[10] | (calib_raw_data[11] << 8));
	calib_data.dig_P4 = (int16_t)(calib_raw_data[12] | (calib_raw_data[13] << 8));
	calib_data.dig_P5 = (int16_t)(calib_raw_data[14] | (calib_raw_data[15] << 8));
	calib_data.dig_P6 = (int16_t)(calib_raw_data[16] | (calib_raw_data[17] << 8));
	calib_data.dig_P7 = (int16_t)(calib_raw_data[18] | (calib_raw_data[19] << 8));
	calib_data.dig_P8 = (int16_t)(calib_raw_data[20] | (calib_raw_data[21] << 8));
	calib_data.dig_P9 = (int16_t)(calib_raw_data[22] | (calib_raw_data[23] << 8));


return 1;
}

uint8_t BMP280_SPI_ReadSensorData(void)
{
	uint8_t temp_data[3];
	uint8_t press_data[3];
	uint32_t temp_raw_data, press_raw_data;

	uint8_t status = BMP280_ReadRegister(BMP280_REG_TEMP_MSB, temp_data, 3); // read 3 bytes of data
	if ( !status)
	{
		printf("Error Reading temp sensor data \r\n");
	}

	status = BMP280_ReadRegister(BMP280_REG_PRESS_MSB, press_data, 3); // read 3 bytes of data
	if ( !status)
	{
		printf("Error Reading press sensor data \r\n");
	}

//	printf(" BMP280 temp data received : 0x%X, 0x%X, 0x%X\r\n", temp_data[0],temp_data[1],temp_data[2]);
//	printf(" BMP280 Press data received : 0x%X, 0x%X, 0x%X\r\n", press_data[0],press_data[1],press_data[2]);

	// Combine the bytes for temperature (20-bit value)
	temp_raw_data = ((int32_t)temp_data[0] << 12) | ((int32_t)temp_data[1] << 4) | (temp_data[2] >> 4);

	// Combine the bytes for pressure (20-bit value)
	press_raw_data = ((int32_t)press_data[0] << 12) | ((int32_t)press_data[1] << 4) | (press_data[2] >> 4);

//	printf(" BMP280 temp data received : %ld\r\n", temp_raw_data);
//	printf(" BMP280 Pressure data received : %ld\r\n", press_raw_data);

	BMP280_ReadSensorCalibData();

	ReadAndCalibrate(temp_raw_data, press_raw_data);

return 1;
}

// Function to apply temperature calibration
float BMP280_CompensateTemperature(int32_t temp_raw_data, int32_t *t_fine) {
    int32_t var1 = ((((temp_raw_data >> 3) - ((int32_t)calib_data.dig_T1 << 1))) * ((int32_t)calib_data.dig_T2)) >> 11;
    int32_t var2 = (((((temp_raw_data >> 4) - ((int32_t)calib_data.dig_T1)) * ((temp_raw_data >> 4) - ((int32_t)calib_data.dig_T1))) >> 12) * ((int32_t)calib_data.dig_T3)) >> 14;
    *t_fine = var1 + var2;
    return (float)((*t_fine * 5 + 128) >> 8) / 100.0; // Temperature in degrees Celsius
}

// Function to apply pressure calibration
float BMP280_CompensatePressure(int32_t press_raw_data, int32_t t_fine) {
    int64_t var1 = ((int64_t)t_fine) - 128000;
    int64_t var2 = var1 * var1 * (int64_t)calib_data.dig_P6;
    var2 = var2 + ((var1 * (int64_t)calib_data.dig_P5) << 17);
    var2 = var2 + (((int64_t)calib_data.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)calib_data.dig_P3) >> 8) + ((var1 * (int64_t)calib_data.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1) * (int64_t)calib_data.dig_P1) >> 33;

    if (var1 == 0) {
        return 0.0; // Avoid division by zero
    }

    int64_t p = 1048576 - press_raw_data;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)calib_data.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)calib_data.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)calib_data.dig_P7) << 4);
    return (float)p / 25600.0; // Pressure in hPa
}


void ReadAndCalibrate(int32_t temp_raw_data, int32_t press_raw_data) {
    int32_t t_fine;
    float temperature = BMP280_CompensateTemperature(temp_raw_data, &t_fine);
    float pressure = BMP280_CompensatePressure(press_raw_data, t_fine);

    printf("Temperature: %.2f°C\r\n", temperature);
    printf("Pressure: %.2f hPa\r\n", pressure);
}
