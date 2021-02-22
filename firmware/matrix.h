/*
 *  Copyright (C) 2017 Juraj Giertl
 *  All Rights Reserved.
 */
#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <avr/io.h>
#include <stdbool.h>
#include "hw_def.h"

#define ROW_0_PIN		PINB1
#define ROW_1_PIN		PINB0
#define ROW_2_PIN		PIND7
#define ROW_3_PIN		PIND6
#define COL_0_PIN		PINB4
#define COL_1_PIN		PINB3
#define COL_2_PIN		PINB2

#define ROW_0_DDR		DDRB
#define ROW_1_DDR		DDRB
#define ROW_2_DDR		DDRD
#define ROW_3_DDR		DDRD
#define COL_0_DDR		DDRB
#define COL_1_DDR		DDRB
#define COL_2_DDR		DDRB

#define ROW_0_PORT      PORTB
#define ROW_1_PORT      PORTB
#define ROW_2_PORT      PORTD
#define ROW_3_PORT      PORTD
#define COL_0_PORT      PORTB
#define COL_1_PORT      PORTB
#define COL_2_PORT      PORTB

#define IDLE_CNT_MAX 610; // is 20 seconds, s * 30.5 
#define MK_KEY_CLEAR (0x2A)
#define MK_KEY_ENTER (0x23)

typedef void (*device_idle)(void);

void matrix_keypad_register_idle_cb(device_idle cb);
void matrix_keypad_idle_reset(void);

bool matrix_keypad_is_enter_key(int8_t key);
bool matrix_keypad_is_clear_key(int8_t key);

void matric_keypad_wait_key_release(void);
int8_t matrix_keypad_process(void);
void matrix_keypad_init(void);

#endif