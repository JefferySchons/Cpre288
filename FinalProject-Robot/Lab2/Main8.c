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
#include "servo.h"

#define DIR_CCW 0
#define DIR_CW 1

#ifdef LAB_8
void main()
{	
	uint8_t direction = DIR_CCW;
	uint8_t degrees = 0;
	uint16_t pulse_len;
	uint8_t pb;
	
	/* Initialization */
	lcd_init();
	init_push_buttons();
	USART_Initialize();
	PING_Initialize();
	
	SERVO_On();
	for(;;)
	{
		pb = read_push_buttons();
		if (direction == DIR_CCW)
		{
			switch(pb)
			{
			case 1:
				/* Increment by 1 degree */
				degrees += 1;
				break;
				
			case 2:
				/* Increment by 2 degrees */
				degrees += 2;
				break;
				
			case 3:
				/* Increment by 5 degrees */
				degrees += 5;
				break;
				
			case 4:
				direction = DIR_CW;
				wait_ms(250); /* Debounce */
				break;
			}
			if (degrees > 180)
			{
				degrees = 180; /* Deal with overrun */
			}
		}
		else if (direction == DIR_CW)
		{
			switch(pb)
			{
			case 1:
				/* Decrement by 1 degree */
				degrees -= 1;
				break;
				
			case 2:
				/* Decrement by 2 degrees */
				degrees -= 2;
				break;
				
			case 3:
				/* Decrement by 5 degrees */
				degrees -= 5;
				break;
				
			case 4:
				direction = DIR_CCW;
				wait_ms(250); /* debounce */
				break;
			}
			if (degrees > 180)
			{
				degrees = 0; /* Deal with underrun */
			}
		}
		pulse_len = SERVO_DegreesToPulseLen(degrees); /* Calculate pulse length (in timer ticks) */
		lprintf("Angle: %d\nPulse_len: %d\nDirection: %d", degrees, pulse_len, direction);
		SERVO_SetBlocking(pulse_len);
	}
}
#endif