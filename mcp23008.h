

#ifndef MCP23008_H
#define MCP23008_H

#include <iostream>
#include <string>
#include <stdint.h>
#include <unistd.h>
#include "myi2c.h"
#include "SPI.h"


#define IODIR	0x00
#define IPOL	0x01
#define GPINTEN	0x02
#define DEFVAL	0x03
#define INTCON	0x04
#define IOCON	0x05
#define GPPU	0x06
#define INTF	0x07
#define INTCAP	0x08
#define GPIO	0x09
#define OLAT	0x0A

// I/O config
#define SEQOP_OFF       0x20
#define SEQOP_ON        0x00
#define DISSLW_ON       0x10
#define DISSLW_OFF      0x00
#define HAEN_ON         0x08
#define HAEN_OFF        0x00
#define ODR_ON          0x04
#define ODR_OFF         0x00
#define INTPOL_HIGH     0x02
#define INTPOL_LOW      0x00
#define IODIR_IN        0xff    // all are in
#define IODIR_OUT       0x00    // all are out
#define IPOL_SAME       0x00
#define IPOL_OPPOSITE   0xff
#define GPINTEN_OFF     0x00
#define GPPU_OFF        0x00
#define UPPER_NIBBLE    0xf0
#define LOWER_NIBBLE    0x0f

#define mcp23008_i2c_address 0x20


class mcp23008
{
    protected:
        unsigned int mcp23008bus;
        unsigned int mcp23008address;
        char mcp23008busfile[64];
        std::string mcp23008dev_name;
        uint8_t GPIO_state;

    public:
        SPIBus* myI2Cbus;        /// CHECK THIS *****************
        mcp23008(unsigned int bus, unsigned int address);
        int openmcp23008()const;
        virtual void closemcp23008();
        //virtual int device_read(int reg_address);
        //virtual int device_write(uint8_t command_reg, uint8_t data);
        void set_pin_state(uint8_t state, uint8_t pin_number);
        void set_pin(uint8_t pin_number);
        void clear_pin(uint8_t pin_number);
        void toggle_pin(uint8_t pin_number);

        virtual ~mcp23008();
};

#endif // MCP23008_H
