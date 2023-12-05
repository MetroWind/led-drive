#include <iostream>
#include <thread>
#include <chrono>

#include <stdint.h>

#include "led.h"

void byteToLEDBytes(uint8_t x, uint8_t* output)
{
    uint32_t result = 0;
    for(int i = 0; i < 8; i++)
    {
        result = result << 3;
        if(x & 1)
        {
            result |= 0b110;
        }
        else
        {
            result |= 0b100;
        }
        x = x >> 1;
    }
    output[0] = ((result >> 16) & 0xff);
    output[1] = ((result >> 8) & 0xff);
    output[2] = (result & 0xff);
}

LEDDrive :: LEDDrive(size_t led_count_,
                     std::unique_ptr<SPIDeviceInterface> spi_,
                     std::unique_ptr<LEDPatternInterface> pattern_)
        : led_count(led_count_), spi(std::move(spi_)),
          pattern(std::move(pattern_)),
          colors(led_count_, Color::BLACK),
          padding_bytes(8),
          raw_data(led_count_ * 9 + padding_bytes, 0)
{
}

std::expected<void, std::string> LEDDrive :: step(uint64_t frame)
{
    pattern->getColors(frame, colors);
    auto result = ledDisplay();
    if(!result.has_value())
    {
        return result;
    }
    return {};
}

std::expected<void, std::string> LEDDrive :: run()
{
    uint64_t frame = 0;
    const std::chrono::microseconds frame_time(1000000 / 60);
    std::cout << "Frame time is " << frame_time << std::endl;

    while(true)
    {
        auto begin = std::chrono::high_resolution_clock::now();
        auto result = step(frame);
        if(!result.has_value())
        {
            return result;
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(frame_time - (end - begin));
        frame++;
    }

    return {};
}

std::expected<void, std::string> LEDDrive :: ledDisplay()
{
    for(size_t i = 0; i < colors.size(); i++)
    {
        byteToLEDBytes(colors[i].g(), raw_data.data() + i*9 + padding_bytes);
        byteToLEDBytes(colors[i].r(),
                       raw_data.data() + i*9 + 3 + padding_bytes);
        byteToLEDBytes(colors[i].b(),
                       raw_data.data() + i*9 + 6 + padding_bytes);
    }
    auto result = spi->write(raw_data);
    if(!result.has_value())
    {
        return result;
    }
    return {};
}
