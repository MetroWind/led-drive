#pragma once

#include <span>

#include <stdint.h>

#include "color.h"

class LEDPatternInterface
{
public:
    // Calculate the colors of the LED stripe, and write the colors
    // into led_colors.
    //
    // This function is called at every frame. Argument “frame” is the
    // frame number, which increases by one at every frame, starting
    // from zero. Argument “led_colors” is a reference to a
    // caller-managed buffer of colors. An implementation of this
    // function should calculate the correct colors of the LED stripe,
    // and write those colors to “led_colors”. The caller will never
    // change the content of “led_colors”, and therefore it has the
    // colors of the previous frame when this function is called. At
    // frame 0 “led_colors” is all-zero.
    virtual void getColors(uint64_t frame, std::span<Color> led_colors) = 0;
};

class Constant : public LEDPatternInterface
{
public:
    Constant() = delete;
    explicit Constant(Color c) : color(std::move(c)) {}
    void getColors(uint64_t _, std::span<Color> led_colors) override;
private:
    const Color color;
};

class Walk : public LEDPatternInterface
{
public:
    Walk() = delete;
    Walk(Color c, int width);
    void getColors(uint64_t _, std::span<Color> led_colors) override;
private:
    const Color color;
    const int width;
    // Location of the center of the peak
    int x;

    void peak(std::span<Color> colors);
};
