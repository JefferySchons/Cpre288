/*
 * ping.c
 *
 * Created: 3/5/2015 2:11:16 PM
 *  Author: drahos
 */ 
#include "ping.h"
#include "lcd.h"
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>

/* Registers Used: 
 * TCCR1B - ICES1 Input Capture Edge Select 0 falling 1 rising; CS1 Clock Select
 * TCNT1H + TCNT1L - Timer Count
 * ICR1H + ICR1L - Input Capture
 * TIMSK - TICIE Timer Input Capture Input Enable
 * 
 * Clock Source: System Clock with /8 Prescaler 
 *      CS1 = 2
 * Maximum pulse width: 18 msec. @2 MHz 18 msec is 36000 cycles. 3600 fits in
 * a uint16_t.
 * 
 * Speed of sound: 340 m/s = 34000 cm/s = .034 cm/us = 29 us/cm
 * 
 * 1 tick = 1/2000000s = 0.5 us
 * 2 tick = 1 us
 * ticks/2 = us
 * 
 * Round trip = divide by 2
 * 
 * Final factor: ticks * 1/2 * 1/29 * 1/2 = ticks * 1/116
 * */

#define CM_FACTOR 116

static volatile uint16_t ticks = 0;
static volatile uint16_t overflows = 0;

void PING_Initialize()
{
    ticks = 0;
	overflows = 0;
    
	/* Rising edge, /8 prescaler */
    TCCR1B |= (1 << ICES1) | (2);
    
    /* Low output for now */
    PORTD = PORTD & ~(1 << 4);
    DDRD |= (1 << 4);
	
	/* Enable input capture interrupt */
	TIMSK |= (1 << TICIE1) | (1 << TOIE1);
}

uint16_t PING_TicksToUS(uint16_t ticks)
{
	/* Ticks are 0.5 us */
	return ticks / 2;
}

uint16_t PING_TicksToCM(uint16_t ticks)
{
	/* Divide by CM_FACTOR to convert to CM */
	return ticks/CM_FACTOR;
}
uint16_t PING_MeasureTicks()
{
	uint16_t startTicks;
	uint16_t stopTicks;
	uint16_t outputStartTick;
	
	/* Rising edge of pulse to sensor */
	DDRD |= (1 << 4);
	PORTD |= (1 << 4); 	
	
	/* 5 us delay */
	outputStartTick = TCNT1;
	while ((TCNT1 - outputStartTick) < 10) {}
	
	/* Falling edge output */
	PORTD = PORTD & ~(1 << 4);

	 /* Set to input mode */
	DDRD = DDRD & ~(1 << 4);
	
	/* Detect and capture rising edge */
	TCCR1B |= (1 << ICES1);
	ticks = 0;
	while (ticks == 0) {}
	startTicks = ticks; 
	
	/* Detect and capture falling edge */
	TCCR1B = TCCR1B & ~(1 << ICES1);
	ticks = 0;
	while (ticks == 0) {}
	stopTicks = ticks;	

	return (stopTicks - startTicks);
}

uint16_t PING_GetOverflows()
{
	return overflows;
}

ISR(TIMER1_CAPT_vect)
{
	/* Store captured value for main thread */
	ticks = ICR1;
}

ISR(TIMER1_OVF_vect)
{
	/* Store captured value for main thread */
	overflows++;
}