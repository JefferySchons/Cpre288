/*
 * main7.c
 *
 * Created: 3/5/2015 2:11:29 PM
 *  Author: drahos
 */ 
#include "movement.h"
#include "lcd.h"
#include "stdint.h"
#include "util.h"
#include "avr_usart.h"
#include "ir.h"
#include "ping.h"
#ifdef LAB_7
void main()
{
	uint8_t i;
	uint16_t dist; /* cm */
	uint16_t ticks; /* Ticks */
	
	/* Initialization */
	lcd_init();
	init_push_buttons();
	USART_Initialize();
	PING_Initialize();
	
	lprintf("Started!");
	for(;;)
	{
		/* Measure Ticks */
		ticks = PING_MeasureTicks();
		
		/* Convert and Print */
		lprintf("Ticks: %d T: %dus D: %dcm OVF: %d", ticks, PING_TicksToUS(ticks), PING_TicksToCM(ticks), PING_GetOverflows());
		
		/* Measure every second */
		wait_ms(200);
	}
}
#endif