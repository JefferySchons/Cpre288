/*
 * Main6.c
 *
 * Created: 2/26/2015 2:14:13 PM
 *  Author: drahos
 */ 



#include "movement.h"
#include "lcd.h"
#include "stdint.h"
#include "util.h"
#include "avr_usart.h"
#include "ir.h"

#if 0
void main()
{
		uint16_t average = 0;
		uint8_t i;
		uint16_t dist; /* mm */
		
		/* Initialization */
		lcd_init();
		init_push_buttons();
		initialize_usart();
		PING_Initialize();
		IR_Initialize();
		
		SERVO_On();
		
		lprintf("Started!");		
		for(;;)
		{
			average = 0;
			for (i = 0; i < 5; i++) /* Gather 5 samples at 50 msec intervals */
			{
				wait_ms(50);
				average += IR_GetDataBlocking();
			}
			average = average / 5; /* Divide to find average */
			dist = IR_ValueToDist(average);
			lprintf("%03d,%03d.%dcm", average, dist/10, dist%10);
		}
}
#endif