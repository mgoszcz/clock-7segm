/*
 * main.c
 *
 * Created: 9/12/2023 9:48:33 PM
 *  Author: marci
 */ 
#define F_CPU 4000000
#include <stdbool.h>
#include <util/delay.h>

#include "display_7s.h"
#include "RTC.h"

unsigned int mainCurrentTime[6] = {0, 0, 0, 0, 0, 0};
int brightness = 100;
bool brightness_increase = false;
int counter =0;
char seconds;
int marker = 0;

void my_delay(int miliseconds) {
	int count = miliseconds / 10;
	for (uint8_t i=0; i<count; i++) _delay_ms(10);
}

void getTime(bool ignoreMarker) {
	char seconds;
	char minutes;
	char hours;
	if (ignoreMarker == true) {
		seconds = GetSeconds();
		mainCurrentTime[4] = (seconds & 0b01110000)>>4;
		mainCurrentTime[5] = (seconds & 0b00001111);
		minutes = GetMinutes();
		mainCurrentTime[2] = (minutes & 0b01110000)>>4;
		mainCurrentTime[3] = (minutes & 0b00001111);
		hours = GetHours();
		mainCurrentTime[0] = (hours & 0b00110000)>>4;
		mainCurrentTime[1] = (hours & 0b00001111);
		return;
	}
	switch (marker) {
		case 0:
			seconds = GetSeconds();
			mainCurrentTime[4] = (seconds & 0b01110000)>>4;
			mainCurrentTime[5] = (seconds & 0b00001111);
			break;
		case 1:
			minutes = GetMinutes();
			mainCurrentTime[2] = (minutes & 0b01110000)>>4;
			mainCurrentTime[3] = (minutes & 0b00001111);
			break;
		case 2:
			hours = GetHours();
			mainCurrentTime[0] = (hours & 0b00110000)>>4;
			mainCurrentTime[1] = (hours & 0b00001111);
			break;
	}
	marker++;
	if (marker == 3) marker = 0;
	
	
	
// 	char weekDay = GetWeekDay();
// 	char MonthDay = GetMonthDay();
// 	char Month = GetMonth();
// 	char Year = GetYear();
	
	
	
}

int main(void)
{
    my_delay(500);
	
	init();
	Initialise_TWI_Master();
	getTime(true);
	while(1)
    {
        if (counter == 5){
// 			dimmer(brightness);
// 			if (brightness_increase == false) {
// 				brightness--;			
// 				if (brightness == 1) brightness_increase = true;
// 			} else {
// 				brightness++;
// 				if (brightness == 100) brightness_increase = false;
// 			}
			counter = 0;
			getTime(false);
		} else {
			counter++;
		}
		display_time(mainCurrentTime);
    }
}