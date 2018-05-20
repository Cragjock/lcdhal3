#ifndef MY_COLOR_H
#define MY_COLOR_H
// https://www.rapidtables.com/web/color/RGB_Color.html

#include <array>
#include <cstdint>

struct s_myColor
{
    uint8_t cRED;
    uint8_t cGREEN;
    uint8_t cBLUE;
};
//typedef s_myColor RGB;
using RGB = s_myColor;
extern const s_myColor table[];

enum my_color {BLACK = 0, WHITE, RED, BLUE, YELLOW,
                GREEN, PURPLE, NAVY, SKY_BLUE, MAGENTA,
                ORANGE, HOT_PINK, TEAL, GRAY, DARK_GRAY};

uint8_t getcolorvalue(enum my_color thecolor);
s_myColor getcolorStruct(enum my_color thecolor);

#endif // MY_COLOR_H
