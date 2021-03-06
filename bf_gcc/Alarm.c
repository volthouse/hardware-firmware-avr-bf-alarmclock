//*****************************************************************************
//
//  File........: Alarm.c
//
//  Author(s)...: PDI
//
//  Target(s)...: ATmega169
//
//  Compiler....: AVR-GCC 4.1.1; avr-libc 1.4.5
//
//  Description.: Alarm
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20133029 - 1.0  - Created                                       - PDI
//*****************************************************************************

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "button.h"
#include "main.h"
#include "Alarm.h"
#include "LCD_functions.h"
#include "BCD.h"
#include "RTC.h"
#include "timer0.h"
#include "sound.h"
#include "usart.h"

/**
 *#define CONCAT(a,b) a##b
 *#ifdef USE_PINx_TOGGLE
 *#  define TOGGLE(a,b) CONCAT(PIN,a) = (1 << (b))
 *#else
 *#  define TOGGLE(a,b) CONCAT(PORT,a) ^= (1 << (b))
 *#endif
 */


#define HOUR       0
#define MINUTE     1

volatile uint8_t  gALARMMINUTE;
volatile uint8_t  gALARMHOUR;
volatile uint8_t  gALARM;
volatile uint8_t  gALARM_MODE;

char PowerSaveTimeout1 = 3;

/******************************************************************************
*
*   Function name:  Alarm_init
*
*   returns:        none
*
*   parameters:     none
*
*   Purpose:        Initialization
*
*******************************************************************************/
void Alarm_init(void)
{
    // initial time and date setting
    gALARMMINUTE   = 0;
    gALARMHOUR     = 12;
	gALARM 		   = FALSE;
	gALARM_MODE    = ALARM_MODE_OFF;
}

/******************************************************************************
*
*   Function name:  CheckAlarm
*
*   returns:        none
*
*   parameters:     none
*
*   Purpose:        check alarm time, set Alram if time equals alarm time
*
*******************************************************************************/
void CheckAlarm(void)
{
	if(gALARM_MODE != ALARM_MODE_OFF && !gALARM && gSECOND == 0 && gMINUTE == gALARMMINUTE && gHOUR == gALARMHOUR)
	{
		char day1 = -1;
		char day2 = -1;
		
		switch(gALARM_MODE)
		{			
			case ALARM_MODE_1_5:
				day1 = 0;
				day2 = 4;
				break;
			case ALARM_MODE_6_7:
				day1 = 5;
				day2 = 6;
				break;
			default:
				return;
		} 
		
		char day = Dayofweek(gDAY, gMONTH, gYEAR);
		
		if(day >= day1 && day <= day2)
		{
			gALARM = TRUE;
            gPowerSaveTimer = 0;		
			PlayAlarm();
		}
	}
}

/******************************************************************************
*
*   Function name:  OnAlarm
*
*   returns:        none
*
*   parameters:     none
*
*   Purpose:        called on alarm, stop playing sound on key pressed
*
*******************************************************************************/
char OnAlarm(char input)
{
	if (input != KEY_NULL)
	{
		StopPlayAlarm();
	}
    
    if(gPowerSaveTimer >= PowerSaveTimeout1)
    {
        StopPlayAlarm();
    }

    if (!gPlaying)
    {
        return ST_AVRBF;
	}
    
	return ST_ON_ALARM;
}


/******************************************************************************
*
*   Function name:  IsAlarmActiveToday
*
*   returns:        true, if alarm active, else false
*
*   parameters:     none
*
*   Purpose:        determine, if alarm today active
*
*******************************************************************************/
char IsAlarmActiveToday()
{
    char day1 = -1;
    char day2 = -1;
    switch(gALARM_MODE)
    {			
        case ALARM_MODE_1_5:
            day1 = 0;
            day2 = 4;
            break;
        case ALARM_MODE_6_7:
            day1 = 5;
            day2 = 6;
            break;
        default:
            return 0;
    } 
    
    char day = Dayofweek(gDAY, gMONTH, gYEAR);
    
    return day >= day1 && day <= day2;
}

/*****************************************************************************
*
*   Function name : ShowAlarm
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Shows the alarm time on the LCD
*
*****************************************************************************/
char ShowAlarm(char input)
{
    uint8_t HH, HL, MH, ML;

    if (gClockFormat == CLOCK_12)    // if 12H clock
        HH = CHAR2BCD2(gTBL_CLOCK_12[gALARMHOUR]);   
    else
        HH = CHAR2BCD2(gALARMHOUR);
        
    HL = (HH & 0x0F) + '0';
    HH = (HH >> 4) + '0';

    MH = CHAR2BCD2(gALARMMINUTE);
    ML = (MH & 0x0F) + '0';
    MH = (MH >> 4) + '0';
	
	LCD_putc(0, 'A');
	LCD_putc(1, 'L');
    LCD_putc(2, HH);
    LCD_putc(3, HL);
    LCD_putc(4, MH);
    LCD_putc(5, ML);
    LCD_putc(6, '\0');

    LCD_Colon(1);

    LCD_UpdateRequired(TRUE, 0);

    if (input == KEY_PREV)
        return ST_ALARM_TIME;
    else if (input == KEY_NEXT)
        return ST_ALARM_TIME_ADJUST;
      
    return ST_ALARM_TIME_FUNC;
}


/*****************************************************************************
*
*   Function name : SetAlarm
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Adjusts the alarm time
*
*****************************************************************************/
char SetAlarm(char input)
{
    static char enter_function = 1;
    static uint8_t time[3];
    static uint8_t mode = HOUR;
    uint8_t HH, HL, MH, ML;

    if (enter_function)
    {
        time[HOUR] = gALARMHOUR;
        time[MINUTE] = gALARMMINUTE;
    }

    if (gClockFormat == CLOCK_12)    // if 12H clock
        HH = CHAR2BCD2(gTBL_CLOCK_12[time[HOUR]]);
    else
        HH = CHAR2BCD2(time[HOUR]);
        
    HL = (HH & 0x0F) + '0';
    HH = (HH >> 4) + '0';

    MH = CHAR2BCD2(time[MINUTE]);
    ML = (MH & 0x0F) + '0';
    MH = (MH >> 4) + '0';

	LCD_putc(0, 'A');
	LCD_putc(1, 'L');
    LCD_putc(2, HH | ((mode == HOUR) ? 0x80 : 0x00));
    LCD_putc(3, HL | ((mode == HOUR) ? 0x80 : 0x00));
    LCD_putc(4, MH | ((mode == MINUTE) ? 0x80 : 0x00));
    LCD_putc(5, ML | ((mode == MINUTE) ? 0x80 : 0x00));
    LCD_putc(6, '\0');

    LCD_Colon(0);

    if (input != KEY_NULL)
        LCD_FlashReset();

    LCD_UpdateRequired(TRUE, 0);
    
    enter_function = 1;

    // Increment/decrement hours, minutes or seconds
    if (input == KEY_PLUS)
        time[mode]++;
    else if (input == KEY_MINUS)
        time[mode]--;
    else if (input == KEY_PREV)
    {
        if (mode == HOUR)
            mode = MINUTE;
        else
            mode--;
    }
    else if (input == KEY_NEXT)
    {
        if (mode == MINUTE)
            mode = HOUR;
        else
            mode++;
    }
    else if (input == KEY_ENTER)
    {
        // store the temporary adjusted values to the global variables
        cli(); // mt __disable_interrupt();
        gALARMHOUR = time[HOUR];
        gALARMMINUTE = time[MINUTE];
        sei(); // mt __enable_interrupt();
        mode = HOUR;
        return ST_ALARM_TIME_FUNC;
    }

    /* OPTIMIZE: Can be solved by using a modulo operation */
    if (time[HOUR] == 255)
        time[HOUR] = 23;
    if (time[HOUR] > 23)
        time[HOUR] = 0;

    if (time[MINUTE] == 255)
        time[MINUTE] = 59;
    if (time[MINUTE] > 59)
        time[MINUTE] = 0;

    enter_function = 0;
    return ST_ALARM_TIME_ADJUST_FUNC;
}

/******************************************************************************
*
*   Function name:  ShowAlarmMode
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose:        Shows the alarm mode
*
*******************************************************************************/
char ShowAlarmMode(char input)
{
    switch(gALARM_MODE)
	{
		case ALARM_MODE_OFF:
			LCD_puts_f(PSTR("Off"),0);
			break;
		case ALARM_MODE_1_5:
			LCD_puts_f(PSTR("1-5"),0);
			break;
		case ALARM_MODE_6_7:
			LCD_puts_f(PSTR("6-7"),0);
			break;
	}    

    LCD_UpdateRequired(TRUE, 0);

    if (input == KEY_PREV)
        return ST_ALARM_TIME_MODE;
    else if (input == KEY_NEXT)
        return ST_ALARM_TIME_MODE_ADJUST;
      
    return ST_ALARM_TIME_MODE_FUNC;
}

/******************************************************************************
*
*   Function name:  SetAlarmMode
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose:        Sets the alarm mode
*
*******************************************************************************/
char SetAlarmMode(char input)
{
    switch(gALARM_MODE)
	{
		case ALARM_MODE_OFF:
			LCD_puts_f(PSTR("Off"),0);
            LCD_AlarmSign(0);
			break;
		case ALARM_MODE_1_5:
			LCD_puts_f(PSTR("1-5"),0);
            LCD_AlarmSign(1);
			break;
		case ALARM_MODE_6_7:
			LCD_puts_f(PSTR("6-7"),0);
            LCD_AlarmSign(1);
			break;
	}    

    LCD_UpdateRequired(TRUE, 0);
    
    // Increment/decrement mode
    if (input == KEY_PLUS && gALARM_MODE < ALARM_MODE_6_7)
        gALARM_MODE++;
    else if (input == KEY_MINUS && gALARM_MODE > ALARM_MODE_OFF)
        gALARM_MODE--;
    else if (input == KEY_ENTER)
    {
        return ST_ALARM_TIME_MODE_FUNC;
    }
    return ST_ALARM_TIME_MODE_ADJUST_FUNC;
}