/*
 * RTC.c
 *
 * Created: 18.04.2023 19:34:00
 *  Author: marci
 */ 
#define F_CPU 4000000			/* Define CPU Frequency e.g. here 8MHz */
#include <avr/io.h>			/* Include AVR std. library file */
#include <util/delay.h>
#include <avr/interrupt.h>
#include "RTC.h"
#include "TWI_Master.h"


void Initialise_TWI_Master(void) {
	TWI_Master_Initialise();
	sei();
}

char RTCRead(char addr) {
	unsigned char messageBuf[4];
	messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The first byte must always consists of General Call code or the TWI slave address.
	messageBuf[1] = addr;             // Send register address
	TWI_Start_Transceiver_With_Data( messageBuf, 2 );
	messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT); // The first byte must always consists of General Call code or the TWI slave address.
	TWI_Start_Transceiver_With_Data( messageBuf, 2 );
	TWI_Get_Data_From_Transceiver(messageBuf, 2);
	return messageBuf[1];
}

void RTCWrite(char addr, char value) {
	unsigned char messageBuf[3];
	messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The first byte must always consists of General Call code or the TWI slave address.
	messageBuf[1] = addr;             // Register address
	messageBuf[2] = value; // data to send
	TWI_Start_Transceiver_With_Data( messageBuf, 3 );
}

char GetSeconds(void) {
	
	char RTCseconds = RTCRead(SECONDS_REGISTER_ADDR);
	return RTCseconds;
}

char GetMinutes(void) {
	char RTCminutes = RTCRead(MINUTES_REGISTER_ADDR);
	return RTCminutes;
}

char GetHours(void) {
	char RTChours = RTCRead(HOURS_REGISTER_ADDR);
	return RTChours;
}

char GetWeekDay(void) {
	return RTCRead(WEEKDAYS_REGISTER_ADDR);
}

char GetMonthDay(void) {
	return RTCRead(MONTHDAYS_REGISTER_ADDR);
}

char GetMonth(void) {
	return RTCRead(MONTHS_REGISTER_ADDR);
}

char GetYear(void) {
	return RTCRead(YEARS_REGISTER_ADDR);
}

char GetTempUpper(void) {
	return RTCRead(TEMP_REGISTER_UPPER_ADDR);
}

char GetTempLower(void) {
	return RTCRead(TEMP_REGISTER_LOWER_ADDR);
}

void SendHours(char value) {
	RTCWrite(HOURS_REGISTER_ADDR, value);
}

void SendMinutes(char value) {
	RTCWrite(MINUTES_REGISTER_ADDR, value);
}

void SendSeconds(char value) {
	RTCWrite(SECONDS_REGISTER_ADDR, value);
}

void SendWeekDay(char value) {
	RTCWrite(WEEKDAYS_REGISTER_ADDR, value);
}

void SendMonthDay(char value) {
	RTCWrite(MONTHDAYS_REGISTER_ADDR, value);
}

void SendMonth(char value) {
	RTCWrite(MONTHS_REGISTER_ADDR, value);
}

void SendYear(char value) {
	RTCWrite(YEARS_REGISTER_ADDR, value);
}