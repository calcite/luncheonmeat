/*
 *  Copyright (C) 2020 Juraj Giertl
 *  All Rights Reserved.
 */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "usart.h"

#ifndef F_CPU
#error "F_CPU not defined!"
#endif

void usart_init(void)
{
    USART_Init(CAL_UBRR);
}

void USART_Init(unsigned int ubrr)
{
#if (defined __AVR_ATmega8__)
    /* Set baud rate */
    // UBRRL = (unsigned char)ubrr;
    // UBRRH = (unsigned char)(ubrr >> 8);
    // /* Enable receiver and transmitter */
    // UCSRB = (1 << RXEN) | (1 << TXEN);
    // /* Set frame format: 8data, 2stop bit */
    // UCSRC = (1 << URSEL) | (1 << USBS) | (3 << UCSZ0);
#elif defined(__AVR_ATmega328PB__)
    /*Set baud rate */
    UBRR0L = (unsigned char)ubrr;
    UBRR0H = (unsigned char)(ubrr >> 8);
    /* Enable receiver and transmitter */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    /* Set frame format: 8data, 2stop bit */
    UCSR0C = (1 << USBS0) | (3 << UCSZ00);
#elif (defined __AVR_ATmega328__)
    /*Set baud rate */
    UBRR0L = (unsigned char)ubrr;
    UBRR0H = (unsigned char)(ubrr >> 8);
    /* Enable receiver and transmitter */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    /* Set frame format: 8data, 2stop bit */
    UCSR0C = (1 << USBS0) | (3 << UCSZ00);
#else
#error "MCU not supported"
#endif
}
void USART_Transmit(unsigned char data)
{
#if (defined __AVR_ATmega8__)
    /* Wait for empty transmit buffer */
    while (!(UCSRA & (1 << UDRE)))
        ;
    /* Put data into buffer, sends the data */
    UDR = data;
#elif (defined __AVR_ATmega328__)
#define TXB8 0
    /* Wait for empty transmit buffer */
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    /* Copy 9th bit to TXB8 */
    UCSR0B &= ~(1 << TXB8);
    if (data & 0x0100)
        UCSR0B |= (1 << TXB8);
    /* Put data into buffer, sends the data */
    UDR0 = data;
#elif defined(__AVR_ATmega328PB__)
#define TXB8 0
    /* Wait for empty transmit buffer */
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    /* Copy 9th bit to TXB8 */
    UCSR0B &= ~(1 << TXB8);
    if (data & 0x0100)
        UCSR0B |= (1 << TXB8);
    /* Put data into buffer, sends the data */
    UDR0 = data;
#else
#error "MCU not supported"
#endif
}

unsigned char USART_Receive(void)
{
#if (defined __AVR_ATmega8__)
    /* Wait for data to be received */
    while (!(UCSRA & (1 << RXC)))
        ;
    /* Get and return received data from buffer */
    return UDR;
#elif (defined __AVR_ATmega328__)
    while (!(UCSR0A & (1 << RXC0)))
        ;
    /* Put data into buffer, sends the data */
    return UDR0;
#elif defined(__AVR_ATmega328PB__)
    /* Wait for data to be received */
    while (!(UCSR0A & (1 << RXC0)))
        ;
    /* Get and return received data from buffer */
    return UDR0;
#else
#error "MCU not supported"
#endif
}

void USART_Flush(void)
{
    unsigned char dummy;
#if (defined __AVR_ATmega8__)
    while (UCSRA & (1 << RXC))
        dummy = UDR;
#elif (defined __AVR_ATmega328__)
    while (UCSR0A & (1 << RXC0))
        dummy = UDR0;
#elif defined(__AVR_ATmega328PB__)
    while (UCSR0A & (1 << RXC0))
        dummy = UDR0;
#else
#error "MCU not supported"
#endif
}

//***************************************************
//Function to transmit hex format data
//first argument indicates type: CHAR, INT or LONG
//Second argument is the data to be displayed
//***************************************************
void transmitHex(unsigned char dataType, unsigned long data)
{
    unsigned char count, i, temp;
    char dataString[] = "0x        ";

    if (dataType == CHAR)
        count = 2;
    if (dataType == INT)
        count = 4;
    if (dataType == LONG)
        count = 8;

    for (i = count; i > 0; i--)
    {
        temp = data % 16;
        if ((temp >= 0) && (temp < 10))
            dataString[i + 1] = temp + 0x30;
        else
            dataString[i + 1] = (temp - 10) + 0x41;

        data = data / 16;
    }

    transmitString(dataString);
}

//***************************************************
//Function to transmit a string in Flash
//***************************************************
void transmitString_F(const char *string)
{
    while (pgm_read_byte(&(*string)))
        USART_Transmit(pgm_read_byte(&(*string++)));
}

//***************************************************
//Function to transmit a string in RAM
//***************************************************
void transmitString(char *string)
{
    while (*string)
        USART_Transmit(*string++);
}
