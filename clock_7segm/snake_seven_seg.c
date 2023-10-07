/*
 * snake_seven_seg.c
 *
 * Created: 07.10.2023 22:09:44
 *  Author: marci
 */ 

#include "display_7s.h"

unsigned char value = 0;
unsigned char currentIndex = 0;

unsigned char sequence[56] = {
	104, 105, 100, 101, 102,
	104, 105, 100, 101, 102,
	104, 105, 100, 101, 102,
	104, 105, 100, 101, 102,
	104, 105, 100, 101, 102,
	104, 105, 100, 101, 102,
	103, 104, 105,
	101, 102, 103, 104, 105,
	101, 102, 103, 104, 105,
	101, 102, 103, 104, 105,
	101, 102, 103, 104, 105,
	101, 102, 103
};

void displayValue(unsigned char counter) {
	unsigned char display[6] = {value, 20, 20, 20, 20, 20};
	display_time(display);
	if (counter == 254) value++;
	if (value == 10) value = 0;
}

unsigned char getDisplayNumber(unsigned char sequenceIndex) {
	if (sequenceIndex < 5 || sequenceIndex > 52) return 0;
	else if ((sequenceIndex >=5 && sequenceIndex < 10) || (sequenceIndex >= 48 && sequenceIndex < 53)) return 1;
	else if ((sequenceIndex >= 10 && sequenceIndex < 15) || (sequenceIndex >= 43 && sequenceIndex < 48)) return 2;
	else if ((sequenceIndex >= 15 && sequenceIndex < 20) || (sequenceIndex >= 38 && sequenceIndex < 43)) return 3;
	else if ((sequenceIndex >= 20 && sequenceIndex < 25) || (sequenceIndex >= 33 && sequenceIndex < 38)) return 4;
	else if (sequenceIndex >= 25 && sequenceIndex < 33) return 5;
	else return 99;
}

void displaySnake(unsigned char counter) {
	unsigned char previousIndex, nextIndex;
	if (currentIndex == 0) previousIndex = 55;
	else previousIndex = currentIndex - 1;
	if (currentIndex == 55) nextIndex = 0;
	else nextIndex = currentIndex + 1;
	displaySegment(getDisplayNumber(previousIndex), sequence[previousIndex], 20);
	displaySegment(getDisplayNumber(currentIndex), sequence[currentIndex], 50);
	displaySegment(getDisplayNumber(nextIndex), sequence[nextIndex], 20);
	if (counter% 25 == 0) currentIndex++;
	if (currentIndex == 56) currentIndex = 0;
}