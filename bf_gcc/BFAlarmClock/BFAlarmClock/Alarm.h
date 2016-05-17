//*****************************************************************************
//
//  File........: Alarm.h
//
//  Author(s)...: PDI Germany
//
//  Target(s)...: ATmega169
//
//  Description.: Functions for Alarm.c
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20130329 - 1.0  - File created                                  - PDI
//
//*****************************************************************************

extern volatile uint8_t  gALARMMINUTE;
extern volatile uint8_t  gALARMHOUR;
extern volatile uint8_t	 gALARM;
extern volatile uint8_t	 gALARM_MODE;

#define ALARM_MODE_OFF	0
#define	ALARM_MODE_1_5	1	// Mon. to Fri.
#define	ALARM_MODE_6_7	2	// Sat. to Sun.

//  Function declarations

void Alarm_init(void);            
void CheckAlarm(void);
char OnAlarm(char input);

char ShowAlarm(char input);
char SetAlarm(char input);

char ShowAlarmMode(char input);
char SetAlarmMode(char input);

char IsAlarmActiveToday(void);
