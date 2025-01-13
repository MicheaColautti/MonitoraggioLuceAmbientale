/* 
 * File:   Timer.c
 * Author: giada
 *
 * Created on November 4, 2024, 3:38 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <p32xxxx.h>

/*
 * 
 */
//If you set timer reset every 1ms, you can use Delayms as shown below. Pass ms as argument you want to wait
void Delayms(unsigned t){
    while (t--)
    { // t x 1ms loop
		TMR2 = 0; // manually reset TMR2 register to guarantee time elaplsing desidered
		while (TMR2 < PR2);
    }
} 

void Timer2_init(void)
{
    T2CONbits.ON = 0;   // Disable Timer2
    T2CONbits.T32 = 0;  // not use 32-bit mode - use 16-bit mode
    T2CONbits.TCKPS = 0b111; //or 0x7, select prescaler 256
    T2CONbits.TCS = 0;  //select internal peripheral clock
    TMR2 = 0;           //Clear TMR2 register
    PR2 = 78;        // Set PR2 register - Calculated to have 0.001s (1ms)- Change it for new delays (See also PBCLK due to #pragma)
    
    /* avvio timer2 */
    T2CONbits.ON = 1;   // Enable Timer2  T2CONbits.TCKPS = 0b111; //select prescaler 256    
}

void MultiVector_mode()
{
	__builtin_disable_interrupts();
	INTCONbits.MVEC = 1;
	__builtin_enable_interrupts();
}
