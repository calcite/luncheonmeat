/*
 *  Copyright (C) 2020 Juraj Giertl
 *  All Rights Reserved.
 */
#include <stdio.h>
#include <math.h>

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <avr/version.h>

#include "matrix.h"
#include "display.h"
#include "usart.h"
#include "hw_def.h"

#ifndef F_CPU
#warning "F_CPU not defined"
#endif

#define LEDG_OFF LED_PORT &= ~(1 << LED_G);
#define LEDG_ON LED_PORT |= (1 << LED_G);

const char fw_ver[] PROGMEM = "v0.2.4";
const uint16_t set_iron_tmp EEMEM = 325;
const uint8_t lcd_back_pwm EEMEM = 128;
const uint8_t lcd_contrast_pwm EEMEM = 128;

char buffer[16];
volatile int16_t t_read = 0, t_set = 0;

void io_init(void)
{
    LED_DDR |= (1 << LED_G);
    LEDG_ON;
}

void init(void)
{
    io_init();
    usart_init();
    matrix_keypad_init();
    seg_display_init();

    _delay_ms(2000);
    LEDG_OFF;
    // enable global interrupts
    sei();
}

typedef struct _num_buf_t
{
#define NUM_BUF_MAX (3)
    uint8_t head_, tail_;
    int16_t number;
    uint8_t digit[NUM_BUF_MAX];
} num_buf_t;

void buf_num_init(num_buf_t *n_buf)
{
    n_buf->head_ = 0;
    n_buf->tail_ = 0;
    n_buf->number = 0;
}

void buf_num_put(num_buf_t *n_buf, uint8_t digit)
{
    n_buf->head_ = (n_buf->head_ + 1) % NUM_BUF_MAX;
    if (n_buf->head_ == n_buf->tail_)
        n_buf->tail_ = (n_buf->tail_ + 1) % NUM_BUF_MAX;

    n_buf->digit[n_buf->head_] = digit;
}

uint8_t buf_num_get(num_buf_t *n_buf)
{
    if (n_buf->head_ == n_buf->tail_) //empty
        return -1;

    uint8_t val = n_buf->digit[n_buf->tail_];
    n_buf->tail_ = (n_buf->tail_ + 1) % NUM_BUF_MAX;
    return val;
}

void buf_num_get_number(num_buf_t *n_buf, int16_t *num)
{
    int8_t tmp = 0;
    uint8_t tail = n_buf->tail_;
    uint8_t head = n_buf->head_;

    n_buf->number = n_buf->digit[tail];

    while (head != tail)
    {
        tail = (tail + 1) % NUM_BUF_MAX;
        n_buf->number *= 10;
        n_buf->number += n_buf->digit[tail];
    }

    *num = n_buf->number;
}

int main(void)
{
    int8_t key;
    int16_t number;
    num_buf_t disp_num_buf;

    uint8_t counter = 0, contrast = 1;

    init();
    buf_num_init(&disp_num_buf);

    TX_NEWLINE;
    transmitString_F(PSTR(" Compiled: " __DATE__ ", "__TIME__
                          "\n\r"));
    transmitString_F(PSTR("*   \tE-mail:  \" jgiertl@alps.cz \"        *\r\n"));
    TX_NEWLINE;

    while (1)
    {
        _delay_ms(25);

        key = matrix_keypad_process();
        transmitHex(INT, key);
        transmitString_F(PSTR("  "));

        transmitHex(INT, contrast);

        seg_display_set_contrast(contrast);

        contrast++;
        if (contrast == 0xff)
            contrast = 0;

        transmitString_F(PSTR(" \r\n--> "));

        if (key == -1)
            continue;

        if (matrix_keypad_is_enter_key(key))
        {
            transmitString_F(PSTR(" enter "));
        }
        else if (matrix_keypad_is_clear_key(key))
        {
            transmitString_F(PSTR(" clear "));
            seg_display_clear();
        }
        else
        {
            transmitString_F(PSTR(" other "));
            buf_num_put(&disp_num_buf, key);

            buf_num_get_number(&disp_num_buf, &number);
            seg_dispaly_set_dec_num(number);
        }

        // _delay_ms(250);
        // _delay_ms(250);
    }

    return 0;
}
