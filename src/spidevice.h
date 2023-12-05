#pragma once
#include <expected>
#include <string>
#include <span>

#include <stdint.h>

class SPIDeviceInterface
{
public:
    SPIDeviceInterface() = default;
    virtual ~SPIDeviceInterface() = default;
    virtual std::expected<std::string, std::string> stateStr() const = 0;
    virtual std::expected<void, std::string>
        write(std::span<const uint8_t> data) = 0;
};
