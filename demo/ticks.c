/*
 * ticks.c
 *
 * Created: 16-05-2020 16:23:20
 *  Author: Mikael Ejberg Pedersen
 */ 

#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>
#include "ticks.h"

static volatile uint16_t cnt_h = 0;

void ticks_init(void)
{
    // Setup RTC to count ticks (= almost ms)
    if (CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm)
        RTC.CLKSEL = RTC_CLKSEL_XOSC32K_gc;     // Use external xtal/clock
    else
        RTC.CLKSEL = RTC_CLKSEL_OSC32K_gc;      // Use internal 32.768 kHz osc
    while (RTC.STATUS & RTC_PERBUSY_bm) {}
    RTC.PER = 0xFFFF;
    RTC.INTFLAGS = RTC_CMP_bm | RTC_OVF_bm;
    RTC.INTCTRL = RTC_OVF_bm;
    while (RTC.STATUS & RTC_CTRLABUSY_bm) {}
    RTC.CTRLA = RTC_PRESCALER_DIV32_gc | RTC_RTCEN_bm;
}

ISR(RTC_CNT_vect)
{
    cnt_h++;
    RTC.INTFLAGS = RTC_OVF_bm;
}

ticks_t ticks_get(void)
{
    union
    {
        ticks_t tick;
        uint16_t t[2];
    } now;
    uint16_t cnt;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        cnt = RTC.CNT;
        now.t[1] = cnt_h;
        now.t[0] = RTC.CNT;
    }
    if (now.t[0] < cnt)
    {
        while (RTC.INTFLAGS & RTC_OVF_bm) {}
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            now.t[1] = cnt_h;
        }
    }

    return now.tick;
}

ticks_t ticks_elapsed(ticks_t t0)
{
    return ticks_get() - t0;
}
