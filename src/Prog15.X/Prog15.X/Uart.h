void UART_ConfigurePins (void) ;
void UART_ConfigureUart () ;
int putU4 (char c) ;
char getU4 (void) ;
void putU4_string (char szData[]) ;
void remap_UART4_pins(void);
void UART4_WriteString(const char *str);
void UART4_ReadString(char* buffer, int maxLength);
void UART4_FlushBuffer(void);

