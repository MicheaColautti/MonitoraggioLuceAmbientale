

#define PMDATA  PMDIN           // redefinition of the PMP data bit

#define HLCD 16 // LCD width=16 characters
#define VLCD 2 // LCD height=2 rows
#define LCD_DATAbits 0x0ff      // Port E bits 0 - 7
#define LCD_DB0     (1<<0)
#define LCD_DB1     (1<<1)
#define LCD_DB2     (1<<2)
#define LCD_DB3     (1<<3)
#define LCD_DB4     (1<<4)
#define LCD_DB5     (1<<5)
#define LCD_DB6     (1<<6)
#define LCD_DB7     (1<<7)

#define ENpin       (1<<4)      // RD4
#define LCD_HIGH    1
#define LCD_LOW     0
#define LCD_EN(a)	{if(a) LATDSET = ENpin; else LATDCLR = ENpin;}	

#define RWpin       (1<<5)      // RD5
#define LCD_WR      0
#define LCD_RD      1
#define LCD_RW(a)	{if(a) LATDSET = RWpin; else LATDCLR = RWpin;}

#define RSpin       (1<<15)     // RB15
#define LCDDATA     1           // address of data register
#define LCDCMD      0           // address of command register
#define LCD_RS(a)	{if(a) LATBSET = RSpin; else LATBCLR = RSpin;}



#define putLCD(d)   writeLCD( LCDDATA, (d))
#define cmdLCD(c)   writeLCD( LCDCMD, (c))
#define clrLCD()    writeLCD( LCDCMD, 1)
#define homeLCD()   writeLCD( LCDCMD, 2)
#define setLCDG(a)  writeLCD( LCDCMD, (a & 0x3F) | 0x40)
#define setLCDC(a)  writeLCD( LCDCMD, (a & 0x7F) | 0x80)
#define busyLCD()   (readLCD( LCDCMD) & 0x80)
#define addrLCD()   (readLCD( LCDCMD) & 0x7F)
#define getLCD()    readLCD( LCDDATA)

void initLCD( void);
void writeLCD( int addr, char c);
char readLCD( int addr);
void putsLCD( char *s);

