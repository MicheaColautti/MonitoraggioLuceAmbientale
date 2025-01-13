#include "TSL2561.h"
#include "i2c.h" 
#include "Uart.h"      // You must have an I2C library that provides i2c_master_* functions
// You must have an I2C library that provides i2c_master_* functions
#include <math.h>     // For powf()
#include <stdint.h>
#include <stdio.h>    // For snprintf()


// Optional: if you need a delay function (e.g. Delayms(500))
extern void Delayms(unsigned int t);

#define TSL2561_ADDR 0x39

#define char stringa[16]; 


// We use 0x80 for single-byte register commands
#define TSL2561_CMD  0x80

// TSL2561 Registers
#define TSL2561_REG_CONTROL   0x00
#define TSL2561_REG_TIMING    0x01
#define TSL2561_REG_DATA0LOW  0x0C
#define TSL2561_REG_DATA0HIGH 0x0D
#define TSL2561_REG_DATA1LOW  0x0E
#define TSL2561_REG_DATA1HIGH 0x0F

// Power control values
#define TSL2561_POWER_ON  0x03
#define TSL2561_POWER_OFF 0x00

// -----------------------------------------------------------------------------
// Low-level function to read a 16-bit channel (CH0 or CH1) from TSL2561
// -----------------------------------------------------------------------------
static uint16_t TSL2561_read_channel(uint8_t reg_low, uint8_t reg_high) {
    uint8_t low, high;

    // Start I2C write
    i2c_master_start();
    i2c_master_send(TSL2561_ADDR << 1);
    i2c_master_send(TSL2561_CMD | reg_low);
    
    // Restart to switch to read mode
    i2c_master_restart();
    i2c_master_send((TSL2561_ADDR << 1) | 1);

    // Read two bytes
    low  = i2c_master_recv(0);  // Read low byte, send ACK=0
    high = i2c_master_recv(1);  // Read high byte, send NACK=1
    i2c_master_stop();

    return ((uint16_t)high << 8) | low;
}

// -----------------------------------------------------------------------------
// Initialize TSL2561: Power on, set 402ms integration time, wait for stable data
// -----------------------------------------------------------------------------
void TSL2561_init(void) {
    // Power ON
    i2c_master_start();
    i2c_master_send(TSL2561_ADDR << 1);
    i2c_master_send(TSL2561_CMD | TSL2561_REG_CONTROL); // 0x80 + 0x00
    i2c_master_send(TSL2561_POWER_ON);
    i2c_master_stop();

    // Give sensor time to power up
    Delayms(200);

    // Set integration time (0x02 => 402 ms)
    i2c_master_start();
    i2c_master_send(TSL2561_ADDR << 1);
    i2c_master_send(TSL2561_CMD | TSL2561_REG_TIMING);  // 0x80 + 0x01
    i2c_master_send(0x02);
    i2c_master_stop();

    // Wait at least one integration cycle before reading (402 ms)
    Delayms(500);
}

// -----------------------------------------------------------------------------
// Read raw channels CH0 and CH1, then convert to approximate LUX using formula
// -----------------------------------------------------------------------------
unsigned int TSL2561_read_lux(void) {
    uint16_t CH0 = TSL2561_read_channel(TSL2561_REG_DATA0LOW, TSL2561_REG_DATA0HIGH);
    uint16_t CH1 = TSL2561_read_channel(TSL2561_REG_DATA1LOW, TSL2561_REG_DATA1HIGH);

    // Se CH0 == 0 oppure saturazione, stampo e ritorno 0
    if (CH0 == 0 || CH0 == 0xFFFF || CH1 == 0xFFFF) {
        UART4_WriteString("Sensore NON VA\r\n");
        return 0;
    }

    float ratio = (float)CH1 / (float)CH0;
    float lux   = 0.0f;

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
    if (lux < 0.0f) {
        lux = 0.0f;
    }

    // Stampo su UART
    static char strbuf[32];
    snprintf(strbuf, sizeof(strbuf), "Light: %u LUX\r\n", (unsigned int)lux);
    UART4_WriteString(strbuf);

    return (unsigned int)lux;
}


#define TSL2561_REG_ID 0x0A

uint8_t TSL2561_read_id(void){

    uint8_t id = 0;
    i2c_master_start();
    i2c_master_send(TSL2561_ADDR << 1);
    i2c_master_send(TSL2561_CMD | TSL2561_REG_ID);
    

    i2c_master_restart();
    i2c_master_send((TSL2561_ADDR << 1) | 1);// read
    id = i2c_master_recv(1); // Leggo 1 byte, NACK dopo

    i2c_master_stop();

    return id;
}