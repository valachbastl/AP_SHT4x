#include "AP_SHT4x.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

AP_SHT4x::AP_SHT4x(i2c_master_bus_handle_t bus, uint8_t address)
{
    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = address;
    dev_cfg.scl_speed_hz = 100000;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus, &dev_cfg, &_dev));
}

esp_err_t AP_SHT4x::read(float &temperature, float &humidity)
{
    return _readRaw(SHT4x_CMD_MEASURE_HIGH, 10, temperature, humidity);
}

esp_err_t AP_SHT4x::readWithHeater(uint8_t cmd, float &temperature, float &humidity)
{
    // 1s heater commands need 1100 ms, 100ms commands need 110 ms
    uint16_t delayMs = (cmd == SHT4x_CMD_HEAT_H_1S ||
                        cmd == SHT4x_CMD_HEAT_M_1S ||
                        cmd == SHT4x_CMD_HEAT_L_1S) ? 1100 : 110;
    return _readRaw(cmd, delayMs, temperature, humidity);
}

esp_err_t AP_SHT4x::selfTest(float tempBefore, float humBefore, float &tempDiff, float minTempDiff)
{
    // Mereni s heaterem (medium heat, 100 ms)
    float tempAfter, humAfter;
    esp_err_t ret = readWithHeater(SHT4x_CMD_HEAT_M_100MS, tempAfter, humAfter);
    if (ret != ESP_OK) return ret;

    // Porovnani - teplota po ohrevu musi byt vyssi
    tempDiff = tempAfter - tempBefore;
    if (tempDiff < minTempDiff) {
        return ESP_ERR_INVALID_RESPONSE;
    }

    return ESP_OK;
}

esp_err_t AP_SHT4x::reset()
{
    uint8_t cmd = SHT4x_CMD_RESET;
    esp_err_t ret = i2c_master_transmit(_dev, &cmd, 1, 100);
    if (ret == ESP_OK) {
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    return ret;
}

esp_err_t AP_SHT4x::readSerial(uint32_t &serial)
{
    uint8_t cmd = SHT4x_CMD_SERIAL;
    esp_err_t ret = i2c_master_transmit(_dev, &cmd, 1, 100);
    if (ret != ESP_OK) return ret;

    vTaskDelay(pdMS_TO_TICKS(1));

    uint8_t data[6];
    ret = i2c_master_receive(_dev, data, 6, 100);
    if (ret != ESP_OK) return ret;

    if (_crc8(&data[0], 2) != data[2] || _crc8(&data[3], 2) != data[5]) {
        return ESP_ERR_INVALID_CRC;
    }

    serial = ((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) |
             ((uint32_t)data[3] << 8)  | (uint32_t)data[4];

    return ESP_OK;
}

esp_err_t AP_SHT4x::_readRaw(uint8_t cmd, uint16_t delayMs, float &temperature, float &humidity)
{
    esp_err_t ret = i2c_master_transmit(_dev, &cmd, 1, 100);
    if (ret != ESP_OK) return ret;

    vTaskDelay(pdMS_TO_TICKS(delayMs));

    uint8_t data[6];
    ret = i2c_master_receive(_dev, data, 6, 100);
    if (ret != ESP_OK) return ret;

    if (_crc8(&data[0], 2) != data[2] || _crc8(&data[3], 2) != data[5]) {
        return ESP_ERR_INVALID_CRC;
    }

    uint16_t raw_temp = (data[0] << 8) | data[1];
    uint16_t raw_hum  = (data[3] << 8) | data[4];

    temperature = -45.0f + 175.0f * (float)raw_temp / 65535.0f;
    humidity    =  -6.0f + 125.0f * (float)raw_hum  / 65535.0f;

    if (humidity < 0.0f)   humidity = 0.0f;
    if (humidity > 100.0f) humidity = 100.0f;

    return ESP_OK;
}

uint8_t AP_SHT4x::_crc8(const uint8_t *data, uint8_t len)
{
    uint8_t crc = 0xFF;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++) {
            crc = (crc & 0x80) ? ((crc << 1) ^ 0x31) : (crc << 1);
        }
    }
    return crc;
}
