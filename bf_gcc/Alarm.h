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

extern volatile uint8_t  gALARMMINUTE;
extern volatile uint8_t  gALARMHOUR;
extern volatile uint8_t	 gALARM;
extern volatile char	 gALARM_MODE;
//mtE

#define CLOCK_24    1
#define CLOCK_12    0

#define ALARM_MODE_OFF	0
#define	ALARM_MODE_1_5	1
#define	ALARM_MODE_6_7	2

//  Function declarations
void Alarm_init(void);            //initialize the Timer Counter 2 in asynchron operation
char ShowAlarm(char input);
char SetAlarm(char input);
void CheckAlarm(void)
char OnAlarm(char input);
void Play_Alarm(void);

char ShowAlarmMode(char input);
char SetAlarmMode(char input);
