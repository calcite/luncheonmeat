/*
 *  Copyright (C) 2017 Juraj Giertl
 *  All Rights Reserved.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "display.h"
#include "hw_def.h"

/* compatibility macros for old style */
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

volatile uint8_t seg_display_data[3] = {0xFF, 0xFF, 0xFF};
volatile uint8_t seg_display_digits[3] = {0, 0, 0};
volatile uint8_t seg_display_contrast = 0x00;
volatile uint8_t seg_display_n_digits = 0x03;
const uint8_t seg_display_digits_in_order[] = {seg_a, seg_b, seg_c, seg_d, seg_e, seg_f, seg_g, seg_dp};

/*
 * function prototypes
 */
inline static void seg_display_write(uint8_t d);
inline static void seg_display_data_port_out(void);
inline static seg_dispaly_set_segment(uint8_t pos, uint8_t seg_char);
inline static void seg_display_pos_gate_clear_all(void);
inline static void seg_display_pos_gate_clear(uint8_t gate_num);
inline static void seg_display_pos_gate_set(uint8_t gate_num);
void seg_display_clear(void);

/*
 * local functions
 */
inline static void seg_display_write(uint8_t d)
{
    if (d & 0x80)
        sbi(SEG_A_PORT, SEG_A_PIN);
    else
        cbi(SEG_A_PORT, SEG_A_PIN);
    if (d & 0x40)
        sbi(SEG_B_PORT, SEG_B_PIN);
    else
        cbi(SEG_B_PORT, SEG_B_PIN);
    if (d & 0x20)
        sbi(SEG_C_PORT, SEG_C_PIN);
    else
        cbi(SEG_C_PORT, SEG_C_PIN);
    if (d & 0x10)
        sbi(SEG_D_PORT, SEG_D_PIN);
    else
        cbi(SEG_D_PORT, SEG_D_PIN);
    if (d & 0x08)
        sbi(SEG_E_PORT, SEG_E_PIN);
    else
        cbi(SEG_E_PORT, SEG_E_PIN);
    if (d & 0x04)
        sbi(SEG_F_PORT, SEG_F_PIN);
    else
        cbi(SEG_F_PORT, SEG_F_PIN);
    if (d & 0x02)
        sbi(SEG_G_PORT, SEG_G_PIN);
    else
        cbi(SEG_G_PORT, SEG_G_PIN);
    if (d & 0x01)
        sbi(SEG_DP_PORT, SEG_DP_PIN);
    else
        cbi(SEG_DP_PORT, SEG_DP_PIN);
}

inline static void seg_display_data_port_out(void)
{ /* defines all data pins as output */
    sbi(SEG_A_DDR, SEG_A_PIN);
    sbi(SEG_B_DDR, SEG_B_PIN);
    sbi(SEG_C_DDR, SEG_C_PIN);
    sbi(SEG_D_DDR, SEG_D_PIN);
    sbi(SEG_E_DDR, SEG_E_PIN);
    sbi(SEG_F_DDR, SEG_F_PIN);
    sbi(SEG_G_DDR, SEG_G_PIN);
    sbi(SEG_DP_DDR, SEG_DP_PIN);
}

inline static void seg_display_pos_gate_clear_all(void)
{
    sbi(SEG_POS_0_PORT, SEG_POS_0_PIN);
    sbi(SEG_POS_1_PORT, SEG_POS_1_PIN);
    sbi(SEG_POS_2_PORT, SEG_POS_2_PIN);
}

inline static void seg_display_pos_gate_clear(uint8_t gate_num)
{
    switch (gate_num)
    {
    case SEG_POS_0:
        sbi(SEG_POS_0_PORT, SEG_POS_0_PIN);
        break;
    case SEG_POS_1:
        sbi(SEG_POS_1_PORT, SEG_POS_1_PIN);
        break;
    case SEG_POS_2:
        sbi(SEG_POS_2_PORT, SEG_POS_2_PIN);
        break;
    }
}

inline static void seg_display_pos_gate_set(uint8_t gate_num)
{
    switch (gate_num)
    {
    case SEG_POS_0:
        cbi(SEG_POS_0_PORT, SEG_POS_0_PIN);
        break;
    case SEG_POS_1:
        cbi(SEG_POS_1_PORT, SEG_POS_1_PIN);
        break;
    case SEG_POS_2:
        cbi(SEG_POS_2_PORT, SEG_POS_2_PIN);
        break;
    }
}

inline static seg_dispaly_set_segment(uint8_t pos, uint8_t seg_char)
{
    seg_display_write(seg_char);
}

void seg_display_set_contrast(uint8_t val)
{
    seg_display_contrast = val;
}

void seg_display_timer_1_init(void)
{
    TCNT1 = 0x0000;
    TCCR1B = (0 << CS12) | (0 << CS11) | (1 << CS10); // Normal operation, F_CPU/1
    TIMSK1 |= (1 << TOIE1);                           // Overflow Interrupt Enable
}

void seg_dispaly_set_digits(uint8_t d2, uint8_t d1, uint8_t d0)
{
    seg_display_data[2] = seg_display_chars[d2];
    seg_display_data[1] = seg_display_chars[d1];
    seg_display_data[0] = seg_display_chars[d0];
}

void seg_dispaly_set_dec_num(uint16_t num)
{
    uint8_t i = 0;
    seg_display_clear();
    if (num == 0)
    {
        seg_display_data[0] = seg_display_chars[0];
    }
    else
    {
        while (num > 0)
        {
            uint8_t digit = num % 10;
            num /= 10;
            seg_display_digits[i] = digit;

            if (digit <= 9 && digit >= 0)
                seg_display_data[i] = seg_display_chars[digit];
            else
                seg_display_data[i] = SEG_DISP_CHAR_CLEAR;

            i++;
        }
    }
#ifdef SEG_DISPALY_SINGLE_DIGIT_OPT
    seg_display_n_digits = ++i;
#endif
}

void seg_display_clear(void)
{
    for (uint8_t i = 0; i < 3; ++i)
        seg_display_data[i] = SEG_DISP_CHAR_CLEAR;
}

void seg_display_wait_animation(uint8_t from, uint8_t to, uint16_t speed)
{
    uint8_t i, j;
    uint16_t wait;

    for (i = 0; i < 6; i++)
    {
        wait = speed;
        for (j = from; j <= to; j++)
        {
            // seg_display_data[j] = ~(1 << i);
            seg_display_data[j] = ~(1 << seg_display_digits_in_order[i]);
        }
        while (0 < wait)
        {
            _delay_ms(1);
            --wait;
        }
    }
}

void seg_display_draw_circle(void)
{
    seg_display_data[2] = ~((1 << seg_a) | (1 << seg_d) | (1 << seg_e) | (1 << seg_f));
    seg_display_data[1] = ~((1 << seg_a) | (1 << seg_d));
    seg_display_data[0] = ~((1 << seg_a) | (1 << seg_b) | (1 << seg_c) | (1 << seg_d));
}

void seg_display_draw_line(void)
{
    for (uint8_t x = 0; x < 3; x++)
    {
        seg_display_data[x] = ~(1 << seg_g);
    }
}

void seg_display_init(void)
{
    seg_display_data_port_out();

    sbi(SEG_POS_0_DDR, SEG_POS_0_PIN); /* POS 0 pin as output */
    sbi(SEG_POS_1_DDR, SEG_POS_1_PIN); /* POS 1 pin as output */
    sbi(SEG_POS_2_DDR, SEG_POS_2_PIN); /* POS 2 pin as output */

    seg_display_timer_1_init();
}

//
ISR(TIMER1_OVF_vect)
{
    static uint8_t pos_show = 0;
    uint8_t temp;
    cli();

    TCNT1 = 0x8000; // - seg_display_contrast;

    // seg_display_pos_gate_clear(pos_show);
    // pos_show = (pos_show + 1) & 3;

    seg_display_pos_gate_clear_all();
    temp = seg_display_data[pos_show];
    seg_dispaly_set_segment(pos_show, temp);
    seg_display_pos_gate_set(pos_show);

    pos_show++;
    if (pos_show >= seg_display_n_digits)
        pos_show = 0;

    sei();
}
