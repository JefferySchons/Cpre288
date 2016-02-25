/*
 * FinalProject.c
 *
 * Created: 4/16/2015 2:45:11 PM
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
#include "command.h"

int main()
{
	/* Initialize all the things */
	lcd_init();
	init_push_buttons();
	USART_Initialize();
	PING_Initialize();
	IR_Initialize();
	COMMAND_Initialize();
	
	/* Turn on servo */
	SERVO_On();
	lprintf("Command handling initialized!");
	
	for(;;)
	{
		/* Check the USART RX buffer and execute commands */
		COMMAND_CheckAndParse();
	}
}