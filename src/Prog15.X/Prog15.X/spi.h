/* 
 * File:   newfile.h
 * Author: giada
 *
 * Created on December 9, 2024, 3:38 PM
 */

#include <p32xxxx.h>

#define CS LATFbits.LATF8 // select line for Serial Flash ROM
#define TCS TRISFbits.TRISF8 // tris control for CS pin

void initSPI1(void);
void EraseFlash(void);
int writeSPI1( int i);
int getFlashID(void);
void writeFlashMem(int addr, short byte);
int readFlashMem(int addr);

