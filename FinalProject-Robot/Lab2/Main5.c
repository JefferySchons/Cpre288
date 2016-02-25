/*
 * Main5.c
 *
 * Created: 2/19/2015 3:56:39 PM
 *  Author: drahos
 */ 

#include "movement.h"
#include "lcd.h"
#include "stdint.h"
#include "util.h"
#include "avr_usart.h"

#ifdef LAB_5

static void doButtonMessage(uint8_t btn) /* Print appropriate message based on button state */
{
	switch(btn)
	{
		case 6:
		usart_puts("Yes\r\n");
		break;
		
		case 5:
		usart_puts("No\r\n");
		break;
		
		case 4:
		usart_puts("Blue, no green, Ahhhh!!\r\n");
		break;
		
		case 3:
		usart_puts("And now for something completely different\r\n");
		break;
		
		case 2:
		usart_puts("My hovercraft is full of eels\r\n");
		break;
		
		case 1:
		usart_puts("It's...\r\n");
		break;
		
		case 0:
		break;
	}
}

int main(void)
{
	uint8_t i;
	uint8_t str[21];
	uint8_t c;
	uint8_t enter_pressed = 0;
	uint8_t last_button_state;
	uint8_t button_state;
	status_t status = STATUS_RINGBUFFER_EMPTY;
	str[20] = 0;
	
	/* Initialization */
	lcd_init();
	init_push_buttons();
	USART_Initialize();
	i = 0;
	button_state = read_push_buttons();
	last_button_state = button_state;
	
	/* Boot messages */
	usart_puts("HELLO WORLD!\r\n");
	lprintf("HELLO WORLD!");
	
	for(;;)
	{
		status = USART_Getc(&c); /* Async getc */
		if (status == STATUS_SUCCESS) /* RX, handle if successful */
		{
			USART_Putc(c); /* Echo */
			lprintf("Char #%d: %c (0x%X)", i+1, c, c);
			str[i] = c; /* Add to buffer */
			if (c == '\r')
			{
				USART_Putc('\n'); /* Finish newline sequence */
				str[i] = 0; /* Set null terminator */
				enter_pressed = 1; /* Handle stuff */
			}
			i++;
			if ((i == 20 || enter_pressed)) /* Handle end of string */
			{
				lprintf(str);
				i = 0;
				enter_pressed = 0;
			}
		}
		
		button_state = read_push_buttons(); /* Update button state */
		if (button_state != last_button_state) /* Handle button state change */
		{
			doButtonMessage(button_state);
		}
		last_button_state = button_state; /* Store last state */
	}
}
#endif