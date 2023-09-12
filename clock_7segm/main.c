/*
 * main.c
 *
 * Created: 9/12/2023 9:48:33 PM
 *  Author: marci
 */ 
#define F_CPU 4000000
#include <stdbool.h>

#include "display_7s.h"

unsigned int mainCurrentTime[6] = {0, 0, 0, 0, 0, 0};
int brightness = 100;
bool brightness_increase = false;
int counter =0;

int main(void)
{
    init();
	while(1)
    {
        if (counter == 100){
			dimmer(brightness);
			if (brightness_increase == false) {
				brightness--;			
				if (brightness == 1) brightness_increase = true;
			} else {
				brightness++;
				if (brightness == 100) brightness_increase = false;
			}
			counter = 0;
		} else {
			counter++;
		}
		mainCurrentTime[0] = 0;
		mainCurrentTime[1] = 0;
		mainCurrentTime[2] = 0;
		mainCurrentTime[3] = 0;
		mainCurrentTime[4] = 0;
		mainCurrentTime[5] = 0;
		display_time(mainCurrentTime);
    }
}