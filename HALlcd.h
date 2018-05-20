
#ifndef MY_halLCD_CLASS_H
#define MY_halLCD_CLASS_H

#include <iostream>
//#include <string>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <iterator>
#include <chrono>		//chrono::milliseconds(1000); need scope chrono
#include <thread>		// for chrono sleep this_thread::sleep_for(chrono::seconds(1));

#include <fcntl.h>  /// for set_term
#include <unistd.h> /// for set_term and sleep


#include "mcp23008.h"
#include "lcdDefine.h"
#include "scolors.h"
#include "SPI.h"

namespace HAL
{

class halLCD
    {
    public:
        halLCD();           /**< default constructor */
        ~halLCD();          /**< default destructor */
        int lcd_open(void);     /**< lcd_init call within this */
        void halLCD_clear(void);
        void halLCD_home(void);

        int halLCD_write(const std::string);
        int halLCD_write(const char * message);
        int halLCD_write(char message);

		void halLCD_bitmap(int);
		bool hal_load_bmp_bank(uint8_t bank_number);
        void halLCD_set_cursor_address(uint8_t address);
		void hal_set_color(uint8_t, uint8_t, uint8_t);
		void hal_set_color(struct s_myColor);
		void hal_set_color(enum my_color);

		void halLCD_usb_splash(std::string mystring);

        template<typename T>
        halLCD& operator<<(const T& data);

    private:
        //int set_term();
        std::ofstream in;
        uint8_t height, width, cursor_col, cursor_row;
        uint8_t cursor_address;
		uint8_t R;
		uint8_t G;
		uint8_t B;
        uint8_t entry_mode;
        uint8_t display_control;
        uint8_t display_shift;
        uint8_t function_set;
        //mcp23008* LCDBus;

    public:

		//char* command_param;
		//bool write_command(char command, int, const char*);
		//bool write_command(char command, int size=0, const char* buffer=0);
		bool hal_write_command(char command);
		bool hal_write_command(char command, int bsize, const std::vector<int> buffer);

        void lcd_close(void);
        void lcd_init(void);
        uint8_t halLCD_get_cursor_address(void);
        void halLCD_display_on(void);
        void halLCD_display_off(void);
        //void lcd_blink_on(void);
        //void lcd_blink_off(void);
        //void lcd_cursor_on(void);
        //void lcd_cursor_off(void);
        void halLCD_backlight_on(void);
        void halLCD_backlight_off(void);
        //void lcd_move_left(void);
        //void lcd_move_right(void);
        //void lcd_left_to_right(void);
        //void lcd_right_to_left(void);
        //void lcd_autoscroll_on(void);
        //void lcd_autoscroll_off(void);
        void hal_set_bmp(void);
        void hal_set_eeprom_bmp();

        void halLCD_write_custom_bitmap(uint8_t location);
        void halLCD_store_custom_bitmap(uint8_t location, uint8_t bitmap[]);
        void halLCD_set_backlight(uint8_t state);

        uint8_t colrow2address(uint8_t col, uint8_t row);
        uint8_t address2col(uint8_t address);
        uint8_t address2row(uint8_t address);

        void halLCD_send_command(uint8_t command);
        void halLCD_send_data(uint8_t data);

        int set_hw_bit(uint8_t state, uint8_t pin_number);
        void lcd_pulse_enable(void);
        void lcd_send_byte(uint8_t byte);
        void lcd_set_rs(uint8_t state);
        void lcd_set_enable(uint8_t state);
        uint8_t flip(uint8_t data);
        void lcd_send_word(uint8_t b);
        void halLCD_send_command8(uint8_t command);
        void sleep_ns(long nanoseconds);
    };

 //   template<typename T>
//	int halLCD::halLCD_write(T message)
//	{
//		this->halLCD_write(message);
		//this->in << message << std::flush;
//		return 0;
//	}
	template<typename T>
	halLCD& halLCD::operator<<(const T& data)
	{
        //this->lcd_write(data);
        this->in << data << std::flush;

	    return *this;
	}

}       // end namespace HAL

#endif // MY_USB_LCD_CLASS_H
