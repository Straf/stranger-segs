/*
 * File:   key.cpp
 * Author: Gabriele Falcioni <foss.dev@falcioni.net>
 *
 * Copyright 2022 Gabriele Falcioni
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "key.hpp"
#include "timer.hpp"

/**
 * The number of ticks whether the key state must be stable
 * to emit a key change event.
 */
constexpr int8_t DEBOUNCE_TICKS = 2;

/*
 * The mask of the GPIO pin connected to the key.
 */
constexpr uint8_t PIN_MASK = 0x08;

static bool inited;
static volatile bool pressed;
static volatile bool pressed_changed;
static int8_t waiting_ticks;
static uint8_t last_ticks;

inline bool get_state() {
    return PORTC.IN & PIN_MASK;
}

inline void enable_irq() {
    if (pressed) {
        // INVEN = 1 (key -> GND when pressed), PULLUPEN = 1 (enabled), IRQ on falling edge
        PORTC.PIN3CTRL = PORT_INVEN_bm | PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
    } else {
        // INVEN = 1 (key -> GND when pressed), PULLUPEN = 1 (enabled), IRQ on rising edge
        PORTC.PIN3CTRL = PORT_INVEN_bm | PORT_PULLUPEN_bm | PORT_ISC_RISING_gc;
    }
}

ISR(PORTC_PORT_vect) {
    // the key state depends on the change event configured for this IRQ
    pressed = (PORTC.PIN3CTRL & PORT_ISC_gm) == PORT_ISC_RISING_gc;
    // disable IRQ
    PORTC.PIN3CTRL &= ~PORT_ISC_gm;
    PORTC.INTFLAGS = PIN_MASK;
    pressed_changed = true;
}

namespace key {

    void init() {
        // ensure that initialisation is done once
        if (inited) return;

        // PC3 is INPUT
        PORTC.DIRCLR = PIN_MASK;
        // INVEN = 1 (key -> GND when pressed), PULLUPEN = 1 (enabled)
        PORTC.PIN3CTRL = PORT_INVEN_bm | PORT_PULLUPEN_bm;
        // clear interrupts
        PORTC.INTFLAGS = PIN_MASK;

        // assumes that key state is stable
        pressed = get_state();
        enable_irq();
        last_ticks = timer::ticks();
        waiting_ticks = DEBOUNCE_TICKS;

        inited = true;
    }

    bool changed() {
        cli();
        const bool result = pressed_changed;
        pressed_changed = false;
        sei();
        return result;
    }

    state_t state() {
        return pressed ? state_t::pressed : state_t::released;
    }

    void run() {
        // do nothing (but update the tick counter) when any port IRQ is enabled
        if (PORTC.PIN3CTRL & PORT_ISC_gm) {
            last_ticks = timer::ticks();
            return;
        }

        // after a key change is initiated by the key IRQ handler,
        // wait for the specified number of ticks (the debounce interval)
        const uint8_t ticks = timer::ticks();
        const uint8_t elapsed = ticks - last_ticks;
        last_ticks = ticks;

        if ((waiting_ticks -= elapsed) > 0) return;

        // if enough ticks have elapsed, enable the key change IRQ again
        enable_irq();
        waiting_ticks = DEBOUNCE_TICKS;
    }
}
