/*
 * main.c
 *
 * Created: 16-05-2020 12:45:30
 * Author : Mikael Ejberg Pedersen
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include "collision_check.h"
#include "ctrl_init.h"
#include "mmi.h"
#include "route.h"
#include "sw_handler.h"
#include "switch_queue.h"
#include "term.h"
#include "ticks.h"
#include "timer.h"
#include "lib/loconet-avrda/hal_ln.h"
#include "lib/loconet-avrda/ln_rx.h"

#ifdef EERAM
#include "eeram.h"
#include "twim.h"
#endif

__attribute__((OS_main))
int main(void)
{
#ifdef EXTXTAL
    // Enable 32.768 kHz crystal oscillator
    _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA,
                     CLKCTRL_RUNSTDBY_bm | CLKCTRL_CSUT_32K_gc | CLKCTRL_LPMODE_bm | CLKCTRL_ENABLE_bm);
    // Wait for crystal to stabilize
    while (!(CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm));
#endif
    // Switch to 24 MHz clock with frequency correction
    _PROTECTED_WRITE(CLKCTRL.PLLCTRLA, CLKCTRL_MULFAC_DISABLE_gc);
    _PROTECTED_WRITE(CLKCTRL.OSCHFCTRLA, CLKCTRL_RUNSTDBY_bm | CLKCTRL_FRQSEL_24M_gc
#ifdef EXTXTAL
                     | CLKCTRL_AUTOTUNE_bm
#endif
        );
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSCHF_gc);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0);
    // Lock clock setup
    _PROTECTED_WRITE(CLKCTRL.MCLKLOCK, CLKCTRL_LOCKEN_bm);

    term_init();
    ticks_init();
#ifdef EERAM
    twim_init();
    eeram_init();
#endif
    hal_ln_init();
    ln_rx_init();
    collision_check_init();
    mmi_init();
    route_init();
    ctrl_init_init();

    sei();

    while (1)
    {
        term_update();
#ifdef EERAM
        twim_update();
        eeram_update();
#endif
        hal_ln_update();
        ln_rx_update();
        timer_update();
        collision_check_update();
        switch_queue_update();
        sw_handler_update();
        mmi_update();
        route_update();
    }

    __builtin_unreachable();
}
