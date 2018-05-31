
#ifndef MY_LCD_CLASS_H
#define MY_LCD_CLASS_H

#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <array>
#include <vector>
#include <bitset>
#include <iterator>
#include <chrono>		//chrono::milliseconds(1000); need scope chrono
#include <thread>		// for chrono sleep this_thread::sleep_for(chrono::seconds(1));

//#include "HALlcd.h"
#include "lcdDefine.h"
#include "SPI.h"
#include "scolors.h"



static const uint8_t ROW_OFFSETS[] = {0, 0x40, 0x14, 0x54};
/// http://web.alfredstate.edu/faculty/weimandn/lcd/lcd_addressing/lcd_addressing_index.html

//typedef enum command_type {command=0x00, data=0x02} RS;
enum class command_type {command, data};
using RS = command_type;

enum class backlight_state {bkOFF, bkON};
using BLT=backlight_state;

enum class transfer_bit {x4bit,x8bit};  /// DL = data length
using xBIT = transfer_bit;

struct LCD_properties
{
    uint8_t rows;
    uint8_t column;
    bool is_color;
    xBIT DL;
};
using LCD = LCD_properties;


class lcddisplay
    {
    public:
        lcddisplay();           /**< default constructor */
        lcddisplay(LCD sLCD);
        ~lcddisplay();          /**< default destructor */
        //int lcd_open(void);     /**< lcd_init call within this */
        void lcd_clear(void);
        void lcd_home(void);

        int lcd_write(const std::string);
        int lcd_write(const char*);
        int lcd_write(char message);

        int hlcd_write(const std::string);
        int hlcd_write(const char*);
        int hlcd_write(char message);

		void lcd_bitmap(int);
		bool load_bmp_bank(uint8_t bank_number);
        void lcd_set_cursor_address(uint8_t address);
		void set_color(uint8_t, uint8_t, uint8_t);
		void set_color(struct s_myColor);
		void set_color(enum my_color);
		void set_bmp(void);
		transfer_bit get_DL() const {return properties.DL;};

        template<typename T>
		int lcd_write(T message);

        template<typename T>
        lcddisplay& operator<<(const T& data);

        template<typename T>
        bool display(T message);


        std::vector<std::vector<char>> bankX;/// for bitmaps, how do array?
        std::vector<std::array<char,8>> bank0;
        std::vector<std::array<char,8>> bank1;

    private:
        template<typename T>
        void lcd_store_custom_bitmap(uint8_t location, const T bitmap);

        //SPIBus* mySPI;
        std::unique_ptr<SPIBus> mySPI;


        uint8_t height, width, cursor_col, cursor_row;
        uint8_t cursor_address;
		uint8_t R;
		uint8_t G;
		uint8_t B;
		RGB theColors;
		LCD properties;
        uint8_t entry_mode;
        uint8_t display_control;
        uint8_t display_shift;
        uint8_t function_set;
		//char* command_param;
		//bool write_command(char command, int, const char*);
		//bool write_command(char command, int size=0, const char* buffer=0);
		bool write_command(char command);
		bool write_command(char command, int bsize, const std::vector<int> buffer);



        void lcd_close(void);
        void lcd_init(void);
        uint8_t lcd_get_cursor_address(void);
        void lcd_display_on(void);
        void lcd_display_off(void);


        //void set_eeprom_bmp();

        void lcd_write_custom_bitmap(uint8_t location);
        //void lcd_store_custom_bitmap(uint8_t location, uint8_t bitmap[]);
        void lcd_set_backlight(uint8_t state);
        void lcd_backlight_on(void);
        void lcd_backlight_off(void);

        uint8_t colrow2address(uint8_t col, uint8_t row);
        uint8_t address2col(uint8_t address);
        uint8_t address2row(uint8_t address);
        void lcd_send_command(uint8_t command);
        void lcd_send_data(uint8_t data);

        int set_hw_bit(uint8_t state, uint8_t pin_number);
        void lcd_pulse_enable(void);
        void lcd_send_byte(uint8_t byte);
        void lcd_send_byteBITSET(uint8_t b);
        void lcd_send_byteBITSET(RS cmd, uint8_t b);


        void lcd_set_rs(uint8_t state);
        void lcd_set_enable(uint8_t state);
        uint8_t flip(uint8_t data);
        unsigned char flip1(unsigned char x);
        unsigned char flip2(char a);


        void lcd_send_word(uint8_t b);
        void lcd_send_command8(uint8_t command);
        void sleep_ns(long nanoseconds);
    };

    template<typename T>
    bool lcddisplay::display(T message)
    {
        //std::cout<<message<<std::endl;
        lcd_write(message);
        return true;
    }

    template<typename T>
	int lcddisplay::lcd_write(T message)
	{
		hlcd_write(message);
		return 0;
	}


	template<typename T>
	lcddisplay& lcddisplay::operator<<(const T& data)
	{
        lcd_write(data);
	    return *this;
	}

	template<typename T>
	void lcddisplay::lcd_store_custom_bitmap(uint8_t location, const T bitmap)
	{
        //assert(location >=0);
        location &= 0x7; // we only have 8 locations, 0-7
        lcd_send_command(LCD_SETCGRAMADDR | (location << 3));
        //int i;
        for (auto i = 0; i < 8; i++)
        {
            lcd_send_data(bitmap[i]);
        }
	}





/// Some utility items
/******************************/
/// convert from string to Type T
	template <typename T>
	T fromstring (std::string s)
	{
        T result;
        std::stringstream str;
        str << s;
        str >> result;
        return result;
	}
/******************************/
/// Convert from type T to string
	template <typename T>
	std::string tostring (T d)
	{
        std::string s;
        std::stringstream str;
        str << d;
        str >> s;
        return s;
    }
    //  examples
    //  string s = tostring<double> ( 123.456 );
	//  double d = fromstring<double>("124.123");
    template <typename T>
	const char tochar (T d)
	{
        std::string s;
        std::stringstream str;
        str << d;
        str >> s;
        return s[0];
    }

#endif // MY_LCD_CLASS_H

/************************************************************
* LCD DDRAM space (hex address) and row-column
*     1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20
*    +---------+--------+--------+----------+--------------------+
* 1  |00|01|02|03|04|05|06|07|08|09|0A|0B|0C|0D|0E|0F|10|11|12|13|
*    +---------+--------+--------+----------+--------------------+
* 2  |40|41|42|43|44|45|46|47|48|49|4A|4B|4C|4D|4E|4F|50|51|52|53|
*    +---------+--------+--------+----------+--------------------+
* 3  |14|15|16|17|18|19|1A|1B|1C|1D|1E|1F|20|21|22|23|24|25|26|27|
*    +---------+--------+--------+----------+--------------------+
* 4  |54|55|56|57|58|59|5A|5B|5C|5D|5E|5F|60|61|62|63|64|65|67|68|
*    +---------+--------+--------+----------+--------------------+
*********/
// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set:
//    DL = 1; 8-bit interface data
//    N = 0; 1-line display
//    F = 0; 5x8 dot character font
// 3. Display on/off control:
//    D = 0; Display off
//    C = 0; Cursor off
//    B = 0; Blinking off
// 4. Entry mode set:
//    I/D = 1; Increment by 1
//    S = 0; No shift
