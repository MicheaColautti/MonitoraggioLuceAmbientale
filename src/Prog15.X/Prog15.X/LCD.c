#include "LCD.h"
#include "Timer.h"
#include <p32xxxx.h>

void initLCD( void)
{
    ANSELE = 0x0000; //RE0:7 as digital
    TRISE = 0x00FF; // RE0:7 as digital input , or 0x0000 as out is the same
    TRISDbits.TRISD4 = 0; // RD4 as digital output ENpin
    TRISDbits.TRISD5 = 0; // RD5 as digital output RWpin
    ANSELBbits.ANSB15 = 0; //RB15 ad digital
    TRISBbits.TRISB15 = 0;  // RB15 as digital output RSpin
    
    // PMP initialization
    PMCON = 0x83BF; // Enable the PMP, long waits
    PMMODE = 0x3FF; // Master Mode 1
    PMAEN = 0x0001; // PMA0 enabled
    // wait for >30ms
    Delayms(30); // Timer configured before externally (i.e. you can use T1 for 1ms interrupt reset)
    PMADDR = LCDCMD; // command register (ADDR = 0)
    PMDATA = 0x38; // set: 8-bit interface, 2 lines, 5x7
    Delayms(1); //>48us
    PMDATA = 0x0c; // ON, no cursor, no blink
    Delayms(1); //>48us
    PMDATA = 0x01; // clear display
    Delayms(2); //>1.6ms
    PMDATA = 0x06; // increment cursor, no shift
    Delayms(2); //>1.6ms
} // initLCD


char readLCD( int addr)
{
    int dummy;
    while( PMMODEbits.BUSY){} // wait for PMP to be available
    PMADDR = addr; // select the command address
    dummy = PMDATA; // init read cycle, dummy read
    while( PMMODEbits.BUSY){} // wait for PMP to be available
    return( PMDATA); // read the status register
} // readLCD

void writeLCD( int addr, char c)
{
    Delayms(1);
    while( busyLCD()){} // wait for LCD driver (KSUU06) to be available
    while( PMMODEbits.BUSY){} // wait for PMP to be available
    PMADDR = addr;
    PMDATA = c;
} // writeLCD

void putsLCD( char *s)
{
    while( *s)
    {
        putLCD( *s++);
    }
} //putsLCD

