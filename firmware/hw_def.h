/*
 *  Copyright (C) 2021 Juraj Giertl
 *  All Rights Reserved.
 */
#ifndef _HW_DEF_H_
#define _HW_DEF_H_

#define T_MAX 380.0
#define T_MIN 50.0
#define PID_KP (8.0)
#define PID_KI (0.0)
#define PID_KD (0.0)

#define PD PC3 /* probe detect */
#define PD_PORT PORTC
#define PD_DDR DDRC
#define PD_PIN PINC

#define LED_G PC5
#define LED_R PC4
#define LED_PORT PORTC
#define LED_DDR DDRC

#define LEDG_ON LED_PORT &= ~(1 << LED_G);
#define LEDG_OFF LED_PORT |= (1 << LED_G);
#define LEDR_ON LED_PORT &= ~(1 << LED_R);
#define LEDR_OFF LED_PORT |= (1 << LED_R);

#define IRON_PW OCR1B // iron heat output 10 bit
#define IRON_ADC ADCH
#define PWM_MAX 0x00ff
#define T2P_CONV_CONST ((PWM_MAX - T_MIN) / T_MAX) // Temperature to PWM convversion
#define T2P_CONV(x) (T2P_CONV_CONST * x + T_MIN)   // Temperature to PWM convversion
// #define T2P_CONV(x) (((PWM_MAX-T_MIN)/T_MAX)*x + T_MIN) // Temperature to PWM convversion

#define A2T_CONV(x) (1.362 * x) // ADC to Temperature convversion

extern volatile int16_t t_read, g_iron_t_set;
extern volatile uint16_t g_adc;

void io_init(void);

#endif // HW_DEF_H
