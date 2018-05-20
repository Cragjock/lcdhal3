
#ifndef MY_TRANSPORT
#define MY_TRANSPORT
#include <string>
#include <iostream>

class Transport
{
    private:
        std::string transport_name;

    public:
        Transport(){std::cout<<"Transport Constr"<<std::endl;};
        Transport(std::string t_name){transport_name=t_name;};
        inline std::string get_name() const {return this->transport_name;};
        virtual ~Transport(){std::cout<<"Transport byebye"<<std::endl;};

        virtual int device_write_swap(uint8_t command_reg, uint16_t data)=0;
        virtual int16_t device_read_swap(uint8_t command)=0;

        virtual int device_read_block(int reg_request, int rd_size, unsigned char* readbuffer)=0;
        virtual int device_write_block(int reg_request, int rd_size, unsigned char* readbuffer)=0;
        virtual int device_read(int reg_address)=0;
        virtual int device_write(uint8_t command_reg, uint8_t data)=0;
};

#endif // MY_TRANSPORT

