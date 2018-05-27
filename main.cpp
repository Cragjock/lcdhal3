#include <iostream>
#include <chrono>		//chrono::milliseconds(1000); need scope chrono
#include <thread>		//for chrono sleep this_thread::sleep_for(chrono::seconds(1));
#include "mylcd.h"
#include "SPI.h"


using namespace std;

    const string sdceng = {0x53,0x74,0x65,0x76,0x65,0x6e};
    //const char sdceng[] = {0x53,0x74,0x65,0x76,0x65,0x6e};        // ok
    //char sdceng[] = {0x53,0x74,0x65,0x76,0x65,0x6e};        // ok
    //const char sdcjp[]={0xbd,0xc3,0xa8,0xb0,0xcc,0xde,0xdd};      // ok
    const string japan = {0xbd,0xc3,0xa8,0xb0,0xcc,0xde,0xdd};

    // uint8_t sdceng[] ={0x53,0x74,0x65,0x76,0x65,0x6e};       // no good
    const char alpha[] ={"123456789a123456789b123456789c123456789d123456789e123456789f123456789g123456789h"};

    char buf[80];
int main()
{

    cout<<"building from scratch "<<endl;
	//this_thread::sleep_for(chrono::seconds(1));
    //SPIBus myBus(0,0);
    //myBus.SPI_write_reg(0x80);     // ==== TESTING
    //myBus.SPI_write_reg(0x00);     // ==== TESTING
    //myBus.SPI_write_reg(0x80);     // ==== TESTING
    //myBus.~SPIBus();


	lcddisplay theLCD;
	this_thread::sleep_for(chrono::seconds(1));
    theLCD<<"Hello\n";
	theLCD.lcd_write(sdceng);
	this_thread::sleep_for(chrono::seconds(1));
    theLCD.lcd_set_cursor_address(0x54);
	theLCD.lcd_write(japan);
	this_thread::sleep_for(chrono::seconds(1));

    sprintf(buf, "sat %c%c all: %x\n",SatLeftBM, SatRightBM, 0x22);
    theLCD.lcd_set_cursor_address(0x54);
    theLCD.lcd_write(buf);
    theLCD.lcd_set_cursor_address(0x54);
    this_thread::sleep_for(chrono::seconds(1));
    theLCD<<((char)SatLeftBM);
    this_thread::sleep_for(chrono::seconds(1));


    theLCD.lcd_clear();
	theLCD<<"insertor check\n";
	theLCD<<SatLeftBM<<SatRightBM;
	this_thread::sleep_for(chrono::seconds(1));
	theLCD.lcd_clear();
	theLCD.lcd_write(alpha);
	this_thread::sleep_for(chrono::seconds(1));

    char bmp = SatLeftBM;
    char bmp1 = SatRightBM;
    char* steve = " steve";
    string clark = "clark";

    theLCD.lcd_clear();
    theLCD.display(SatLeftBM);
    theLCD.display(bmp1);
    theLCD.display(steve);
    theLCD.display(clark);
    theLCD<<bmp<<bmp1;
    theLCD<<LeftBM<<MiddleBM<<RightBM;
    this_thread::sleep_for(chrono::seconds(1));

    //RS test = data;
    //theLCD.LCD_send(test, 0x23);



    return 0;
}


