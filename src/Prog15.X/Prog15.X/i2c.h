/*
 * Title: i2c lib CT header file
 * author: CTa
 * date: 01.12.2022
 *  
 */

#define SLAVE_ADDR  0x1D// 0b0011101 accelerometer
#define MASTER_WRITE 0
#define MASTER_READ 1

extern     unsigned char out_x[2];
extern     unsigned char out_y[2];
extern     unsigned char out_z[2];
extern int id;




void i2c_master_setup(void);
void i2c_master_start(void);
void i2c_master_restart(void);
void i2c_master_send(unsigned char byte);
unsigned char i2c_master_recv(int ack);
void i2c_master_ack(int val);
void i2c_master_stop(void);
void accelertometer_setup(void);
void read_acc_id(void);
void reac_acc_xyz(void);

