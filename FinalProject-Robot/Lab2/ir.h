#ifndef __IR_H
#define __IR_H
#include <stdint.h>
#include <avr/pgmspace.h>

/*!
 * \file ir.h
 * Asynchronous IR api.
 * \author Jake Drahos
 */

/*! Initialize IR
 * This must be called to set up the interrupts
 * before any other IR functions are called. If a RAM lookup table
 * is being used, this function will also populate the lookup table.
 */
void IR_Initialize();

/*! Get the most recent IR measurement
 * 
 * The IR API is completely asynchronous. The ADC is
 * continuously sampled. IR_GetData() simply returns the
 * most recent sample. IR_GetDataBlocking() blocks until
 * new data is available before returning.
 *
 * \return Raw ADC value of IR sensor
 */
uint16_t IR_GetData();

/*! Block until new data is available, then return it.
 * 
 * Similar to IR_GetData(), but waits for new data
 * before returning.
 *
 * \return Raw ADC value of IR sensor
 */
uint16_t IR_GetDataBlocking();

/*! Check if new data is available
 * 
 * \return TRUE if new data is available, FALSE if not
 */
uint8_t IR_NewData();

/*! Convert an IR value to distance
 *
 * Uses the lookup table (RAM or ROM) to convert an IR value
 * from IR_GetData() to millimeters.
 * \param value ADC Value from IR_GetData()
 * \return Distance in millimeters
 */
uint16_t IR_ValueToDist(uint16_t value);

/*! Macro that wraps IR_GetDataBlocking() in IR_ValueToDist()
 */
#define IR_GetDistBlocking() IR_ValueToDist(IR_GetDataBlocking())

/*! Macro that wraps IR_GetData() in IR_ValueToDist()
 */
#define IR_GetDist() IR_ValueToDist(IR_GetData())

/*! Define this to 1 to put the lookup table in ROM. Requires lookup_table.c 
 * If it is defined to 0, the lookup table will be put in RAM.
 */
#define LOOKUP_TABLE_IN_ROM 0

#endif