/* 
 * File:   Uart.c
 * Author: giada
 *
 * Created on October 3, 2024, 3:47 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <p32xxxx.h>
#include "Uart.h"
#include "Timer.h"

/*
 * 
 */

void UART_ConfigurePins(){ // used to configure UART4 TX and RX
    TRISFbits.TRISF12 = 0 ; //TX digital output
    RPF12R = 2 ; // 0010 U4TX = Mapping U4TX to RPF12;
    TRISFbits.TRISF13 = 1;//RX digital  input
    U4RXR = 9 ; // 1001 RF13 = Mapping U4RX to RPF13
}


void UART_ConfigureUart(){
    remap_UART4_pins();
    unsigned int PbusClock = 20000000;
    unsigned int UartBrg = 0 ;
    U4MODEbits.ON = 0 ;
    U4MODEbits.SIDL = 0 ;
    U4MODEbits.IREN = 0 ;
    U4MODEbits.RTSMD = 0 ;
    U4MODEbits.UEN0 = 0 ;
    U4MODEbits.UEN1 = 0 ;
    U4MODEbits.WAKE = 0 ;
    U4MODEbits.LPBACK = 0 ;
    U4MODEbits.ABAUD = 0 ;
    U4MODEbits.RXINV = 0 ;
    U4MODEbits.PDSEL1 = 0 ;
    U4MODEbits.PDSEL0 = 0 ;
    U4MODEbits.STSEL = 0 ;
    U4MODEbits.BRGH = 0 ;
    /* calculate brg */
    UartBrg = (int) ((20000000 / (16 * 9600) - 1) + 0.5) ; // add 0.5 to round
    U4BRG = UartBrg ;
    U4STAbits.UTXEN = 1;
    U4STAbits.URXEN = 1;
    U4MODEbits.ON = 1 ;
}

void remap_UART4_pins(void) {
   
    U4RXR = 0b1001; // RF13 -> UART4 RX
    RPF12R = 2; // RF12 -> UART4 TX
}

int putU4 (char c){
    while(U4STAbits.UTXBF == 1); // 1 = occupato 
        U4TXREG = c ;
}

char getU4 (void){
    while (!U4STAbits.URXDA) ; // wait for a new char to arrive
        return U4RXREG; // read char from receive buffer
}

void putU4_string (char szData[ ]){
    char *pData = szData;
    while (*pData){
        putU4((*(pData++))) ;
    }
}

void UART4_WriteString(const char *str) {
    while (*str) {
        putU4(*str++);
    }
}


void UART4_ReadString(char* buffer, int maxLength) {
    int i = 0;
    char c;
    while (i < maxLength - 1) {
        c = getU4();
        if (c == '\n' || c == '\r') break;  // Fine riga
        buffer[i++] = c;
    }
    buffer[i] = '\0';  // Termina la stringa
}

int isU4Available(void) {
    return U4STAbits.URXDA;  // Returns 1 if data is available, 0 otherwise
}

void UART4_FlushBuffer() {
    while (isU4Available()) {
        getU4();  // Consume all leftover characters
    }
}