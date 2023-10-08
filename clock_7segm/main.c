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
#include "snake_seven_seg.h"

const int monthDaysCount[12] = {
	31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
const int century[2] = {2000, 2100};

unsigned char currentDisplay[6] = {20, 7, 20, 5, 20, 8};
unsigned char currentTime[3] = {0, 0, 0}; // HOURS, MINUTES, SECONDS
unsigned char currentDate[4] = {0, 0, 0, 0}; // CENTURY, YEAR, MONTH, DAY
bool editMode = false, blinkerOff = false, editModeButtonBlocker = false, brightnessIncrease = true;
unsigned char mainCounter = 0, blinkCounter = 0, relaxMode = 0, relaxModeBrightness = 0;
int buttonCounter1 = 0;
int buttonCounter2 = 0;
char marker = 0;
enum mode{Time, Date, Temp};
char currentMode = Time;
char editIndex = 0;

void my_delay(int miliseconds) {
	int count = miliseconds / 10;
	for (uint8_t i=0; i<count; i++) _delay_ms(10);
}


unsigned char bcdToDec(unsigned char bcd) {
	return (((bcd & 0xf0) >> 4) * 10) + (bcd &0xf);
}

unsigned char decToBcd(unsigned char dec) {
	char decimals = dec/10;
	char units = dec%10;
	return (decimals<<4) + units;
}

unsigned char incrementBcd(unsigned char value) {
	return decToBcd(bcdToDec(value) + 1);
}

bool isLeapYear() {
	int year = 2000 + (century[currentDate[0]] * 100) + bcdToDec(currentDate[1]);
	if (year%4 != 0) return false;
	if (year%100 != 0) return true;
	if (year%400 != 0) return false;
	return true;
}

void displayError(unsigned char errornumber) {
	unsigned char displayError[6] = {25, 33, 33, 20, 0, errornumber};
	while(1) display_time(displayError);
}

void getTime(bool ignoreMarker) {
	char oldSeconds = currentTime[2];
	if (ignoreMarker == true) {
		currentTime[2] = GetSeconds();
		currentTime[1] = GetMinutes();
		currentTime[0] = GetHours() & 0b111111;
	} else {
		switch (marker) {
			case 0:
			currentTime[2] = GetSeconds();
			break;
			case 1:
			currentTime[1] = GetMinutes();
			break;
			case 2:
			currentTime[0] = GetHours() & 0b111111;
			break;
		}
		marker++;
		if (marker == 3) marker = 0;
	}
	if (currentTime[0] == 0x3f || currentTime[1] == 0xff || currentTime[2] == 0xff) displayError((unsigned char)1);
	currentDisplay[4] = (currentTime[2] & 0b01110000)>>4;
	currentDisplay[5] = (currentTime[2] & 0b00001111);
	currentDisplay[2] = (currentTime[1] & 0b01110000)>>4;
	currentDisplay[3] = (currentTime[1] & 0b00001111);
	if (((currentTime[0] & 0b00110000)>>4) == 0) {
		currentDisplay[0] = 20;
	} else {
		currentDisplay[0] = (currentTime[0] & 0b00110000)>>4;
	}
	currentDisplay[1] = (currentTime[0] & 0b00001111);
	if (oldSeconds != currentTime[2]) toggleSeparator();
}

void getDate(bool ignoreMarker) {
	if (ignoreMarker == true) {
		currentDate[3] = GetMonthDay();
		currentDate[2] = GetMonth() & 0b11111;
		currentDate[1] = GetYear();
		currentDate[0] = GetMonth() & 0b10000000 >> 7;
		} else {
		switch (marker) {
			case 0:
			currentDate[3] = GetMonthDay();
			break;
			case 1:
			currentDate[2] = GetMonth() & 0b11111;
			break;
			case 2:
			currentDate[1] = GetYear();
			currentDate[0] = GetMonth() & 0b10000000 >> 7;
			break;
		}
		marker++;
		if (marker == 3) marker = 0;
	}
	if (((currentDate[3] & 0b00110000)>>4) == 0) {
		currentDisplay[0] = 20;
		} else {
		currentDisplay[0] = (currentDate[3] & 0b00110000)>>4;
	}
	if (currentDate[2] == 0x1f || currentDate[3] == 0xff) displayError((unsigned char)1);
	currentDisplay[1] = (currentDate[3] & 0b00001111) + 10;
	currentDisplay[2] = (currentDate[2] & 0b00010000)>>4;
	currentDisplay[3] = (currentDate[2] & 0b00001111) + 10;
	currentDisplay[4] = (currentDate[1] & 0b11110000)>>4;
	currentDisplay[5] = (currentDate[1] & 0b00001111);
}

void getTemp(bool ignoreMarker) {
	int temp_upper = (int)GetTempUpper();
	int temp_lower = (int)GetTempLower();
	if (temp_upper == 255 || temp_lower == 255) displayError((unsigned char)1);
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
		turnOnSeparator();
		currentDisplay[4] = currentTime[2];
		currentDisplay[5] = currentTime[2];
		currentDisplay[2] = (currentTime[1] & 0b01110000)>>4;
		currentDisplay[3] = (currentTime[1] & 0b00001111);
		if (((currentTime[0] & 0b00110000)>>4) == 0) {
			currentDisplay[0] = 20;
		} else {
			currentDisplay[0] = (currentTime[0] & 0b00110000)>>4;
		}
		currentDisplay[1] = (currentTime[0] & 0b00001111);
		if (blinkerOff) {
			currentDisplay[editIndex * 2] = 20;
			currentDisplay[editIndex * 2 + 1] = 20;
		}
		
	} else if (editIndex == 2) {
		turnOffSeparator();
		currentDisplay[0] = 20;
		currentDisplay[1] = 20;
		currentDisplay[2] = 2;
		currentDisplay[3] = currentDate[0];
		currentDisplay[4] = (currentDate[1] & 0b11110000)>>4;
		currentDisplay[5] = (currentDate[1] & 0b00001111);
		if (blinkerOff) {
			currentDisplay[2] = 20;
			currentDisplay[3] = 20;
			currentDisplay[4] = 20;
			currentDisplay[5] = 20;
		}
	} else if (editIndex == 3 || editIndex == 4) {
		currentDisplay[0] = 20;
		currentDisplay[1] = 20;
		if (((currentDate[3] & 0b00110000)>>4) == 0) {
			currentDisplay[2] = 20;
		} else {
			currentDisplay[2] = (currentDate[3] & 0b00110000)>>4;
		}
		currentDisplay[3] = (currentDate[3] & 0b00001111) + 10;
		currentDisplay[4] = (currentDate[2] & 0b00010000)>>4;
		currentDisplay[5] = (currentDate[2] & 0b00001111) + 10;
		if (blinkerOff && editIndex == 3) {
			currentDisplay[4] = 20;
			currentDisplay[5] = 20;
		}
		if (blinkerOff && editIndex == 4) {
			currentDisplay[2] = 20;
			currentDisplay[3] = 20;
		}
	}
}

void incrementCurrentIndex() {
	switch (editIndex) {
		case 0:
		currentTime[0] = incrementBcd(currentTime[0]);
		if (bcdToDec(currentTime[0]) == 24) currentTime[0] = 0;
		break;
		case 1:
		currentTime[1] = incrementBcd(currentTime[1]);
		if (bcdToDec(currentTime[1]) == 60) currentTime[1] = 0;
		break;
		case 2:
		currentDate[1] = incrementBcd(currentDate[1]);
		if (bcdToDec(currentDate[1]) == 100) {
			if (currentDate[0] == 0) currentDate[0] = 1;
			else currentDate[0] = 0;
			currentDate[1] = 0;
		}
		break;
		case 3:
		currentDate[2] = incrementBcd(currentDate[2]);
		if (bcdToDec(currentDate[2]) == 13) currentDate[2] = 1;
		break;
		case 4:
		currentDate[3] = incrementBcd(currentDate[3]);
		if (bcdToDec(currentDate[3]) > 28 && bcdToDec(currentDate[2]) == 2 && !isLeapYear()) currentDate[3] = 1;
		if (bcdToDec(currentDate[3]) > monthDaysCount[bcdToDec(currentDate[2]) - 1]) currentDate[3] = 1;
		break;
	}
}

void sendDataToRTC() {
	if (SendHours(currentTime[0]) != 0) displayError((unsigned char)2);
	if (SendMinutes(currentTime[1]) != 0) displayError((unsigned char)2);
	if (SendSeconds(0) != 0) displayError((unsigned char)2);
	if (SendMonthDay(currentDate[3]) != 0) displayError((unsigned char)2);
	if (SendMonth((currentDate[0] << 7) + currentDate[2]) != 0) displayError((unsigned char)2);
	if (SendYear(currentDate[1]) != 0) displayError((unsigned char)2);
}

void relaxMode1() {
	if (relaxModeBrightness == 0) {
		currentDisplay[0] = 20;
		currentDisplay[1] = 20;
		currentDisplay[2] = 20;
		currentDisplay[3] = 20;
		currentDisplay[4] = 20;
		currentDisplay[5] = 20;
		currentDisplay[rand() % 6] = rand() % 7 + 100;
	}
	if (brightnessIncrease) relaxModeBrightness++;
	else relaxModeBrightness--;
	if (relaxModeBrightness == 100) {
		brightnessIncrease = false;
	} else if (relaxModeBrightness == 0) {
		brightnessIncrease = true;
	}
	dimmer(relaxModeBrightness);
}

void getDataToDisplay() {
	
	if (editMode) {
		displayEditMode();
		return;
	}
	if (relaxMode != 0) {
		switch(relaxMode) {
			case 1:
				relaxMode1();
				return;
			case 2:
				return;
		}
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
}

void toggleMode() {
	if (relaxMode != 0) {
		relaxMode = 0;
		return;
	}
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

void toggleRelaxMode() {
	turnOffSeparator();
	relaxMode++;
	relaxModeBrightness = 0;
	if (relaxMode == 3) relaxMode = 1;
}

char getBrigthness() {
	ADCSRA |=(1<<ADSC);
	char r = ADC / 10;
	ADCSRA &= ~(1<<ADSC);
	if (r > 100) r = 100;
// 	currentDisplay[0] = 20;
// 	currentDisplay[1] = 20;
// 	currentDisplay[2] = 20;
// 	currentDisplay[3] = 20;
// 	currentDisplay[4] = 20;
// 	currentDisplay[5] = 20;
//  	char i = 0;
//  	while(r > 0) {
//  	 	char mod  = r % 10;
//  	 	r = r / 10;
//  	 	currentDisplay[5-i] = mod;
//  	 	i++;
//   	}
 	return r;
}



int main(void)
{
    my_delay(500);
	DDRC &= ~(1<<PINC3);
	DDRC &= ~(1<<PINC2);
	DDRC &= ~(1<<PINC0);
	PORTC |= (1<<PINC3);
	PORTC |= (1<<PINC2);
	PORTC |= (1<<PINC0);
	init();
	Initialise_TWI_Master();
	getTime(true);
	ADMUX |=(1<<REFS0);
	ADCSRA |=(1<<ADEN)|(1<<ADFR)|(1<<ADPS0);
	
	
	while(1)
    {
		if (!(PINC & (1<<PINC2))) {
			if (!editModeButtonBlocker) buttonCounter1++;
			if (buttonCounter1 > 250 && ! editMode) {
				editMode = true;
				buttonCounter1 = 0;
				getTime(true);
				getDate(true);
				turnOffSeparator();
				currentTime[2] = 0;
				editModeButtonBlocker = true;
			}
		} else {
			 if (buttonCounter1 > 10 && !editMode) {
				toggleMode();
				buttonCounter1 = 0;
			} else if (buttonCounter1 > 10 && editMode) {
				editIndex++;
				buttonCounter1 = 0;
				if (editIndex == 5) 
				{
					editMode = false;
					editIndex = 0;
					sendDataToRTC();
				}

			}
			editModeButtonBlocker = false;
		 }
		 if (!(PINC & (1<<PINC3))) {
			 buttonCounter2++;
			 blinkerOff = false;
		 }  else if (buttonCounter2 > 10 && !editMode) {
			 toggleRelaxMode();
			 buttonCounter2 = 0;
		 }
		 if (buttonCounter2 > 50 && editMode) {
			 incrementCurrentIndex();
			 buttonCounter2 = 0;
			 blinkerOff = false;
		 } 
		
		if (mainCounter%3 == 0){
 			dimmer(100 - getBrigthness());
			if (relaxMode != 2)	getDataToDisplay();
		} 
		mainCounter++;
		blinkCounter++;
		if (mainCounter == 251) mainCounter = 0;
		if (blinkCounter == 50) 
		{
			blinkCounter = 0;
			blinkerOff = !blinkerOff;
		}
		if (relaxMode != 2) display_time(currentDisplay);
		else displaySnake(mainCounter);
    }
}