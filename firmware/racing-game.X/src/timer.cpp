/* 
 * File:   timer.cpp
 * Author: Gabriele Falcioni
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

#include "timer.hpp"

static bool inited;
static volatile uint8_t elapsed_ticks;
static volatile uint16_t elapsed_secs;

static bool compare_enabled;
static volatile bool compare_triggered;
static volatile uint8_t compare_ticks;

ISR(RTC_PIT_vect) {
    RTC.PITINTFLAGS = RTC_PI_bm;
    ++elapsed_secs;
}

ISR(RTC_CNT_vect) {
    RTC.INTFLAGS = RTC_OVF_bm;
    if (++elapsed_ticks == compare_ticks) compare_triggered = true;
}

inline void disable_secs_irq() {
    RTC.PITINTCTRL = 0;
}
inline void enable_secs_irq() {
    RTC.PITINTCTRL = RTC_PI_bm;
}

inline void disable_ticks_irq() {
    RTC.INTCTRL = 0;
}

inline void enable_ticks_irq() {
    RTC.INTCTRL = RTC_OVF_bm;
}

constexpr uint16_t rtc_per(uint16_t nticks) {
    // assumes CLK_RTC = 32.768 kHz and PRESCALER = DIV8 (4.096 kHz)
	return (uint16_t)((float)4096 / nticks + 0.5);
}

namespace timer {

    void init() {
        // ensure that initialisation is done once
        if (inited) return;

        cli();

        // wait until all registers are synchronised
        while (RTC.STATUS) ;
        // frequency = 49.95 Hz (50 ticks are slightly slower than a second)
        RTC.PER = rtc_per(TICKS_PER_SEC);
        // restart counter
        RTC.CNT = 0;
        // clear interrupts
        RTC.INTFLAGS = RTC_OVF_bm;
        // enable interrupts
        RTC.INTCTRL = RTC_OVF_bm;
        // RUNSTDBY = 1 (run in sleep mode), PRESCALER = DIV8 (4.096 kHz), RTCEN = 1 (enabled)
        RTC.CTRLA = RTC_RUNSTDBY_bm | RTC_PRESCALER_DIV8_gc | RTC_RTCEN_bm;

        // wait until all registers are synchronised
        while (RTC.PITSTATUS) ;
        // clear interrupts
        RTC.PITINTFLAGS = RTC_PI_bm;
        // enable interrupts
        RTC.PITINTCTRL = RTC_PI_bm;
        // PERIOD = RTC_CLK/32768 (1 Hz), PITEN = 1 (enabled)
        RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc | RTC_PITEN_bm;

        sei();

        inited = true;
    }

    void reset() {
        compare_enabled = false;
        elapsed_ticks = 0;
        disable_secs_irq();
        elapsed_secs = 0;
        enable_secs_irq();
    }

    uint8_t ticks() {
        return elapsed_ticks;
    }

    uint16_t seconds() {
        disable_secs_irq();
        const uint16_t secs = elapsed_secs;
        enable_secs_irq();
        return secs;
    }

    void enable(uint8_t ticks) {
        compare_enabled = true;
        disable_ticks_irq();
        compare_ticks = ticks + elapsed_ticks;
        compare_triggered = false;
        enable_ticks_irq();
    }

    void disable() {
        compare_enabled = false;
    }

    bool elapsed() {
        return compare_enabled && compare_triggered;
    }

    uint8_t remaining_ticks() {
        if (!compare_enabled || compare_triggered) return 0;
        return compare_ticks - elapsed_ticks;
    }
}
