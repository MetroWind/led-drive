#pragma once

#include <memory>
#include <expected>
#include <vector>

#include "color.h"
#include "pattern.h"
#include "spidevice.h"

class LEDDrive
{
public:
    LEDDrive() = delete;
    LEDDrive(size_t led_count,
             std::unique_ptr<SPIDeviceInterface> spi,
             std::unique_ptr<LEDPatternInterface> pattern);

    std::expected<void, std::string> step(uint64_t frame);
    std::expected<void, std::string> run();

private:
    const size_t led_count;
    std::unique_ptr<SPIDeviceInterface> spi;
    std::unique_ptr<LEDPatternInterface> pattern;
    std::vector<Color> colors;
    const size_t padding_bytes;
    std::vector<uint8_t> raw_data;

    // Display “colors” on the LED stripe. Member “raw_data” is
    // changed during the process.
    std::expected<void, std::string> ledDisplay();
};
