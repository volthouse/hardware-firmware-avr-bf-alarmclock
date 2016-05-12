#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <avr/io.h>


#if BIBOBOX

#define F_CPU 16000000UL

#define BAUD 57600
#define BAUD_TOL 2

#elif STK300

#define F_CPU 4000000UL
#define BAUD 9600
#define BAUD_TOL 2

#elif STK500

#define F_CPU 8000000UL

#define BAUD 9600
#define BAUD_TOL 2

#endif

#if BIBOBOX

#define	PIN_LED1				1
#define	PIN_LED1_MASK			1 << PIN_LED1
#define	PIN_LED1_PORT			PORTC
#define	PIN_LED1_DDR			DDRC

#define	PIN_LED2				0
#define	PIN_LED2_MASK			1 << PIN_LED2
#define	PIN_LED2_PORT			PORTC
#define	PIN_LED2_DDR			DDRC

#define	PIN_LED3				3
#define	PIN_LED3_MASK			1 << PIN_LED3
#define	PIN_LED3_PORT			PORTC
#define	PIN_LED3_DDR			DDRC

#define	PIN_LED4				2
#define	PIN_LED4_MASK			1 << PIN_LED4
#define	PIN_LED4_PORT			PORTC
#define	PIN_LED4_DDR			DDRC

#define PIN_TXEN				3
#define	PIN_TXEN_MASK			1 << PIN_TXEN
#define	PIN_TXEN_PORT			PORTD
#define	PIN_TXEN_DDR			DDRD

#define PIN_REL1				6
#define PIN_REL1_MASK			1 << PIN_REL1
#define PIN_REL1_PORT			PORTD
#define PIN_REL1_DDR			DDRD

#define PIN_REL2				7
#define PIN_REL2_MASK			1 << PIN_REL2
#define PIN_REL2_PORT			PORTD
#define PIN_REL2_DDR			DDRD

#define PIN_REL3				5
#define PIN_REL3_MASK			1 << PIN_REL3
#define PIN_REL3_PORT			PORTD
#define PIN_REL3_DDR			DDRD

#define PIN_REL4				4
#define PIN_REL4_MASK			1 << PIN_REL4
#define PIN_REL4_PORT			PORTD
#define PIN_REL4_DDR			DDRD

#define PIN_CID0				0
#define PIN_CID0_MASK			1 << PIN_CID0
#define PIN_CID0_PORT			PINB
#define PIN_CID0_DDR			DDRB

#define PIN_CID1				1
#define PIN_CID1_MASK			1 << PIN_CID1
#define PIN_CID1_PORT			PINB
#define PIN_CID1_DDR			DDRB

#define PIN_CID2				2
#define PIN_CID2_MASK			1 << PIN_CID2
#define PIN_CID2_PORT			PINB
#define PIN_CID2_DDR			DDRB

#define PIN_CID3				2
#define PIN_CID3_MASK			1 << PIN_CID3
#define PIN_CID3_PORT			PIND
#define PIN_CID3_DDR			DDRD

#define PINS_LEDS_PORT			PORTC
#define PINS_LEDS_MASK			PIN_LED1_MASK | PIN_LED2_MASK | PIN_LED3_MASK | PIN_LED4_MASK
#define PINS_LEDS_DDR			DDRC

#define PINS_REL_PORT			PORTD
#define PINS_REL_MASK			PIN_REL1_MASK | PIN_REL2_MASK | PIN_REL3_MASK | PIN_REL4_MASK
#define PINS_REL_DDR			DDRD

#elif STK300 || STK500

#define	PIN_LED1				5
#define	PIN_LED1_MASK			1 << PIN_LED1
#define	PIN_LED1_PORT			PORTB
#define	PIN_LED1_DDR			DDRB

#define	PIN_LED2				4
#define	PIN_LED2_MASK			1 << PIN_LED2
#define	PIN_LED2_PORT			PORTB
#define	PIN_LED2_DDR			DDRB

#define	PIN_LED4				5
#define	PIN_LED4_MASK			1 << PIN_LED4
#define	PIN_LED4_PORT			PORTC
#define	PIN_LED4_DDR			DDRC

#define PIN_REL1				2
#define PIN_REL1_MASK			1 << PIN_REL1
#define PIN_REL1_PORT			PORTB
#define PIN_REL1_DDR			DDRB

#define PIN_REL2				3
#define PIN_REL2_MASK			1 << PIN_REL2
#define PIN_REL2_PORT			PORTB
#define PIN_REL2_DDR			DDRB

#define PIN_REL3				1
#define PIN_REL3_MASK			1 << PIN_REL3
#define PIN_REL3_PORT			PORTB
#define PIN_REL3_DDR			DDRB

#define PIN_REL4				0
#define PIN_REL4_MASK			1 << PIN_REL4
#define PIN_REL4_PORT			PORTB
#define PIN_REL4_DDR			DDRB

#define PINS_REL_PORT			PORTB
#define PINS_REL_MASK			PIN_REL1_MASK | PIN_REL2_MASK | PIN_REL3_MASK | PIN_REL4_MASK
#define PINS_REL_DDR			DDRB

#define PINS_BININ_PORT			PORTD
#define PINS_BININ_DDR			DDRD

#endif


void board_init(void);


#endif // USER_BOARD_H
