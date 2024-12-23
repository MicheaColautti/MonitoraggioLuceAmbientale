/* 
 * File:   Pin.c
 * Author: giada
 *
 * Created on October 7, 2024, 4:28 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <p32xxxx.h>
#include "Pin.h"

/*
 * 
 */
void Init_pins(){
    TRISA = 0x00; // setta la porta A dei led come output digitale
    LATA = 0x00;
    ANSELA = 0x0000; 
    
    // Configurazione GPIO
    TRISDbits.TRISD2 = 0;  // LED RGB RED
    TRISDbits.TRISD12 = 0;  // LED RGB GREEN
    TRISDbits.TRISD3 = 0;  // LED RGB BLUE
    TRISFbits.TRISF0 = 1;  // BTNC
     
}



