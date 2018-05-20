
#include "scolors.h"

using namespace std;

const array<s_myColor,15> a_table =
    {{   {0,0,0},        // black
        {255,255,255},  // white
        {255,0,0},      // red
        {0,0,255},      // blue
        {255,255,0},    // yellow
        {0,255,0},      // green
        {128,0,128},   // purple
        {0,0,128},       // navy
        {135,206,225},       // sky blue
        {255,0,255},      // magenta
        {255,165,0},      // orange
        {255,105,180},      // hot pink
        {0,128,128},            // teal
        {128,128,128},       // grey
        {10,10,10}       //dark grey
    }};


const s_myColor table[]=
    {   {0,0,0},        // black
        {255,255,255},  // white
        {255,0,0},      // red
        {0,0,255},      // blue
        {255,255,0},    // yellow
        {0,255,0},      // green
        {128,0,128},   // purple
        {0,0,128},       // navy
        {135,206,225},       // sky blue
        {255,0,255},      // magenta
        {255,165,0},      // orange
        {255,105,180},      // hot pink
        {0,128,128},            // teal
        {128,128,128},       // grey
        {10,10,10}       //dark grey
    };

uint8_t getcolorvalue(enum my_color thecolor)
{
    return table[thecolor].cRED;
}

s_myColor getcolorStruct(enum my_color thecolor)
{
    s_myColor getColor;
    getColor.cRED = a_table[thecolor].cRED;
    //std::cout<<"table 9,3 "<<*(table+7)<<std::endl;
    // std::cout<<"cRED "<<getColor.cRED<<std::endl;
    getColor.cBLUE = table[thecolor].cBLUE;
    getColor.cGREEN = table[thecolor].cGREEN;

    return getColor;
}

