/*
 * Main9.c
 *
 * Created: 3/26/2015 2:20:51 PM
 *  Author: drahos
 */ 
#include "obstacle_detection.h"
#include "movement.h"
#include "lcd.h"
#include "stdint.h"
#include "util.h"
#include "avr_usart.h"
#include "ir.h"
#include "ping.h"
#include "servo.h"

#include <math.h>
#include <stdio.h>

#define DEG_TO_RAD(x) ((x / 360.0f) * 6.2820f)

#define NO_CURRENT_OBJECT 0xff
#define MAX_NUMBER_OF_OBSTACLES 10

#define DEGREES_IN_FRONT 180

#ifdef LAB_9
int main()
{
	/* Reused in for loops */
	uint8_t i;
	
	/* Obstacle storage */
	obstacle_t obstacles[MAX_NUMBER_OF_OBSTACLES];
	uint8_t num_obstacles = 0;
	
	/* Initialization */
	lcd_init();
	init_push_buttons();
	USART_Initialize();
	PING_Initialize();
	IR_Initialize();
	
	SERVO_On();	
	lprintf("Ready to scan!");
	
	for(;;)
	{
		USART_Puts("Ready to scan!\r\n");
		
		/* Wait on button press */
		wait_ms(500);
		while(!(read_push_buttons())) {}
		
		lprintf("Scanning...");
		USART_Puts("Scanning...\r\n");
		
		SERVO_SetDegreesBlocking(0);
		wait_ms(500);
		
		num_obstacles = OBSTACLE_SweepAndDetect(obstacles, MAX_NUMBER_OF_OBSTACLES);
		
		lprintf("%d found!", num_obstacles);
		
		/* Wait for button press */
		while (!(read_push_buttons())) {}
					
		if (num_obstacles > 0)
		{	
						
			for(i = 0; i < num_obstacles; i++)
			{
				lprintf("I: %d A:%d\nDist (cm): %d\nAngular Width: %d\nWidth (cm):%d",
						i,
						obstacles[i].center_angle,
						obstacles[i].sonar_dist,
						obstacles[i].angular_size,
						obstacles[i].linear_size);
				
				/* Wait for button press */
				wait_ms(1000);
				while (!(read_push_buttons())) {}
			}
			
			SERVO_SetDegreesNonBlocking(obstacles[OBSTACLE_FindSmallest(obstacles, num_obstacles)].center_angle);
		}
		else
		{
			/* Reset when done */
			SERVO_SetDegreesNonBlocking(0);
		}
	}
}
#endif