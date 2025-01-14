#include "TSL2561.h"
#include "i2c.h"
#include "Uart.h"  // La libreria I2C e la UART sono necessarie per il funzionamento
#include <math.h>  // Per la funzione powf()
#include <stdint.h>
#include <stdio.h> // Per la funzione snprintf()

// Optional: se hai bisogno di una funzione di delay (es. Delayms(500))
extern void Delayms(unsigned int t);

// Definizione dell'indirizzo I2C del sensore TSL2561
#define TSL2561_ADDR 0x29

// Registri del TSL2561
#define TSL2561_REG_CONTROL   0x00
#define TSL2561_REG_TIMING    0x01
#define TSL2561_REG_DATA0LOW  0x0C
#define TSL2561_REG_DATA0HIGH 0x0D
#define TSL2561_REG_DATA1LOW  0x0E
#define TSL2561_REG_DATA1HIGH 0x0F
#define TSL2561_REG_ID        0x0A

// Comandi per il controllo di accensione/spegnimento
#define TSL2561_POWER_ON  0x03
#define TSL2561_POWER_OFF 0x00

// Funzione per leggere un canale del sensore (canale 0 o canale 1)
static uint16_t TSL2561_read_channel(uint8_t reg_low, uint8_t reg_high) {
    uint8_t low, high;

    // Avvia la comunicazione I2C
    i2c_master_start();
    i2c_master_send(TSL2561_ADDR << 1);  // Invia indirizzo I2C (scrittura)
    i2c_master_send(TSL2561_CMD | reg_low);  // Invia il registro da leggere

    // Ripristina per la lettura
    i2c_master_restart();
    i2c_master_send((TSL2561_ADDR << 1) | 1);  // Invia indirizzo I2C (lettura)

    // Legge i due byte (low e high)
    low  = i2c_master_recv(0);  // Legge il byte basso (ack)
    high = i2c_master_recv(1);  // Legge il byte alto (Nack)

    i2c_master_stop();

    return ((uint16_t)high << 8) | low;
}

// Funzione per inizializzare il sensore TSL2561
void TSL2561_init(void) {
    // Accendi il sensore (comando di accensione)
    i2c_master_start();
    i2c_master_send(TSL2561_ADDR << 1);
    i2c_master_send(TSL2561_CMD | TSL2561_REG_CONTROL);  // Registro di controllo
    i2c_master_send(TSL2561_POWER_ON);
    i2c_master_stop();

    // Attendi che il sensore si accenda completamente
    Delayms(200);

    // Leggi l'ID del sensore
    uint8_t id = TSL2561_read_id();
    if (id != 0x10 && id != 0x50) {
        UART4_WriteString("ID sensore non valido\r\n");
    } else {
        UART4_WriteString("Sensore TSL2561 inizializzato correttamente\r\n");
    }

    // Imposta il tempo di integrazione a 402 ms (valore 0x02)
    i2c_master_start();
    i2c_master_send(TSL2561_ADDR << 1);
    i2c_master_send(TSL2561_CMD | TSL2561_REG_TIMING);  // Registro di timing
    i2c_master_send(0x02);  // Imposta 402 ms di integrazione
    i2c_master_stop();

    // Attendi il completamento dell'integrazione
    Delayms(500);
}

// Funzione per leggere l'ID del sensore TSL2561
uint8_t TSL2561_read_id(void) {
    uint8_t id = 0;

    // Avvia la comunicazione I2C per leggere l'ID
    i2c_master_start();
    i2c_master_send(TSL2561_ADDR << 1);
    i2c_master_send(TSL2561_CMD | TSL2561_REG_ID);  // Registro ID

    // Ripristina per la lettura dell'ID
    i2c_master_restart();
    i2c_master_send((TSL2561_ADDR << 1) | 1);  // Invia indirizzo I2C per lettura

    // Leggi l'ID del sensore
    id = i2c_master_recv(1);  // Leggi un byte (NACK dopo)

    i2c_master_stop();

    return id;
}

// Funzione per leggere i dati di luce (lux) dal sensore
unsigned int TSL2561_read_lux(void) {
    uint16_t CH0 = TSL2561_read_channel(TSL2561_REG_DATA0LOW, TSL2561_REG_DATA0HIGH);  // Canale 0
    uint16_t CH1 = TSL2561_read_channel(TSL2561_REG_DATA1LOW, TSL2561_REG_DATA1HIGH);  // Canale 1

    // Se i valori sono invalidi, stampa un messaggio e ritorna 0
    if (CH0 == 0 || CH0 == 0xFFFF || CH1 == 0xFFFF) {
        UART4_WriteString("Sensore NON VA\r\n");
        return 0;
    }

    // Calcola il rapporto tra i canali e determina il valore di lux
    float ratio = (float)CH1 / (float)CH0;
    float lux = 0.0f;

    // Calcolo dei lux in base al rapporto
    if (ratio <= 0.5f) {
        lux = 0.0304f * CH0 - 0.062f * CH0 * powf(ratio, 1.4f);
    } else if (ratio <= 0.61f) {
        lux = 0.0224f * CH0 - 0.031f * CH1;
    } else if (ratio <= 0.80f) {
        lux = 0.0128f * CH0 - 0.0153f * CH1;
    } else if (ratio <= 1.30f) {
        lux = 0.00146f * CH0 - 0.00112f * CH1;
    } else {
        lux = 0.0f;
    }

    // Evita valori negativi
    if (lux < 0.0f) {
        lux = 0.0f;
    }

    // Stampa il valore di lux
    static char strbuf[32];
    snprintf(strbuf, sizeof(strbuf), "Light:%u LUX\r\n", (unsigned int)lux);
    UART4_WriteString(strbuf);

    return (unsigned int)lux;
}
