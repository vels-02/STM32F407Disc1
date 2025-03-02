/*
 * BMP280_Sensor.c
 *
 *  Created on: Mar 1, 2025
 *      Author: velu
 */


#include "BMP280_Sensor.h"
#include "stdio.h"
#include <stdint.h>


BMP280_CalibData_t calib_data;


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

void BMP280_ApplySensorCalibData(uint8_t *calib_raw_data)
{

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

}

void ProcessSensorData(uint8_t *temp_data, uint8_t *press_data)
{
	uint32_t temp_raw_data, press_raw_data;
	// Combine the bytes for temperature (20-bit value)
	temp_raw_data = ((int32_t)temp_data[0] << 12) | ((int32_t)temp_data[1] << 4) | (temp_data[2] >> 4);

	// Combine the bytes for pressure (20-bit value)
	press_raw_data = ((int32_t)press_data[0] << 12) | ((int32_t)press_data[1] << 4) | (press_data[2] >> 4);
	ReadAndCalibrate(temp_raw_data, press_raw_data);

}
