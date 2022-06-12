/*
 * File:   test.cpp
 * Author: Gabriele Falcioni <foss.dev@falcioni.net>
 *
 * Copyright 2022 Gabriele Falcioni
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 *  publish, distribute, sublicense, and/or sell copies of the Software,
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

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "display.hpp"
#include "key.hpp"
#include "test.hpp"
#include "timer.hpp"

static void loop() {
    display::show_segments(display::segment::a);
    _delay_ms(100);
    display::show_segments(display::segment::b);
    _delay_ms(100);
    display::show_segments(display::segment::c);
    _delay_ms(100);
    display::show_segments(display::segment::d);
    _delay_ms(100);
    display::show_segments(display::segment::e);
    _delay_ms(100);
    display::show_segments(display::segment::f);
    _delay_ms(100);
}

static void race() {
    display::show_segments(display::segment::a);
    _delay_ms(100);
    display::show_segments(display::segment::b);
    _delay_ms(100);
    display::show_segments(display::segment::g);
    _delay_ms(100);
    display::show_segments(display::segment::e);
    _delay_ms(100);
    display::show_segments(display::segment::d);
    _delay_ms(100);
    display::show_segments(display::segment::c);
    _delay_ms(100);
    display::show_segments(display::segment::g);
    _delay_ms(100);
    display::show_segments(display::segment::f);
    _delay_ms(100);
}

static void sequence() {
    display::show_segments(display::segment::a);
    _delay_ms(500);
    display::show_segments(display::segment::b);
    _delay_ms(500);
    display::show_segments(display::segment::c);
    _delay_ms(500);
    display::show_segments(display::segment::d);
    _delay_ms(500);
    display::show_segments(display::segment::e);
    _delay_ms(500);
    display::show_segments(display::segment::f);
    _delay_ms(500);
    display::show_segments(display::segment::g);
    _delay_ms(500);
    display::show_segments(display::segment::dp);
    _delay_ms(500);
}

static const char characters[] PROGMEM = {
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_-."
};

namespace test {

    void all_segments() {
        display::init();
        for(;;) {
            sequence();
        }
    }
    
    void racing_segments() {
        display::init();
        for(;;) {
            loop();
            race();
        }
    }

    void all_characters() {
        display::init();
        sei();
        const char* ptr = characters;
        for(;;) {
            char c = pgm_read_byte(ptr++);
            if (c) {
                display::show_char(c);
            } else {
                ptr = characters;
            }
            _delay_ms(10);
        }
    }

    void timer_ticks() {
        timer::init();
        display::init();
        for(;;) {
            const uint8_t elapsed = timer::ticks();
            const uint8_t digit = (elapsed >> 3) % 10;
            display::show_char('0' + digit);
        }
    }

    void timer_seconds() {
        timer::init();
        display::init();
        uint8_t last = 0;
        uint8_t digit = 0;
        display::show_char('0');
        for(;;) {
            uint8_t elapsed = timer::seconds();
            if (last != elapsed) {
                last = elapsed;
                if (++digit == 10) digit = 0;
                display::show_char('0' + digit);
            }
        }
    }

    void timer_elapsed() {
        timer::init();
        display::init();
        uint8_t digit = 0;
        display::show_char('0');
        timer::enable(15); // 3/10s
        for(;;) {
            if (timer::elapsed()) {
                timer::enable(15); // 3/10s
                if (++digit == 10) digit = 0;
                display::show_char('0' + digit);
            }
        }
    }

    void key_states() {
        display::init();
        timer::init();
        key::init();
        if (key::state() == key::state_t::pressed) {
            display::show_char('_');
        } else {
            display::show_char('n');
        }
        for(;;) {
            key::run();
            if (key::changed()) {
                if (key::state() == key::state_t::pressed) {
                    display::show_segments(display::segment::d);
                } else {
                    display::show_segments(
                        display::segment::c
                        | display::segment::e
                        | display::segment::g);
                }
            }
        }
    }

}
