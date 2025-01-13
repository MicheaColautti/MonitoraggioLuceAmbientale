/*
 * Title: i2c lib CT
 * author: CTa
 * date: 01.12.2022
 *  
 */

#include <p32xxxx.h>
#include "i2c.h"
#include "Timer.h"

unsigned char out_x[2];  // definizione della variabile
unsigned char out_y[2];  // definizione della variabile
unsigned char out_z[2];  // definizione della variabile
int id;

// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be callled in the correct order as per the I2C protocol
// BasysMX3 Accelerometer --> SCL1 = RG2, SDA1 = RG3


void i2c_master_setup(void) 
{
    TRISGbits.TRISG2 = 1; // SCL
    TRISGbits.TRISG3 = 1; // SDA
    
    I2C1CON = 0x0000; // use default settings for I2C
    I2C1BRG = 186; // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2
    // Fsck is the freq (100 kHz here), PGD = 104 ns
    I2C1CONbits.ON = 1; // turn on the I2C1 module
}

// Start a transmission on the I2C bus
void i2c_master_start(void) 
{
    I2C1CONbits.SEN = 1; // send the start bit
    while(I2C1CONbits.SEN) { ; } // wait for the start bit to be sent
}

void i2c_master_restart(void) 
{
    I2C1CONbits.RSEN = 1; // send a restart
    while(I2C1CONbits.RSEN) { ; } // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) 
{ // send a byte to slave
    I2C1TRN = byte; // if an address, bit 0 = 0 for write, 1 for read
    while(I2C1STATbits.TRSTAT) { ; } // wait for the transmission to finish
    if(I2C1STATbits.ACKSTAT) { // if this is high, slave has not acknowledged
    //put_U4_String("I2C2 Master: failed to receive ACK\r\n");
    }
}


unsigned char i2c_master_recv(int ack) 
{ 
    I2C1CONbits.RCEN = 1;              // Abilita la ricezione
    while (!I2C1STATbits.RBF) { ;}    // Attendi che i dati siano ricevuti
    unsigned char data = I2C1RCV;      // Leggi i dati
    i2c_master_ack(ack);               // Invia ACK o NACK
    return data;
}


void i2c_master_ack(int val) 
{ // sends ACK = 0 (slave should send another byte)
    // or NACK = 1 (no more bytes requested from slave)
    I2C1CONbits.ACKDT = val; // store ACK/NACK in ACKDT
    I2C1CONbits.ACKEN = 1; // send ACKDT
    while(I2C1CONbits.ACKEN) { ; } // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) 
{ // send a STOP:
    I2C1CONbits.PEN = 1; // comm is complete and master relinquishes bus
    while(I2C1CONbits.PEN) { ; } // wait for STOP to complete
}
