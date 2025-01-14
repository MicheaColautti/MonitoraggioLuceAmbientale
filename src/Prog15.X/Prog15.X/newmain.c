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

// Soglie LUX (dal più luminoso al più buio)
// Queste soglie sono un esempio, si possono regolare in base alle esigenze.
static const int thresholds[NUM_LEDS] = {7000, 6000, 5000, 4000, 3000, 2000, 1000, 500};

#define MAX_COMMAND_LENGTH 20
static char uart_command[MAX_COMMAND_LENGTH];

volatile unsigned int last_lux = 0; // Ultima misura LUX
volatile int monitoring = 0;        // Flag monitoraggio attivo

char stringaSuLCD[16]; // Buffer per scritte su LCD

// Prototipi funzioni
void init_hardware(void);
void init_menu(void);
void start_monitoring(void);
void stop_monitoring(void);
void display_last_detection(void);
void reset_last_detection(void);
void update_leds(int lux);
void beep(void);
void BTNC_Interrupt_Init(void);

// Interrupt INT4, salva ultimo lux su flash e ferma monitoraggio
void __attribute__((interrupt(ipl2), vector(_EXTERNAL_4_VECTOR))) ButtonInterrupt(void) {
    IFS0bits.INT4IF = 0;
    // Salva solo un byte (LUX limitato a 0-255)
    writeFlashMem(0x00, (unsigned char)(last_lux & 0xFF));
    stop_monitoring();
}

int main(int argc, char** argv) {
    init_hardware();
    UART4_WriteString("Test UART avviata.\r\n");
    init_menu();

    while (1) {
        if (monitoring) {
            int lux = (int)TSL2561_read_lux();
            last_lux = lux;
            update_leds(lux);

            // Calcola quanti LED accesi in base alle soglie
            int num_leds = 0;
            for (int i = 0; i < NUM_LEDS; i++) {
                if (lux <= thresholds[i]) {
                    num_leds = i + 1;
                } else {
                    break;
                }
            }

            // Aggiorna LCD
            cmdLCD(0x01); // Clear display
            cmdLCD(0x80); // Prima riga
            snprintf(stringaSuLCD, sizeof(stringaSuLCD), "Light:%d LUX", lux);
            putsLCD(stringaSuLCD);
            cmdLCD(0xC0); // Seconda riga
            snprintf(stringaSuLCD, sizeof(stringaSuLCD), "LED accesi:%d", num_leds);
            putsLCD(stringaSuLCD);

            Delayms(500);
        }
    }
    return 0;
}

// Inizializzazione hardware
void init_hardware() {
    Timer2_init();
    BTNC_Interrupt_Init();
    Init_pins();
    audio_init(); 
    init_ADC();
    initLCD();
    i2c_master_setup();
    TSL2561_init(); // Inizializza sensore di luce
    UART_ConfigurePins();
    UART_ConfigureUart();
    initSPI1(); // Inizializza SPI per Flash
    
    // LED RGB Verde all?accensione
    LED_RGB_RED = 0;
    LED_RGB_GREEN = 1;
    LED_RGB_BLUE = 0;
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
        init_menu();
    } else if (strcmp(uart_command, "3") == 0) {
        reset_last_detection();
        init_menu();
    } else {
        UART4_WriteString("Errore: comando non valido\r\n");
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
    int stored_lux = readFlashMem(0x00);
    char buffer[50];
    sprintf(buffer, "Last Light: %d LUX\r\n", stored_lux);
    UART4_WriteString(buffer);
}

// Funzione 3: Reset ultima detezione
void reset_last_detection(void) {
    EraseFlash();
    UART4_WriteString("Ultima detezione resettata.\r\n");
    
    //debugging - verifica cancellazione dei dati 
    for (int i = 0; i < 10; i++) {
    int value = readFlashMem(i); // Legge i primi 10 indirizzi
    char buffer[50];
    sprintf(buffer, "Address %d: %d\r\n", i, value);
    UART4_WriteString(buffer);
}
}

// Gestione dei LED in base alla luminosità
void update_leds(int lux) {
    int num_leds = 0;
    for (int i = 0; i < NUM_LEDS; i++) {
        if (lux <= thresholds[i]) {
            num_leds = i + 1;
        } else {
            break;
        }
    }

    unsigned char pattern = 0x00;
    for (int i = 0; i < num_leds; i++) {
        pattern |= (1 << i);
    }
    LATA = (LATA & 0xFF00) | pattern; 
}

// Breve beep a 10kHz, 50% duty
void beep(){ 
    OC1CONbits.ON = 1;            // Accende OC1     
    Delayms(1000);
    OC1CONbits.ON = 0; 
}

void BTNC_Interrupt_Init(void) {
    INT4 = 0x04;
    INTCONbits.INT4EP = 0;    // fronte di salita (o discesa in base alle necessità)
    IPC4bits.INT4IP = 2;   
    IPC4bits.INT4IS = 0;   
    IFS0bits.INT4IF = 0;   
    IEC0bits.INT4IE = 1;   
}