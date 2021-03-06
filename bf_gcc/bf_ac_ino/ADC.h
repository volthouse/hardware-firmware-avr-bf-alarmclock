#define TEMPERATURE_SENSOR  0
#define CELSIUS             3
#define FAHRENHEIT           4

#ifdef __cplusplus
extern "C"{
#endif

//#define Vref                2.85

void ADC_init(char );
int ADC_read(void);
void ADC_periphery(void);

// Temperature sensor function
char TemperatureFunc(char);

#ifdef __cplusplus
} // extern "C"
#endif