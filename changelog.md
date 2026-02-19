# Changelog

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
