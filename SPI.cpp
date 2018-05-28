//#include <stdio.h>
//#include <stdint.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
//#include <sys/epoll.h>
//#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "SPI.h"

using namespace std;


/************************************************************
* Adafruit 74HC595 backpack SPI Latch port looks like:
* +---------+----+----+----+--------------------+
* | 7       | 6  | 5  | 4  | 3  | 2 | 1  | 0  |
* +---------+----+----+----+--------------------+
* | bklight | db4|db5 |db6 |db7 | E | RS | N/A|
* +---------+----+----+----+--------------------+
*    NOTE: DB4-7 are in backwards order, need to flip

// ==================================================
* Adafruit backpack I2C MCP23008 port looks like:
* +---------+-----+----+----+--------------------+
* | GP7     | GP6 |GP5 |GP4 |GP3 |GP2 |GP1 |GP0  |
* +---------+-----+----+----+--------------------+
* | bklight | db7 |db6 |db5 |db4 | E  | RS | N/A|
* +---------+-----+----+----+--------------------+
*
***********************************************************/



//static const uint8_t lsb = 0; /// ioctl( m_fd, SPI_IOC_WR_LSB_FIRST, &lsb );SPI_LSB_FIRST
static const int lsb = 0;

static const uint8_t spi_mode = 0;     // defines clock polarity edge was = 0
static const uint8_t spi_bpw = 8; // bits per word
static const uint32_t spi_speed = 30000000; // 10MHz 10000000  the 595 spec is 20mhz max
static const uint16_t spi_delay = 0;
static const char * spidev[2][2] = {
    {"/dev/spidev0.0", "/dev/spidev0.1"},
    {"/dev/spidev1.0", "/dev/spidev1.1"},
};
string myspidev ="/dev/spidev0.0";

/******************************/
/// int SPI_open(int bus, int chip_select)
SPIBus::SPIBus(unsigned int bus, unsigned int chip)
{
    this->bus = bus;
    this->chipset = chip;
    this->spibus_name = "SPIbus";

    #ifdef __sclark__        // #ifdef __linux__
	string hello = "Hello from Linux";
	in.open("/dev/spidev0.0", ios::out | ios::binary);
	if (!in)
	{
		cout << "LCDstream failed to open" << endl;
	}

    #elif _WIN32
	string hello = "Hello from Windows";
	in.open("COM4", ios::out | ios::binary);
	if (!in)
	{
		cout << "LCDstream failed to open" << endl;
	}
#else
#endif

    openspi();

}
/******************************/
int SPIBus::openspi()
{
    int fd = 0;
    int bus = this->bus;
    int chip_select = this->chipset;
    // if ((fd = open(spidev[bus][chip_select], O_RDWR)) < 0)
    if ((fd = open(myspidev.c_str(), O_RDWR)) < 0)
        {
            fprintf(stderr,"SPI_open: ERROR Could not open SPI device (%s).\n",spidev[bus][chip_select]);
            return -1;
        }
    if (ioctl(fd, SPI_IOC_WR_MODE, &spi_mode) < 0)
        {
            fprintf(stderr, "SPI_open: ERROR Could not set SPI mode.\n");
            close(fd);
            return -1;
        }
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bpw) < 0)
        {
            fprintf(stderr, "SPI_open: ERROR Could not set SPI bits per word.\n");
            close(fd);
            return -1;
        }
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed) < 0)
        {
            fprintf(stderr, "SPI_open: ERROR Could not set SPI speed.\n");
            close(fd);
            return -1;
        }

    if (ioctl(fd, SPI_IOC_WR_LSB_FIRST, &lsb) < 0)
        {
            fprintf(stderr, "SPI_open: ERROR Could not set SPI lsb.\n");
            printf("ERROR: %s\n", strerror(errno));
            close(fd);
            return -1;
        }
        /// int mycheck = lsb;


        this->fd = fd;

    return fd;
}
/******************************/
SPIBus::~SPIBus()
{
    closespi();

}
/******************************/
void SPIBus::closespi()
{
	::close(this->fd);
	this->fd = -1;
	this->in.close();
}

/******************************/
int SPIBus::device_read(int data)
{
    int thertn = SPIBus::device_write(data);
    return thertn;
}

/******************************/
///void SPI_write_reg(uint8_t data, int fd)
/// transfer order starts with MSB on R-Pi
int SPIBus::device_write(uint8_t data)
{
    uint8_t tx_buf[1] = {data};
    uint8_t rx_buf[sizeof tx_buf];

    struct spi_ioc_transfer spi;
    memset (&spi, 0, sizeof(spi));
    spi.tx_buf = (unsigned long) tx_buf;
    spi.rx_buf = (unsigned long) rx_buf;
    spi.len = sizeof tx_buf;
    spi.delay_usecs = spi_delay;
    spi.speed_hz = spi_speed;
    spi.bits_per_word = spi_bpw;

    /***  do the SPI transaction   ***/
    if ((ioctl(this->fd, SPI_IOC_MESSAGE(1), &spi) < 0))
    {
        fprintf(stderr,
                "SPI_write_reg: There was a error during the SPI "
                "transaction.\n");
    }
    this->set_latch(data);      // bit flip occurs before this device write entry
    return 0;
}

uint8_t SPIBus::set_latch(uint8_t latch)
{
    this->latched = latch;
    return this->latched;
}

/*******************************************************************/
int SPIBus::SPI_open(int bus, int chip_select)
{
    int fd;

    if ((fd = open(spidev[bus][chip_select], O_RDWR)) < 0)
        {
            fprintf(stderr,"SPI_open: ERROR Could not open SPI device (%s).\n",spidev[bus][chip_select]);
            return -1;
        }
    if (ioctl(fd, SPI_IOC_WR_MODE, &spi_mode) < 0)
        {
            fprintf(stderr, "SPI_open: ERROR Could not set SPI mode.\n");
            close(fd);
            return -1;
        }
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bpw) < 0)
        {
            fprintf(stderr, "SPI_open: ERROR Could not set SPI bits per word.\n");
            close(fd);
            return -1;
        }
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed) < 0)
        {
            fprintf(stderr, "SPI_open: ERROR Could not set SPI speed.\n");
            close(fd);
            return -1;
        }

    return fd;
}
/******************************/
/*** this will do a read and write *********************************/
void SPIBus::SPI_write_reg(uint8_t data)
{
    uint8_t tx_buf[1] = {data};
    uint8_t rx_buf[sizeof tx_buf];

    struct spi_ioc_transfer spi;
    memset (&spi, 0, sizeof(spi));
    spi.tx_buf = (unsigned long) tx_buf;
    spi.rx_buf = (unsigned long) rx_buf;
    spi.len = sizeof tx_buf;
    spi.delay_usecs = spi_delay;
    spi.speed_hz = spi_speed;
    spi.bits_per_word = spi_bpw;

    /***  do the SPI transaction   ***/
    if ((ioctl(this->fd, SPI_IOC_MESSAGE(1), &spi) < 0))
    {
        fprintf(stderr,
                "SPI_write_reg: There was a error during the SPI "
                "transaction.\n");
                // printf("ERROR: %s\n", strerror(errno));
    }
    this->set_latch(data);

}
/******************************/
/***
* data in should be a 1 SET or 0 CLEAR
***/
void SPIBus::SPI_write_bit(uint8_t state, uint8_t bit_num)
{
    uint8_t reg_data = get_latch(); // should be last latched value

    if (state)
    {
        reg_data |= 1 << bit_num; // set
    }
    else
    {
        reg_data &= 0xff ^ (1 << bit_num); // clear
    }
    return SPI_write_reg(reg_data); // do it and SPI_write will update latched data
}


/******************************/
void SPIBus::set_pin_state(uint8_t state, uint8_t bit_num)
{
    SPI_write_bit(state, bit_num);

}
/******************************/
    int SPIBus::device_write_swap(uint8_t command_reg, uint16_t data){return 0;}
    int16_t SPIBus::device_read_swap(uint8_t command){return 0;}

    int SPIBus::device_read_block(int reg_request, int rd_size, unsigned char* readbuffer){return 0;}
    int SPIBus::device_write_block(int reg_request, int rd_size, unsigned char* readbuffer){return 0;}
