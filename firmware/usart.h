/*
 *  Copyright (C) 2020 Juraj Giertl
 *  All Rights Reserved.
 */
#ifndef _USART_H_
#define _USART_H_

#define BAUD 19200
// #define BAUD 9600
#define CAL_UBRR (F_CPU / 16 / BAUD - 1)

#define USART_SP_CLEAR_FLAG (0xFFFF)

#define CHAR 0
#define INT 1
#define LONG 2

#define TX_NEWLINE          \
    {                       \
        usart_transmit(0x0d); \
        usart_transmit(0x0a); \
    }

void usart_init(void);
void usart_transmit(unsigned char data);
void usart_flush(void);
unsigned char usart_receive(void);

void transmitString_F(const char *);
void transmitString(char *);
void transmitHex(unsigned char dataType, unsigned long data);


void usart_simple_protocol_init(void);
void usart_simple_protocol_transmit(int16_t number);
void usart_simple_protocol_transmit_clear(void);

#endif
