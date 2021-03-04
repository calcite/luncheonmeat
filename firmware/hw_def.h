/*
 *  Copyright (C) 2021 Juraj Giertl
 *  All Rights Reserved.
 */
#ifndef _HW_DEF_H_
#define _HW_DEF_H_

#include <avr/version.h>

#define HW_VER_TAG "v0.2.4"
#define SW_VER_TAG "v0.4.3"
#define COMMENT "Luncheonmeat (" SW_VER_TAG "). "              \
                "Simple keypad controle device with display. " \
                "Compiled: " __DATE__ ", " __TIME__ ". "       \
                "Using AVR LIBC (" __AVR_LIBC_VERSION_STRING__ ")."

#define LED_G PC0
#define LED_PORT PORTC
#define LED_DDR DDRC

#define MATRIX_LAYPUT_PHONE_INVERTED

#define SEG_DISPLAY_INVERTED
#define SEG_DISPALY_SINGLE_DIGIT_OPT

void io_init(void);

#endif // HW_DEF_H
