/*
 * mmi.c
 *
 * Created: 25-03-2024 11:16:23
 *  Author: Mikael Ejberg Pedersen
 */

#include <avr/io.h>
#include <stdint.h>
#include "mmi.h"
#include "ticks.h"

#define LED_ON_TICKS    TICKS_FROM_MS(200)
#define LED_OFF_TICKS   TICKS_FROM_MS(200)
#define LED_DELAY_TICKS TICKS_FROM_MS(1500)

#define BUTTON_DEBOUNCE_TICKS TICKS_FROM_MS(50)

#define OPERATING_LEVEL_MIN 1
#define OPERATING_LEVEL_MAX 4

static uint8_t  operating_level = 0;

typedef enum
{
    LED_INIT = 0,
    LED_OFF,
    LED_ON,
    LED_DELAY
} led_state_t;

static led_state_t led_state = LED_INIT;

typedef enum
{
    BUTTON_IDLE = 0,
    BUTTON_ACTIVE_DEBOUNCE,
    BUTTON_ACTIVE,
    BUTTON_IDLE_DEBOUNCE
} button_state_t;


void mmi_init(void)
{
    // Setup LED0. Only for AVR128DA48 CNano
    PORTC.DIRSET = PIN6_bm;
    PORTC.OUTSET = PIN6_bm;

    // Setup SW0. Only for AVR128DA48 CNano
    PORTC.DIRCLR = PIN7_bm;
    PORTC.PIN7CTRL = PORT_PULLUPEN_bm;
}


static void led_update(void)
{
    static ticks_t  time;
    static uint8_t  count;

    switch (led_state)
    {
    case LED_INIT:
    default:
        PORTC.OUTSET = PIN6_bm;
        count = 0;
        time = ticks_get();
        led_state = LED_OFF;
        break;

    case LED_OFF:
        if (ticks_elapsed(time) >= LED_OFF_TICKS)
        {
            time = ticks_get();
            if (count < operating_level)
            {
                PORTC.OUTCLR = PIN6_bm;
                led_state = LED_ON;
            }
            else
            {
                led_state = LED_DELAY;
            }
        }
        break;

    case LED_ON:
        if (ticks_elapsed(time) >= LED_ON_TICKS)
        {
            time = ticks_get();
            PORTC.OUTSET = PIN6_bm;
            led_state = LED_OFF;
            count++;
        }
        break;

    case LED_DELAY:
        if (ticks_elapsed(time) >= LED_DELAY_TICKS)
        {
            time = ticks_get();
            led_state = LED_OFF;
            count = 0;
        }
        break;
    }
}


static void button_update(void)
{
    static button_state_t state = BUTTON_IDLE;
    static ticks_t  time;

    switch (state)
    {
    case BUTTON_IDLE:
    default:
        if (!(PORTC.IN & PIN7_bm))
        {
            time = ticks_get();
            state = BUTTON_ACTIVE_DEBOUNCE;
        }
        break;

    case BUTTON_ACTIVE_DEBOUNCE:
        if (PORTC.IN & PIN7_bm)
        {
            state = BUTTON_IDLE;
        }
        else if (ticks_elapsed(time) >= BUTTON_DEBOUNCE_TICKS)
        {
            state = BUTTON_ACTIVE;
            operating_level++;
            if (operating_level > OPERATING_LEVEL_MAX)
                operating_level = OPERATING_LEVEL_MIN;
            led_state = LED_INIT;
        }
        break;

    case BUTTON_ACTIVE:
        if (PORTC.IN & PIN7_bm)
        {
            time = ticks_get();
            state = BUTTON_IDLE_DEBOUNCE;
        }
        break;

    case BUTTON_IDLE_DEBOUNCE:
        if (!(PORTC.IN & PIN7_bm))
        {
            state = BUTTON_ACTIVE;
        }
        else if (ticks_elapsed(time) >= BUTTON_DEBOUNCE_TICKS)
        {
            state = BUTTON_IDLE;
        }
        break;
    }
}


void mmi_update(void)
{
    led_update();
    button_update();
}


void mmi_operating_level_set(uint8_t level)
{
    operating_level = level;
}


uint8_t mmi_operating_level_get(void)
{
    return operating_level;
}
