# AP_SHT4x

SHT4x (SHT40/SHT41/SHT45) temperature and humidity sensor driver for ESP-IDF new I2C master API.

## Features

- High/medium/low precision measurement
- Heater control (6 modes: 3 intensities x 2 durations)
- Self-test with heater (validates sensor responsiveness)
- Soft reset and serial number readout
- CRC-8 validation on all responses

## Installation

### PlatformIO

Add to `platformio.ini`:

```ini
lib_deps =
    https://github.com/valachbastl/AP_SHT4x.git
```

Or with specific version:

```ini
lib_deps =
    https://github.com/valachbastl/AP_SHT4x.git#v1.0.1
```

## Usage

### Basic Reading

```cpp
#include "AP_SHT4x.h"

// i2c_bus from i2c_new_master_bus()
AP_SHT4x sht(i2c_bus, 0x44);

float temperature, humidity;
esp_err_t ret = sht.read(temperature, humidity);
if (ret == ESP_OK) {
    printf("T: %.1f °C  H: %.1f %%\n", temperature, humidity);
}
```

### Reading with Heater

```cpp
float temp, hum;
// Medium heat, 100ms pulse - also returns measurement
esp_err_t ret = sht.readWithHeater(SHT4x_CMD_HEAT_M_100MS, temp, hum);
```

Available heater commands:

| Command | Intensity | Duration |
|---------|-----------|----------|
| `SHT4x_CMD_HEAT_H_1S` | High | 1 s |
| `SHT4x_CMD_HEAT_H_100MS` | High | 100 ms |
| `SHT4x_CMD_HEAT_M_1S` | Medium | 1 s |
| `SHT4x_CMD_HEAT_M_100MS` | Medium | 100 ms |
| `SHT4x_CMD_HEAT_L_1S` | Low | 1 s |
| `SHT4x_CMD_HEAT_L_100MS` | Low | 100 ms |

### Self-Test

Compares last known temperature with measurement after heater pulse. If the temperature difference is below threshold, sensor is likely faulty.

```cpp
float tempDiff;
esp_err_t ret = sht.selfTest(lastTemp, lastHum, tempDiff);
if (ret == ESP_OK) {
    printf("Sensor OK (dT: %.1f °C)\n", tempDiff);
} else if (ret == ESP_ERR_INVALID_RESPONSE) {
    printf("Sensor FAIL (dT: %.1f °C)\n", tempDiff);
}
```

### Utility

```cpp
// Soft reset
sht.reset();

// Read serial number
uint32_t serial;
sht.readSerial(serial);
```

## API Reference

| Method | Description |
|--------|-------------|
| `AP_SHT4x(bus, address)` | Constructor (address 0x44 default for SHT4x) |
| `read(temp, hum)` | High precision measurement (~10 ms) |
| `readWithHeater(cmd, temp, hum)` | Measurement with heater pulse |
| `selfTest(tempBefore, humBefore, tempDiff, minTempDiff)` | Heater self-test (minTempDiff default 2.0 °C) |
| `reset()` | Soft reset (~1 ms) |
| `readSerial(serial)` | Read 32-bit serial number |

## Author

Petr Adámek
