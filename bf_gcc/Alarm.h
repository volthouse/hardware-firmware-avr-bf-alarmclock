//*****************************************************************************
//
//  File........: Alarm.h
//
//  Author(s)...: PDI Germany
//
//  Target(s)...: ATmega169
//
//  Description.: Functions for RTC.c
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20130329 - 1.0  - File created                                  - LHM
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas
//
//*****************************************************************************


//mtA
//extern char gSECOND;
//extern char gMINUTE;
//extern char gHOUR;
//extern char gDAY;
//extern char gMONTH;
extern volatile uint8_t  gALARMMINUTE;
extern volatile uint8_t  gALARMHOUR;
extern volatile uint8_t	 gALARM;
//mtE

#define CLOCK_24    1
#define CLOCK_12    0

//  Function declarations
void Alarm_init(void);            //initialize the Timer Counter 2 in asynchron operation
char ShowAlarm(char input);
char SetAlarm(char input);
char CheckAlarm(char input);
char OnAlarm(char input);
void Play_Alarm(void);
