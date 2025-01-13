#include <p32xxxx.h>


void audio_init()
{
    TRISBbits.TRISB14 = 0;        // Imposta il pin RB14 come uscita
    ANSELBbits.ANSB14 = 0;        // Disabilita la funzione analogica sul pin RB14 
    RPB14R = 0x0C;                // Configura RB14 come OC1 (Output Compare 1)

    /* Configurazione del Timer3 e OC1 */
    T3CONbits.TCKPS = 0;          // Imposta il prescaler 1:1
    PR3 = 1999;                    // T_pwm = ((PR+1)*Presc)/PBCLK --> (PR+1) = (PBCLK / Freq_pwm) * Presc 
    TMR3 = 0;                     // Inizializza il contatore del Timer3

    OC1CONbits.ON = 0;            // Spegne OC1 per la configurazione
    OC1CONbits.OCM = 6;           // Imposta la modalità PWM su OC1; Fault pin is disabled
    OC1CONbits.OCTSEL = 1;        // Imposta Timer3 come clock sorgente per OC1

    OC1RS = 1000;                  // Imposta il Duty Cycle per il beep (50%)
    OC1R = 1000;                     // Inizializza il valore di OC1R

    T3CONbits.ON = 1;             // Accende il Timer3
}
