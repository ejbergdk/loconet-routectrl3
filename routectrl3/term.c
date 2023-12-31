/*
 * term.c
 *
 * Created: 16-05-2020 16:54:20
 *  Author: Mikael Ejberg Pedersen
 */

#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include "term.h"
#include "lib/avr-shell-cmd/cmd.h"

#define BAUDRATE 115200UL
#define BAUD_REG ((64 * F_CPU + 8 * BAUDRATE) / (16 * BAUDRATE))


/**************/
/* TX section */

#define TXBUF_LEN  512
static char     txbuf[TXBUF_LEN];
static uint16_t txbuf_widx = 0;
static volatile uint16_t txbuf_ridx = 0;

ISR(USART1_DRE_vect)
{
    USART1.TXDATAL = txbuf[txbuf_ridx++];
    if (txbuf_ridx >= TXBUF_LEN)
        txbuf_ridx = 0;
    if (txbuf_ridx == txbuf_widx)
        USART1.CTRLA &= ~USART_DREIE_bm;
}

static int tx_char(char c, FILE *stream)
{
    if (c == '\n')
        tx_char('\r', stream);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        txbuf[txbuf_widx++] = c;
        if (txbuf_widx >= TXBUF_LEN)
            txbuf_widx = 0;
        if (!(USART1.CTRLA & USART_DREIE_bm))
            USART1.CTRLA |= USART_DREIE_bm;
    }

    return 0;
}

static void prompt(void)
{
    printf_P(PSTR("AVR128DA>"));
}


/**************/
/* RX section */

#define RXBUF_LEN 32
static char     rxbuf[RXBUF_LEN];
static volatile uint16_t rxbuf_widx = 0;
static uint16_t rxbuf_ridx = 0;

ISR(USART1_RXC_vect)
{
    rxbuf[rxbuf_widx++] = USART1.RXDATAL;
    if (rxbuf_widx >= RXBUF_LEN)
        rxbuf_widx = 0;
}

#define LINE_LEN 128

void term_update(void)
{
    static char     line[LINE_LEN];
    static uint16_t chars = 0;
    uint16_t        i;
    char            c;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        i = rxbuf_widx;
    }
    if (i == rxbuf_ridx)
        return;

    c = rxbuf[rxbuf_ridx++];
    if (rxbuf_ridx >= RXBUF_LEN)
        rxbuf_ridx = 0;

    switch (c)
    {
    case 0x0d:                 // Carriage return
        putchar('\n');
        line[chars] = 0;
        chars = 0;
        cmd_split_exec(line);
        prompt();
        break;

    case 0x7f:                 // Backspace
        if (chars > 0)
        {
            chars--;
            printf_P(PSTR("\b\e[1P"));
        }
        break;

    default:
        if ((c >= 0x20) && (chars < (LINE_LEN - 1)))
        {
            line[chars++] = c;
            putchar(c);
        }
        break;
    }
}


/**********/
/* Common */

static FILE     dbg_uart = FDEV_SETUP_STREAM(tx_char, NULL, _FDEV_SETUP_WRITE);

void term_init(void)
{
    PORTC.OUTSET = PIN0_bm;
    PORTC.DIRSET = PIN0_bm;
    PORTC.DIRCLR = PIN1_bm;
    USART1.CTRLA = USART_RS485_DISABLE_gc;
    USART1.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc | USART_CHSIZE_8BIT_gc;
    USART1.BAUD = BAUD_REG;
    USART1.DBGCTRL = USART_DBGRUN_bm;
    USART1.CTRLB = USART_RXEN_bm | USART_TXEN_bm | USART_RXMODE_NORMAL_gc;
    USART1.STATUS = USART_RXCIF_bm | USART_TXCIF_bm;
    USART1.CTRLA |= USART_RXCIE_bm;

    stdout = &dbg_uart;
    prompt();
}
