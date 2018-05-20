
#include "myi2c.h"

using namespace std;


//const int myI2C_address[5]= {I2C_SLAVE_ADDR_IMU, 0x1c, 0x5c, 0x5f, 0x6f};
//static const char * devName = "/dev/i2c-1";
//static int I2C_Init(const char* devname, int sadrress);

const char * i2cdev[2] = {"/dev/ic2-0","/dev/i2c-1"};

//i2c_smbus_write_block_data(int file, __u8 command, __u8 length, __u8 *values);
int I2CBus::device_write_block(int reg_request, int rd_size, unsigned char* readbuffer)
{
    int result = i2c_smbus_write_i2c_block_data(this->ptrfile, reg_request, rd_size, readbuffer);
    if(result < 0)
        {
            perror("Failed to read from the i2c bus");
            exit(1);
        }

    return result;
}

int I2CBus::device_read_block(int reg_request, int rd_size, unsigned char* readbuffer)
{
    int result = i2c_smbus_read_i2c_block_data(this->ptrfile, reg_request, rd_size, readbuffer);
    if(result < 0)
        {
            perror("Failed to read from the i2c bus");
            exit(1);
        }

    return result;
}

I2CBus::I2CBus(unsigned int bus, unsigned int address) : Transport()
{
	this->ptrfile=-1;
	this->i2cbus = bus;
	this->i2caddress = address;
	this->i2cdev_name = i2cdev[bus];
	snprintf(busfile, sizeof(busfile), "/dev/i2c-%d", bus);
	this->openi2c();
    //_i2cbus = bus;
    //_i2caddr = address;
	//snprintf(busfile, sizeof(busfile), "/dev/i2c-%d", bus);
	//openfd();
}

void I2CBus::closei2c()
{
	::close(this->ptrfile);
	this->ptrfile = -1;
}

I2CBus::~I2CBus()
{
	if(this->ptrfile!=-1) this->closei2c();
}

int I2CBus::openi2c()
{
    /// const char * i2cdev[2] = {"/dev/ic2-0","/dev/i2c-1"};
    int bus = 1; ///force to /dev/i2c-1

    //this->file = ::open(i2cdev[bus], O_RDWR); // do i need ::open ???
    this->ptrfile = ::open(busfile, O_RDWR);
    if (this->ptrfile == -1)
        {
            perror(i2cdev[bus]);
            int errsv = errno;
            return -1;
        }

    if (ioctl(this->ptrfile, I2C_SLAVE, i2caddress) < 0)
    {
        perror("Failed to acquire bus access and/or talk to slave");
        //exit(1);
        return -1;
    }
    cout<<"ptrfile in i2copen "<<this->ptrfile<<endl;
    return this->ptrfile;

    //void I2C::openfd() {
	//if ((fd = open(busfile, O_RDWR)) < 0) {
	//	syslog(LOG_ERR, "Couldn't open I2C Bus %d [openfd():open %d]", _i2cbus, rrno);}
	//if (ioctl(fd, I2C_SLAVE, _i2caddr) < 0) {
	//	syslog(LOG_ERR, "I2C slave %d failed [openfd():ioctl %d]", _i2caddr, errno);}

}

int I2CBus:: device_read(int reg_request)
{
    int result=i2c_smbus_read_byte_data(this->ptrfile, reg_request);
    if (result < 0)
        {
            perror("Failed to read from the i2c bus");
            exit(1);
        }
    return result;
}

int I2CBus:: device_write(uint8_t reg_request, uint8_t data)
{
    int result = i2c_smbus_write_byte_data(this->ptrfile, reg_request, data);
    if (result < 0)
        {
            perror("Failed to write to the i2c bus");
            exit(1);
        }
    return result;
}

int I2CBus::device_write_swap(uint8_t command_reg, uint16_t data)
{
    /** use the byte swap header **/
    uint16_t data_swap = bswap_16(data);

    printf("write swap: data in: %x, data swapped: %x\n", data, data_swap);

    int res = i2c_smbus_write_word_data(this->ptrfile, command_reg, data_swap);
    /** S Addr Wr [A] Comm [A] DataLow [A] DataHigh [A] P **/
    if (res<0)
    {
        printf("result i2c write error");
        return -1;
    }
    return 0;
}

int16_t I2CBus::device_read_swap(uint8_t command)
{
        int16_t res = i2c_smbus_read_word_data(this->ptrfile, command);
        /** S Addr Wr [A] Comm [A] S Addr Rd [A] [DataLow] A [DataHigh] NA P **/
        if (res == -1)
        {
            printf("Read error");
            return -1;
        }

        /** use the byte swap header **/
        uint16_t res_swap = bswap_16(res);
        printf("read swap: data in: %x, data swapped: %x\n", res, res_swap);

        return res_swap;      // return the read data
}
