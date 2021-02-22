/*
 *  Copyright (C) 2017 Juraj Giertl
 *  All Rights Reserved.
 */
#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "hw_def.h"

#ifdef SEG_DISPLAY_INVERTED
#define seg_a		2//7
#define seg_b		1//6
#define seg_c		4//5
#define seg_d		5//4
#define seg_e		0//3
#define seg_f		7//2
#define seg_g		6//1
#define seg_dp		3//0

/* Segment multiplexing */
#define SEG_POS_0              2
#define SEG_POS_1              1
#define SEG_POS_2              0

#else

#define seg_a		7
#define seg_b		6
#define seg_c		5
#define seg_d		4
#define seg_e		3
#define seg_f		2
#define seg_g		1
#define seg_dp		0

/* Segment multiplexing */
#define SEG_POS_0              0
#define SEG_POS_1              1
#define SEG_POS_2              2
#endif

#define A		0
#define N		1


#define SEG_A_PIN		PINE1
#define SEG_B_PIN		PINE0
#define SEG_C_PIN		PINC3
#define SEG_D_PIN		PINC2
#define SEG_E_PIN		PINC1
#define SEG_F_PIN		PIND3
#define SEG_G_PIN		PIND4
#define SEG_DP_PIN		PINC4

#define SEG_A_DDR		DDRE
#define SEG_B_DDR		DDRE
#define SEG_C_DDR		DDRC
#define SEG_D_DDR		DDRC
#define SEG_E_DDR		DDRC
#define SEG_F_DDR		DDRD
#define SEG_G_DDR		DDRD
#define SEG_DP_DDR		DDRC

#define SEG_A_PORT      PORTE
#define SEG_B_PORT      PORTE
#define SEG_C_PORT      PORTC
#define SEG_D_PORT      PORTC
#define SEG_E_PORT      PORTC
#define SEG_F_PORT      PORTD
#define SEG_G_PORT      PORTD
#define SEG_DP_PORT     PORTC


#define SEG_POS_0_DDR          DDRC
#define SEG_POS_0_PORT         PORTC
#define SEG_POS_0_PIN          PINC5

#define SEG_POS_1_DDR          DDRD
#define SEG_POS_1_PORT         PORTD
#define SEG_POS_1_PIN          PIND2

#define SEG_POS_2_DDR          DDRD
#define SEG_POS_2_PORT         PORTD  
#define SEG_POS_2_PIN          PIND5

#define SEG_DISP_CHAR_CLEAR    (0xFF) // 0xFF

static uint8_t __attribute__((used)) seg_display_chars[18] =
{
	(A << seg_a) | (A << seg_b) | (A << seg_c) | (A << seg_d) | (A << seg_e) | (A << seg_f) | (N << seg_g) | (N << seg_dp),//0
	(N << seg_a) | (A << seg_b) | (A << seg_c) | (N << seg_d) | (N << seg_e) | (N << seg_f) | (N << seg_g) | (N << seg_dp),//1
	(A << seg_a) | (A << seg_b) | (N << seg_c) | (A << seg_d) | (A << seg_e) | (N << seg_f) | (A << seg_g) | (N << seg_dp),//2
	(A << seg_a) | (A << seg_b) | (A << seg_c) | (A << seg_d) | (N << seg_e) | (N << seg_f) | (A << seg_g) | (N << seg_dp),//3
	(N << seg_a) | (A << seg_b) | (A << seg_c) | (N << seg_d) | (N << seg_e) | (A << seg_f) | (A << seg_g) | (N << seg_dp),//4
	(A << seg_a) | (N << seg_b) | (A << seg_c) | (A << seg_d) | (N << seg_e) | (A << seg_f) | (A << seg_g) | (N << seg_dp),//5
	(A << seg_a) | (N << seg_b) | (A << seg_c) | (A << seg_d) | (A << seg_e) | (A << seg_f) | (A << seg_g) | (N << seg_dp),//6
	(A << seg_a) | (A << seg_b) | (A << seg_c) | (N << seg_d) | (N << seg_e) | (N << seg_f) | (N << seg_g) | (N << seg_dp),//7
	(A << seg_a) | (A << seg_b) | (A << seg_c) | (A << seg_d) | (A << seg_e) | (A << seg_f) | (A << seg_g) | (N << seg_dp),//8
	(A << seg_a) | (A << seg_b) | (A << seg_c) | (A << seg_d) | (N << seg_e) | (A << seg_f) | (A << seg_g) | (N << seg_dp),//9
	(A << seg_a) | (A << seg_b) | (A << seg_c) | (N << seg_d) | (A << seg_e) | (A << seg_f) | (A << seg_g) | (N << seg_dp),//A ,10
	(A << seg_a) | (A << seg_b) | (A << seg_c) | (A << seg_d) | (A << seg_e) | (A << seg_f) | (A << seg_g) | (N << seg_dp),//B ,11
	(A << seg_a) | (N << seg_b) | (N << seg_c) | (A << seg_d) | (A << seg_e) | (A << seg_f) | (N << seg_g) | (N << seg_dp),//C ,12
	(A << seg_a) | (A << seg_b) | (A << seg_c) | (A << seg_d) | (A << seg_e) | (A << seg_f) | (N << seg_g) | (N << seg_dp),//D ,13
	(A << seg_a) | (N << seg_b) | (N << seg_c) | (A << seg_d) | (A << seg_e) | (A << seg_f) | (A << seg_g) | (N << seg_dp),//E ,14
	(A << seg_a) | (N << seg_b) | (N << seg_c) | (N << seg_d) | (A << seg_e) | (A << seg_f) | (A << seg_g) | (N << seg_dp),//F ,15
	(A << seg_a) | (A << seg_b) | (N << seg_c) | (N << seg_d) | (N << seg_e) | (A << seg_f) | (A << seg_g) | (N << seg_dp),//degree
	(N << seg_a) | (N << seg_b) | (N << seg_c) | (N << seg_d) | (N << seg_e) | (N << seg_f) | (A << seg_g) | (N << seg_dp),//minus

};

void seg_display_wait_animation(uint8_t from, uint8_t to, uint16_t speed);

void seg_dispaly_set_digits(uint8_t d2, uint8_t d1, uint8_t d0);
void seg_dispaly_set_dec_num(uint16_t num);

void seg_display_draw_circle(void);
void seg_display_draw_line(void);
void seg_display_clear(void);

void seg_display_set_contrast(uint8_t val);

void seg_display_init(void);

#endif
