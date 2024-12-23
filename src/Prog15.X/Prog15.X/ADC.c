/* 
 * File:   ADC.c
 * Author: giada
 *
 * Created on December 17, 2024, 8:50 AM
 */

#include <stdio.h>
#include <stdlib.h>

#include <p32xxxx.h>

void init_ADC(){
    ANSELBbits.ANSB2 = 1;// = 0xFFFB ; // PORTB = Digital; RB2 = analog
    /* uncomment for Manual sampling */
    //AD1CON1 = 0x0000;   // SAMP bit = 0 ends sampling
                        // and starts converting
    /* uncomment for Automatic sampling */
    AD1CON1 = 0x0004;   // ASAM bit = 1 implies acquisition
                        // starts immediately after last
                        // conversion is done
    AD1CHS = 0x00020000; // Connect RB2/AN2 as CH0 input
                         // in this example RB2/AN2 is the input
    AD1CSSL = 0;
    AD1CON3 = 0x0002; // TAD = internal 6 TPB
    AD1CON2 = 0;
    AD1CON1SET = 0x8000 ; // turn on the ADC
}

int adc_measure()
{
    /* uncomment for Automatic sampling */
    Delayms(100); // for 100 ms
    AD1CON1CLR = 0x0002; // start Converting
    
    /* uncomment for Manual sampling */
//    AD1CON1SET = 0x0002 ; // start sampling...
//    Delayms(100); // for 100 ms
//    AD1CON1CLR = 0x0002; // start Converting
    while(!(AD1CON1 & 0x0001)); // conversion done ?
    return ADC1BUF0;
}