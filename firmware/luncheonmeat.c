/*
 *  Copyright (C) 2020 Juraj Giertl
 *  All Rights Reserved.
 */
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "matrix.h"
#include "display.h"
#include "usart.h"
#include "hw_def.h"

#ifndef F_CPU
#warning "F_CPU not defined"
#endif

#define GET_INT(s) ((s)-0x30)


const char comment[] PROGMEM = COMMENT;
const char fw_ver[] PROGMEM = HW_VER_TAG;
const char sw_ver[] PROGMEM = SW_VER_TAG;
const uint8_t seg_disp_contrast EEMEM = 128;

volatile bool zero_flag = false;

typedef struct _num_buf_t
{
#define NUM_BUF_MAX (3)
    uint8_t head_, tail_;
    bool full, empty;
    int16_t number;
    uint8_t digit[NUM_BUF_MAX];
} num_buf_t;

num_buf_t disp_num_buf;

/*
 * local functions
 */
void buf_num_init(num_buf_t *n_buf)
{
    n_buf->head_ = 0;
    n_buf->tail_ = 0;
    n_buf->full = false;
    n_buf->empty = true;
    n_buf->number = 0;
    for (uint8_t i = 0; i < NUM_BUF_MAX; ++i)
        n_buf->digit[i] = 0;
}

void buf_num_put(num_buf_t *n_buf, uint8_t digit)
{
    n_buf->head_ = (n_buf->head_ + 1) % NUM_BUF_MAX;
    if (n_buf->head_ == n_buf->tail_)
    {
        n_buf->tail_ = (n_buf->tail_ + 1) % NUM_BUF_MAX;
        n_buf->full = true;
    }

    n_buf->digit[n_buf->head_] = digit;
    n_buf->empty = false;
}

uint8_t buf_num_get(num_buf_t *n_buf)
{
    n_buf->full = false;
    if (n_buf->head_ == n_buf->tail_) //empty
    {
        n_buf->empty = true;
        return -1;
    }
    uint8_t val = n_buf->digit[n_buf->tail_];
    n_buf->tail_ = (n_buf->tail_ + 1) % NUM_BUF_MAX;
    return val;
}

bool buf_num_full(num_buf_t *n_buf)
{
    return n_buf->full;
}

bool buf_num_empty(num_buf_t *n_buf)
{
    return n_buf->empty;
}

void buf_num_get_number(num_buf_t *n_buf, int16_t *num)
{
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

void io_init(void)
{
    LED_DDR |= (1 << LED_G);
    LEDG_OFF;
}

void init(void)
{
    io_init();
    usart_init();
    usart_simple_protocol_init();
    matrix_keypad_init();
    seg_display_init();

    // enable global interrupts
    sei();

    seg_dispaly_set_digits(GET_INT(sw_ver[1]), GET_INT(sw_ver[3]), GET_INT(sw_ver[5]));
    _delay_ms(2000);
    LEDG_ON;
}

void device_idle_callback(void)
{
    seg_display_clear();
    buf_num_init(&disp_num_buf);
}

int main(void)
{
    int8_t key;
    int16_t number;
    uint32_t et_count = 0; //elapsed time count

    init();
    matrix_keypad_register_idle_cb(device_idle_callback);
    buf_num_init(&disp_num_buf);

    seg_display_set_contrast(seg_disp_contrast);

    seg_display_wait_animation(0, 2, 100);
    seg_display_wait_animation(0, 2, 100);
    seg_display_clear();

    while (1)
    {
        _delay_ms(25);
        key = matrix_keypad_process(&et_count);

        if (key == -1)
            continue;

        if (matrix_keypad_is_enter_key(key))
        {
            // transmitString_F(PSTR(" enter "));
            if (buf_num_empty(&disp_num_buf))
                continue;

            buf_num_get_number(&disp_num_buf, &number);
            usart_simple_protocol_transmit(number);

            buf_num_init(&disp_num_buf);
            seg_display_draw_circle();
            _delay_ms(500);
            seg_display_clear();
        }
        else if (matrix_keypad_is_clear_key(key))
        {
            // transmitString_F(PSTR(" clear "));
            buf_num_init(&disp_num_buf);

            if (matric_keypad_is_long_push(et_count))
                usart_simple_protocol_transmit_clear();

            seg_display_draw_line();
            _delay_ms(500);
            seg_display_clear();
        }
        else
        {
            if (buf_num_full(&disp_num_buf))
                continue;

            if (zero_flag)
            {
                zero_flag = false;
                buf_num_init(&disp_num_buf);
            }

            if (buf_num_empty(&disp_num_buf) && key == 0)
                zero_flag = true;

            buf_num_put(&disp_num_buf, key);
            buf_num_get_number(&disp_num_buf, &number);
            seg_dispaly_set_dec_num(number);
        }
        // transmitHex(INT, MK_LONG_PUSH_TRESHHOLD);
        // transmitString_F(PSTR(" \r\n--> "));
        // if (contrast == 0xff)
        //     contrast = 0;
        // contrast++;
        // seg_display_set_contrast(contrast);
    }
    return 0;
}
