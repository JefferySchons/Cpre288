/*
 * Lab2.c
 *
 * Created: 1/29/2015 1:57:10 PM
 *  Author: drahos
 
 Robot Black #3
 
 drahos@iastate.edu
 
 http://github.com/drahosj
 http://github.cmo/m2i-cysat/cysat-satellitesoftware
 */ 


#include "movement.h"
#include "lcd.h"
#include "stdint.h"
#include "util.h"
#include "avr_usart.h"

void timer_init(void);

#if 0

volatile uint8_t flag_200ms = 0;
volatile uint8_t flag_1s = 0;

/* Hours, minutes, seconds not volatile because they are only modified
 * in the main application context
 */
static uint8_t hours = 0;
static uint8_t minutes = 0;
static uint8_t seconds = 0;

static void handleSecondTick()
{
	/* No need to disable other interrupts while handling
	 * since only the flags are volatile
	 */
	seconds++; /* Increment tick */
	if (seconds == 60) /* Handle rollovers. Does cascade */
	{
		seconds = 0;
		minutes++;
		if (minutes == 60) /* Minutes rollover */
		{
			minutes = 0;
			hours++;
			if (hours == 24) /* Hours rollover */
			{
				hours = 0;
			}
		}
	}
}

static void handle200msTick()
{
	/* No need to disable other interrupts while handling
	 * since only the flags are volatile
	 */
	
	/* Poll buttons and handle increment/decrement */
	uint8_t button = read_push_buttons();
	switch (button)
	{
		case 6:
			hours++;
			break;
		case 5:
			hours--;
			break;
		case 4:
			minutes++;
			break;
		case 3:
			minutes--;
			break;
		case 2:
			seconds++;
			break;
		case 1:
			seconds--;
			break;
		case 0:
			break;
	}
				
	/* Handle rollovers while pushing buttons. Does not cascade*/
	if (hours == 24)
		hours = 0;
	if (hours > 24)
		hours = 23;
				
	if (minutes == 60)
		minutes = 0;
	if (minutes > 60)
		minutes = 59;
				
	if (seconds == 60)
		seconds = 0;
	if (seconds > 60)
		seconds = 59;
				
	/* Formatting string and print */
	lprintf("%02d:%02d:%02d", hours, minutes, seconds);
}
#endif