/*!
 * \file ping.h
 * \author Jake Drahos
 * 
 * Blocking driver for Ping))) sensor.
 */ 


#ifndef PING_H_
#define PING_H_
#include <stdint.h>

/*! Initialization function. Call before using any other ping.h functions
*/
void PING_Initialize();

/*! Perform a Ping))) measurement
 *
 * This function will block for the duration of the measurement, then return
 * the number of timer ticks. This tick count must be converted using
 * PING_TicksToUS() or PING_TicksToCM().
 *
 * \return Number of timer ticks in pulse
 */
uint16_t PING_MeasureTicks();

/*! Convert ticks to microseconds
 *
 * \param ticks Tick count from PING_MeasureTicks()
 * \return Number of microseconds in the pulse
 */
uint16_t PING_TicksToUS(uint16_t ticks);

/*! Convert ticks to centimeters
 *
 * \param ticks Tick count from PING_MeasureTicks()
 * \return Distance in centimeters to the nearest object
 */
uint16_t PING_TicksToCM(uint16_t ticks);

/*! Get the number of overflows that have occurred
 *
 * Utility function that isn't really useful any more
 */
uint16_t PING_GetOverflows();

/*! Wrapper macro. Wraps PING_MeasureTicks() in PING_TicksToCM() */
#define PING_MeasureCM() PING_TicksToCM(PING_MeasureTicks())

#endif /* PING_H_ */