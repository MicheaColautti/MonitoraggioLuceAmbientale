#ifndef TSL2561_H
#define TSL2561_H

#include <stdint.h>  // Aggiungi questa linea per i tipi uint8_t e uint16_t

// Indirizzo I2C del sensore TSL2561
#define TSL2561_ADDR 0x39

// Comandi per l'accesso ai registri del sensore TSL2561
#define TSL2561_CMD 0xA0
#define TSL2561_REG_CONTROL 0x00
#define TSL2561_REG_TIMING 0x01
#define TSL2561_REG_DATA0LOW 0x0C
#define TSL2561_REG_DATA0HIGH 0x0D

// Comandi di controllo
#define TSL2561_POWER_ON 0x03
#define TSL2561_POWER_OFF 0x00

// Funzione di inizializzazione del sensore
void TSL2561_init(void);

// Funzione per leggere i dati grezzi dal sensore
uint16_t TSL2561_read_raw(void);

// Funzione per leggere il valore luminoso in Lux
unsigned int TSL2561_read_lux(void);

#endif // TSL2561_H
