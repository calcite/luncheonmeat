/*
 *  Copyright (C) 2020 Juraj Giertl
 *  All Rights Reserved.
 */
#ifndef _USART_H_
#define _USART_H_

#define BAUD 19200
// #define BAUD 9600
#define CAL_UBRR (F_CPU / 16 / BAUD - 1)

#define CHAR 0
#define INT 1
#define LONG 2

#define TX_NEWLINE          \
    {                       \
        USART_Transmit(0x0d); \
        USART_Transmit(0x0a); \
    }

void usart_init(void);
void USART_Init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
void USART_Flush(void);
unsigned char USART_Receive(void);

void transmitString_F(const char *);
void transmitString(char *);
void transmitHex(unsigned char dataType, unsigned long data);

#endif
