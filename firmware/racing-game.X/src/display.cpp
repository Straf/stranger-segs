/* 
 * File:   display.cpp
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

#include "display.hpp"

/**
 * The interval to keep a segment turned on, in milliseconds.
 * Controls the timing of the segment multiplexer.
 */
constexpr uint16_t MUX_INTERVAL_MS = 10;

namespace drive {
    inline void none() {
        PORTA.OUTCLR = 0xC0;
        PORTB.OUTCLR = 0x3F;
    }
    inline void a() {
        none();
        PORTB.OUTSET = 1 << 5;
    }
    inline void b() {
        none();
        PORTB.OUTSET = 1 << 4;
    }
    inline void c() {
        none();
        PORTB.OUTSET = 1 << 2;
    }
    inline void dp() {
        none();
        PORTB.OUTSET = 1 << 3;
    }
    inline void d() {
        none();
        PORTB.OUTSET = 1 << 1;
    }
    inline void e() {
        none();
        PORTB.OUTSET = 1 << 0;
    }
    inline void f() {
        none();
        PORTA.OUTSET = 1 << 7;
    }
    inline void g() {
        none();
        PORTA.OUTSET = 1 << 6;
    }
}

namespace maps {
    using namespace display::segment;

    static const uint8_t digits[] PROGMEM = {
        a|b|c|d|e|f,    // 0
        b|c,            // 1
        a|b|d|e|g,      // 2
        a|b|c|d|g,      // 3
        b|c|f|g,        // 4
        a|c|d|f|g,      // 5
        a|c|d|e|f|g,    // 6
        a|b|c,          // 7
        a|b|c|d|e|f|g,  // 8
        a|b|c|d|f|g,    // 9
    };

    static const uint8_t lower[] PROGMEM = {
        a|b|c|e|f|g,    // A
        c|d|e|f|g,      // b
        d|e|g,          // c
        b|c|d|e|g,      // d
        a|d|e|f|g,      // E
        a|e|f|g,        // F
        a|c|d|e|f,      // G
        c|e|f|g,        // h
        e,              // i
        b|c|d|e,        // J
        0,              // k
        d|e|f,          // L
        a|b|c|e|f,      // M
        c|e|g,          // n
        c|d|e|g,        // o
        a|b|e|f|g,      // P
        a|b|c|f|g,      // q
        e|g,            // r
        a|c|d|f|g,      // S
        d|e|f|g,        // t
        c|d|e,          // u
        0,              // v
        0,              // w
        0,              // x
        b|c|d|f|g,      // y
        0,              // z
    };

    static const uint8_t upper[] PROGMEM = {
        a|b|c|e|f|g,    // A
        c|d|e|f|g,      // b
        a|d|e|f,        // C
        b|c|d|e|g,      // d
        a|d|e|f|g,      // E
        a|e|f|g,        // F
        a|c|d|e|f,      // G
        b|c|e|f|g,      // H
        e|f,            // I
        b|c|d|e,        // J
        0,              // k
        d|e|f,          // L
        a|b|c|e|f,      // M
        c|e|g,          // n
        a|b|c|d|e|f,    // O
        a|b|e|f|g,      // P
        a|b|c|f|g,      // q
        e|g,            // r
        a|c|d|f|g,      // S
        d|e|f|g,        // t
        b|c|d|e|f,      // U
        0,              // v
        0,              // w
        0,              // x
        b|c|d|f|g,      // y
        0,              // z
    };
}

static bool inited;
static uint8_t current_segs; // not declared volatile since access is atomic and one-way (read or write, never both)
static uint8_t mux_count;    // not declared volatile since access is not concurrent

ISR(TCB0_INT_vect) {
    if (current_segs & (1 << mux_count)) {
        switch (mux_count) {
            case 0: drive::a(); break;
            case 1: drive::b(); break;
            case 2: drive::c(); break;
            case 3: drive::d(); break;
            case 4: drive::e(); break;
            case 5: drive::f(); break;
            case 6: drive::g(); break;
            case 7: drive::dp(); break;
        }
    } else {
        drive::none();
    }
    mux_count = (mux_count + 1) & 0x07;
}

constexpr uint16_t tcb_top(uint16_t ms) { // assumes CLKSEL = CLK_PER/DIV2
	return (uint16_t)((float)F_CPU * ms / 2000 + 0.5);
}

inline void setup_timer() {
    // RUNSTDBY = 1, CLKSEL = 1 (CLK_PER/DIV2), ENABLE = 0 (disabled)
    TCB0.CTRLA = TCB_RUNSTDBY_bm | TCB_CLKSEL_CLKDIV2_gc;
    // CNTMODE = 0 (INT)
    TCB0.CTRLB = 0x00;
    // reset counter
    TCB0.CNT = 0;
    // TOP ~= 200 Hz
    TCB0.CCMP = tcb_top(MUX_INTERVAL_MS);
   // CAPT = 1 (INT cleared)
    TCB0.INTFLAGS = TCB_CAPT_bm;
    // CAPT = 1 (INT enabled)
    TCB0.INTCTRL = TCB_CAPT_bm;
}

inline void start_timer() {
    // reset MUX state
    mux_count = 0;
    // start timer
    TCB0.CTRLA |= TCB_ENABLE_bm;
}

inline void stop_timer() {
    // stop timer
    TCB0.CTRLA &= ~TCB_ENABLE_bm;
    // CAPT = 1 (INT cleared)
    TCB0.INTFLAGS = TCB_CAPT_bm;
}

namespace display {

    void init() {
        // ensure that initialisation is done once
        if (inited) return;

        PORTA.DIR |= 0xC0; // set PORTA[6..7] to outputs
        PORTA.OUT &= 0x3F; // turn segments off
        PORTB.DIR |= 0x3F; // set PORTB[0..5] to outputs
        PORTB.OUT &= 0xC0; // turn segments off

        current_segs = 0;
        setup_timer();

        inited = true;
    }

    void off() {
        current_segs = 0;
        if (TCB0.STATUS) stop_timer();
        drive::none();
    }

    void show_char(char code) {
        show_segments( char_to_segs(code) );
    }

    void show_segments(uint8_t map) {
        if ((current_segs = map)) {
            if (!TCB0.STATUS) start_timer();
        } else {
            if (TCB0.STATUS) stop_timer();
        }
    }

    uint8_t char_to_segs(char code) {
        bool has_dp = (bool)(code & 0x80);
        uint8_t map = 0; // all segments off

        // convert code to 7-bit ASCII
        code &= 0x7F;

        if ('0' <= code && code <= '9') {
            map = pgm_read_byte(maps::digits + (code - '0'));
        } else if ('A' <= code && code <= 'Z') {
            map = pgm_read_byte(maps::upper + (code - 'A'));
        } else if ('a' <= code && code <= 'z') {
            map = pgm_read_byte(maps::lower + (code - 'a'));
        } else switch (code) {
            case '-':
                code = segment::g;
                break;
            case '_':
                code = segment::d;
                break;
            case '.':
                return segment::dp;
        }

        // turn on the decimal point (dp) when the character has the msb set
        if (has_dp) map |= segment::dp;

        return map;
    }

}
