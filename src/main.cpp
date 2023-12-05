#include <chrono>
#include <expected>
#include <iostream>
#include <memory>
#include <span>
#include <string>
#include <thread>
#include <vector>

#include <stdint.h>

#include <cxxopts.hpp>

#include "color.h"
#include "led.h"
#include "pattern.h"
#include "spidevice-bcm2835.h"
#include "spidevice-spidev.h"
#include "spidevice.h"

int main(int argc, char** argv)
{
    cxxopts::Options cmd_options("shot_process", "Make fancy screenshot");
    cmd_options.add_options()
        ("f,freq", "SPI frequency",
         cxxopts::value<uint32_t>()->default_value("3000000"))
        ("o,off", "Dim all LEDs")
        ("c,count", "Number of LEDs",
         cxxopts::value<size_t>()->default_value("60"))
        ("h,help", "Print this message.");

    auto opts = cmd_options.parse(argc, argv);

    if (opts.count("help"))
    {
        std::cout << cmd_options.help() << std::endl;
        return 0;
    }
    uint32_t speed = 3000000;
    if(opts.count("freq"))
    {
        speed = opts["freq"].as<uint32_t>();
    }
    int mode = 0;
    if(opts.count("mode"))
    {
        mode = opts["mode"].as<int>();
    }

    size_t led_count = 60;
    if(opts.count("count"))
    {
        led_count = opts["count"].as<size_t>();
    }

    auto spi_result = SPIBcm2835::openDevice(speed);
    if(!spi_result.has_value())
    {
        std::cerr << spi_result.error() << std::endl;
        return 1;
    }

    if(opts.count("off"))
    {
        LEDDrive led(led_count, std::move(*spi_result),
                     std::make_unique<Constant>(Color::BLACK));
        auto result = led.step(0);
        if(!result.has_value())
        {
            std::cerr << result.error() << std::endl;
            return 1;
        }
        return 0;
    }


    LEDDrive led(60, std::move(*spi_result),
                 std::make_unique<Walk>(Color{0, 255, 255}, 8));
    auto result = led.run();
    if(!result.has_value())
    {
        std::cerr << result.error() << std::endl;
        return 1;
    }
    return 0;
}
