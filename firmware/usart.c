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

/*
 * function prototypes
 */
void usart_init_internal(unsigned int ubrr);

/*
 * local functions
 */

void usart_init(void)
{
    usart_init_internal(CAL_UBRR);
}

inline void usart_init_internal(unsigned int ubrr)
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

inline void usart_transmit(unsigned char data)
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

inline unsigned char usart_receive(void)
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

inline void usart_flush(void)
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
        usart_transmit(pgm_read_byte(&(*string++)));
}

//***************************************************
//Function to transmit a string in RAM
//***************************************************
void transmitString(char *string)
{
    while (*string)
        usart_transmit(*string++);
}

typedef struct _simple_protocol_packet_t
{
    uint8_t version : 4;
    uint8_t reserved : 4;
    int16_t number;
    uint8_t crc;
    char end;
} __attribute__((packed)) simple_protocol_packet_t;

simple_protocol_packet_t tx_packet;

void usart_simple_protocol_init(void)
{
    tx_packet.version = 0x1;
    tx_packet.reserved = 0xf;
    tx_packet.number = 0x0000;
    tx_packet.crc = 0x00;
    tx_packet.end = '\n';
}

void usart_simple_protocol_cal_crc(void)
{
    uint8_t byte, crc, fbck, i, j;
    const uint8_t CRC_POLY = 0x18; // 0X18 = X^8+X^5+X^4+X^0

    uint8_t *data = (uint8_t *)&tx_packet;
    uint8_t data_len = sizeof(simple_protocol_packet_t) - 2;

    crc = 0x00;
    for (j = 0; j != data_len; j++)
    {
        byte = data[j];
        for (i = 0; i < 8; i++)
        {
            fbck = (crc ^ byte) & 0x01;
            if (fbck == 0x01)
                crc ^= CRC_POLY;
            crc = (crc >> 1) & 0x7F;
            if (fbck == 0x01)
                crc |= 0x80;
            byte >>= 1;
        }
    }

    tx_packet.crc = crc;
}

void usart_simple_protocol_transmit(int16_t number)
{
    tx_packet.number = number;
    usart_simple_protocol_cal_crc();

    uint8_t *data = (uint8_t *)&tx_packet;
    for (uint8_t i = 0; i < sizeof(simple_protocol_packet_t); ++i)
    {
        usart_transmit(data[i]);
    }
}

void usart_simple_protocol_transmit_clear(void)
{
    usart_simple_protocol_transmit(USART_SP_CLEAR_FLAG);
}
