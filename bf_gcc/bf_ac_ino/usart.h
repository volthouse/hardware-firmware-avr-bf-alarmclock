#ifndef UART_H_
#define UART_H_

#ifdef __cplusplus
extern "C"{
#endif

// #define WITH_TRANSMIT

void USART_Init(unsigned int baudrate);
char Usart_Rx(void);
#ifdef WITH_TRANSMIT
void Usart_Tx(char);
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif
