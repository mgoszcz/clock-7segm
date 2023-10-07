/*
 * display_7s.h
 *
 * Created: 12.09.2023 21:50:57
 *  Author: marci
 */ 
#include <avr/io.h>

#define LED_DATA_DIR DDRD // Data port direction (cathodes)
#define LED_CONTROL_DIR DDRB // Control port direction (anodes)
#define LED_DATA_PORT PORTD
#define LED_CONTROL_PORT PORTB
#define HD PB0 // hours decimal
#define HU PB1 // hours units
#define MD PB2 // minutes decimal
#define MU PB3 // minutes units
#define SD PB4 // seconds decimal
#define SU PB5 // seconds units
#define SEPARATOR_PORT PORTC
#define SEPARATOR_PIN PINC1
#define SEPARATOR_DIR DDRC

void display_time(unsigned char currentTime[]);
void my_delay_us(int microseconds);
void dimmer(unsigned int brightness);
void init();
void turnOnSeparator();
void turnOffSeparator();
void toggleSeparator();
void displaySegment(unsigned char displayNumber, unsigned char segmentCode, unsigned char brightness);