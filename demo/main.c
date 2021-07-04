/*
 * main.c
 *
 * Created: 16-05-2020 12:45:30
 * Author : Mikael Ejberg Pedersen
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include "collision_check.h"
#include "term.h"
#include "ticks.h"
#include "timer.h"
#include "lib/loconet-avrda/hal_ln.h"
#include "lib/loconet-avrda/ln_rx.h"

__attribute__((OS_main))
int main(void)
{
    // Enable 32.768 kHz crystal oscillator
    _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA,
                     CLKCTRL_RUNSTDBY_bm | CLKCTRL_CSUT_32K_gc | CLKCTRL_LPMODE_bm | CLKCTRL_ENABLE_bm);
    // Wait for crystal to stabilize
    while (!(CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm));
    // Switch to 24 MHz clock with frequency correction
    _PROTECTED_WRITE(CLKCTRL.PLLCTRLA, CLKCTRL_MULFAC_DISABLE_gc);
    _PROTECTED_WRITE(CLKCTRL.OSCHFCTRLA, CLKCTRL_RUNSTDBY_bm | CLKCTRL_FREQSEL_24M_gc | CLKCTRL_AUTOTUNE_bm);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSCHF_gc);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0);
    // Lock clock setup
    _PROTECTED_WRITE(CLKCTRL.MCLKLOCK, CLKCTRL_LOCKEN_bm);

    term_init();
    ticks_init();
    hal_ln_init();
    ln_rx_init();
    collision_check_init();

    // Setup LED0. Only for AVR128DA48 CNano
    PORTC.DIRSET = PIN6_bm;
    PORTC.OUTSET = PIN6_bm;

    sei();

    while (1)
    {
        term_update();
        hal_ln_update();
        ln_rx_update();
        timer_update();
        collision_check_update();
    }

    __builtin_unreachable();
}
