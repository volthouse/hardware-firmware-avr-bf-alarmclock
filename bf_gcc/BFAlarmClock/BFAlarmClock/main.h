//Revisions number
#define SWHIGH  0
#define SWLOW   1
#define SWLOWLOW 0

// main.h

void Initialization(void);
unsigned char StateMachine(char state, unsigned char stimuli);
char BootFunc(char input);
char PowerSaveFunc(char input);
char AutoPower(char input);
char KeyClick(char input);
void Delay(unsigned int millisec);
char Revision(char input);
void OSCCAL_calibration(void);

char ShowInfo(char input);

#define BOOL    char

#ifndef FALSE
#define FALSE   0
#define TRUE    (!FALSE)
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#define AUTO    3

// Macro definitions
#define sbiBF(port,bit)  (port |= (1<<bit))   //set bit in port
#define cbiBF(port,bit)  (port &= ~(1<<bit))  //clear bit in port

// Menu state machine states
#define ST_AVRBF                        10
#define ST_AVRBF_REV                    11
#define ST_TIME                         20
#define ST_TIME_CLOCK                   21
#define ST_TIME_CLOCK_FUNC              22
#define ST_TIME_CLOCK_ADJUST            23
#define ST_TIME_CLOCK_ADJUST_FUNC       24
#define ST_TIME_CLOCKFORMAT_ADJUST      25
#define ST_TIME_CLOCKFORMAT_ADJUST_FUNC 36
#define ST_TIME_DATE                    27
#define ST_TIME_DATE_FUNC               28
#define ST_TIME_DATE_ADJUST             29
#define ST_TIME_DATE_ADJUST_FUNC        30
#define ST_TIME_DATEFORMAT_ADJUST       31
#define ST_TIME_DATEFORMAT_ADJUST_FUNC  32
#define ST_MUSIC                        40
#define ST_SOUND_MUSIC                  41
#define ST_MUSIC_SELECT                 42
#define ST_MUSIC_PLAY                   43
#define ST_TEMPERATURE                  60
#define ST_TEMPERATURE_FUNC             61
#define ST_OPTIONS                      90
#define ST_OPTIONS_DISPLAY              91
#define ST_OPTIONS_DISPLAY_CONTRAST     92
#define ST_OPTIONS_DISPLAY_CONTRAST_FUNC 93
#define ST_OPTIONS_BOOT                 94
#define ST_OPTIONS_BOOT_FUNC            95
#define ST_OPTIONS_POWER_SAVE           96
#define ST_OPTIONS_POWER_SAVE_FUNC      97
#define ST_OPTIONS_AUTO_POWER_SAVE      98
#define ST_OPTIONS_AUTO_POWER_SAVE_FUNC 99
#define ST_OPTIONS_KEYCLICK             100
#define ST_OPTIONS_KEYCLICK_FUNC        101

#define ST_ALARM_TIME                   111
#define ST_ALARM_TIME_FUNC              112
#define ST_ALARM_TIME_ADJUST            113
#define ST_ALARM_TIME_ADJUST_FUNC       114
#define ST_ALARM_TIME_CLOCK             115
#define ST_ALARM_TIME_MODE              116
#define ST_ALARM_TIME_MODE_FUNC         117
#define ST_ALARM_TIME_MODE_ADJUST       118
#define ST_ALARM_TIME_MODE_ADJUST_FUNC  119
#define ST_ON_ALARM                     125
