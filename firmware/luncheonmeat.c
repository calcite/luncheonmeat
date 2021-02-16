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

const char fw_ver[] PROGMEM = "v0.2.4";
const uint16_t set_iron_tmp EEMEM = 325;
const uint8_t lcd_back_pwm EEMEM = 128;
const uint8_t lcd_contrast_pwm EEMEM = 128;

char buffer[16];
volatile int16_t t_read = 0, t_set = 0;

void io_init(void)
{
    // PD_DDR &= ~(0 << PD); // as input
    // PD_PORT |= (1 << PD); // pullups

    LED_DDR |= (1 << LED_G);
    // LED_PORT&=~(1<<LED_G)|(1<<LED_R);
    LEDG_OFF;
}

void pwm1_init(void)
{
    DDRB |= (1 << DDB1); // enable PB1 for PWM output OC1A - lcd contrast
    DDRB |= (1 << DDB2); // enable PB2 for PWM output OC1B
    // FAST PWM 10 bit
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11) | (1 << WGM10);
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);

    TIMSK1 |= (1 << TOIE1); // Overflow Interrupt Enable

    OCR1A = 0x00ff;
    OCR1B = 0x00df;

    // OCR1B = 0x03FF; // for full power
}

void pwm2_init(void)
{
    TCNT2 = 0x00;
    DDRB |= (1 << DDB3);                  // enable PB3 for PWM output OC2
    TCCR2A = (1 << WGM21) | (1 << WGM20); // Fast PWM
    TCCR2A |= (1 << COM2A1);              // Clear OC2 on Compare Match, set OC2 at BOTTOM, (non - inverting mode)
    TCCR2A |= (1 << CS22);                // clkT2S/64 (From prescaler)
    // TIMSK |= (1 << TOIE2);               // Overflow Interrupt Enable
    OCR2A = 0x7f;
}

void init(void)
{
    io_init();
    usart_init();
    // pwm1_init(); // pid_init
    // pwm2_init();

    _delay_ms(2000);
    // enable global interrupts
    sei();
}

int main(void)
{
    init();

    TX_NEWLINE;
    transmitString_F(PSTR("*   For mor info pleas contact me at:       *\r\n"));
    transmitString_F(PSTR("*   \tE-mail:  \" jurinko08@gmail.com \"    *\r\n"));
    TX_NEWLINE;

    while (1)
    {
        LEDG_ON;
        _delay_ms(250);
        LEDG_OFF;
        _delay_ms(250);

        transmitString_F(PSTR("*   \tE-mail:  \" jurinko08@gmail.com \"    *\r\n"));
    }

    return 0;
}

ISR(ADC_vect)
{
    int16_t temp;
    temp = ADC;
}
