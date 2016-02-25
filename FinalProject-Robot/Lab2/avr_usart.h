/*!
 * \file avr_usart.h
 *
 * \author Jake Drahos
 * This contains the USART functions. The USART driver is completely
 * asynchronous. User-space functions only manipulate a collection
 * of ringbuffers. The ringbuffers are only accessed single-ended by
 * either the ISR or userspace,
 * so they are interrupt-safe.
 * Because it is asynchronous, it is FAST.
 */ 

#include "string.h"
#include <stdint.h>
#ifndef USART_H
#define USART_H

/*! Used for status codes */
typedef uint8_t status_t;

/*! Operation successful */
#define STATUS_SUCCESS 0

/*! Tried to pull from empty buffer */
#define STATUS_RINGBUFFER_EMPTY 80

/* Initialization function. Call before ANY usart operations */
status_t USART_Initialize();

/*! Put a character into the TX buffer
 *
 * \param c Character to eventually TX
 * \return STATUS_SUCCESS or explosions
 */
status_t USART_Putc(char c);

/*! Get a character from the ringbuffer
 * 
 * \param c Store character in c
 * \return STATUS_SUCCESS or STATUS_RINGBUFFER_EMPTY
 */
status_t USART_Getc(char * c);

/*! Pull a string from the receive buffer
 * 
 * Fetches up to maxlen chars and returns them to buffer. Does not
 * null-terminate or anything like that. Call USART_Gets(buff, 1)
 * to fetch a single character.
 *
 * \param buffer Destination for pulled string
 * \param maxlen Maximum length of string to fetch from buffer
 * \return The actual number of characters fetched.
 */
int16_t USART_Gets(char * buffer, uint16_t maxlen);

/*! Print up to len chars to the TX buffer
 *
 * \param str String to print
 * \param len Length of str
 * \return Number of characters printed
 */
int16_t USART_Print(char * str, uint16_t len);

/*! Printf wrapper that uses USART
 *  Do not use because it explodes! (probably)
 *
 * \param fmt Why are you still reading?
 * \return Seriously, stop. Don't use this.
 */
int16_t USART_Printf(const char *fmt, ...);

/*! Wrapper for USART_Print() that automatically calculates the
 * length. Use this with null-terminated strings.
 */
#define USART_Puts(str) USART_Print(str, strlen(str));
#endif

