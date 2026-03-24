# Changelog

## [1.0.2] - 2026-03-24

### Added
- Sentinel konstanty `NO_SENSOR_TEMP = -126.0f` (senzor nenalezen) a `INVALID_TEMP = -127.0f` (chyba cteni) jako verejne cleny tridy

### Changed
- `_readRaw()`: pokud `i2c_master_transmit` selze s ESP_ERR_NOT_FOUND / ESP_ERR_TIMEOUT / ESP_ERR_INVALID_STATE, nastavi temperature a humidity na `NO_SENSOR_TEMP` (-126.0f) misto `INVALID_TEMP` (-127.0f)
- `selfTest()`: odebran nepoužívaný parametr `humBefore` — self-test porovnava pouze teplotu pred a po ohrevu
- Konzistentni s AP_DS18B20 a AP_MS5611

## [1.0.1] - 2026-02-19

### Fixed
- `read()`, `readWithHeater()`: pri chybe I2C nebo CRC jsou nyni `temperature` a `humidity` nastaveny na -127.0f (drive byly neinicializovane)

## [1.0.0] - 2026-02-16

### Added
- Mereni teploty a vlhkosti `read(temperature, humidity)` - high precision
- Mereni s heaterem `readWithHeater(cmd, temperature, humidity)` - 6 rezimu (3 intenzity x 2 doby)
- Self-test `selfTest(tempBefore, humBefore, tempDiff, minTempDiff)` - porovnani teploty pred a po ohrevu
- Soft reset `reset()`
- Cteni serioveho cisla `readSerial(serial)`
- CRC-8 validace vsech odpovedi
- Podpora SHT40, SHT41, SHT45 (spolecny protokol)
