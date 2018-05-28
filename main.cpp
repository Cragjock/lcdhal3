#include <iostream>
#include <chrono>		//chrono::milliseconds(1000); need scope chrono
#include <thread>		//for chrono sleep this_thread::sleep_for(chrono::seconds(1));
#include "mylcd.h"
#include "SPI.h"
#include <vector>
#include "pitime.h"

using namespace std;

    // NIST
    char NL = '\n';
    char EL = '\0';
    int k=0;
    char str_filenm[33];

    int (*device_open)(void);
    //char* (*alt_pitime)();
    int (*alt_pitime)(char*);
    void get_NIST(char*);
    // end NIST

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

    /// NIST
    char* NISTbuffer;
	NISTbuffer = new char[100];
    buf_pitime(NISTbuffer);

    vector<char> vecnist(100);
	strcpy(&vecnist[0], NISTbuffer);	// STD reference page 278
	vecnist.shrink_to_fit();

    string mynist(NISTbuffer);
	string line_1 = mynist.substr(7, 8);
	string line_2 = mynist.substr(16, 8);

	size_t found;
	cout << line_1 << endl;
	cout << line_2 << endl;
	found = mynist.find("UTC");
	string line_3 = mynist.substr(found, 9);
	cout << line_3 << endl;
	cout << mynist << endl;

	lcddisplay theLCD;
    this_thread::sleep_for(chrono::seconds(1));

    theLCD.lcd_clear();

 	theLCD.lcd_set_cursor_address(0x00);
	theLCD.lcd_write(line_3.c_str());
	theLCD.lcd_write("Time");
	theLCD.lcd_set_cursor_address(0x40);
	theLCD.lcd_write(line_1.c_str());
	theLCD.lcd_set_cursor_address(0x14);
	theLCD.lcd_write(line_2.c_str());
	theLCD.lcd_set_cursor_address(0x54);
    sprintf(buf, "%c====== %c =======%c\n",LeftBM, MiddleBM,RightBM);
    theLCD.lcd_write(buf);
    /// end NIST


	this_thread::sleep_for(chrono::seconds(3));
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
	theLCD<<SatLeftBM<<SatRightBM<<LeftBM<<MiddleBM<<RightBM<<HourGlassEmptyBM<<HourGlassFillingBM<<HourGlassFullBM<<"\n";
	this_thread::sleep_for(chrono::seconds(3));
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

    return 0;
}


