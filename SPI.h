#ifndef MY_SPI_H
#define MY_SPI_H

#include <string>
#include <fstream>

//using std::string;

class SPIBus
{
    private:
        uint8_t bus;    // default = 0
        uint8_t chipset;    // default = 0
        int fd;
        char busfile[64];
        uint8_t reg_data;
        std::string spibus_name;
        std::ofstream in;
        uint8_t latched;

        void SPI_write_bit(uint8_t data, uint8_t bit_num);

    public:
        SPIBus(unsigned int bus, unsigned int address);
        virtual ~SPIBus();
        void SPI_write_reg(uint8_t data);
        inline uint8_t get_latch(void) const {return this->latched;}
        uint8_t set_latch(uint8_t latch);

        virtual int SPI_open(int bus, int chip_select);
        virtual void closespi();
        virtual int openspi();


        int device_write_swap(uint8_t command_reg, uint16_t data);
        int16_t device_read_swap(uint8_t command);

        virtual int device_read_block(int reg_request, int rd_size, unsigned char* readbuffer);
        virtual int device_write_block(int reg_request, int rd_size, unsigned char* readbuffer);

        virtual int device_read(int reg_address);
        virtual int device_write(uint8_t command_reg);

        void set_pin_state(uint8_t data, uint8_t bit_num);

};

#endif // MY_SPI_H

         /********************************
         Writes an 8 bit value to the 595 latch.
         data The data byte to be written.
         fd The file descriptor for SPI

        void SPI_write_reg(uint8_t data);

        * Writes a single bit to the register specified. Must also specify
        * which hardware address and file descriptor to use.
        * data The data to write.
        * fd The file descriptor for SPI
        *
        void SPI_write_bit(uint8_t data, uint8_t bit_num);
        **************************/
