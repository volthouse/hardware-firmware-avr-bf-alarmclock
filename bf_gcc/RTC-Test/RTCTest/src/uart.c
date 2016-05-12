#include "board.h"
#include "ring_buffer.h"
#include "uart.h"
#include "bit_tools.h"
#include "main.h"

#include <string.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>

#define BUFFER_SIZE 24

volatile uint16_t send_data = 0;

uint8_t out_buffer[BUFFER_SIZE];
uint8_t in_buffer[BUFFER_SIZE];

struct ring_buffer ring_buffer_out;
struct ring_buffer ring_buffer_in;

/**
 * \brief Data RX interrupt handler
 *
 * UART Empfang von Daten
 */
ISR(USART_RX_vect)
{
	ring_buffer_put(&ring_buffer_in, UDR0);
}

/**
 * \brief Data TX interrupt handler
 *
 * UART Senden von Daten, wenn USART frei ist
 */
ISR(USART_UDRE_vect)
{
	if (!ring_buffer_is_empty(&ring_buffer_out)) {
		UDR0 = ring_buffer_get(&ring_buffer_out);
	}
	else {
		// no more data to send, turn off data ready interrupt
		UCSR0B &= ~(1 << UDRIE0);
	}
}

inline void uart_init(void)
{
#if defined UBRR0H
	// get the values from the setbaud tool
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
#else
#error "Device is not supported by the driver"
#endif

#if USE_2X
	UCSR0A |= (1 << U2X0);
#endif

	// RX und TX aktivieren, Interrupts Empfang und Sender aktivieren
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0) | (1 << RXCIE0);

	// 8-bit, 2 stop bit, no parity, asynchronous UART
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00) | (1 << USBS0) |
			(0 << UPM01) | (0 << UPM00) | (0 << UMSEL01) |
			(0 << UMSEL00);

	ring_buffer_out = ring_buffer_init(out_buffer, BUFFER_SIZE);
	ring_buffer_in = ring_buffer_init(in_buffer, BUFFER_SIZE);
}

inline void uart_putchar(uint8_t data)
{
	cli();

	if (ring_buffer_is_empty(&ring_buffer_out)) {
		UCSR0B |=  (1 << UDRIE0);
	}
	ring_buffer_put(&ring_buffer_out, data);

	sei();
}

inline bool uart_putchar_sync(uint8_t data)
{
	bool result = false;
	
	cli();
	
	if(UCSR0A & (1<<UDRE0)) {
		UDR0 = data;
		result = true;
	}
	
	sei();
	
	return result;
}

inline uint8_t uart_getchar(void)
{
	return ring_buffer_get(&ring_buffer_in);
}

inline bool uart_char_available(void)
{
	return !ring_buffer_is_empty(&ring_buffer_in);
}

inline void uart_putchars(char* buf, uint8_t size)
{
	for (uint8_t i = 0; i < size; i++)
	{
		char c = buf[i];
		if(c) {
			uart_putchar(buf[i]);
		} else {
			break;
		}
	}
}
