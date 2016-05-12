
#ifndef UART_H_
#define UART_H_

void uart_init(void);
void uart_putchar(uint8_t data);
bool uart_putchar_sync(uint8_t data);
bool uart_char_available(void);
uint8_t uart_getchar(void);

void uart_putchars(char* buf, uint8_t size);

#endif /* UART_H_ */