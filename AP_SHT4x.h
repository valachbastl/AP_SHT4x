#pragma once

#include "driver/i2c_master.h"

// SHT4x measurement commands
#define SHT4x_CMD_MEASURE_HIGH   0xFD  // High precision, no heater
#define SHT4x_CMD_MEASURE_MED    0xF6  // Medium precision, no heater
#define SHT4x_CMD_MEASURE_LOW    0xE0  // Low precision, no heater

// SHT4x heater commands (ohrev + mereni soucasne)
#define SHT4x_CMD_HEAT_H_1S     0x39  // High heat, 1 s
#define SHT4x_CMD_HEAT_H_100MS  0x32  // High heat, 100 ms
#define SHT4x_CMD_HEAT_M_1S     0x2F  // Medium heat, 1 s
#define SHT4x_CMD_HEAT_M_100MS  0x24  // Medium heat, 100 ms
#define SHT4x_CMD_HEAT_L_1S     0x15  // Low heat, 1 s
#define SHT4x_CMD_HEAT_L_100MS  0x0E  // Low heat, 100 ms

// SHT4x utility commands
#define SHT4x_CMD_SERIAL         0x89  // Read serial number
#define SHT4x_CMD_RESET          0x94  // Soft reset

class AP_SHT4x
{
public:
    /**
     * @brief Konstruktor
     * @param bus I2C master bus handle (z i2c_new_master_bus)
     * @param address I2C adresa (0x44 vychozi pro SHT40/41/45)
     */
    AP_SHT4x(i2c_master_bus_handle_t bus, uint8_t address);

    /**
     * @brief Zmeri teplotu a vlhkost (high precision, ~10 ms)
     * @param temperature Vystupni teplota [°C]
     * @param humidity Vystupni relativni vlhkost [%]
     * @return ESP_OK pri uspechu
     */
    esp_err_t read(float &temperature, float &humidity);

    /**
     * @brief Zapne heater a zmeri teplotu a vlhkost
     * @param cmd Heater prikaz (SHT4x_CMD_HEAT_*)
     * @param temperature Vystupni teplota [°C]
     * @param humidity Vystupni relativni vlhkost [%]
     * @return ESP_OK pri uspechu
     */
    esp_err_t readWithHeater(uint8_t cmd, float &temperature, float &humidity);

    /**
     * @brief Self-test: porovna posledni zname hodnoty s merenim po ohrevu
     * @param tempBefore Posledni znama teplota pred ohrevem [°C]
     * @param humBefore Posledni znama vlhkost pred ohrevem [%]
     * @param tempDiff Vystupni rozdil teplot (po ohrevu - pred) [°C]
     * @param minTempDiff Minimalni ocekavany rozdil teplot [°C] (vychozi 2.0)
     * @return ESP_OK = senzor OK, ESP_ERR_INVALID_RESPONSE = senzor nereaguje na ohrev
     */
    esp_err_t selfTest(float tempBefore, float humBefore, float &tempDiff, float minTempDiff = 2.0f);

    /**
     * @brief Provede soft reset senzoru (~1 ms)
     * @return ESP_OK pri uspechu
     */
    esp_err_t reset();

    /**
     * @brief Precte seriove cislo senzoru
     * @param serial Vystupni 32bit seriove cislo
     * @return ESP_OK pri uspechu
     */
    esp_err_t readSerial(uint32_t &serial);

private:
    i2c_master_dev_handle_t _dev;

    esp_err_t _readRaw(uint8_t cmd, uint16_t delayMs, float &temperature, float &humidity);
    static uint8_t _crc8(const uint8_t *data, uint8_t len);
};
