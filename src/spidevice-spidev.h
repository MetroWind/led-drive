#pragma once
#include <memory>
#include <span>
#include <expected>
#include <string>

#include <stdint.h>

#include "spidevice.h"

class SPIDevice : public SPIDeviceInterface
{
public:
    SPIDevice(const SPIDevice&) = delete;
    SPIDevice(SPIDevice&&) = default;
    SPIDevice& operator=(const SPIDevice&) = delete;
    ~SPIDevice() override;

    static std::expected<std::unique_ptr<SPIDevice>, std::string>
    openDevice(const char* spd_dev, uint32_t speed);

    std::expected<std::string, std::string> stateStr() const override;

    std::expected<void, std::string>
    write(std::span<const uint8_t> data) override;

private:
    SPIDevice() = default;
    int fd = -1;
    unsigned char mode;
    unsigned char bits_per_word;
    unsigned int speed;
    std::vector<uint8_t> buffer;
    size_t last_write_size = 0;
};
