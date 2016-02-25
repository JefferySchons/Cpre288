/* Servo.c */

/* Prelab questions
 * Pulse Widths: 1-2ms every 20 ms. Full CCW at 1000 uS and full CW at 2000 uS. 1520 uS for center
 *
 * Registers used:
 *    TCCR3A - COM3B1:0 = 2 (non-inverting mode) | WGM31:0 = 3 (half of 0x0F for Fast mode, OCR3A as top)
 *    TCCR3B - WGM33:2 = 3 (other half of full WGM) | CS31 (/8 prescaler)
 *    TCNT3 - Load with 0 to start timer
 *    OCR3A - TOP
 *    OCR3B - Load with calculated PWM value
 * 
 * /8 Prescaler, TOP = 43002
 * In non-inverting mode, the output pin is SET when TOP is hit and the timer resets to zero when OCRnx is hit.
 * 
 * Timer3 Channel B output pin OC3B = PE4 = SERV3B
 */
 
 #define TOP 43002
 
 #include "servo.h"
 #include "avr/io.h"
 #include "util.h"
 #include "math.h"
 #include "stdlib.h"
 
 uint16_t SERVO_PositionToPulseLen(uint8_t position)
 {
	/* Given a 0-256 position, calculate 1000-2000usec for OCR
	 * 2000 us for 0-256
	 */
	return position * 16 + 800;
 } 
 
 uint16_t SERVO_DegreesToPulseLen(uint8_t degrees)
 {
	/* Given a 0-180 degrees, calculate 1000-2000usec for OCR
	 * 2000 us for 0-180
	 */
	if (degrees > 100)
	{
		degrees -= 2; /* Account for some nonlinearity */
	}
	return degrees * 20 + 975;
 }
 
 void SERVO_On()
 {
	/* Initialize all registers */
	TCCR3A = (2 << COM3B0) | (3 << WGM30);
	TCCR3B = (3 << WGM32) | (1 << CS31);
	OCR3A = TOP;
	OCR3B = SERVO_PositionToPulseLen(0); /* Set servo to full CW */
	TCNT3 = 0; /* Start timer */
	DDRE |= (1 << DDE4); /* Set OC pin as output */
 }
 
 void SERVO_Off()
 {
	TCCR3B = 0; /* Set CS to 0, stopping timer */
 }
 
 uint16_t SERVO_SetNonBlocking(uint16_t ocr)
 {
	uint16_t last_ocr = OCR3B;
	OCR3B = ocr; /* Load OCR3B with value at which output should shut off */
	
	return 70 + (((abs(ocr-last_ocr))/20) * 10); /* Estimate good time to delay */
 }