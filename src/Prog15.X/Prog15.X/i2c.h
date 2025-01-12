#ifndef I2C_H
#define I2C_H

// Sets up I2C1 at ~100kHz, using RG2 as SCL1 and RG3 as SDA1 (Basys MX3 style).
void i2c_master_setup(void);

// Sends a START condition.
void i2c_master_start(void);

// Sends a RESTART condition.
void i2c_master_restart(void);

// Sends a byte (address or data).
void i2c_master_send(unsigned char byte);

// Receives one byte, then sends either ACK=0 or NACK=1.
unsigned char i2c_master_recv(int ack);

// Sends an ACK=0 or NACK=1 after receiving a byte.
void i2c_master_ack(int val);

// Sends a STOP condition.
void i2c_master_stop(void);

#endif