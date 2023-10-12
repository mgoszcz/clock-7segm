/*
 * CFile1.c
 *
 * Created: 12.09.2023 21:50:27
 *  Author: marci
 */ 
#define F_CPU 4000000
#include <util/delay.h>
#include <stdbool.h>
#include "display_7s.h"
#include "coder_7seg.h"

unsigned int TIME_OFF, TIME_ON;
bool separator_on = false;

void displayInit() {
	TIME_OFF = 1900;
	TIME_ON = 100;
	SEPARATOR_DIR |= (1<<SEPARATOR_PIN);
}
	
void my_delay_us(int microseconds) {
	int micro = microseconds % 1000;
	int mili = microseconds / 1000;
	for (int i=0; i<mili; i++) {
		_delay_ms(1);
	}
	for (int i=0; i< micro; i+= 10) {
		_delay_us(10);
	}
}

void dimmer(unsigned int brightness) {
	TIME_ON = 20 * brightness / 100;
	TIME_ON = TIME_ON * 100;
	TIME_OFF = 2000 - TIME_ON;
}

void turnOnSeparator() {
	separator_on = true;
}

void turnOffSeparator() {
	separator_on = false;
}

void toggleSeparator() {
	if (separator_on) {
		turnOffSeparator();
	} else {
		turnOnSeparator();
	}
}

void display_time (unsigned char currentTime[]) {
	// temporary begin
	//unsigned int num = TIME_ON, i=0;
	//num = 3000 * (100 / 100);
// 	while(num > 0) {
// 		int mod  = num % 10;
// 		num = num / 10;
// 		currentTime[5-i] = mod;
// 		i++;
// 	}
	// temporary end
	LED_DATA_PORT = codeToDisplay(currentTime[0]);
	my_delay_us(TIME_OFF);
	if (codeToDisplay(currentTime[0]) != 0) LED_CONTROL_PORT |= (1 << HD); // high
	my_delay_us(TIME_ON);
	LED_CONTROL_PORT &= ~(1 << HD);  // low
	_delay_us(10);
	
	
	LED_DATA_PORT = codeToDisplay(currentTime[1]);
	my_delay_us(TIME_OFF);
	if (codeToDisplay(currentTime[1]) != 0) LED_CONTROL_PORT |= (1 << HU); // high
	my_delay_us(TIME_ON);
	LED_CONTROL_PORT &= ~(1 << HU);  // low
	_delay_us(10);
	
	LED_DATA_PORT = codeToDisplay(currentTime[2]);
	my_delay_us(TIME_OFF);
	if (codeToDisplay(currentTime[2]) != 0) LED_CONTROL_PORT |= (1 << MD); // high
	my_delay_us(TIME_ON);
	LED_CONTROL_PORT &= ~(1 << MD);  // low
	_delay_us(10);
	
	LED_DATA_PORT = codeToDisplay(currentTime[3]);
	my_delay_us(TIME_OFF);
	if (codeToDisplay(currentTime[3]) != 0) LED_CONTROL_PORT |= (1 << MU); // high
	my_delay_us(TIME_ON);
	LED_CONTROL_PORT &= ~(1 << MU);  // low
	_delay_us(10);
	
	LED_DATA_PORT = codeToDisplay(currentTime[4]);
	my_delay_us(TIME_OFF);
	if (codeToDisplay(currentTime[4]) != 0) LED_CONTROL_PORT |= (1 << SD); // high
	my_delay_us(TIME_ON);
	LED_CONTROL_PORT &= ~(1 << SD);  // low
	_delay_us(10);
	
	LED_DATA_PORT = codeToDisplay(currentTime[5]);
	my_delay_us(TIME_OFF);
	if (codeToDisplay(currentTime[5]) != 0) LED_CONTROL_PORT |= (1 << SU); // high
	my_delay_us(TIME_ON);
	LED_CONTROL_PORT &= ~(1 << SU);  // low
	_delay_us(10);
	
	if (separator_on) {
		my_delay_us(TIME_OFF);
		SEPARATOR_PORT |= (1 << SEPARATOR_PIN); // high
		my_delay_us(TIME_ON);
		SEPARATOR_PORT &= ~(1 << SEPARATOR_PIN);  // low
	} else {
		my_delay_us(TIME_OFF + TIME_ON);
	}
}

// Display specific segment on specific display with specific brightness
// Display number [0-5], segmentCode [100-107], brightness [0-100]
void displaySegment(unsigned char displayNumber, unsigned char segmentCode, unsigned char brightness) {
	int time_on = (20 * brightness / 100) * 100;
	int time_off = 2000 - time_on;
	LED_DATA_PORT = codeToDisplay(segmentCode);
	my_delay_us(time_off);
	if (codeToDisplay(segmentCode) != 0) LED_CONTROL_PORT |= (1 << displayNumber); // high
	my_delay_us(time_on);
	LED_CONTROL_PORT &= ~(1 << displayNumber);  // low
	_delay_us(10);
	LED_DATA_PORT = 0;
}