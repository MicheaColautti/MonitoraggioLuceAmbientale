#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <p32xxxx.h>

#include "Uart.h"
#include "LCD.h"
#include "i2c.h"
#include "Timer.h"
#include "ADC.h"
#include "Audio_PMW.h"
#include "spi.h"
#include "Pin.h"
#include "TSL2561.h"    

// Dichiarazioni delle funzioni
void init_hardware(void);
void init_menu(void);
void start_monitoring(void);
void stop_monitoring(void);
void display_last_detection(void);
void reset_last_detection(void);
void beep(void);
void BTNC_Interrupt_Init(void);
void update_leds(int lux);

// Configurazione FUSE del microcontrollore
#pragma config FNOSC = FRCPLL 
#pragma config FSOSCEN = OFF 
#pragma config POSCMOD = XT 
#pragma config OSCIOFNC = ON 
#pragma config FPBDIV = DIV_2 
#pragma config FPLLIDIV = DIV_2 
#pragma config FPLLMUL = MUL_20 
#pragma config FPLLODIV = DIV_2 
#pragma config JTAGEN = OFF 
#pragma config FWDTEN = OFF

// Definizioni per LED RGB
#define LED_RGB_RED     LATDbits.LATD2
#define LED_RGB_GREEN   LATDbits.LATD12
#define LED_RGB_BLUE    LATDbits.LATD3

// Numero di LED sulla porta A
#define NUM_LEDS 8
#define MAX_LUX 1800 // Valore massimo di LUX per 8 LED accesi
#define MAX_COMMAND_LENGTH 20
static char uart_command[MAX_COMMAND_LENGTH];

volatile unsigned int last_lux = 0; // Ultima misura LUX
volatile int monitoring = 0;        // Flag monitoraggio attivo
char stringaSuLCD[16]; // Buffer per scritte su LCD
volatile int interrupt_triggered = 0;  // Flag per indicare che l'interrupt è stato attivato


// Interrupt INT4, imposta la flag per indicare che l'interrupt è stato attivato
void __attribute__((interrupt(ipl1AUTO), vector(_EXTERNAL_4_VECTOR))) ButtonInterrupt(void) {
    if (monitoring)
        interrupt_triggered = 1;  
    Delayms(100);
    IFS0bits.INT4IF = 0;  // Pulisce il flag dell'interrupt
}

int main(int argc, char** argv) {
    init_hardware();
    init_menu();
    
    while (1) {
        if (interrupt_triggered) {
            char debug_buffer[50];
            snprintf(debug_buffer, sizeof(debug_buffer), "Interrupt Triggered. Last lux: %d\r\n", last_lux);
            UART4_WriteString(debug_buffer);  
            
            // Scrive l'ultimo valore di lux nella memoria flash
            EraseFlash();
            writeFlashMem(0x00, (unsigned char)(last_lux & 0xFF));  // Byte meno significativo
            writeFlashMem(0x01, (unsigned char)((last_lux >> 8) & 0xFF));  // Byte più significativo        

            stop_monitoring();
            // Reset del flag
            interrupt_triggered = 0;
        }
        if (monitoring) {
            int lux = (int)TSL2561_read_lux();
            last_lux = lux;
            update_leds(lux);  
            
            // Aggiorna LCD
            cmdLCD(0x01); // Clear display
            cmdLCD(0x80); // Prima riga
            snprintf(stringaSuLCD, sizeof(stringaSuLCD), "Light:%d LUX", lux);
            putsLCD(stringaSuLCD);    
            cmdLCD(0xC0); // Seconda riga
            snprintf(stringaSuLCD, sizeof(stringaSuLCD), "LED accesi:%d", (lux * NUM_LEDS) / MAX_LUX);
            putsLCD(stringaSuLCD);
            Delayms(500);
        }
    }
    return 0;
}

//Inizializzazione hardware
void init_hardware() {
    MultiVector_mode();
    _nop();
    Timer2_init();
    Init_pins();
    BTNC_Interrupt_Init();
    UART_ConfigurePins();
    UART_ConfigureUart();
    audio_init(); 
    init_ADC();
    initLCD();
    i2c_master_setup();
    TSL2561_init(); // Inizializza sensore di luce
    initSPI1(); // Inizializza SPI per Flash
    EraseFlash(); // cancella la memoria flash
    
    
    // LED RGB Verde all'accensione
    LED_RGB_RED = 0;
    LED_RGB_GREEN = 1;
    LED_RGB_BLUE = 0;
}


void BTNC_Interrupt_Init(void) {
    INT4R = 0x04;   
    INTCONbits.INT4EP = 0;  // Impostazione fronte di discesa
    // Configura priorità e flag dell'interrupt
    IPC4bits.INT4IP = 1;    // Imposta priorità alta
    IPC4bits.INT4IS = 0;     // EXT0 sub-priority 0
    IFS0bits.INT4IF = 0;    // Pulisce il flag dell'interrupt
    IEC0bits.INT4IE = 1;    // Abilita l'interrupt INT4
}

// Menu iniziale su UART
void init_menu(void) {
    UART4_WriteString("Menu:\r\n");
    UART4_WriteString("1. Avvia monitoraggio luce ambientale\r\n");
    UART4_WriteString("2. Visualizza ultima detezione luminosa\r\n");
    UART4_WriteString("3. Reset ultima detezione\r\n");
    
    UART4_ReadString(uart_command, MAX_COMMAND_LENGTH);
    uart_command[MAX_COMMAND_LENGTH - 1] = '\0'; // assicurare terminazione

    if (strcmp(uart_command, "1") == 0) {
        start_monitoring();
    } else if (strcmp(uart_command, "2") == 0){
        display_last_detection();
        UART4_FlushBuffer();
        init_menu();
    } else if (strcmp(uart_command, "3") == 0) {
        reset_last_detection();
        UART4_FlushBuffer();
        init_menu();
    } else {
        UART4_WriteString("Errore: comando non valido\r\n");
        UART4_FlushBuffer();
        init_menu();
    }


}

// Funzione 1: Avvio monitoraggio
void start_monitoring(void) {
    monitoring = 1;
    beep(); // Beep iniziale
    LED_RGB_GREEN = 0;
    LED_RGB_BLUE = 1;
}

// Interrompe il monitoraggio e torna al menu
void stop_monitoring(void) {
    monitoring = 0;
    LED_RGB_BLUE = 0;
    LED_RGB_GREEN = 1;
    init_menu();
}

// Funzione 2: Visualizza ultima detezione
void display_last_detection(void) {
    // Legge i due byte consecutivi dalla memoria flash
    int low_byte = readFlashMem(0x00);         // Byte meno significativo
    int high_byte = readFlashMem(0x01);        // Byte più significativo

    // Ricostruisce il valore 16-bit
    int stored_lux = (high_byte << 8) | low_byte;
    
    // Prepara il messaggio da visualizzare
    char buffer[50];
    sprintf(buffer, "Last Light: %d LUX\r\n", stored_lux);

    // Invia il messaggio tramite UART
    UART4_WriteString(buffer);
}

// Funzione 3: Reset ultima detezione
void reset_last_detection(void) {
    // Cancella la memoria flash
    UART4_WriteString("Inizio cancellazione flash...\r\n");
    EraseFlash();
    UART4_WriteString("Ultima detezione resettata.\r\n");
}

// Funzione per aggiornare i LED in base al valore di Lux
void update_leds(int lux) {
    // Calcola il numero di LED da accendere in base al valore di lux
    int num_leds = NUM_LEDS - (lux * NUM_LEDS) / MAX_LUX;

    // Aggiorna i LED
    unsigned char pattern = 0x00;
    for (int i = 0; i < num_leds; i++) {
        pattern |= (1 << i);  // Accende il LED corrispondente
    }

    // Imposta il pattern sui LED (porta A)
    LATA = (LATA & 0xFF00) | pattern;
}

// Breve beep a 10kHz, 50% duty
void beep(){ 
    OC1CONbits.ON = 1;            // Accende OC1     
    Delayms(1000);
    OC1CONbits.ON = 0; 
}
