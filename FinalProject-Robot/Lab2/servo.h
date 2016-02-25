/*!
 * \file servo.h
 *
 * \author Jake Drahos
 *
 * Blocking/NonBlocking Servo API
 *
 * This API uses a number of conversion functions and a core
 * set function, SERVO_SetNonBlocking(). It also contains several
 * wrapper macros that wrap the conversion and call to the set function
 * in a single statement.
 */ 


#ifndef SERVO_H_
#define SERVO_H_

#include <stdint.h>
#include "util.h"
 
/*! Turn the servo control on.
 *
 * This resets the servo to 0 degrees and powers it on.
 */
void SERVO_On();

/*! Disable servo and stop PWM output, allowing free rotation */
void SERVO_Off();

/*! Convert the given absolute position to a PWM value 
 *
 * \param position 0-255 value to be converted to min-full PWM value
 * \return PWM value that can be given to SERVO_SetNonBlocking() or SERVO_SetBlocking()
 */
uint16_t SERVO_PositionToPulseLen(uint8_t position);

/*! Convert the given number of degrees to a PWM value
 * \param degrees 0-180 degrees to convert
 * \return PWM value for SERVO_SetNonBlocking() or SERVO_SetBlocking()
 */
uint16_t SERVO_DegreesToPulseLen(uint8_t degrees);

/*! Set the PMW value and return recommended msec delay 
 *
 * This function wil probably not be called directly, since the
 * PWM value required for this function must be calculated with
 * one of the conversion functions. It is wrapped by
 * several macros.
 * 
 * This function is asynchronous and returns immediately, but gives
 * a recommended msec delay.
 *
 * \param pwm Raw PWM value to set.
 * \return Recommended delay in msec. 
 */
uint16_t SERVO_SetNonBlocking(uint16_t pwm);

/*! Wrapper macro that blocks based on the recommended block time */
#define SERVO_SetBlocking(x) wait_ms(SERVO_SetNonBlocking(x))

/*! Wrapper macro that converts degrees, sets, and returns the recommended delay */
#define SERVO_SetDegreesNonBlocking(x) SERVO_SetNonBlocking(SERVO_DegreesToPulseLen(x))

/*! Wrapper macro that converts 0-255, sets, and returns the recommended delay */
#define SERVO_SetPositionNonBlocking(x) SERVO_SetNonBlocking(SERVO_PositionToPulseLen(x))

/*! Wrapper macro that converts degrees, sets, and blocks the recommended delay time */
#define SERVO_SetDegreesBlocking(x) SERVO_SetBlocking(SERVO_DegreesToPulseLen(x))



#endif /* SERVO_H_ */