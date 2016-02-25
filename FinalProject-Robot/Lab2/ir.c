#include "ir.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>

static volatile uint16_t ir_value;
static volatile uint8_t ir_new_data;

/* Set up the lookup table in either ROM or RAM */
#if LOOKUP_TABLE_IN_ROM
	extern int16_t * lookup_table;
	#define TABLE_LOOKUP(i) pgm_read_word(&lookup_table[i])
#else
	uint16_t ram_lookup_table[1024];
	#define TABLE_LOOKUP(i) ram_lookup_table[i]
	
	/* Conversion factors to calculate lookup table */
	#define CONV_A 393700.0f
	#define CONV_EXP -1.258f
#endif

void IR_Initialize()
{
    ir_value = 0;
    ir_new_data = 0;
	uint16_t i = 0;
    
    ADCSRA |= (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2); /* Prescaler 128 (16MHz/128 = 125 KHz ) */
    ADMUX |= (1 << REFS0) | (1 << REFS1); /* Use internal reference */
    ADMUX = (ADMUX & 0xD0) | (0x02); /* Select channel ADC2 */
    // ADCSRA |= (1 << ADFR); /* Free-running mode */
    
    ADCSRA |= (1 << ADEN); /* Enable ADC */
    ADCSRA |= (1 << ADSC); /* Start free-running conversions ADC */
    
    ADCSRA |= (1 << ADIE);
	
#if !LOOKUP_TABLE_IN_ROM
	for(i = 0; i < 1024; i++)
	{
		ram_lookup_table[i] = CONV_A * pow(i, CONV_EXP); /* Populate lookup table */
	}
#endif
}

uint16_t IR_GetData()
{
	ir_new_data = 0; /* Reset flag */
	return ir_value;
}

uint16_t IR_GetDataBlocking()
{
	/* Wait for new data */
	while (ir_new_data == 0) {}
	return IR_GetData();
}

uint8_t IR_NewData()
{
	return ir_new_data;
}

uint16_t IR_ValueToDist(uint16_t val)
{
	/* Convert ADC value to distance from lookup table */
	return TABLE_LOOKUP(val);
}

ISR(ADC_vect)
{
	ir_value = ADC; /* Store value */
	ir_new_data = 1; /* Set Flag */
	ADCSRA |= (1 << ADSC); /* Start next conversion */
}