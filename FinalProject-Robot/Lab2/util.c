/**
 * util.c: utility functions for the Atmel platform
 * 
 * For an overview of how timer based interrupts work, see
 * page 111 and 133-137 of the Atmel Mega128 User Guide
 *
 * @author Zhao Zhang & Chad Nelson
 * @date 06/26/2012
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "util.h"

// delete the following 4 lines after implementing all functions
#define REPLACEME 0

// Global used for interrupt driven delay functions
volatile unsigned int timer2_tick;
void timer2_start(char unit);
void timer2_stop();


/// Blocks for a specified number of milliseconds
void wait_ms(unsigned int time_val) {
	//Seting OC value for time requested
	OCR2=250; 				//Clock is 16 MHz. At a prescaler of 64, 250 timer ticks = 1ms.
	timer2_tick=0;
	timer2_start(0);

	//Waiting for time
	while(timer2_tick < time_val);

	timer2_stop();
}


// Start timer2
void timer2_start(char unit) {
	timer2_tick=0;
	if ( unit == 0 ) { 		//unit = 0 is for slow 
        TCCR2=0b00001011;	//WGM:CTC, COM:OC2 disconnected,pre_scaler = 64
        TIMSK|=0b10000000;	//Enabling O.C. Interrupt for Timer2
	}
	if (unit == 1) { 		//unit = 1 is for fast
        TCCR2=0b00001001;	//WGM:CTC, COM:OC2 disconnected,pre_scaler = 1
        TIMSK|=0b10000000;	//Enabling O.C. Interrupt for Timer2
	}
	sei();
}


// Stop timer2
void timer2_stop() {
	TIMSK&=~0b10000000;		//Disabling O.C. Interrupt for Timer2
	TCCR2&=0b01111111;		//Clearing O.C. settings
}


// Interrupt handler (runs every 1 ms)
ISR (TIMER2_COMP_vect) {
	timer2_tick++;
}




/// Initialize PORTC to accept push buttons as input
void init_push_buttons(void) {
	DDRC &= 0xC0;  //Setting PC0-PC5 to input
	PORTC |= 0x3F; //Setting pins' pull up resistors
}

/// Return the position of button being pushed
/**
 * Return the position of button being pushed.
 * @return the position of the button being pushed.  A 1 is the rightmost button.  0 indicates no button being pressed
 */
unsigned char read_push_buttons(void) {
	int i;
	
	for (i = 5; i >= 0; i--)
	{
		if ((~PINC) & (1 << i)) /* Check bits from left to right */
			return i + 1; /* Return first bit found (from left) */
	}
	return 0;
}




/// Initialize PORTC for input from the shaft encoder
void shaft_encoder_init(void) {
	DDRC &= 0x3F;	//Setting PC6-PC7 to input
	PORTC |= 0xC0;	//Setting pins' pull-up resistors
}

/// Read the shaft encoder
/**
 * Reads the two switches of the shaft encoder and compares the values
 * to the previous read.  This function should be called very frequently
 * for the best results.
 *
 * @return a value indicating the shaft encoder has moved:
 * 0 = no rotation (switches did not change)
 * 1 = CW rotation
 * -1 = CCW rotation
 */

#define ENC_MASK 0xC0
#define ENC_GROOVE 0xC0
#define ENC_OUT 0x00
#define ENC_CW 0x40
#define ENC_CCW 0x80
signed char read_shaft_encoder(void) {
	//
	// DELETE ME - How the shaft encoder works
	// ----------------------------------------
	// The shaft encoder (the knob next to the push buttons) is connected to the left
	// two bits of PINC.  If you spin the knob, you'll notice that it likes to come to rest
	// at specific angles (we'll call them grooves).  There are two mechanical switches (A and B) 
	// that toggle based on which way you rotate the knob.  If you start in a groove and start  
	// to rotate the knob CW, one of the switches will drop first (say A), then the other will 
	// drop after rotating a tiny bit further (say B).  As you approach the next groove, A rises, then B.
	//
	// In other words, PINC will look like this if rotating clockwise:
	// PINC => 0b11xx xxxx (in a groove)
	// PINC => 0b01xx xxxx (rotating out of the groove CW)
	// PINC => 0b00xx xxxx (in the middle of two grooves)
	// PINC => 0b10xx xxxx (approaching the next groove)
	// PINC => 0b11xx xxxx (in the next groove)
	//

	// static variable to store the old value of A and B.
	// This variable will only be initialized the first time you call this function.
	static unsigned char old_value = ENC_GROOVE;		// Step 2: Based on how you decided to mask PINC in step 1, what would new_value be if the switch is in a groove?
	
	// Function variables
	unsigned char new_value = (PINC & ENC_MASK);			// Step 1: Decide how to read PINC so that the push buttons are masked
	signed char rotation = 0;


	if (old_value == ENC_GROOVE) {		// If the knob was in a groove
		if (new_value == ENC_CW)		// Rotating CW
			rotation = 1;
		if (new_value == ENC_CCW)		// Rotating CCW
			rotation = -1;
	}
	old_value = new_value;

	return rotation;
}



/// Initialize PORTE to control the stepper motor
void stepper_init(void) {
	DDRE |= 0xF0;  	//Setting PE4-PE7 to output
	PORTE &= 0x8F;  //Initial postion (0b1000) PE4-PE7
	wait_ms(2);
	PORTE &= 0x0F;  //Clear PE4-PE7
}

/// Turn the Stepper Motor
/**
 * Turn the stepper motor a given number of steps. 
 *
 * @param num_steps A value between 1 and 200 steps (1.8 to 360 degrees)
 * @param direction Indication of direction: 1 for CW and -1 for CCW 
 */
static uint8_t current_position = 0;
#define STEPPER_MASK 0xF0
#define STEP_DELAY 2
void  move_stepper_motor_by_step(int num_steps, int direction) {
	//
	// DELETE ME 
	// ----------
	// The stepper motor operates off PORTE, which also powers the servo for the sonar.  It's
	// important not to confuse the two.  The stepper motor connects to the iRobot, so the iRobot
	// must be powered on.
	// 
	// Pins 4 through 7 of PORTE control the stepper motor.  In order to turn the motor one "step", 
	// you'll need to send a series of signals to PORTE.  It's important that you only affect the 
	// left 4 bits of PORTE and do not alter the right 4 bits when sending output signals to PORTE.
	//
	// To turn clockwise, send the following pattern to the left four bits of PORTE:
	// 0b0001  - starting position
	// 0b0010  - one step CW
	// 0b0100  - two steps CW
	// 0b1000  - three steps CW
	// 0b0001  - four steps CW
	// ...
	// To turn counter clockwise, simply reverse the order.
	//
    // Remember to wait a short amount of time between each step so that the motor has time to turn.

	// INSERT CODE HERE

	// Requirements
	// - Use bitwise operators to ensure the right 4 bits of PORTE are not affected
	// - Set the left 4 bits of PORTE to 0's at the end of your function (to allow the stepper motor to spin freely)
	int16_t i = 0;
	if (direction == 1) /* CW */
	{
		for (i = 0; i < num_steps; i++)
		{
			PORTE = ((0x10 << (current_position++ % 4)) & STEPPER_MASK); /* 2^8 is a multiple of 4 and unsigned overflow IS defined behavior! */
			wait_ms(STEP_DELAY);
		}
	}
	else if (direction == -1) /* CCW */
	{
		for (i = 0; i < num_steps; i++)
		{
			PORTE = ((0x10 << (current_position-- % 4)) & STEPPER_MASK);  /* See above comment. -1 overflows to 255. 255 mod 4 is 3. */
			wait_ms(STEP_DELAY);
		}
	}
	
}
