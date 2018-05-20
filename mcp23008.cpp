
#include "mcp23008.h"

mcp23008::mcp23008(unsigned int bus, unsigned int address)  // :I2CBus(bus,address)
{
    mcp23008bus=0;
    //int readdata = device_read(GPIO);
    myI2Cbus = new SPIBus(bus,address);

    openmcp23008();
}

int mcp23008::openmcp23008()const
{
    const uint8_t ioconfig = SEQOP_OFF | DISSLW_OFF | HAEN_ON | ODR_OFF | INTPOL_LOW;
    int res = myI2Cbus->device_write(IOCON, ioconfig);
    res = myI2Cbus->device_write(IODIR, 0x00);
    res = myI2Cbus->device_write(IPOL, 0x00);
    // disable interrupts and no need for DEFVAL, INTCON
    res = myI2Cbus->device_write(GPINTEN, 0x00);  // disable ints

    return 1;
}

mcp23008::~mcp23008()
{
    delete myI2Cbus;

}

void mcp23008::closemcp23008()
{
   // ::close(this->ptrfile);
}

void mcp23008::set_pin_state(uint8_t state, uint8_t pin_number)
{
    GPIO_state = myI2Cbus->device_read(GPIO);

    switch (state)
    {
        case 1:
            GPIO_state = GPIO_state | (1<< pin_number);
            myI2Cbus->device_write(GPIO, GPIO_state);
		break;
        case 0:
            GPIO_state &= 0xff ^ (1<< pin_number);
            myI2Cbus->device_write(GPIO, GPIO_state);
		break;
        default:
		std::cout<<"total lost on state"<<std::endl;
		break;
	}
}
void mcp23008::set_pin(uint8_t pin_number)    /// set to 1
{
    GPIO_state = myI2Cbus->device_read(GPIO);
    GPIO_state = GPIO_state | (1<< pin_number);
    myI2Cbus->device_write(GPIO, GPIO_state);
}
void mcp23008::clear_pin(uint8_t pin_number)  /// set to 0
{
    GPIO_state = myI2Cbus->device_read(GPIO);
    GPIO_state &= 0xff ^ (1<< pin_number);
    myI2Cbus->device_write(GPIO, GPIO_state);
}
void mcp23008::toggle_pin(uint8_t pin_number) /// invert current state
{
    GPIO_state = myI2Cbus->device_read(GPIO);
}
