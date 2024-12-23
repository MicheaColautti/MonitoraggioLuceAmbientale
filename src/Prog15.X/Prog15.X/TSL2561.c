#include "i2c.h"
#include "TSL2561.h"

// Indirizzo del sensore TSL2561
#define TSL2561_ADDR 0x39

// Definizioni dei registri del TSL2561
#define TSL2561_CMD 0xA0 // Comando per i registri
#define TSL2561_REG_CONTROL 0x00 // Registro di controllo
#define TSL2561_REG_TIMING 0x01 // Registro del timing
#define TSL2561_REG_DATA0LOW 0x0C // Registro di dati, byte basso (dati LUX)
#define TSL2561_REG_DATA0HIGH 0x0D // Registro di dati, byte alto (dati LUX)

// Comandi per il controllo del sensore
#define TSL2561_POWER_ON 0x03  // Comando per accendere il sensore
#define TSL2561_POWER_OFF 0x00 // Comando per spegnere il sensore

// Inizializzazione del sensore TSL2561
void TSL2561_init(void) {
    i2c_master_start();  
    i2c_master_send(TSL2561_ADDR << 1); // Scrittura all'indirizzo del sensore
    i2c_master_send(TSL2561_CMD | TSL2561_REG_CONTROL); // Seleziona il registro di controllo
    i2c_master_send(TSL2561_POWER_ON);  // Accende il sensore
    i2c_master_stop();  

    // Impostazione della durata di integrazione per la lettura dei dati
    i2c_master_start();
    i2c_master_send(TSL2561_ADDR << 1);  
    i2c_master_send(TSL2561_CMD | TSL2561_REG_TIMING); // Seleziona il registro di timing
    i2c_master_send(0x02);  // Imposta il tempo di integrazione, 402ms
    i2c_master_stop();
}

// Funzione per leggere i dati LUX dal sensore
uint16_t TSL2561_read_raw(void) {
    uint8_t low, high;
    
    // Avvia la comunicazione I2C per leggere i dati
    i2c_master_start();
    i2c_master_send(TSL2561_ADDR << 1);                     // Scrittura all'indirizzo del sensore
    i2c_master_send(TSL2561_CMD | TSL2561_REG_DATA0LOW);    // Seleziona il registro di dati basso
    i2c_master_restart();                                   // Riavvia la comunicazione per la lettura
    i2c_master_send((TSL2561_ADDR << 1) | 1);               // Imposta l'indirizzo in lettura
    low = i2c_master_recv(0);                               // Legge il byte basso dei dati
    high = i2c_master_recv(1);                              // Legge il byte alto dei dati
    i2c_master_stop();                                      // Termina la comunicazione

    return (high << 8) | low;  // Combina i byte alto e basso
}

// Funzione per convertire i dati grezzi in Lux
unsigned int TSL2561_read_lux(void) {
    uint16_t raw_data = TSL2561_read_raw();
    unsigned int lux = raw_data * 0.034; // Fattore di conversione approssimato (può variare)
    return lux; 
}
