#include <bitset>
#include <cerrno>
#include <cstring>
#include <expected>
#include <iostream>
#include <memory>
#include <span>
#include <sstream>
#include <vector>

#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "spidevice-spidev.h"

std::string stdErrorWithMsg(std::string_view msg)
{
    std::stringstream ss;
    ss << msg << ": " << std::strerror(errno) << ".";
    return ss.str();
}

SPIDevice :: ~SPIDevice()
{
    if(fd >= 0)
    {
        close(fd);
    }
}

std::expected<std::unique_ptr<SPIDevice>, std::string>
SPIDevice :: openDevice(const char* spd_dev, uint32_t speed)
{
    if(spd_dev == nullptr)
    {
        return std::unexpected("Null device path");
    }
    std::unique_ptr<SPIDevice> spi(new SPIDevice());

    //----- SET SPI MODE -----
    //SPI_MODE_0 (0,0)      CPOL = 0, CPHA = 0, Clock idle low, data is clocked in on rising edge, output data (change) on falling edge
    //SPI_MODE_1 (0,1)      CPOL = 0, CPHA = 1, Clock idle low, data is clocked in on falling edge, output data (change) on rising edge
    //SPI_MODE_2 (1,0)      CPOL = 1, CPHA = 0, Clock idle high, data is clocked in on falling edge, output data (change) on rising edge
    //SPI_MODE_3 (1,1)      CPOL = 1, CPHA = 1, Clock idle high, data is clocked in on rising, edge output data (change) on falling edge
    spi->mode = SPI_MODE_0;
    spi->bits_per_word = 8;
    spi->speed = speed;        //1000000 = 1MHz (1uS per bit)

    spi->fd = open(spd_dev, O_RDWR);
    if (spi->fd < 0)
    {
        std::stringstream ss;
        ss << "Failed to open SPI device at " << spd_dev << ": "
           << std::strerror(errno) << ".";
        return std::unexpected(ss.str());
    }

    int status = ioctl(spi->fd, SPI_IOC_WR_MODE, &spi->mode);
    if(status < 0)
    {
        return std::unexpected(
            stdErrorWithMsg("Failed to set SPI mode (WR)"));
    }

    status = ioctl(spi->fd, SPI_IOC_RD_MODE, &spi->mode);
    if(status < 0)
    {
        return std::unexpected(
            stdErrorWithMsg("Failed to read SPI mode (RD)"));
    }
    std::cout << "SPI mode has been set to " << int(spi->mode) << std::endl;

    status = ioctl(spi->fd, SPI_IOC_WR_BITS_PER_WORD, &spi->bits_per_word);
    if(status < 0)
    {
        return std::unexpected(
            stdErrorWithMsg("Failed to set SPI bits per word (WR)"));
    }

    status = ioctl(spi->fd, SPI_IOC_RD_BITS_PER_WORD, &spi->bits_per_word);
    if(status < 0)
    {
        return std::unexpected(
            stdErrorWithMsg("Failed to set SPI bits per word (RD)"));
    }

    status = ioctl(spi->fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi->speed);
    if(status < 0)
    {
        return std::unexpected(
            stdErrorWithMsg("Failed to set SPI speed (WR)"));
    }

    status = ioctl(spi->fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi->speed);
    if(status < 0)
    {
        return std::unexpected(
            stdErrorWithMsg("Failed to set SPI speed (RD)"));
    }
    std::cout << "SPI speed has been set to " << spi->speed << std::endl;

    return std::move(spi);
}

std::expected<std::string, std::string> SPIDevice :: stateStr() const
{
    unsigned char mode;
    int status = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if(status < 0)
    {
        return std::unexpected(stdErrorWithMsg("Failed to read SPI mode"));
    }
    unsigned char bits_per_word;
    status = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits_per_word);
    if(status < 0)
    {
        return std::unexpected(
            stdErrorWithMsg("Failed to read SPI bits per word"));
    }
    uint32_t speed;
    status = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if(status < 0)
    {
        return std::unexpected(
            stdErrorWithMsg("Failed to read SPI speed"));
    }

    std::stringstream ss;
    ss << "Mode: " << int(mode) << "\nBits/word: " << int(bits_per_word)
       << "\nSpeed: " << speed << "\n";
    return ss.str();
}

std::expected<void, std::string>
SPIDevice :: write(std::span<const uint8_t> data)
{
    size_t data_size = data.size();
    // for(size_t i = 0; i < data_size; i++)
    // {
    //     std::cout << std::bitset<8>(data[i]) << " ";
    // }
    // std::cout << std::endl;

    spi_ioc_transfer spi;
    std::memset(&spi, 0, sizeof(spi_ioc_transfer));
    spi.tx_buf = uint64_t(data.data());
    spi.rx_buf = 0;         //receive into "data"
    spi.len = data_size;
    spi.delay_usecs = 0;
    spi.speed_hz = speed;
    spi.bits_per_word = bits_per_word;
    spi.cs_change = 0;      // 0=Set CS high after a transfer,
    // 1=leave CS set low
    int status = ioctl(fd, SPI_IOC_MESSAGE(1), &spi);
    if(status < 0)
    {
        return std::unexpected(
            stdErrorWithMsg("Failed to write data"));
    }
    return std::expected<void, std::string>();
}
