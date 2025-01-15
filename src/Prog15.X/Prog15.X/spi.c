/* 
 * File:   spi.c
 * Author: giada
 *
 * Created on December 9, 2024, 3:37 PM
 */

#include <stdio.h>
#include <stdlib.h>

#include "spi.h"

void initSPI1(void)
{
    TRISFbits.TRISF2 = 0; // RF2 as Digital Input SDI for flash, SDO for MCU 
    TRISFbits.TRISF7 = 1; //RF7 ad Digital Output SDO for flash, SDI for MCU
    TRISFbits.TRISF6 = 0; // RF6 as Digital Output SCK
    
    RPF2R = 0x08; // SDI1 ? RF2
    SDI1R = 0x0F; // SDO1 ? RF7
    
    TCS = 0; // RF8 as Digital Output Chip Selection - see #define
    CS = 1; // Chip select low - see #define
    
    SPI1CON = 0x8120; // SPI on, 8-bit master,CKE=1,CKP=0
//    SPI1CONbits.MSTEN = 1;      // SPI Master enable
//    SPI1CONbits.CKE = 1;        // Set for SPI Mode 0
//    SPI1CONbits.ON = 1;         // Enable SPI1
    SPI1BRG = 15; // Fsck = Fpb/(2 * (15+1))
}

unsigned char readSPI1(void) {
    // Avvia la lettura SPI
    SPI1BUF = 0x00;  // Invia un byte vuoto per avviare la lettura (Master Out, Slave In)
    
    // Attendi che la lettura sia completa (SPI1STATbits.SPIRBF è 1 quando il buffer di ricezione è pronto)
    while (!SPI1STATbits.SPIRBF) {
        // Attendi che il dato venga ricevuto
    }
    
    // Leggi il dato ricevuto dal buffer SPI
    return SPI1BUF;
}


void EraseFlash(void)
{
    UART4_WriteString("Inizio cancellazione flash...\r\n");
    
    // write enable
    CS = 0;
    writeSPI1(0x06);
    CS = 1;
    Delayms(10);
    
    UART4_WriteString("Comando di cancellazione inviato...\r\n");

    // full erase command
    CS = 0;
    writeSPI1(0x60);  // Full chip erase command
    CS = 1;

    UART4_WriteString("Comando di cancellazione eseguito, attendo completamento...\r\n");
    
    // Polling per attendere la fine dell'operazione di cancellazione
    CS = 0;
    writeSPI1(0x05);  // Read status register command
    unsigned char status = readSPI1();  // Leggi lo stato della flash
    CS = 1;
    
    // Polling: attende finché il bit "Busy" non è 0 (Operazione finita)
    while (status & 0x01) {  // Bit 0 indica se la flash è occupata
        Delayms(10);  // Attende un po' prima di controllare di nuovo
        CS = 0;
        writeSPI1(0x05);  // Leggi di nuovo lo stato
        status = readSPI1();
        CS = 1;
    }
    
    UART4_WriteString("Cancellazione completata.\r\n");

    // Cancellazione completata, disabilita la scrittura
    CS = 0;
    writeSPI1(0x04);  // Write disable
    CS = 1;
}


// send one byte of data and receive one back at the same time
int writeSPI1( int i)
{
    while(!SPI1STATbits.SPITBE);	// wait for TX buffer to be empty
    SPI1BUF = i; // write to buffer for TX
    while( !SPI1STATbits.SPIRBF); // wait for transfer complete
    return (int)SPI1BUF; // read the received value
}//writeSPI1


void writeFlashMem(int addr, short byte){
    short value = 0;
    int address = 0;
    
    value = byte;
    address = addr;
    // write enable
    CS = 0;
    writeSPI1(0x06);
    CS = 1;
    
     // send a Write command
    CS = 0; // select the Serial EEPROM
    writeSPI1( 0x02); // send command, ignore data Page Program
    writeSPI1(address >> 16); // send MSB of memory address
    writeSPI1(address >> 8); // send LSB of memory address
    writeSPI1(address); // send LSB of memory address
    writeSPI1(value); // send the actual data
    // send more data here to perform a page write
    CS = 1; // start actual EEPROM write cycle
    
    // write disable
    CS = 0;
    writeSPI1(0x04);
    CS = 1;
}

int readFlashMem(int addr)
{
    int tmp=0;
     // send a read command
    CS = 0; // select the Serial EEPROM
    writeSPI1(0x03); // send command Read Data, ignore data
    writeSPI1(addr >> 16); // send MSB of memory address
    writeSPI1(addr >> 8); // send LSB of memory address
    writeSPI1(addr); // send LSB of memory address

    tmp = writeSPI1(0); // send dummy, read data 

    CS = 1; // terminate the read sequence
    return tmp;
}