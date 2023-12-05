#pragma once
#include <expected>
#include <memory>
#include <span>
#include <string>

#include <stdint.h>

#include <bcm2835.h>

#include "spidevice.h"

class SPIBcm2835 : public SPIDeviceInterface
{
public:
    SPIBcm2835(const SPIBcm2835&) = delete;
    SPIBcm2835(SPIBcm2835&&) = default;
    SPIBcm2835& operator=(const SPIBcm2835&) = delete;
    ~SPIBcm2835() override = default;

    static std::expected<std::unique_ptr<SPIBcm2835>, std::string>
    openDevice(uint32_t speed)
    {
        if(!bcm2835_init())
        {
            return std::unexpected("Failed to initialize BCM2835.");
        }

        if(!bcm2835_spi_begin())
        {
            return std::unexpected(
                "bcm2835_spi_begin failed. Are you running as root?");
        }

        bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
        bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
        bcm2835_spi_set_speed_hz(speed);
        bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
        bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
        std::unique_ptr<SPIBcm2835> spi(new SPIBcm2835());
        spi->speed = speed;
        return spi;
    }

    std::expected<std::string, std::string> stateStr() const override
    {
        return std::unexpected("Unimplemented");
    }

    std::expected<void, std::string>
    write(std::span<const uint8_t> data) override
    {
        bcm2835_spi_writenb(reinterpret_cast<const char*>(data.data()), data.size());
        return std::expected<void, std::string>();
    }

private:
    SPIBcm2835() = default;
    unsigned int speed;
};
