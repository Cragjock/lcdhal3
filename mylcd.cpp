
#include "mylcd.h"

using namespace std;



/******************************/
lcddisplay::lcddisplay()
{
	// cout << "lcddisplay In constructor" << endl;
	mySPI = new SPIBus(0,0);

    function_set = 0;
    entry_mode = 0;
    display_control = 0;
    this->lcd_init();
}

lcddisplay::lcddisplay(LCD sLCD)
{
	cout << "lcddisplay In lcd constructor" << endl;
	mySPI = new SPIBus(0,0);
	this->properties.rows=sLCD.rows;
	this->properties.column=sLCD.column;
	this->properties.is_color=sLCD.is_color;
    function_set = 0;
    entry_mode = 0;
    display_control = 0;
    this->lcd_init();
}

/******************************/
lcddisplay::~lcddisplay()
{
    lcd_close();
}

/******************************/
void lcddisplay::lcd_close()
{
    lcd_clear();
    lcd_send_command(LCD_DISPLAYCONTROL | LCD_CURSOROFF);
    lcd_backlight_off();
    mySPI->~SPIBus();
}

/******************************/
void lcddisplay::lcd_init(void)
{
    mySPI->set_latch(0x98);
    function_set = 0;
    //   int res = LCDBus->myI2Cbus->device_write(GPIO, 0x00);  // why is this here?

/// setup sequence per HD44780 spec, page 46
    this_thread::sleep_for(chrono::milliseconds(5));  //sleep_ns(DELAY_SETUP_0_NS);     // 15ms
/***** Special function case 1 ***************************************/
/*** Function set, 8 bit interface ***/
    function_set |= (LCD_8BITMODE | BKL_ON);                // TEST
    lcd_send_command8(LCD_FUNCTIONSET | function_set);      // TEST
    this_thread::sleep_for(chrono::milliseconds(1));   //sleep_ns(DELAY_SETUP_2_NS);     // 1ms
/***** Special function case 2 ***************************************/
/*** Function set, 8 bit interface ***/
    function_set |= (LCD_8BITMODE | BKL_ON);                // TEST
    lcd_send_command8(LCD_FUNCTIONSET | function_set);      // TEST
    this_thread::sleep_for(chrono::microseconds(200));  //sleep_ns(DELAY_SETUP_2_NS);     // 1ms

/***** Special function case 3 **************************************/
/*** Function set, 8 bit interface ***/
    function_set |= (LCD_8BITMODE | BKL_ON);                // TEST
    lcd_send_command8(LCD_FUNCTIONSET | function_set);      // TEST
    this_thread::sleep_for(chrono::microseconds(200));  //sleep_ns(DELAY_SETUP_2_NS);     // 1ms

/***** Initial function set for 4 bits etc **************************/
/*** Function set, 4 bit interface ***/
    function_set = LCD_4BITMODE | BKL_ON;   // TEST but had to change |= to =
    lcd_send_command8(LCD_FUNCTIONSET | function_set);      // TEST
    this_thread::sleep_for(chrono::microseconds(200));  //sleep_ns(DELAY_SETUP_2_NS);     // 1ms

/**************** Now in 4 bit mode ********************************/
/*** and now normal operation can start ***/
    function_set |= LCD_4BITMODE | LCD_2LINE | LCD_5X8DOTS;
    lcd_send_command(LCD_FUNCTIONSET | function_set); // 0x28 command
    this_thread::sleep_for(chrono::microseconds(40));   //sleep_ns(DELAY_SETTLE_NS);

    display_control |= LCD_DISPLAYOFF | LCD_CURSOROFF | LCD_BLINKOFF;
    lcd_send_command(LCD_DISPLAYCONTROL | display_control); // x08 command

    lcd_clear();


    entry_mode |= LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    lcd_send_command(LCD_ENTRYMODESET | entry_mode);
    this_thread::sleep_for(chrono::microseconds(40));   //sleep_ns(DELAY_SETTLE_NS);

    display_control |= LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKON; //x0f
    lcd_send_command(LCD_DISPLAYCONTROL | display_control);
    this_thread::sleep_for(chrono::microseconds(10));   //sleep_ns(DELAY_SETTLE_NS);

/*** Initialization done *****/

    this->lcd_write("LCD Initialization\ncomplete\n");
}

/******************************/
/// ********************* WRITE ********************
int lcddisplay::lcd_write(const string message)
{
	int size = message.size();

	lcd_write(message.c_str());

	//this->in << message << flush;
	return size;
}

int lcddisplay::lcd_write(char message)
{
    string my_message(1,message);   // stl book, page 666 and 667
    lcd_write(my_message.c_str());
    return 1;
}

int lcddisplay::lcd_write(const char * message)
{
    lcd_send_command(LCD_SETDDRAMADDR | cursor_address);

    // for each character in the message
    while (*message)
    {
        if (*message == '\n')
        {
            int newrow = address2row(cursor_address);
            //lcd_set_cursor(0, (newrow+1));   // lcd_set_cursor(0, 1);
            bool cur_test = cursor_address & 0x40;
            if(cur_test)
            {
                if(cursor_address>83)
                    newrow=0;
                else
                    newrow=2;
            }
            else
            {
                if(cursor_address>19)
                    newrow=3;
                else
                    newrow=1;
            }
            //lcd_set_cursor(ROW_OFFSETS[newrow]);
            lcd_set_cursor_address(ROW_OFFSETS[newrow]);
          }
        else
        {
            lcd_send_data(*message);
            cursor_address++;

/**< cursor position check due to main 154. it should work */
            if(cursor_address == 40)
            {
                cursor_address=84;
                lcd_send_command(LCD_SETDDRAMADDR | cursor_address);
            }
            else if(cursor_address == 20)
            {
                cursor_address=64;
                lcd_send_command(LCD_SETDDRAMADDR | cursor_address);
            }
            else if(cursor_address == 84)
            {
                cursor_address=20;
                lcd_send_command(LCD_SETDDRAMADDR | cursor_address);
            }
            else if(cursor_address == 104)
            {
                cursor_address=0;
                lcd_send_command(LCD_SETDDRAMADDR | cursor_address);
            }
/**< end cursor position check */

        }
        //printf("cursor: %d\ncol from cursor: %d\nrow from cursor: %d\n*****\n",cursor_address,address2col(cursor_address),address2row(cursor_address) );
        message++;
    }
    return cursor_address;
}
/// ********************* END WRITE *****************


void lcddisplay::set_color(struct s_myColor theColors)
{
	this->R = theColors.cRED;
	this->G = theColors.cGREEN;
	this->B = theColors.cBLUE;
	set_color(theColors.cRED, theColors.cGREEN, theColors.cBLUE);
}

void lcddisplay::set_color(enum my_color thecolor)
{
    set_color(table[thecolor].cRED, table[thecolor].cGREEN, table[thecolor].cBLUE);
}

void lcddisplay::set_color(uint8_t red, uint8_t green, uint8_t blue)
{
	string reply;
	this->R = red;
	this->G = green;
	this->B = blue;
}

/******************************/
void lcddisplay::set_bmp()
{
	/// per spec, only allowed 8 custom characters
	// vector<int>custom_one{ 0xfe, 0x4e, 0x00, 0x4, 0xe, 0xe, 0xe, 0x1f, 0x0, 0x4, 0x0 }; // caution using sine id is 0x00
	//vector<int>bmLeft = { 0xfe, 0x4e, 0x01, 8, 12, 10, 9, 10, 12, 8, 0 };
	//vector<int>bmMiddle = { 0xfe, 0x4e, 0x02, 0, 0, 31, 14, 4, 14, 31, 0 };
	//vector<int>bmRight = { 0xfe, 0x4e, 0x03, 2, 6, 10, 18, 10, 6, 2, 0 };


/***************************
	vector<int>bmSatLeft = { 0xfe, 0x4e, SatLeftBM, 0, 20, 21, 21, 31, 21, 20, 20 };
	vector<int>bmSatright = { 0xfe, 0x4e, 0x05, 0, 5, 21, 21, 31, 21, 5, 5 };
	vector<int>bmHand = { 0xfe, 0x4e, 0x06, 4,14,30,31,31,31,14,14};
	vector<int>bmCheck = { 0xfe, 0x4e, 0x07, 0,1,3,22,28,8,0,0};

	vector<int>bmHGempty = { 0xfe, 0x4e, 0x01, 31,17,10,4,10,17,31,0};
	vector<int>bmHGfilling = { 0xfe, 0x4e, 0x02, 31,17,10,4,14,31,31,0};
	vector<int>bmHGFull = { 0xfe, 0x4e, 0x03, 31,31,14,4,14,31,31,0};


    copy(bmHGempty.cbegin(), bmHGempty.cend(), ostream_iterator<char>(in));
	copy(bmHGfilling.cbegin(), bmHGfilling.cend(), ostream_iterator<char>(in));
	copy(bmHGFull.cbegin(), bmHGFull.cend(), ostream_iterator<char>(in));


	copy(bmSatLeft.cbegin(), bmSatLeft.cend(), ostream_iterator<char>(in));
	copy(bmSatright.cbegin(), bmSatright.cend(), ostream_iterator<char>(in));
	copy(bmHand.cbegin(), bmHand.cend(), ostream_iterator<char>(in));
	copy(bmCheck.cbegin(), bmCheck.cend(), ostream_iterator<char>(in));

***********************************/


/******** BITMAP SET UP ************************/
    lcd_send_command(LCD_DISPLAYCONTROL | LCD_DISPLAYOFF | LCD_CURSOROFF | LCD_BLINKOFF); // x08 command
    uint8_t bmLeft[]= {8,12,10,9,10,12,8,0};
    uint8_t bmMiddle[]={0,0,31,14,4,14,31,0};
    uint8_t bmRight[]={2,6,10,18,10,6,2,0};
    uint8_t bmSatLeft[]={0,20,21,21,31,21,20,20};
    uint8_t bmSatright[]= {0,5,21,21,31,21,5,5};
    uint8_t bmhand[]= {4,14,30,31,31,31,14,14};
    uint8_t bmCheck[] = {0,1,3,22,28,8,0,0};
    uint8_t bmXXX[] = {0,27,14,4,12,27,0,0};


    lcd_store_custom_bitmap(1, bmLeft); // store
    lcd_store_custom_bitmap(2, bmMiddle); // store
    lcd_store_custom_bitmap(3, bmRight); // store
    lcd_store_custom_bitmap(4, bmSatLeft); // store
    lcd_store_custom_bitmap(5, bmSatright); // store
    lcd_store_custom_bitmap(6, bmhand); // store
    lcd_store_custom_bitmap(7, bmCheck); // store
    lcd_store_custom_bitmap(0, bmXXX); // store

    lcd_send_command(LCD_RETURNHOME);
    lcd_send_command(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKON);
}

/******************************/
void lcddisplay::lcd_set_cursor_address(uint8_t address)
{
	if (address > 16)
		cursor_row = 2;
	else
		cursor_row = 1;

	cursor_col = (address & 0x0f) + 1;

    //printf("cur address before mod x%x\n", address);
    cursor_address = address % 103;
    //cursor_address = address % LCD_RAM_WIDTH;
    //printf("cur address after mod x%x\n", cursor_address);
    lcd_send_command(LCD_SETDDRAMADDR | cursor_address);
}

/******************************/
void lcddisplay::lcd_clear(void)
{
    lcd_send_command(LCD_CLEARDISPLAY);
    sleep_ns(DELAY_CLEAR_NS);		/* 2.6 ms  - added JW 2014/06/26 */
    lcd_set_cursor_address(0);
}

/******************************/
void lcddisplay::lcd_home(void)
{
    lcd_send_command(LCD_RETURNHOME);
    sleep_ns(DELAY_CLEAR_NS);		/* 2.6 ms  - added JW 2014/06/26 */
    cursor_address = 0;
	lcd_set_cursor_address(0);
}

/******************************/

void lcddisplay::lcd_display_on(void)
{
    display_control |= LCD_DISPLAYON;
    lcd_send_command(LCD_DISPLAYCONTROL | display_control);
}

void lcddisplay::lcd_display_off(void)
{
    display_control &= 0xff ^ LCD_DISPLAYON;
    lcd_send_command(LCD_DISPLAYCONTROL | display_control);
}

void lcddisplay::lcd_backlight_on(void)
{
    mySPI->set_pin_state(Bit_Set, PIN_BKL);
}

void lcddisplay::lcd_backlight_off(void)
{
    mySPI->set_pin_state(Bit_Clear, PIN_BKL);
}

void lcddisplay::lcd_send_command(uint8_t command)
{
    mySPI->set_pin_state(Bit_Clear, PIN_RS);
    lcd_send_byte(command);
    sleep_ns(DELAY_SETTLE_NS);
}

void lcddisplay::lcd_send_command8(uint8_t command)
{
    lcd_send_word(command);
    sleep_ns(DELAY_SETTLE_NS);
}

void lcddisplay::lcd_send_data(uint8_t data)
{
    //lcd_set_rs(1);
    mySPI->set_pin_state(Bit_Set, PIN_RS);
    lcd_send_byte(data);
    sleep_ns(DELAY_SETTLE_NS);
}

void lcddisplay::lcd_send_byte(uint8_t b)
{
    uint8_t orig_b = b;
    uint8_t flip_b = flip(b); // align to chip output
    b=flip_b;

    //uint8_t current_state = myI2C_read_data(I2C_fp, GPIO);
    uint8_t current_state = mySPI->get_latch(); //device_read(GPIO);
    current_state &= DATA_MASK; // clear the data bits

    //send high nibble (0bXXXX0000)
    uint8_t new_byte = current_state | ((b & 0x0f) << 3); //b & 0xf0) >> 1);
    //SPI_write_reg(new_byte, SPI_fd);
    //int res = myI2C_write_data(I2C_fp, GPIO, new_byte);
    int res = mySPI->device_write(new_byte);


    //printf("high nibble 0x%x\n", new_byte);
    lcd_pulse_enable();

    //send low nibble (0b0000XXXX)
    new_byte = current_state | ((b & 0xf0) >> 1);  // set nibble GOOD b & 0x0f)<<3);
    //printf("low nibble 0x%x\n", new_byte);
    //res = myI2C_write_data(I2C_fp, GPIO, new_byte);
    res = mySPI->device_write(new_byte);
    lcd_pulse_enable();

}
void lcddisplay::lcd_send_word(uint8_t mydata) /// for init only
{
    mydata = mydata & 0xef; // strip bkl
    uint8_t flip_mydata = flip(mydata); // align to chip output

    uint8_t current_state = mydata & DATA_MASK; // clear the data bits and preserve other settings
    uint8_t new_byte = current_state | ((mydata & 0x78) >> 1);
    int res = mySPI->device_write(new_byte);
    lcd_pulse_enable();
}

void lcddisplay::lcd_pulse_enable(void)
{
    mySPI->set_pin_state(Bit_Set, PIN_E);
    //LCDBus->set_pin(PIN_E);

    sleep_ns(DELAY_PULSE_NS);

    mySPI->set_pin_state(Bit_Clear, PIN_E);
    //mySPI->clear_pin(PIN_E);

    sleep_ns(DELAY_PULSE_NS);
}


void lcddisplay::lcd_set_backlight(uint8_t state)
{
    int32_t rec = mySPI->get_latch(); // device_read(GPIO);
}


uint8_t lcddisplay::colrow2address(uint8_t col, uint8_t row)
{
    return col + ROW_OFFSETS[row];
}

uint8_t lcddisplay::address2col(uint8_t address)
{
    if(address > 0x40)
    {
        address = address - 0x40;
    }
    return address % LCD_WIDTH;   // #define LCD_WIDTH 20
    //return address % ROW_OFFSETS[1];
}

uint8_t lcddisplay::address2row(uint8_t address)
{
    int MASK = 0x40;
    int row=0;
    int i =0;
    int value;

    if((address & MASK)== MASK)
    {
        row=1;
        if(address>83)
            return row=3;
    }
    else
    {   row=0;
        if(address>13)
            return row=2;
    }
    if(address > ROW_OFFSETS[3])    // greater than 0x54
    {
        return row=3;
    }
    for(i=0; i<4; i++)
    {
        value = cursor_address - ROW_OFFSETS[i];
        if((value < 19) && (value >= 0) )
        {
            row = i;
            return row;
        }
        row=i; // ??????
    }
    address > ROW_OFFSETS[1] ? 1 : 0;
    return row;
    //return address > ROW_OFFSETS[1] ? 1 : 0;
}

void lcddisplay::sleep_ns(long nanoseconds)
{
    struct timespec time0, time1;
    time0.tv_sec = 0;
    time0.tv_nsec = nanoseconds;
    nanosleep(&time0 , &time1);
    //return 0;
}



/******************************/
void lcddisplay::lcd_bitmap(int bmp_id)
{

}

/******************************/



static int max(int a, int b)
{
    return a > b ? a : b;
}

static int min(int a, int b)
{
    return a < b ? a : b;
}


uint8_t lcddisplay::flip(uint8_t data)
{
    char flip = data;  //   0b01100110; // starting data in
    char mirror=   0b00000000; // flipped data
    char mask =    0b00000001;

    mirror = ((mirror<<1) + (flip & mask));
    flip = flip >> 1;
    mirror = ((mirror<<1) + (flip & mask));
    flip = flip >> 1;
    mirror = ((mirror<<1) + (flip & mask));
    flip = flip >> 1;
    mirror = ((mirror<<1) + (flip & mask));
    flip = flip >> 1;
    mirror = ((mirror<<1) + (flip & mask));;
    flip = flip >> 1;
    mirror = ((mirror<<1) + (flip & mask));
    flip = flip >> 1;
    mirror = ((mirror<<1) + (flip & mask));
    flip = flip >> 1;
    mirror = ((mirror<<1) + (flip & mask));

    return mirror;

}

void lcddisplay::lcd_store_custom_bitmap(uint8_t location, uint8_t bitmap[])
{
    location &= 0x7; // we only have 8 locations 0-7
    lcd_send_command(LCD_SETCGRAMADDR | (location << 3));
    int i;
    for (i = 0; i < 8; i++)
    {
        lcd_send_data(bitmap[i]);
    }
}

/// ********************* WRITE ********************
// these 3 hwrite seem to be needed so the template is not confused
int lcddisplay::hlcd_write(const string message)
{
	int size = message.size();

	lcd_write(message.c_str());

	//this->in << message << flush;
	return size;
}

int lcddisplay::hlcd_write(char message)
{
    string my_message(1,message);   // stl book, page 666 and 667
    lcd_write(my_message.c_str());
    return 1;
}

int lcddisplay::hlcd_write(const char * message)
{
    lcd_send_command(LCD_SETDDRAMADDR | cursor_address);

    // for each character in the message
    while (*message)
    {
        if (*message == '\n')
        {
            int newrow = address2row(cursor_address);
            //lcd_set_cursor(0, (newrow+1));   // lcd_set_cursor(0, 1);
            bool cur_test = cursor_address & 0x40;
            if(cur_test)
            {
                if(cursor_address>83)
                    newrow=0;
                else
                    newrow=2;
            }
            else
            {
                if(cursor_address>19)
                    newrow=3;
                else
                    newrow=1;
            }
            //lcd_set_cursor(ROW_OFFSETS[newrow]);
            lcd_set_cursor_address(ROW_OFFSETS[newrow]);
          }
        else
        {
            lcd_send_data(*message);
            cursor_address++;

/**< cursor position check due to main 154. it should work */
            if(cursor_address == 40)
            {
                cursor_address=84;
                lcd_send_command(LCD_SETDDRAMADDR | cursor_address);
            }
            else if(cursor_address == 20)
            {
                cursor_address=64;
                lcd_send_command(LCD_SETDDRAMADDR | cursor_address);
            }
            else if(cursor_address == 84)
            {
                cursor_address=20;
                lcd_send_command(LCD_SETDDRAMADDR | cursor_address);
            }
            else if(cursor_address == 104)
            {
                cursor_address=0;
                lcd_send_command(LCD_SETDDRAMADDR | cursor_address);
            }
/**< end cursor position check */

        }
        //printf("cursor: %d\ncol from cursor: %d\nrow from cursor: %d\n*****\n",cursor_address,address2col(cursor_address),address2row(cursor_address) );
        message++;
    }
    return cursor_address;
}
/// ********************* END WRITE *****************



