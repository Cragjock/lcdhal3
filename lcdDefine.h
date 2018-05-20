
#ifndef MY_DEFINE_H
#define MY_DEFINE_H

/******** text commands */
#define LCD_AUTOSCROLL_ON           0x51
#define LCD_AUTOSCROLL_OFF          0x52
#define LCD_CLEAR                   0x58
#define LCD_CHANGESPLASH            0x40           /// COL * ROW chars!
#define LCD_AUTOWRAPLINE_ON         0x43
#define LCD_AUTOWRAPLINE_OFF        0x44
#define LCD_SETCURSOR_POSITION      0x47     /// 2 args: col, row
#define LCD_HOME                    0x48
#define LCD_MOVECURSOR_BACK         0x4C
#define LCD_MOVECURSOR_FORWARD      0x4D
#define LCD_UNDERLINECURSOR_ON      0x4A
#define LCD_UNDERLINECURSOR_OFF     0x4B
#define LCD_BLOCKCURSOR_ON          0x53
#define LCD_BLOCKCURSOR_OFF         0x54

/****** special chars */
#define LCD_CUSTOM_CHARACTER 0x4E       /// 9 args: char #, 8 bytes data
#define LCD_SAVECUSTOMCHARBANK 0xC1     /// 9 args: char #, 8 bytes data
#define LCD_LOADCUSTOMCHARBANK 0xC0     /// 9 args: char #, 8 bytes data

/***** Numbers & Bargraphs */
// not supported until we know what these look like
//#define LCD_PLACEMEDIUMDIGIT 0x6F

/***** display func */
#define LCD_DISPLAY_ON              0x42            /// backlight. 1 argument afterwards, in minutes
#define LCD_DISPLAY_OFF             0x46
#define LCD_SET_BRIGHTNESS          0x99         /// 1 arg: scale
#define LCD_SETSAVE_BRIGHTNESS      0x98     /// 1 arg: scale
#define LCD_SET_CONTRAST            0x50           /// 1 arg
#define LCD_SETSAVE_CONTRAST        0x91       /// 1 arg

/***** GPO commands */
#define LCD_GPO_OFF                 0x56
#define LCD_GPO_ON                  0x57
#define LCD_GPO_START_ONOFF         0xC3

/***** Extra Adafruit commands */
#define LCD_RGBBACKLIGHT            0xD0      /// 3 args - R G B
#define LCD_SETSIZE                 0xD1           /// 2 args - Cols & Rows
#define LCD_TESTBAUD                0xD2          /// zero args, prints baud rate to uart


#define LCD_COMMAND                 0xFE
//#define LCD_END_COMMAND 0x9A

// storage of the current backlight state
#define COLS_ADDR 0
#define ROWS_ADDR 1
#define SPLASH_ADDR 2
#define BACKLIGHT_R_ADDR 100
#define BACKLIGHT_G_ADDR 101
#define BACKLIGHT_B_ADDR 102
#define BACKLIGHT_STATE_ADDR 103
#define BACKLIGHT_BRIGHTNESS_ADDR 104
#define AUTOSCROLL_ADDR 105
#define BAUDRATE_ADDR 106  // and 107, 108
#define CONTRAST_ADDR 109
#define CUSTOMCHARBANKS_ADDR 110
// 110 thru 430 !
#define GPO_1_START_ONOFF_ADDR 431
#define GPO_2_START_ONOFF_ADDR 432
#define GPO_3_START_ONOFF_ADDR 434
#define GPO_4_START_ONOFF_ADDR 435
#define LAST_ADDR 436
#define EEPROM_END 511

#define ROWS 2
#define COLS 16


/*************** bitmap items ********/
#define XXXBM 0x08
#define LeftBM 0x01
#define MiddleBM 0x02
#define RightBM 0x03
#define SatLeftBM 0x04
#define SatRightBM 0x05
#define HandBM 0x06
#define CheckBM 0x07
#define HourGlassEmptyBM 0x01
#define HourGlassFillingBM 0x02
#define HourGlassFullBM 0x03


#define full 0x01
#define half_fill_upper 0x02
#define half_fill_lower 0x03
#define full_L_upper 0x04
//#define full_L_lower 0x04
#define half_L_lower 0x05
#define half_L_upper 0x06
//#define full_R_upper
#define half_R_lower 0x07
#define half_R_upper 0x08


/// merging for I2C HAL
#define DELAY_PULSE_NS 1000 // 1us
#define DELAY_SETTLE_NS 40000 // 40us
#define DELAY_CLEAR_NS 2600000L // 2.6ms
#define DELAY_SETUP_0_NS 15000000L // 15ms
#define DELAY_SETUP_1_NS 5000000L // 5ms
#define DELAY_SETUP_2_NS 1000000L // 1ms

// Adafruit backpack I2C
#ifdef MCP23008
#define PIN_RW 0        // output 0, no connection on 595, grounded on LCD input
#define PIN_RS 1        // output 1
#define PIN_E 2         // output 2
#define PIN_D4 3        // output 6
#define PIN_D5 4        // output 5
#define PIN_D6 5        // output 4
#define PIN_D7 6        // output 3
#define PIN_BKL 7       // output 7

#define DATA_MASK 0x87
#endif // MCP23008

#ifdef ADA595           // Adafruit backpack SPI
#define PIN_RW 0        // output 0, no connection on 595, grounded on LCD input
#define PIN_RS 1        // output 1
#define PIN_E 2         // output 2
#define PIN_D7 3        // output 3
#define PIN_D6 4        // output 4
#define PIN_D5 5        // output 5
#define PIN_D4 6        // output 6
#define PIN_BKL 7       // output 7
#define DATA_MASK 0x87
#endif // ADA595

#ifdef PFC8574T
#define PIN_RW 1        // output 0, no connection on 595, grounded on LCD input
#define PIN_RS 0        // output 1
#define PIN_E 2        // output 2
#define PIN_D7 7        // output 3
#define PIN_D6 6        // output 4
#define PIN_D5 5        // output 5
#define PIN_D4 4        // output 6
#define PIN_BKL 7       // output 7
#define DATA_MASK 0x0f
#endif // PFC8574T

#ifdef PICADMCP23S17
#define PIN_RW 0        // output 0, no connection on 595, grounded on LCD input
#define PIN_RS 1        // output 1
#define PIN_E 2         // output 2
#define PIN_D7 3        // output 3
#define PIN_D6 4        // output 4
#define PIN_D5 5        // output 5
#define PIN_D4 6        // output 6
#define PIN_BKL 7       // output 7
#define DATA_MASK 0xf0
#endif // PICADMCP23S17


#define Bit_Set 1
#define Bit_Clear 0

#define RS_on 0x04
#define RS_off 0x00
#define E_on 0x08
#define E_off 0x00

// LCD commands
#define LCD_CLEARDISPLAY 0x01       // RS = 0
#define LCD_RETURNHOME 0x02         // RS = 0
#define LCD_ENTRYMODESET 0x04       // RS = 0
#define LCD_DISPLAYCONTROL 0x08     // RS = 0
#define LCD_CURSORSHIFT 0x10        // RS = 0
#define LCD_FUNCTIONSET 0x20        // RS = 0
#define LCD_SETCGRAMADDR 0x40       // RS = 0
#define LCD_SETDDRAMADDR 0x80       // RS = 0

//#define LCD_WRITECGDDRAM            // RS = 1, no specific command data
#define LCD_NEWLINE 0xC0
#define LCD_SPACE 0x20

// entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// display control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// not a specific command
#define BKL_ON 0x80
#define BKL_OFF 0x00

// display shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5X10DOTS 0x04
#define LCD_5X8DOTS 0x00

//#define LCD_MAX_LINES 2
//#define LCD_WIDTH 16


#define LCD_MAX_LINES 4
#define LCD_WIDTH 20
#define LCD_RAM_WIDTH 80 // RAM is 80 wide

static const uint8_t ROW_OFFSETS[] = {0, 0x40, 0x14, 0x54};

/// from I2C
#define swab16 __swab16 // http://lxr.free-electrons.com/source/include/linux/swab.h#L6
#define LBYTE(LB) (LB & 0x0FF)
#define HBYTE(HB) ((HB & 0xF00)>>8)


/// I2C slave address
#define I2C_SLAVE_ADDR_RTC      0x6f        /// TBD
#define I2C_SLAVE_ADDR_IMU      0x6b        /// Pololu IMU
#define I2C_SLAVE_ADDR_MAG      0x1e        /// Pololu mag
#define I2C_SLAVE_ADDR_RHTEMP   0x5f        /// MEMS HTS221
#define I2C_SLAVE_ADDR_PRESSURE 0x5c        /// MEMS LP25H
#define I2C_SLAVE_ADDR_PCA9685  0x40
#define I2C_SLAVE_ADDR_ADS1015  0x48
#define I2C_SLAVE_ADDR_MCP23008 0x20

#define READ_BUF_G         0x00
#define READ_BUF_XL        0x08



#endif // MY_DEFINE_H
