/*
 * ticks.c
 *
 * Created: 16-05-2020 16:23:20
 *  Author: Mikael Ejberg Pedersen
 */ 

#include <stdint.h>
#include <avr/io.h>
#include "ticks.h"

void ticks_init(void)
{
    // Setup RTC to count ticks (= almost ms)
	if (CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm)
	    RTC.CLKSEL = RTC_CLKSEL_XOSC32K_gc;     // Use external xtal/clock
	else
	    RTC.CLKSEL = RTC_CLKSEL_OSC32K_gc;      // Use internal 32.768 kHz osc
    while (RTC.STATUS & RTC_PERBUSY_bm) {}
    RTC.PER = 0xFFFF;
    RTC.INTCTRL = 0;
    while (RTC.STATUS & RTC_CTRLABUSY_bm) {}
    RTC.CTRLA = RTC_PRESCALER_DIV32_gc | RTC_RTCEN_bm;
}

ticks_t ticks_get(void)
{
	return RTC.CNT;
}

ticks_t ticks_elapsed(ticks_t t0)
{
	return ticks_get() - t0;
}
