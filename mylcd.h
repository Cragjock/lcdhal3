
#ifndef MY_LCD_CLASS_H
#define MY_LCD_CLASS_H

#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <iterator>
#include <chrono>		//chrono::milliseconds(1000); need scope chrono
#include <thread>		// for chrono sleep this_thread::sleep_for(chrono::seconds(1));

//#include "HALlcd.h"
#include "lcdDefine.h"
#include "SPI.h"
#include "scolors.h"

struct LCD_properties
{
    uint8_t rows;
    uint8_t column;
    bool is_color;
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

        template<typename T>
		int lcd_write(T message);

        template<typename T>
        lcddisplay& operator<<(const T& data);

        template<typename T>
        bool display(T message);

    private:
        SPIBus* mySPI;

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
        void lcd_store_custom_bitmap(uint8_t location, uint8_t bitmap[]);
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
        void lcd_set_rs(uint8_t state);
        void lcd_set_enable(uint8_t state);
        uint8_t flip(uint8_t data);
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
