#include <span>

#include <stdint.h>

#include "color.h"
#include "pattern.h"

void Constant :: getColors(uint64_t _, std::span<Color> led_colors)
{
    for(Color& c: led_colors)
    {
        c = color;
    }
}

Walk :: Walk(Color c, int width_)
        : color(std::move(c)), width(width_), x(0)
{
}

void Walk :: getColors(uint64_t _, std::span<Color> led_colors)
{
    peak(led_colors);
    x++;
    if(x >= led_colors.size())
    {
        x = 0;
    }
}

void Walk :: peak(std::span<Color> colors)
{
    size_t half_width = width / 2;
    size_t begin = x < half_width ? 0 : x - half_width;
    size_t end = x + half_width >= colors.size() ? colors.size() - 1 :
        x + half_width;
    double k = 1.0 / double(half_width);
    for(size_t i = begin; i <= x; i++)
    {
        double coef = k * (int(i) - (x - half_width));
        colors[i].g() = uint8_t(color.g() * coef);
        colors[i].r() = uint8_t(color.r() * coef);
        colors[i].b() = uint8_t(color.b() * coef);
    }
    for(size_t i = x + 1; i <= end; i++)
    {
        double coef = 1.0 - (i - x) * k;
        colors[i].g() = uint8_t(color.g() * coef);
        colors[i].r() = uint8_t(color.r() * coef);
        colors[i].b() = uint8_t(color.b() * coef);
    }
}
