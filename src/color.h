// -*- mode: c++; -*-
#pragma once

#include <array>

#include <stdint.h>

struct Color
{
    std::array<uint8_t, 3> channels;
    uint8_t g() const { return channels[0]; }
    uint8_t& g() { return channels[0]; }
    uint8_t r() const { return channels[1]; }
    uint8_t& r() { return channels[1]; }
    uint8_t b() const { return channels[2]; }
    uint8_t& b() { return channels[2]; }

    static const Color BLACK;
    static const Color WHITE;
};
