/*
 * main.c
 *
 * Created: 9/12/2023 9:48:33 PM
 *  Author: marci
 */ 
#define F_CPU 4000000
#include <avr/io.h>	
#include <stdbool.h>
#include <util/delay.h>

#include "display_7s.h"
#include "RTC.h"

unsigned char currentDisplay[6] = {0, 0, 0, 0, 0, 0};
unsigned char currentTime[3] = {0, 0, 0}; // HOURS, MINUTES, SECONDS
unsigned char currentDate[4] = {0, 0, 0, 0}; // CENTURY, YEAR, MONTH, DAY
unsigned char newTime[5] = {0, 0, 0, 0, 0};
char brightness = 100;
bool brightness_increase = false, editMode = false, blinkPartOff = false;
char readCounter = 0, blinkCounter = 0;
int buttonCounter = 0;
char marker = 0;
enum mode{Time, Date, Temp};
char currentMode = Time;
char editIndex = 0;

void my_delay(int miliseconds) {
	int count = miliseconds / 10;
	for (uint8_t i=0; i<count; i++) _delay_ms(10);
}

void getTime(bool ignoreMarker) {
	char seconds = (currentDisplay[4] << 4) + currentDisplay[5];
	char oldSeconds = seconds;
	char minutes = (currentDisplay[2] << 4) + currentDisplay[3];
	char hours = (currentDisplay[0] << 4) + currentDisplay[1];
	if (ignoreMarker == true) {
		seconds = GetSeconds();
		minutes = GetMinutes();
		hours = GetHours();
	} else {
		switch (marker) {
			case 0:
			seconds = GetSeconds();
			break;
			case 1:
			minutes = GetMinutes();
			break;
			case 2:
			hours = GetHours();
			break;
		}
		marker++;
		if (marker == 3) marker = 0;
	}
	currentTime[0] = hours & 0b111111;
	currentTime[1] = minutes & 0b1111111;
	currentTime[2] = seconds & 0b1111111;
	currentDisplay[4] = (seconds & 0b01110000)>>4;
	currentDisplay[5] = (seconds & 0b00001111);
	currentDisplay[2] = (minutes & 0b01110000)>>4;
	currentDisplay[3] = (minutes & 0b00001111);
	currentDisplay[0] = (hours & 0b00110000)>>4;
	currentDisplay[1] = (hours & 0b00001111);
	if (oldSeconds != seconds) toggleSeparator();
}

void getDate(bool ignoreMarker) {
	int du, mu;
	du = currentDisplay[1];
	mu = currentDisplay[3];
	if (du > 10) du -= 10;
	if (mu > 10) mu -= 10;

	char days = (currentDisplay[0] << 4) + du;
	char months = (currentDisplay[2] << 4) + mu;
	char years = (currentDisplay[4] << 4) + currentDisplay[5];
	if (ignoreMarker == true) {
		days = GetMonthDay();
		months = GetMonth();
		years = GetYear();
		} else {
		switch (marker) {
			case 0:
			days = GetMonthDay();
			break;
			case 1:
			years = GetYear();
			break;
			case 2:
			years = GetYear();
			break;
		}
		marker++;
		if (marker == 3) marker = 0;
	}
	currentDate[0] = months & 0b10000000;
	currentDate[1] = years;
	currentDate[2] = months & 0b11111;
	currentDate[3] = days & 0b00111111;
	currentDisplay[0] = (days & 0b00110000)>>4;
	currentDisplay[1] = (days & 0b00001111) + 10;
	currentDisplay[2] = (months & 0b00010000)>>4;
	currentDisplay[3] = (months & 0b00001111) + 10;
	currentDisplay[4] = (years & 0b11110000)>>4;
	currentDisplay[5] = (years & 0b00001111);
}

void getTemp(bool ignoreMarker) {
	int temp_upper = (int)GetTempUpper();
	int temp_lower = (int)GetTempLower();
	bool negative = false;
	if (temp_upper & 0x80) negative = true;
	temp_upper &= 0x7f;
	float temp = ((temp_upper << 8) + temp_lower) / 256.;
	int whole = (int)temp;
	float fractional_part = temp - whole;
	int fractional_int = (int)(fractional_part * 100);
	if (fractional_int == 0 || fractional_int == 25) fractional_int = 0;
	if (fractional_int == 50 || fractional_int == 75) fractional_int = 5;
	if (negative) {
		currentDisplay[0] = 62;
	} else {
		currentDisplay[0] = 20;
	}
	int secondDigit = whole % 10;
	int firstDigit = (whole / 10) % 10;
	if (firstDigit > 0) {
		currentDisplay[1] = firstDigit;
	} else {
		currentDisplay[1] = 20;	
	}
	currentDisplay[2] = secondDigit + 10;
	currentDisplay[3] = fractional_int;
	currentDisplay[4] = 60;
	currentDisplay[5] = 23;
}

void displayEditMode() {
	if (editIndex == 0 || editIndex == 1) {
		currentDisplay[4] = 20;
		currentDisplay[5] = 20;
		currentDisplay[2] = (currentTime[1] & 0b01110000)>>4;
		currentDisplay[3] = (currentTime[1] & 0b00001111);
		currentDisplay[0] = (currentTime[0] & 0b00110000)>>4;
		currentDisplay[1] = (currentTime[0] & 0b00001111);
		
	}
	if (blinkPartOff) {
		currentDisplay[0] = 20;
		currentDisplay[1] = 20;
	}
	if (blinkCounter == 50) {
		blinkCounter = 0;
		blinkPartOff = !blinkPartOff;
	}
	blinkCounter++;
}

void getDataToDisplay() {
	
	if (editMode) {
		displayEditMode();
		return;
	}
	switch (currentMode) {
		case Time:
			getTime(false);
			break;
		case Date:
			getDate(false);
			break;
		case Temp:
			getTemp(false);
			break;
	}
	
// 	currentDisplay[4] = 0;
// 	currentDisplay[5] = 0;
// 	currentDisplay[2] = 20;
// 	currentDisplay[3] = 20;
// 	currentDisplay[0] = 20;
// 	currentDisplay[1] = signCounter;
// 	unsigned int num = signCounter, i=0;
// 	while(num > 0) {
//  		int mod  = num % 10;
//  		num = num / 10;
//  		currentDisplay[5-i] = mod;
//  		i++;
//  	}
}

void toggleMode() {
	if (currentMode == Temp) {
		currentMode = Time;
	} else {
		currentMode++;
	}
	switch (currentMode) {
		case Time:
			getTime(true);
			turnOnSeparator();
			break;
		case Date:
			getDate(true);
			turnOffSeparator();
			break;
		case Temp:
			turnOffSeparator();
	}
}

int main(void)
{
    my_delay(500);
	DDRC &= ~(1<<PINC3);
	DDRC &= ~(1<<PINC2);
	PORTC |= (1<<PINC3);
	PORTC |= (1<<PINC2);
	init();
	Initialise_TWI_Master();
	getTime(true);
	// PORTC |= (1<<PINC1);

	
	while(1)
    {
        if (!(PINC & (1<<PINC2))) {
			buttonCounter++;
			if (buttonCounter > 500) {
				editMode = true;
				buttonCounter = 0;
				getTime(true);
				getDate(true);
				turnOffSeparator();
			}
		} else if (buttonCounter > 20 && !editMode) {
			toggleMode();
			buttonCounter = 0;
		} 
		
		if (readCounter == 5){
// 			dimmer(brightness);
// 			if (brightness_increase == false) {
// 				brightness--;			
// 				if (brightness == 1) brightness_increase = true;
// 			} else {
// 				brightness++;
// 				if (brightness == 100) brightness_increase = false;
// 			}
			readCounter = 0;
			getDataToDisplay();
		} else {
			readCounter++;
		}
		display_time(currentDisplay);
    }
}