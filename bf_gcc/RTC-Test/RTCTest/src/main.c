// RTC RV-3049-C2 Test

#include "board.h"

#include <stdio.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "bit_tools.h"
#include "main.h"
#include "uart.h"

#define RTC_READ	0x80
#define RTC_WRITE	0x0
#define RTC_SR		0x10
#define RTC_PON		0x20
#define RTC_RESET	0x10
#define RTC_V1F		0x04
#define RTC_V2F		0x08
#define RTC_AE_S	0x80
#define RTC_THE		0x02

#define RTC_CTRL_INT		0x01
#define RTC_CTRL_INT_FLAG	0x02
#define RTC_CTRL_STATUS		0x03
#define RTC_CTRL_RESET		0x04
#define RTC_CLOCK			0x08
#define RTC_ALARM			0x10
#define RTC_TEMPERATURE		0x20
#define RTC_EPROM_CTRL		0x30

#define CS_ON() Set_bits(PORTB, (1 << PB0))
#define CS_OFF() Clr_bits(PORTB, (1 << PB0))

typedef struct _DateTime {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t weekday;
} TDateTime;

static TDateTime now;


static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }

static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }
	
static int map(int x, int in_min, int in_max, int out_min, int out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


static uint8_t spi_transfer(uint8_t _data)
{
  SPDR = _data;
  while (!(SPSR & _BV(SPIF)))
    ;
  return SPDR;
}

static void rtc_set_datetime(TDateTime* dt) 
{
      
	uint8_t seconds = bin2bcd(dt->second);
	uint8_t minutes = bin2bcd(dt->minute);
	uint8_t hours = bin2bcd(dt->hour);
	//uint8_t weekdays = bin2bcd(0);
	uint8_t days = bin2bcd(dt->day);
	uint8_t months = bin2bcd(dt->month);
	uint8_t years = bin2bcd(dt->year);

	CS_ON();       
	spi_transfer(RTC_CLOCK | RTC_WRITE);
	spi_transfer(seconds);
	spi_transfer(minutes);
	spi_transfer(hours);
	spi_transfer(days);
	spi_transfer(0);      
	spi_transfer(months);
	spi_transfer(years);      
	CS_OFF();
}

static void rtc_get_datetime(TDateTime* dt) 
{
	CS_ON();
	spi_transfer(RTC_CLOCK | RTC_READ);
	dt->second = spi_transfer(0);
	dt->minute = spi_transfer(0);
	dt->hour = spi_transfer(0);      
	dt->day = spi_transfer(0);
	dt->weekday = spi_transfer(0);
	dt->month = spi_transfer(0);
	dt->year = spi_transfer(0);
	CS_OFF();
	
	dt->second = bcd2bin(dt->second);
	dt->minute = bcd2bin(dt->minute);
	dt->hour = bcd2bin(dt->hour);      
	dt->day =	bcd2bin(dt->day);
	dt->weekday = bcd2bin(dt->weekday);
	dt->month = bcd2bin(dt->month);
	dt->year = bcd2bin(dt->year);
}

static void rtc_set_alarm(TDateTime* dt) 
{
	TDateTime alarm_enabled = { 0 };
	
	// mark alarm settings
	alarm_enabled.second = dt->second & RTC_AE_S;
	alarm_enabled.minute = dt->minute & RTC_AE_S;
	alarm_enabled.hour = dt->hour & RTC_AE_S;
	alarm_enabled.weekday = dt->weekday & RTC_AE_S;
	alarm_enabled.day = dt->day & RTC_AE_S;
	alarm_enabled.month = dt->month & RTC_AE_S;
	alarm_enabled.year = dt->year & RTC_AE_S;
	
	// clear alarm settings
	dt->second &= ~RTC_AE_S;
	dt->minute &= ~RTC_AE_S;
	dt->hour &= ~RTC_AE_S;
	dt->day &= ~RTC_AE_S;
	dt->month &= ~RTC_AE_S;
	dt->year &= ~RTC_AE_S;
	
	uint8_t seconds = bin2bcd(dt->second);
	uint8_t minutes = bin2bcd(dt->minute);
	uint8_t hours = bin2bcd(dt->hour);
	//uint8_t weekdays = bin2bcd(0);
	uint8_t days = bin2bcd(dt->day);
	uint8_t months = bin2bcd(dt->month);
	uint8_t years = bin2bcd(dt->year - 2000);

	CS_ON();       
	spi_transfer(RTC_ALARM | RTC_WRITE);
	spi_transfer(seconds | alarm_enabled.second);	
	spi_transfer(minutes | alarm_enabled.minute);
	spi_transfer(hours | alarm_enabled.hour);
	spi_transfer(days | alarm_enabled.day);
	spi_transfer(0);      
	spi_transfer(months | alarm_enabled.month);
	spi_transfer(years | alarm_enabled.year);
	CS_OFF();
}

static void rtc_clear_alarm_int(void)
{
	CS_ON();
	spi_transfer(RTC_CTRL_INT_FLAG | RTC_WRITE);
	spi_transfer(0);
	CS_OFF();	
}

static int rtc_get_temperature(void)
{
	CS_ON();
	spi_transfer(RTC_TEMPERATURE | RTC_READ);
	uint8_t data = spi_transfer(0);	
	CS_OFF();
	
	return map(data, 0, 250, -60, 190);
}

static void rtc_debug(void)
{
	uint8_t reg[26] = {
		0x00, 0x01, 0x02, 0x03, 0x04,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
		0x18, 0x19,
		0x20,
		0x30, 0x31, 0x32, 0x33
	};
	
	for (uint8_t i = 0; i < 26; i++)
	{
		CS_ON();
		spi_transfer(reg[i] | RTC_READ);
		uint8_t d = spi_transfer(0);
		CS_OFF();
	
		char buf[20] = { 0 };
		sprintf(buf, "Reg 0x%02x: 0x%02x\r\n", reg[i], d);
		uart_putchars(buf, 20);
		_delay_ms(100);
	}
}

int main(void)
{			
	cli();
	
	board_init();
	uart_init();
	
	sei();

	uart_putchars("RTC Test\r\n", 10);
	
	DDRB = 0;
	// AVR-PIN	Signal						RTC-PIN
	// PB0		CS		Chipselect			3
	// PB3		MOSI	Master Output		9
	// PB4		MISO	Master Input		5
	// PB5		SCK		Serial Clock		4
	
	Set_bits(DDRB,  (1 << PB0) | (1 << PB3) | (1 << PB5) | (1 << PB2));
	
	CS_OFF();
	
	SPCR = (1<<SPE) | (1 << MSTR) | (0 << SPI2X) | (0 << SPR1) | (0 << SPR0);
	
	// RTC Software Reset
	CS_ON();
	spi_transfer(RTC_CTRL_RESET | RTC_WRITE);
	spi_transfer(RTC_RESET);
	CS_OFF();
	
	//debug_rtc();
	_delay_ms(100);
	
	// RTC Status
	CS_ON();
	uint8_t ctrl_Status = spi_transfer(RTC_CTRL_STATUS);
	CS_OFF();
	
    if(ctrl_Status & RTC_PON) {
		//clear POWER-ON bit
		ctrl_Status &= ~RTC_PON;
        
		CS_ON();
		spi_transfer(RTC_CTRL_STATUS | RTC_WRITE);
		spi_transfer(ctrl_Status);
		CS_OFF();
    }
	  
    if(ctrl_Status & RTC_SR) {
		//clear SELF-RECOVERY bit
		ctrl_Status &= ~RTC_SR;
        
		CS_ON();
		spi_transfer(RTC_CTRL_STATUS | RTC_WRITE);
		spi_transfer(ctrl_Status);
		CS_OFF();
    }
	
	CS_ON();
	spi_transfer(RTC_CTRL_INT_FLAG | RTC_WRITE);
	spi_transfer(0);
	CS_OFF();
	
	CS_ON();
	spi_transfer(RTC_CTRL_INT | RTC_WRITE);
	spi_transfer(1);
	CS_OFF();
	
	CS_ON();
	spi_transfer(RTC_EPROM_CTRL | RTC_WRITE);
	spi_transfer(RTC_THE);
	CS_OFF();
	
	
	// set datetime	
	now.year = 16;
	now.month = 5;
	now.day = 12;
	
	now.hour = 12;
	now.minute = 00;
	now.second = 00;	
	rtc_set_datetime(&now);
	
	
	// set alarm
	TDateTime alarm = { 0 };	
	alarm.second = 5 | RTC_AE_S;
	//alarm.minute = 0 | RTC_AE_S;
	rtc_set_alarm(&alarm);
	
	
	rtc_debug();
	
	
	uint8_t s = 0;
	
	for (;;)
	{
		rtc_get_datetime(&now);
		
		int temperature = rtc_get_temperature();						
				
		if(now.second != s) {
			s = now.second;
			
			char buf[50] = { 0 };		
			sprintf(buf, "%d.%d.%d - %d:%d:%d  Temp:%d\r\n", now.day, now.month, now.year, now.hour, now.minute, now.second, temperature);
		
			uart_putchars(buf, 50);
			
			if(s == (alarm.second + 1))
			{
				rtc_clear_alarm_int();		
			}
		}
	}

}


