
#include "mylcd.h"

using namespace std;


/******************************/
lcddisplay::lcddisplay()
{
	// cout << "lcddisplay In constructor" << endl;
	//mySPI = new SPIBus(0,0);
	//mySPI(new SPIBus(0,0));
	mySPI= unique_ptr<SPIBus>(new SPIBus(0,0));  //myI2C=new I2CBus(1,0x20);

    this->function_set = 0;
    this->entry_mode = 0;
    this->display_control = 0;
    properties.rows = 4;
    properties.column = 20;
    properties.DL = xBIT::x8bit;    /// default for HD8870 spec

    this->lcd_init();
    this->set_bmp();
}

lcddisplay::lcddisplay(LCD sLCD)
{
	cout << "lcddisplay In lcd constructor" << endl;
	//mySPI = new SPIBus(0,0);
	mySPI= unique_ptr<SPIBus>(new SPIBus(0,0));  //myI2C=new I2CBus(1,0x20);

	this->properties.rows=sLCD.rows;
	this->properties.column=sLCD.column;
	this->properties.is_color=sLCD.is_color;
	this->properties.DL = sLCD.DL;
    function_set = 0;
    entry_mode = 0;
    display_control = 0;
    this->lcd_init();
    this->set_bmp();
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
    //mySPI->~SPIBus();
}

/******************************/
void lcddisplay::lcd_init(void)
{
    //  mySPI->set_latch(0x98); // why is this here?
    function_set = 0;
    //   int res = LCDBus->myI2Cbus->device_write(GPIO, 0x00);  // why is this here?
    int sc_temp =0;

/// setup sequence per HD44780 spec, page 46
    this_thread::sleep_for(chrono::milliseconds(15));  //sleep_ns(DELAY_SETUP_0_NS);     // 15ms
/***** Special function case 1 ***************************************/
/*** Function set, 8 bit interface ***/
    function_set |= (LCD_8BITMODE | BKL_ON);
    sc_temp=function_set;
    //lcd_send_command8(LCD_FUNCTIONSET | function_set);
    //lcd_send_command8(0x30);
    lcd_send_byteBITSET(0x30);
    this_thread::sleep_for(chrono::milliseconds(2));   //sleep_ns(DELAY_SETUP_2_NS);     // 1ms

/***** Special function case 2 ***************************************/
/*** Function set, 8 bit interface ***/
    function_set |= (LCD_8BITMODE | BKL_ON);
    sc_temp=function_set;
     //lcd_send_command8(LCD_FUNCTIONSET | function_set);
    //lcd_send_command8(0x30);
    lcd_send_byteBITSET(0x30);
    this_thread::sleep_for(chrono::microseconds(200));  //sleep_ns(DELAY_SETUP_2_NS);     // 1ms

/***** Special function case 3 **************************************/
/*** Function set, 8 bit interface ***/
    function_set |= (LCD_8BITMODE | BKL_ON);
    sc_temp=function_set;
    //lcd_send_command8(LCD_FUNCTIONSET | function_set);
    //lcd_send_command8(0x30);
    lcd_send_byteBITSET(0x30);
    this_thread::sleep_for(chrono::microseconds(200));  //sleep_ns(DELAY_SETUP_2_NS);     // 1ms

/***** Initial function set for 4 bits etc **************************/
/*** Function set, 4 bit interface ***/
    function_set = LCD_4BITMODE | BKL_ON;   // TEST but had to change |= to =
    sc_temp=function_set;
    //lcd_send_command8(LCD_FUNCTIONSET | function_set);
    //lcd_send_command8(0xa0);
    lcd_send_byteBITSET(0x20);    /// not working here ??? the xa0 caused an error
    this_thread::sleep_for(chrono::microseconds(200));  //sleep_ns(DELAY_SETUP_2_NS);     // 1ms

/**************** Now in 4 bit mode ********************************/
/*** and now normal operation can start ***/
    properties.DL = xBIT::x4bit;
    function_set |= LCD_4BITMODE | LCD_2LINE | LCD_5X8DOTS;
    sc_temp=function_set;
    //lcd_send_command(LCD_FUNCTIONSET | function_set); // 0x28 command
    lcd_send_command(0xa8);
    this_thread::sleep_for(chrono::microseconds(40));   //sleep_ns(DELAY_SETTLE_NS);

    display_control |= LCD_DISPLAYOFF | LCD_CURSOROFF | LCD_BLINKOFF;
    sc_temp=display_control;
    //lcd_send_command(LCD_DISPLAYCONTROL | display_control); // x08 command
    lcd_send_command(0x08); // x08 command
    this_thread::sleep_for(chrono::microseconds(40));

    lcd_clear();            /// is this the problem command ???

    entry_mode |= LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    lcd_send_command(LCD_ENTRYMODESET | entry_mode);
    this_thread::sleep_for(chrono::microseconds(40));   //sleep_ns(DELAY_SETTLE_NS);

    display_control |= LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKON; //x0f
    sc_temp=display_control;
    lcd_send_command(LCD_DISPLAYCONTROL | display_control);
    this_thread::sleep_for(chrono::microseconds(10));   //sleep_ns(DELAY_SETTLE_NS);

/*** Initialization done *****/

    //lcd_send_byteBITSET(0x02);      /// test, command is clear
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
bool lcddisplay::load_bmp_bank(uint8_t bank_number)     /// command is 0xfe, 0xc0, bank_number (0-4)
{

    if(bank_number == 0)
    {
        for(int i=0; i<8; i++)
            lcd_store_custom_bitmap((i+1), bank0[i]); // store
    }
    else


    for(int i=0; i<8; i++)
    {
        lcd_store_custom_bitmap((i+1), bank1[i]); // store
    }



	return true;
}


/******************************/
void lcddisplay::set_bmp()
{
/// https://www.quinapalus.com/hd44780udg.html
/******** BITMAP SET UP ************************/
    lcd_send_command(LCD_DISPLAYCONTROL | LCD_DISPLAYOFF | LCD_CURSOROFF | LCD_BLINKOFF); // x08 command

    /// bank 0
    //uint8_t bmLeft[]= {8,12,10,9,10,12,8,0};
    //uint8_t bmMiddle[]={0,0,31,14,4,14,31,0};
    //uint8_t bmRight[]={2,6,10,18,10,6,2,0};
    //uint8_t bmSatLeft[]={0,20,21,21,31,21,20,20};
    //uint8_t bmSatright[]= {0,5,21,21,31,21,5,5};
    //uint8_t bmhand[]= {4,14,30,31,31,31,14,14};
    //uint8_t bmCheck[] = {0,1,3,22,28,8,0,0};
    //uint8_t bmXXX[] = {0,27,14,4,12,27,0,0};
    vector<char>vbmLeft= {8,12,10,9,10,12,8,0};
    vector<char>vbmMiddle={0,0,31,14,4,14,31,0};
    vector<char>vbmRight={2,6,10,18,10,6,2,0};
    vector<char>vbmSatLeft={0,20,21,21,31,21,20,20};
    vector<char>vbmSatright= {0,5,21,21,31,21,5,5};
    vector<char>vbmhand= {4,14,30,31,31,31,14,14};
    vector<char>vbmCheck = {0,1,3,22,28,8,0,0};
    vector<char>vbmXXX = {0,27,14,4,12,27,0,0};

    /// bank 0
    array<char,8>abmLeft= {8,12,10,9,10,12,8,0};
    array<char,8>abmMiddle={0,0,31,14,4,14,31,0};
    array<char,8>abmRight={2,6,10,18,10,6,2,0};
    array<char,8>abmSatLeft={0,20,21,21,31,21,20,20};
    array<char,8>abmSatright= {0,5,21,21,31,21,5,5};
    array<char,8>abmHGempty = { 31,17,10,4,10,17,31,0};
    array<char,8>abmHGfilling = { 31,17,10,4,14,31,31,0};
    array<char,8>abmHGFull = {31,31,14,4,14,31,31,0};

    /// bank 1
    array<char,8>abmToRight = {0,20,22,23,22,20,0,0};
	array<char,8>abmToLeft = {0,5,13,29,13,5,0,0};
	array<char,8>abmToUp = {0,0,4,14,31,0,31,0};
	array<char,8>abmToDown = {31,0,31,14,4,0,0,0};
    array<char,8>abmHPacOpen = {14,31,28,24,28,31,14,0};
    array<char,8>abmHPacClosed = {14,31,31,31,31,31,14,0};
    array<char,8>abmhand= {4,14,30,31,31,31,14,14};
    array<char,8>abmCheck = {0,1,3,22,28,8,0,0};

    /// bank 2
    array<char,8>abmXXX = {0,27,14,4,12,27,0,0};


    //uint8_t bmHGempty[] = { 31,17,10,4,10,17,31,0};
	//uint8_t bmHGfilling[] = { 31,17,10,4,14,31,31,0};
	//uint8_t bmHGFull[] = {31,31,14,4,14,31,31,0};
	//uint8_t bmHPacOpen[] = {14,31,28,24,28,31,14,0};
	//uint8_t bmHPacClosed[] = {14,31,31,31,31,31,14,0};
	//uint8_t bmToRight[] = {0,20,22,23,22,20,0,0};
	//uint8_t bmToLeft[] = {0,5,13,29,13,5,0,0};
	//uint8_t bmToUp[] = {0,0,4,14,31,0,31,0};
	//uint8_t bmToDown[] = {31,0,31,14,4,0,0,0};
    vector<char>vbmHGempty = { 31,17,10,4,10,17,31,0};
    vector<char>vbmHGfilling = { 31,17,10,4,14,31,31,0};
    vector<char>vbmHGFull = {31,31,14,4,14,31,31,0};
    vector<char>vbmHPacOpen = {14,31,28,24,28,31,14,0};
    vector<char>vbmHPacClosed = {14,31,31,31,31,31,14,0};
    vector<char>vbmToRight = {0,20,22,23,22,20,0,0};
	vector<char>vbmToLeft = {0,5,13,29,13,5,0,0};
	vector<char>vbmToUp = {0,0,4,14,31,0,31,0};

	/// bank n
	vector<char>vbmToDown = {31,0,31,14,4,0,0,0};


	//lcd_store_custom_bitmap(1, vbmToRight); // store
	//lcd_store_custom_bitmap(2, vbmToLeft); // stor


    //lcd_store_custom_bitmap(1, bmLeft); // store
    //lcd_store_custom_bitmap(2, bmMiddle); // store
    //lcd_store_custom_bitmap(3, vbmRight); // store
    //lcd_store_custom_bitmap(4, vbmSatLeft); // store
    //lcd_store_custom_bitmap(5, vbmSatright); // store
    //lcd_store_custom_bitmap(6, bmhand); // store
    //lcd_store_custom_bitmap(7, bmCheck); // store
    //lcd_store_custom_bitmap(0, bmXXX); // store

    //lcd_store_custom_bitmap(6, vbmHGempty); // store
    //lcd_store_custom_bitmap(7, vbmHGfilling); // store
    //lcd_store_custom_bitmap(0, vbmHGFull); // store

    //vector<char>test = {'a', 'b', 'c', 'd', 'e'};
    //std::vector<std::vector<char> > bank0;
    bankX.push_back(std::vector<char>(vbmToRight));
    bankX.push_back(std::vector<char>(vbmToLeft));
    bankX.push_back(std::vector<char>(vbmToUp));
    bankX.push_back(std::vector<char>(vbmToDown));
    bankX.push_back(std::vector<char>(vbmSatLeft));
    lcd_store_custom_bitmap(2, bankX[2]); // store
    //vector<array<char,8>> var(8);
    //array<char,8>arrTL = {0,5,13,29,13,5,0,0};
    //var[0]=arrTL;

    bank0=
    {
        {abmLeft},
        {abmMiddle},
        {abmRight},
        {abmSatLeft},
        {abmSatright},
        {abmHGempty},
        {abmHGfilling},
        {abmHGFull},
    };

    bank1=
    {
        {abmToRight},
        {abmToLeft},
        {abmToUp},
        {abmToDown},
        {abmHPacOpen},
        {abmHPacClosed},
        {abmhand},
        {abmCheck},

    };

    //vector<std::array<char,8>>::const_iterator pos;
    //for(pos = bank0.begin(); pos!=bank0.end(); ++pos)
    for(int i=0; i<8; i++)
    {
        lcd_store_custom_bitmap((i+1), bank0[i]); // store
    }


    //lcd_send_command(LCD_RETURNHOME);
    lcd_home();
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
    //lcd_send_command(LCD_RETURNHOME);    //  test
    sleep_ns(DELAY_CLEAR_NS);		/* 2.6 ms  - added JW 2014/06/26 */
    //this_thread::sleep_for(chrono::milliseconds(5));
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
    #ifdef BITSET
    //lcd_send_byteBITSET(command);       /// does this work ??????
    lcd_send_byteBITSET(RS::command, command);
    #else
    mySPI->set_pin_state(Bit_Clear, PIN_RS);
    lcd_send_byte(command);
    sleep_ns(DELAY_SETTLE_NS);
    #endif
}

void lcddisplay::lcd_send_command8(uint8_t command)
{
    lcd_send_word(command);
    sleep_ns(DELAY_SETTLE_NS);
}

void lcddisplay::lcd_send_data(uint8_t data)
{
    #ifdef BITSET
    lcd_send_byteBITSET(RS::data, data);
    #else
    mySPI->set_pin_state(Bit_Set, PIN_RS);
    lcd_send_byte(data);
    sleep_ns(DELAY_SETTLE_NS);
    #endif // BITSET

}

/******************************/
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

/******************************/
void lcddisplay::lcd_send_byteBITSET(RS cmd, uint8_t b)
{
    /// create mirror matching HW output
        uint8_t orig_b = b;
        uint8_t flip_b = flip1(b); /// use table lookup
        b=flip_b;
    /// align output byte  7   6     5     4     3     2 1  0
    /// align to HW output BKL DB4/0 DB5/1 DB6/2 DB7/3 E RS NC

        bitset<8> UPPERbits(((b & UpperNibbleMask)<<3));
        bitset<8> LOWERbits(((b & 0xf0)>>1));

    /// common settings
    /// turn E and back light on,
        UPPERbits.set(PIN_E);
        UPPERbits.set(PIN_BKL);
        LOWERbits.set(PIN_E);
        LOWERbits.set(PIN_BKL);

    if(cmd == RS::command)
    {
    /// RS = 0 command register
        UPPERbits.reset(PIN_RS);
        LOWERbits.reset(PIN_RS);
    }
    else if(cmd == RS::data)
    {
     /// RS = 1 data register
        UPPERbits.set(PIN_RS);
        LOWERbits.set(PIN_RS);
    }
        uint8_t wtf = static_cast<int>(UPPERbits.to_ulong());   /// E set for pulse
        UPPERbits.reset(PIN_E);
        uint8_t wtf1 = static_cast<int>(UPPERbits.to_ulong()); /// E reset for pulse

    ///send high nibble (0bXXXX0000), BKL DB4/0 DB5/1 DB6/2
        //int res = mySPI->device_write(wtf1);
        //this_thread::sleep_for(chrono::microseconds(1));
        int res = mySPI->device_write(wtf);
        this_thread::sleep_for(chrono::nanoseconds(100)); ///per spec pg 52/58 E pulse width s/b 230ns min
        res = mySPI->device_write(wtf1);
        this_thread::sleep_for(chrono::nanoseconds(100));

    /// DL=4 bit mode or 8?
    /// If in 8bit, done & exit, no need for sending lower nibble
        if(properties.DL == xBIT::x8bit)
        {
            return;
        }
        else
        {
        wtf = static_cast<int>(LOWERbits.to_ulong());   /// E set for pulse
        LOWERbits.reset(PIN_E);
        wtf1 = static_cast<int>(LOWERbits.to_ulong()); /// E reset for pulse

        ///send low nibble (0b0000XXXX), DB7/3 E RS NC
        //res = mySPI->device_write(wtf1);
        //this_thread::sleep_for(chrono::microseconds(1));
        res = mySPI->device_write(wtf);
        this_thread::sleep_for(chrono::nanoseconds(100));
        res = mySPI->device_write(wtf1);
        this_thread::sleep_for(chrono::nanoseconds(100));
        return;
        }
}


/******************************/
void lcddisplay::lcd_send_byteBITSET(uint8_t b)
{
    /// http://www.learncpp.com/cpp-tutorial/3-8a-bit-flags-and-bit-masks/
    /// this works for RS = 0 only, at the moment
    /// create mirror matching HW output
    uint8_t orig_b = b;
    uint8_t flip_b = flip(b);
    b=flip_b;
    /// align output byte  7   6     5     4     3     2 1  0
    /// align to HW output BKL DB4/0 DB5/1 DB6/2 DB7/3 E RS NC

    bitset<8> UPPERbits(((b & UpperNibbleMask)<<3));
    bitset<8> LOWERbits(((b & 0xf0)>>1));

    /// turn E and back light on
    UPPERbits.set(PIN_E);
    UPPERbits.set(PIN_BKL);
    LOWERbits.set(PIN_E);
    LOWERbits.set(PIN_BKL);

    uint8_t wtf = static_cast<int>(UPPERbits.to_ulong());   /// E set
    UPPERbits.reset(PIN_E);
    uint8_t wtf1 = static_cast<int>(UPPERbits.to_ulong()); /// E reset

    ///send high nibble (0bXXXX0000), BKL DB4/0 DB5/1 DB6/2
    //int res = mySPI->device_write(wtf1);
    //this_thread::sleep_for(chrono::microseconds(1));
    int res = mySPI->device_write(wtf);
    this_thread::sleep_for(chrono::microseconds(1));
    res = mySPI->device_write(wtf1);
    this_thread::sleep_for(chrono::microseconds(1));

    /// Am I in DL=4 bit mode or 8?
    /// If in 8bit, done & exit, no need for sending lower nibble
    if(properties.DL == xBIT::x8bit)
    {
        return;
    }
    else
    {
        wtf = static_cast<int>(LOWERbits.to_ulong());   /// E set
        LOWERbits.reset(PIN_E);
        wtf1 = static_cast<int>(LOWERbits.to_ulong()); /// E reset

        ///send low nibble (0b0000XXXX), DB7/3 E RS NC
        //res = mySPI->device_write(wtf1);
        //this_thread::sleep_for(chrono::microseconds(1));
        res = mySPI->device_write(wtf);
        this_thread::sleep_for(chrono::microseconds(1));
        res = mySPI->device_write(wtf1);
        this_thread::sleep_for(chrono::microseconds(1));

    }

}

/// THIS HAD MANY ERRORS IN IT
void lcddisplay::lcd_send_word(uint8_t mydata) /// for init only LCD only needs one data transfer in 8 bit mode
{
    mydata = mydata & 0x7f;// 0xef; // strip bkl
    uint8_t flip_mydata = flip1(mydata); // align to chip output

    uint8_t current_state = mydata & DATA_MASK; // clear the data bits and preserve other settings

    //uint8_t new_byte = current_state | ((mydata & 0x78) >> 1);
    uint8_t new_byte = current_state | (flip_mydata << 3);

    int res = mySPI->device_write(new_byte | 0x80   );
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

/******************************/
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
/*************************** has been TEMPALTIZED
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
******************************************/

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
unsigned char lcddisplay::flip1(unsigned char x)
{
    static const unsigned char table[] = {
        0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
        0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
        0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
        0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
        0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
        0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
        0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
        0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
        0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
        0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
        0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
        0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
        0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
        0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
        0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
        0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
        0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
        0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
        0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
        0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
        0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
        0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
        0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
        0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
        0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
        0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
        0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
        0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
        0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
        0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
        0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
        0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
    };


    const vector<char> test_table = {
        0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
        0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
        0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
        0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
        0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
        0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
        0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
        0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
        0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
        0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
        0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
        0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
        0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
        0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
        0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
        0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
        0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
        0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
        0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
        0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
        0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
        0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
        0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
        0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
        0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
        0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
        0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
        0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
        0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
        0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
        0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
        0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
    };
    return test_table[x];
    //return table[x];
}

unsigned char lcddisplay::flip2(char a)
{
  return ((a & 0x1)  << 7) | ((a & 0x2)  << 5) |
         ((a & 0x4)  << 3) | ((a & 0x8)  << 1) |
         ((a & 0x10) >> 1) | ((a & 0x20) >> 3) |
         ((a & 0x40) >> 5) | ((a & 0x80) >> 7);
}

/******************************************
void lcddisplay::lcd_send_byteBITSET(RS cmd, uint8_t b)
{
    if(cmd == RS::command)
    {
        //cout<<"OMG command "<<b<<endl;
    /// create mirror matching HW output
        uint8_t orig_b = b;
        uint8_t flip_b = flip(b);
        b=flip_b;
    /// align output byte  7   6     5     4     3     2 1  0
    /// align to HW output BKL DB4/0 DB5/1 DB6/2 DB7/3 E RS NC

        bitset<8> UPPERbits(((b & UpperNibbleMask)<<3));
        bitset<8> LOWERbits(((b & 0xf0)>>1));

    /// turn E and back light on, RS = 0 command register
        UPPERbits.reset(PIN_RS);
        UPPERbits.set(PIN_E);
        UPPERbits.set(PIN_BKL);
        LOWERbits.reset(PIN_RS);
        LOWERbits.set(PIN_E);
        LOWERbits.set(PIN_BKL);

        uint8_t wtf = static_cast<int>(UPPERbits.to_ulong());   /// E set
        UPPERbits.reset(PIN_E);
        uint8_t wtf1 = static_cast<int>(UPPERbits.to_ulong()); /// E reset

    ///send high nibble (0bXXXX0000), BKL DB4/0 DB5/1 DB6/2
        //int res = mySPI->device_write(wtf1);
        //this_thread::sleep_for(chrono::microseconds(1));
        int res = mySPI->device_write(wtf);
        this_thread::sleep_for(chrono::microseconds(1));
        res = mySPI->device_write(wtf1);
        this_thread::sleep_for(chrono::microseconds(1));

    /// DL=4 bit mode or 8?
    /// If in 8bit, done & exit, no need for sending lower nibble
        if(properties.DL == xBIT::x8bit)
        {
            return;
        }
        else
        {
        wtf = static_cast<int>(LOWERbits.to_ulong());   /// E set
        LOWERbits.reset(PIN_E);
        wtf1 = static_cast<int>(LOWERbits.to_ulong()); /// E reset

        ///send low nibble (0b0000XXXX), DB7/3 E RS NC
        //res = mySPI->device_write(wtf1);
        //this_thread::sleep_for(chrono::microseconds(1));
        res = mySPI->device_write(wtf);
        this_thread::sleep_for(chrono::microseconds(1));
        res = mySPI->device_write(wtf1);
        this_thread::sleep_for(chrono::microseconds(1));
        return;
        }
    }

    if(cmd == RS::data)
    {
        //cout<<"OMG data "<<b<<endl;
        /// create mirror matching HW output
        uint8_t orig_b = b;
        uint8_t flip_b = flip(b);
        b=flip_b;
    /// align output byte  7   6     5     4     3     2 1  0
    /// align to HW output BKL DB4/0 DB5/1 DB6/2 DB7/3 E RS NC

        bitset<8> UPPERbits(((b & UpperNibbleMask)<<3));
        bitset<8> LOWERbits(((b & 0xf0)>>1));

    /// turn E and back light on, RS = 1 data register
        UPPERbits.set(PIN_RS);
        UPPERbits.set(PIN_E);
        UPPERbits.set(PIN_BKL);
        LOWERbits.set(PIN_RS);
        LOWERbits.set(PIN_E);
        LOWERbits.set(PIN_BKL);

        uint8_t wtf = static_cast<int>(UPPERbits.to_ulong());   /// E set
        UPPERbits.reset(PIN_E);
        uint8_t wtf1 = static_cast<int>(UPPERbits.to_ulong()); /// E reset

    ///send high nibble (0bXXXX0000), BKL DB4/0 DB5/1 DB6/2
        //int res = mySPI->device_write(wtf1);
        //this_thread::sleep_for(chrono::microseconds(1));
        int res = mySPI->device_write(wtf);
        this_thread::sleep_for(chrono::microseconds(1));
        res = mySPI->device_write(wtf1);
        this_thread::sleep_for(chrono::microseconds(1));

    /// DL=4 bit mode or 8?
    /// If in 8bit, done & exit, no need for sending lower nibble
        if(properties.DL == xBIT::x8bit)
        {
            return;
        }
        else
        {
        wtf = static_cast<int>(LOWERbits.to_ulong());   /// E set
        LOWERbits.reset(PIN_E);
        wtf1 = static_cast<int>(LOWERbits.to_ulong()); /// E reset

        ///send low nibble (0b0000XXXX), DB7/3 E RS NC
        //res = mySPI->device_write(wtf1);
        //this_thread::sleep_for(chrono::microseconds(1));
        res = mySPI->device_write(wtf);
        this_thread::sleep_for(chrono::microseconds(1));
        res = mySPI->device_write(wtf1);
        this_thread::sleep_for(chrono::microseconds(1));
        return;
        }
    }
}
***********************/
