/*
 * RTC.h
 *
 * Created: 18.04.2023 19:34:43
 *  Author: marci
 */ 


#define SECONDS_REGISTER_ADDR 0x00
#define MINUTES_REGISTER_ADDR 0x01
#define HOURS_REGISTER_ADDR 0x02
#define WEEKDAYS_REGISTER_ADDR 0x03
#define MONTHDAYS_REGISTER_ADDR 0x04
#define MONTHS_REGISTER_ADDR 0x05
#define YEARS_REGISTER_ADDR 0x06
#define CONTROL_REGISTER_ADDR 0x0E
#define TWI_targetSlaveAddress 0x68

void Initialise_TWI_Master(void);
char GetSeconds(void);
char GetMinutes(void);
char GetHours(void);
char GetWeekDay(void);
char GetMonthDay(void);
char GetMonth(void);
char GetYear(void);
void SendHours(char value);
void SendMinutes(char value);
void SendSeconds(char value);
void SendWeekDay(char value);
void SendMonthDay(char value);
void SendMonth(char value);
void SendYear(char value);
