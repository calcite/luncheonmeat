/*
 *  Copyright (C) 2017 Juraj Giertl
 *  All Rights Reserved.
 */
#include "matrix.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

/* compatibility macros for old style */
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define PIN(x) (*(&x - 2)) // return address of PIN from PORT

volatile static uint16_t g_idle_cnt = IDLE_CNT_MAX;
volatile static device_idle g_dev_idle_cb = NULL;

#define matric_keypad_idle_cnt_reset() g_idle_cnt = IDLE_CNT_MAX

/*
 * function prototypes
 */

/*
 * local functions
 */

#define timer2_set_time() TCNT1 = SET_TIME_TIMER1;
#define timer2_run() TCCR2B |= (1 << CS22) | (0 << CS21) | (1 << CS20)
#define timer2_stop() TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20))

void matrix_keypad_timer_2_init(void)
{
    // Normal operation
    TCCR2B = (0 << CS22) | (0 << CS21) | (1 << CS20); // /1

    // TCCR1A = (1 << COM1A1) | (1 << COM1B1);
    // TCCR1A |= (0 << WGM11) | (0 << WGM10)
    // TCCR1B |= (0 << WGM12) ;

    TIMSK2 |= (1 << TOIE2); // Overflow Interrupt Enable

    TCNT2 = 0x0000;
}

static inline void matric_keypad_set_col_port_out(void)
{
    sbi(COL_0_DDR, COL_0_PIN); /* COL 0 pin as output */
    sbi(COL_1_DDR, COL_1_PIN); /* COL 1 pin as output */
    sbi(COL_2_DDR, COL_2_PIN); /* COL 2 pin as output */
}

static inline void matric_keypad_set_row_port_in(void)
{
    cbi(ROW_0_DDR, ROW_0_PIN); /* ROW 0 pin as input */
    cbi(ROW_1_DDR, ROW_1_PIN); /* ROW 1 pin as input */
    cbi(ROW_2_DDR, ROW_2_PIN); /* ROW 2 pin as input */
    cbi(ROW_3_DDR, ROW_3_PIN); /* ROW 2 pin as input */
}

static inline void matric_keypad_set_row_port_pull_up(void)
{
    sbi(ROW_0_PORT, ROW_0_PIN); /* ROW 0 pin as input */
    sbi(ROW_1_PORT, ROW_1_PIN); /* ROW 1 pin as input */
    sbi(ROW_2_PORT, ROW_2_PIN); /* ROW 2 pin as input */
    sbi(ROW_3_PORT, ROW_3_PIN); /* ROW 2 pin as input */
}

int8_t matric_keypad_row_read(uint8_t row, int8_t h_key)
{
    if (bit_is_clear(PIN(ROW_0_PORT), ROW_0_PIN))
    {
        if (h_key != 0)
            return -1;
        else
            h_key = (row << 4) | 0x0A;
    }
    if (bit_is_clear(PIN(ROW_1_PORT), ROW_1_PIN))
    {
        if (h_key != 0)
            return -1;
        else
            h_key = (row << 4) | 0x0B;
    }
    if (bit_is_clear(PIN(ROW_2_PORT), ROW_2_PIN))
    {
        if (h_key != 0)
            return -1;
        else
            h_key = (row << 4) | 0x0C;
    }
    if (bit_is_clear(PIN(ROW_3_PORT), ROW_3_PIN))
    {
        if (h_key != 0)
            return -1;
        else
            h_key = (row << 4) | 0x0D;
    }
    return h_key;
}

int8_t matric_keypad_read(void)
{
    int8_t h_key = 0;

    COL_0_PORT &= ~(_BV(COL_0_PIN));
    h_key = matric_keypad_row_read(1, h_key);
    COL_0_PORT |= (_BV(COL_0_PIN));
    if (h_key == -1)
        goto ERROR;
    _delay_us(10);

    COL_1_PORT &= ~(_BV(COL_1_PIN));
    h_key = matric_keypad_row_read(2, h_key);
    COL_1_PORT |= (_BV(COL_1_PIN));
    if (h_key == -1)
        goto ERROR;
    _delay_us(10);

    COL_2_PORT &= ~(_BV(COL_2_PIN));
    h_key = matric_keypad_row_read(3, h_key);
    COL_2_PORT |= (_BV(COL_2_PIN));
    if (h_key == -1)
        goto ERROR;
    _delay_us(10);

ERROR:
    if (h_key != 0)
        matric_keypad_idle_cnt_reset();
    return h_key;
}

void matric_keypad_wait_key_release(void)
{
    int key;
    do
    {
        _delay_us(50);
        key = matric_keypad_read();
    } while (key != 0);
}

int8_t matrix_keypad_decode_key(int8_t key_hex_num)
{
    switch (key_hex_num)
    {
    case 0x1A:
        return 9;
        break;
    case 0x2A:
        return 8;
        break;
    case 0x3A:
        return 7;
        break;
    case 0x1B:
        return 6;
        break;
    case 0x2B:
        return 5;
        break;
    case 0x3B:
        return 4;
        break;
    case 0x1C:
        return 3;
        break;
    case 0x2C:
        return 2;
        break;
    case 0x3C:
        return 1;
        break;
    case 0x1D:
        return MK_KEY_ENTER;
        break;
    case 0x2D:
        return 0;
        break;
    case 0x3D:
        return MK_KEY_CLEAR;
        break;
    default:
        return -1;
        break;
    }
}

inline bool matrix_keypad_is_enter_key(int8_t key)
{
    return key == MK_KEY_ENTER;
}

inline bool matrix_keypad_is_clear_key(int8_t key)
{
    return key == MK_KEY_CLEAR;
}

int8_t matrix_keypad_process()
{
    int8_t key;

    //0 adresa, 1 prikaz, data0, data1, data2, data3, data4, data5, data6;
    //casova klavesnice klavesovy buffer
    // #define max_ptr 12
    // int zmacknute_klavesy_buff[max_ptr] = {0};
    // uint8_t tlac_ptr = 0;

    key = matric_keypad_read();
    matric_keypad_wait_key_release();
    // while (1)
    // {
    //     switch (key)
    //     {
    //     case 0:
    //         break;
    //     case -1:
    //         matric_keypad_wait_key_release();
    //         tlac_ptr = 0;
    //         return;
    //         break;
    //     case KLC_ENTER: //KLC_CLEAR //vyhodnoceni zmacknutych klaves
    //         // Vyhodnoceni_zmacknutych_klaves_casu(zmacknute_klavesy_buff, tlac_ptr);
    //         matric_keypad_wait_key_release();
    //         tlac_ptr = 0;
    //         return key;
    //         break;
    //     default:
    //         if (tlac_ptr < (max_ptr - 1))
    //             zmacknute_klavesy_buff[tlac_ptr++] = key;
    //         else
    //         {
    //             matric_keypad_wait_key_release();
    //             tlac_ptr = 0;
    //             return key;
    //         }
    //         matric_keypad_wait_key_release();
    //         break;
    //     }

    //     key = matric_keypad_read();
    // }

    return matrix_keypad_decode_key(key);
}

void matrix_keypad_register_idle_cb(device_idle cb)
{
    if (cb != NULL)
    {
        g_dev_idle_cb = cb;
    }
}

void matrix_keypad_init(void)
{
    matrix_keypad_timer_2_init();

    matric_keypad_set_row_port_in();
    matric_keypad_set_row_port_pull_up();
    matric_keypad_set_col_port_out();
}

ISR(TIMER2_OVF_vect)
{
    if (g_idle_cnt == 0)
    {
        timer2_stop();
        if (g_dev_idle_cb != NULL)
            g_dev_idle_cb();
    }
    g_idle_cnt--;
}
