/*
 * avr_usart.c
 *
 * Created: 2/19/2015 2:03:52 PM
 *  Author: drahos
 */ 
#include "avr_usart.h"
#include "stdio.h"
#include <stdarg.h>
#include "stdlib.h"

#include <avr/interrupt.h>
#include <util/atomic.h>

#define RINGBUFFER_SIZE 256

#define bitSet(reg, bit) ((reg) |= (bit))
#define bitClear(reg, bit) ((reg) &= (~(bit)))


static uint8_t txBuffer[RINGBUFFER_SIZE];
static uint8_t rxBuffer[RINGBUFFER_SIZE];

/* Tail is accessed only by ISR, head is accessed only by userspace */
static uint8_t *txHead = txBuffer;
static uint8_t *txTail = txBuffer;

/* Tail is accessed only in userspace, head is accessed only in ISR */
static uint8_t *rxHead = txBuffer;
static uint8_t *rxTail = txBuffer;

static status_t usart_send_byte();
static status_t usart_get_byte();

#define UDR UDR0
#define UCSRA UCSR0A
#define UCSRB UCSR0B
#define UCSRC UCSR0C
#define UBRRL UBRR0L
#define UBRRH UBRR0H

/* Use asynchronous uart */
#define USE_ASYNC_RX 1
#define USE_ASYNC_TX 1

status_t USART_Initialize()
{
	UBRRH = 0;
	UBRRL = 34;
    
	UCSRA = (1<<U2X); /* 2X speed */
    UCSRB = (1<<RXEN) | (1<<TXEN) | (1<<RXCIE); /* TX and RX enabled, RX and DR Empty interrupts enabled */
	UCSRC = (1<<USBS) | (3<<UCSZ0); /* no parity, 2 stop bits, 8 bit */
    
    return STATUS_SUCCESS;
}

/* Put character into the TX ringbuffer. If UART is not already active, start it */
status_t USART_Putc(char byte)
{
	/* Define to switch between synchronous and asynchronous uart */
#ifdef USE_ASYNC_TX
    *txHead = byte;
    txHead++;
    if (txHead >= txBuffer + RINGBUFFER_SIZE)
        txHead = txBuffer;
		
	/* AVR Doesn't have software interrupts, so we can't just trigger the
	 * interrupt here and let it handle the "first char" situation. Atomically
	 * check to see if the USART is active, and if it isn't, kick it off 
	 
	 **/
    ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		if (UCSRA & (1<<UDRE))
		{
			usart_send_byte();
		}
	}
	UCSRB |= (1<<UDRIE);
#else
	while (( UCSRA & (1<<UDRE)) == 0 ) {}
	UDR = byte;
#endif 
    return STATUS_SUCCESS;
}

/* Return the tail character of the RX Ringbuffer and advance the tail */
status_t USART_Getc(char * byte)
{
	/* Define to switch between synchronous and asynchronous UART */
#if USE_ASYNC_RX
    if (rxHead == rxTail)
        return STATUS_RINGBUFFER_EMPTY;
    
    *byte = *rxTail;
    rxTail++; /* Advance tail and handle wraparound condition */
    if (rxTail >= rxBuffer + RINGBUFFER_SIZE)
        rxTail = rxBuffer;
#else
	while ((UCSRA&(1<<RXC)) == 0) {}
	*byte = UDR;
#endif
    return STATUS_SUCCESS;
}

/* Put string of length len into TX ringbuffer */
int16_t USART_Print(char *str, uint16_t len)
{
    uint16_t i = 0;
    for(i = 0; i < len; i++)
    {
        USART_Putc(str[i]);
    }
    return len;
}

/* Get up to maxLen chars from RXbuffer and store in buffer */
int16_t USART_Gets(char *buffer, uint16_t maxLen)
{
    uint16_t len = 0; 
    char *cursor;
    
    cursor = buffer;
    while ((len < maxLen) && (USART_Getc(cursor) == STATUS_SUCCESS)) /* Abort on maxLen or rxBuffer empty */
    {
        cursor++;
        len++;
    }
    
    return len;
}

/* Called by ISR to retrieve from DR and put into rx buffer */
static status_t usart_get_byte()
{
	*rxHead = UDR;
	rxHead++;
	if(rxHead >= rxBuffer + RINGBUFFER_SIZE)
		rxHead = rxBuffer;
	return STATUS_SUCCESS;
}

/* Called by ISR to put from tail of TX buffer into DR */
static status_t usart_send_byte()
{
    if (txHead != txTail) 
    {
        UDR = *txTail & 0xff; /* TX tail byte and advance tail pointer */
        txTail++;
        if (txTail >= txBuffer + RINGBUFFER_SIZE)
			txTail = txBuffer;
        return STATUS_SUCCESS;
    }
    else
    {
		UCSRB &= ~(1<<UDRIE);
        return STATUS_RINGBUFFER_EMPTY; /* Handle empty buffer condition */
    }
}

/* Send the next queued byte when the DR is empty */
ISR(USART0_UDRE_vect)
{
	usart_send_byte();
}

/* Put the received byte into the RX queue */
ISR(USART0_RX_vect)
{
	usart_get_byte();
}