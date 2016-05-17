//***************************************************************************
//
//  File........: main.c
//
//  Author(s)...: 
//
//  Target(s)...: ATmega169
//
//  Compiler....: AVR-GCC 4.3.2; avr-libc 1.6.6 (WinAVR 20090313)
//
//  Description.: AVR Butterfly Alarm Clock based on Atmels BF Code
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                        - SIGN.
//
//  20130401 - 1.0  - Created
//
//***************************************************************************

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/version.h>

#if __AVR_LIBC_VERSION__ < 10405UL
#warning "avr-libc >= version 1.4.5 recommended"
#warning "This code has not been tested with older versions."
#endif

#include "main.h"
#include "LCD_functions.h"
#include "LCD_driver.h"
#include "button.h"
#include "RTC.h"
#include "Alarm.h"
#include "timer0.h"
#include "BCD.h"
#include "usart.h"
#include "sound.h"
#include "ADC.h"
#include "dataflash.h"
#include "test.h"
#include "menu.h"
#include "rtcRV3049.h"

#define pLCDREG_test (*(char *)(0xEC))

char PowerSaveTimeout = 5;      // Initial value, enable power save mode after 30 min
BOOL AutoPowerSave    = TRUE;   // Variable to enable/disable the Auto Power Save func
volatile BOOL gKeyClickStatus  = FALSE;  // Variable to enable/disable keyclick

char PowerSave = FALSE;

unsigned char state;  // holds the current state, according to "menu.h"


/*****************************************************************************
*
*   Function name : main
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Contains the main loop of the program
*
*****************************************************************************/
__attribute__ ((OS_main)) int main(void)
{    
    unsigned char nextstate;
    PGM_P statetext;
    char (*pStateFunc)(char);
    char input;
    uint8_t i, j;
    char buttons;
    char last_buttons;

    last_buttons='\0';  // mt

    state = ST_AVRBF;
    nextstate = ST_AVRBF;
    statetext = NULL;
    pStateFunc = ShowInfo;


    // Program initalization
    Initialization();
    sei();

    for (;;)            // Main loop
    {
        if(!PowerSave)          // Do not enter main loop in power-save
        {
            // Plain menu text
            if (statetext)
            {
                LCD_puts_f(statetext, 1);
                LCD_Colon(0);
                statetext = NULL;
            }
 
            input = getkey();           // Read buttons
    
            if (pStateFunc)
            {
                // When in this state, we must call the state function
                nextstate = pStateFunc(input);
            }
            else if (input != KEY_NULL)
            {
                // Plain menu, clock the state machine
                nextstate = StateMachine(state, input);
            }
    
            if (nextstate != state)
            {
                state = nextstate;
                for (i=0; (j=pgm_read_byte(&menu_state[i].state)); i++)
                {
                    if (j == state)
                    {
                        statetext =  (PGM_P) pgm_read_word(&menu_state[i].pText);
                        pStateFunc = (PGM_VOID_P) pgm_read_word(&menu_state[i].pFunc);
                        break;
                    }
                }
            }
        }
        
        
        //enable ATmega169 power save modus if autopowersave
        if(AutoPowerSave)
        {
            if(gPowerSaveTimer >= PowerSaveTimeout)
            {
                state = ST_AVRBF;
                gPowerSaveTimer = 0;
                PowerSave = TRUE;
            }
        }
        
        
        // If the joystick is held in the UP and DOWN position at the same time,
        // activate test-mode
        if( !(PINB & (1<<PINB7)) && !(PINB & (1<<PINB6)) ) {
            Test();
        }
        
        // Check if the joystick has been in the same position for some time, 
        // then activate auto press of the joystick
        buttons = (~PINB) & PINB_MASK;
        buttons |= (~PINE) & PINE_MASK;
        
        if( buttons != last_buttons ) 
        {
            last_buttons = buttons;
            gAutoPressJoystick = FALSE;
        }
        else if( buttons )
        {
            if( gAutoPressJoystick == TRUE)
            {
                PinChangeInterrupt();
                gAutoPressJoystick = AUTO;
            }
            else    
                gAutoPressJoystick = AUTO;
        }

        if(gALARM)
		{
			gALARM = FALSE;
			state = ST_ON_ALARM;
			nextstate = ST_AVRBF;
			statetext = MT_ALARM;
			pStateFunc = &OnAlarm;
			
			if(PowerSave)
            {     
				PowerSave = FALSE;
				
				for(i = 0; i < 20; i++ ) // set all LCD segment register to the variable ucSegments
				{
					*(&pLCDREG_test + i) = 0x00;
				}
				
				sbiBF(LCDCRA, 7);           // enable LCD
				input = getkey();           // Read buttons                
            }
		}        
        
        // go to SLEEP
        if(!gPlaying && !gALARM)              // Do not enter Power save if using UART or playing tunes
        {
            if(PowerSave)
                cbiBF(LCDCRA, 7);             // disable LCD

            set_sleep_mode(SLEEP_MODE_PWR_SAVE);
            sleep_mode();

            if(PowerSave)
            {
                if(!(PINB & 0x40))              // press UP to wake from SLEEP
                {
                    PowerSave = FALSE;
                    
                    for(i = 0; i < 20; i++ ) // set all LCD segment register to the variable ucSegments
                    {
                        *(&pLCDREG_test + i) = 0x00;
                    }
                    
                    sbiBF(LCDCRA, 7);           // enable LCD
                    input = getkey();           // Read buttons
                }
            }
        }
        else
        {
           set_sleep_mode(SLEEP_MODE_IDLE);
           sleep_mode();
        }
		
		
		
    }
    return 0;
}




/*****************************************************************************
*
*   Function name : StateMachine
*
*   Returns :       nextstate
*
*   Parameters :    state, stimuli
*
*   Purpose :       Shifts between the different states
*
*****************************************************************************/
unsigned char StateMachine(char state, unsigned char stimuli)
{
    unsigned char nextstate = state;    // Default stay in same state
    unsigned char i, j;

    for (i=0; ( j=pgm_read_byte(&menu_nextstate[i].state) ); i++ )
    {
        if ( j == state && 
             pgm_read_byte(&menu_nextstate[i].input) == stimuli)
        {
            nextstate = pgm_read_byte(&menu_nextstate[i].nextstate);
            break;
        }
    }

    return nextstate;
}




/*****************************************************************************
*
*   Function name : Initialization
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Initializate the different modules
*
*****************************************************************************/
void Initialization(void)
{
    char tst;           // dummy

    OSCCAL_calibration();       // calibrate the OSCCAL byte
        
    CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable

    // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
    CLKPR = (1<<CLKPS1) | (1<<CLKPS0);

    // Disable Analog Comparator (power save)
    ACSR = (1<<ACD);

    // Disable Digital input on PF0-2 (power save)
    DIDR0 = (7<<ADC0D);

    PORTB = (15<<PB0);       // Enable pullup on 
    PORTE = (15<<PE4);

    sbiBF(DDRB, 5);               // set OC1A as output
    sbiBF(PORTB, 5);              // set OC1A high
            
    Button_Init();              // Initialize pin change interrupt on joystick
    
    Alarm_init();
    
    RTC_init();                 // Start timer2 asynchronous, used for RTC clock

    Timer0_Init();              // Used when playing music etc.

    USART_Init(12);             // Baud rate = 9600bps
    
    DF_SPI_init();              // init the SPI interface to communicate with the DataFlash
    
    tst = Read_DF_status();

    DF_CS_inactive;             // disable DataFlash
        
    LCD_Init();                 // initialize the LCD
	
	rtc_test();
}





/*****************************************************************************
*
*   Function name : BootFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Reset the ATmega169 which will cause it to start up in the 
*                   Bootloader-section. (the BOOTRST-fuse must be programmed)
*
*****************************************************************************/

char BootFunc(char input)
{
    static char enter = 1;
    
    if(enter)
    {
        enter = 0;
        LCD_puts_f(PSTR("Jump to bootloader"), 1);
    }
    else if(input == KEY_ENTER)
    {
        WDTCR = (1<<WDCE) | (1<<WDE);     //Enable Watchdog Timer to give reset
        while(1);   // wait for watchdog-reset, since the BOOTRST-fuse is 
                    // programmed, the Boot-section will be entered upon reset.
    }
    else if (input == KEY_PREV)
    {
        enter = 1;
        return ST_OPTIONS_BOOT;
    }
    
    return ST_OPTIONS_BOOT_FUNC;
}





/*****************************************************************************
*
*   Function name : PowerSaveFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Enable power save
*
*****************************************************************************/

char PowerSaveFunc(char input)
{
    static char enter = 1;    
    
    if(enter)
    {
        enter = 0;
        LCD_puts_f(PSTR("Press enter to sleep"), 1);
    }
    else if(input == KEY_ENTER)
    {
        PowerSave = TRUE;
        enter = 1;
        return ST_AVRBF;
    }
    else if (input == KEY_PREV)
    {
        enter = 1;
        return ST_OPTIONS_POWER_SAVE;
    }
        
    return ST_OPTIONS_POWER_SAVE_FUNC;

}




/*****************************************************************************
*
*   Function name : AutoPower
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Enable/Disable auto power save
*
*****************************************************************************/

static void AutoPowerShowMin(void)
{
	char PH,PL; 
	
	PH = CHAR2BCD2(PowerSaveTimeout);
	PL = (PH & 0x0F) + '0';
	PH = (PH >> 4) + '0';
	
	LCD_putc(0, 'M');
	LCD_putc(1, 'I');
	LCD_putc(2, 'N');
	LCD_putc(3, ' ');
	LCD_putc(4, PH);
	LCD_putc(5, PL);
	LCD_putc(6, '\0');
	
	LCD_UpdateRequired(TRUE, 0);
}

char AutoPower(char input)
{
    static char enter = 1;    
    
    if(enter)
    {
        enter = 0;
        
        if(AutoPowerSave)  
            AutoPowerShowMin();
        else
            LCD_puts_f(PSTR("Off"),1);     
    }
    else if(input == KEY_ENTER)
    {
         enter = 1;

         return ST_OPTIONS_AUTO_POWER_SAVE;
    }
    else if (input == KEY_PLUS)
    {

        PowerSaveTimeout += 5;
         
        if(PowerSaveTimeout > 90)
        {
            PowerSaveTimeout = 90;
        }
        else
        {    
            AutoPowerSave = TRUE;
            AutoPowerShowMin();
        }
    }
    else if (input == KEY_MINUS)
    {
        if(PowerSaveTimeout)
            PowerSaveTimeout -= 5;

        if(PowerSaveTimeout < 5)
        {
            AutoPowerSave = FALSE;
            PowerSaveTimeout = 0;
            LCD_puts_f(PSTR("Off"),1);
        }
        else
        {   
            AutoPowerSave = TRUE;
            AutoPowerShowMin();
        }
    }
        
    return ST_OPTIONS_AUTO_POWER_SAVE_FUNC;    
}




/*****************************************************************************
*
*   Function name : KeyClick
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Enable/Disable keyclick
*
*****************************************************************************/
char KeyClick(char input)
{
	static uint8_t enter = 1;
	uint8_t show;

	if ( enter ) {
		enter = 0;
		show  = 1;
	}
	else {
		show = 0;
	}
		
	if (input == KEY_ENTER)
	{
		enter = 1;
		return ST_OPTIONS_KEYCLICK;
	}

	if ( (input == KEY_PLUS) || (input == KEY_MINUS) ) {
		gKeyClickStatus = ~gKeyClickStatus;
		show = 1;
	}

	if ( show ) {
		if ( gKeyClickStatus )
			LCD_puts_f(PSTR("On"),1);
		else
			LCD_puts_f(PSTR("Off"),1);
		LCD_UpdateRequired(TRUE, 0);
	}

	return ST_OPTIONS_KEYCLICK_FUNC;
}




/*****************************************************************************
*
*   Function name : Delay
*
*   Returns :       None
*
*   Parameters :    unsigned int millisec
*
*   Purpose :       Delay-loop
*
*****************************************************************************/
void Delay(unsigned int millisec)
{
	uint8_t i;

	while (millisec--) {
		for (i=0; i<125; i++) {
			asm volatile ("nop"::);
		}
	}
}



/*****************************************************************************
*
*   Function name : Revision
*
*   Returns :       None
*
*   Parameters :    char input
*
*   Purpose :       Display the software revision
*
*****************************************************************************/
char Revision(char input)
{
    static char enter = 1;
    
    if(enter)
    {
        enter = 0;
        
        LCD_putc(0, 'R');
        LCD_putc(1, 'E');
        LCD_putc(2, 'V');
        LCD_putc(3, (SWHIGH + 0x30));
        LCD_putc(4, (SWLOW + 0x30));
        LCD_putc(5, (SWLOWLOW + 0x30));
        LCD_putc(6, '\0');
        
        LCD_UpdateRequired(TRUE, 0);          
    }
    else if (input == KEY_PREV)
    {
        enter = 1;
        return ST_AVRBF;
    }
    
    return ST_AVRBF_REV;
}

/*****************************************************************************
*
*   Function name : ShowInfo
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Shows time and date
*
*****************************************************************************/
char ShowInfo(char input)
{
    if(gSECOND % 4) {         
        ShowClock(0);
    }
    else
    {
        ShowDate1(0);
    }    
    
    if (input != KEY_NULL)
    {        
        return ST_TIME;
    }
    
    return ST_AVRBF;
}


/*****************************************************************************
*
*   Function name : OSCCAL_calibration
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Calibrate the internal OSCCAL byte, using the external 
*                   32,768 kHz crystal as reference
*
*****************************************************************************/
void OSCCAL_calibration(void)
{
    unsigned char calibrate = FALSE;
    int temp;
    unsigned char tempL;

    CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
    // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
    CLKPR = (1<<CLKPS1) | (1<<CLKPS0);
    
    TIMSK2 = 0;             //disable OCIE2A and TOIE2

    ASSR = (1<<AS2);        //select asynchronous operation of timer2 (32,768kHz)
    
    OCR2A = 200;            // set timer2 compare value 

    TIMSK0 = 0;             // delete any interrupt sources
        
    TCCR1B = (1<<CS10);     // start timer1 with no prescaling
    TCCR2A = (1<<CS20);     // start timer2 with no prescaling

    while((ASSR & 0x01) | (ASSR & 0x04));       //wait for TCN2UB and TCR2UB to be cleared

    Delay(1000);    // wait for external crystal to stabilise
    
    while(!calibrate)
    {
        cli(); // disable global interrupt
        
        TIFR1 = 0xFF;   // delete TIFR1 flags
        TIFR2 = 0xFF;   // delete TIFR2 flags
        
        TCNT1H = 0;     // clear timer1 counter
        TCNT1L = 0;
        TCNT2 = 0;      // clear timer2 counter
           
        while ( !(TIFR2 & (1<<OCF2A)) );   // wait for timer2 compareflag

        TCCR1B = 0; // stop timer1

        sei(); // enable global interrupt
    
        if ( (TIFR1 & (1<<TOV1)) )
        {
            temp = 0xFFFF;      // if timer1 overflows, set the temp to 0xFFFF
        }
        else
        {   // read out the timer1 counter value
            tempL = TCNT1L;
            temp = TCNT1H;
            temp = (temp << 8);
            temp += tempL;
        }
    
        if (temp > 6250)
        {
            OSCCAL--;   // the internRC oscillator runs to fast, decrease the OSCCAL
        }
        else if (temp < 6120)
        {
            OSCCAL++;   // the internRC oscillator runs to slow, increase the OSCCAL
        }
        else
            calibrate = TRUE;   // the interRC is correct

        TCCR1B = (1<<CS10); // start timer1
    }
}

#if 0
/*****************************************************************************
*
*   Function name : watchdog_off_at_init
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Disable the Watchdog
*                   Function disabled by default.
*                   Might be interesting for special bootloaders
*                   (not the default Butterfly bootloader)
*                   This function has been added by Martin Thomas
*
*****************************************************************************/
void watchdog_off_at_init(void) __attribute__((naked)) __attribute__((section(".init3")));
void watchdog_off_at_init(void)
{
	wdt_reset();
	MCUSR = 0;
	wdt_disable();
}
#endif
