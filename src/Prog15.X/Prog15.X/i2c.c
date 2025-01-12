#include <p32xxxx.h>
#include "i2c.h"
#include "Timer.h"      // if you need Delayms(), etc.
#include "Uart.h"       // if you use UART4_WriteString() in debug

// Configure I2C at ~100 kHz for PIC32
void i2c_master_setup(void) 
{
    // 1) Configure SDA1 (RG3) and SCL1 (RG2) as inputs.
    //    If your board uses different pins, adjust accordingly!
    TRISGbits.TRISG2 = 1; // SCL1
    TRISGbits.TRISG3 = 1; // SDA1

    // 2) Turn off and reset I2C1
    I2C1CON = 0x0000;

    // 3) Set baud rate for 100 kHz @ PBCLK = 40 MHz
    //    I2C1BRG = [FPB/(2*FSCK) - 2], e.g. 198
    I2C1BRG = 198;

    // 4) Optional: disable slew rate for 100 kHz
    I2C1CONbits.DISSLW = 1;

    // 5) Enable I2C1 module
    I2C1CONbits.ON = 1;
}

// Start a transmission on the I2C bus
void i2c_master_start(void) 
{
    I2C1CONbits.SEN = 1; // send the start bit
    while(I2C1CONbits.SEN) { ; } // wait until done
}

// Send a repeated start
void i2c_master_restart(void) 
{
    I2C1CONbits.RSEN = 1; // send a restart
    while(I2C1CONbits.RSEN) { ; } // wait until done
}

// Send a byte to the slave
void i2c_master_send(unsigned char byte) 
{
    I2C1TRN = byte;                  // if address, bit0=0 for write, 1 for read
    while(I2C1STATbits.TRSTAT) { ; } // wait for the transmission to finish

    // Check for ACK
    if(I2C1STATbits.ACKSTAT) {
        // Slave didn't acknowledge
        // You can debug-print or set an error flag
        UART4_WriteString("I2C: NO ACK\r\n");
    }
}

// Receive a byte from the slave
// 'ack' = 0 to ACK (request more bytes), =1 to NACK (no more bytes)
unsigned char i2c_master_recv(int ack) 
{
    I2C1CONbits.RCEN = 1;             // Enable receive mode
    while(!I2C1STATbits.RBF) { ; }    // wait for data
    unsigned char data = I2C1RCV;     // read the byte

    // Send ACK or NACK
    i2c_master_ack(ack);
    return data;
}

// Send ACK (val=0) or NACK (val=1)
void i2c_master_ack(int val) 
{
    I2C1CONbits.ACKDT = val; // 0=ACK, 1=NACK
    I2C1CONbits.ACKEN = 1;   // start transmission of ACKDT
    while(I2C1CONbits.ACKEN) { ; } // wait until done
}

// Send a STOP
void i2c_master_stop(void) 
{
    I2C1CONbits.PEN = 1;     // comm is complete, relinquish bus
    while(I2C1CONbits.PEN) { ; } // wait until done
}