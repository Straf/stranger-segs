/* 
 * File:   game_ui.cpp
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

#include <avr/pgmspace.h>
#include <stdint.h>

#include "display.hpp"
#include "game_ui.hpp"
#include "key.hpp"
#include "timer.hpp"

constexpr uint8_t FLASH_SPEED = 5;

namespace ui {

    void wait_key_released() {
        while (key::state() == key::state_t::pressed) key::run();
    }

    bool wait_key_pressed_or_timer_elapsed() {
        do {
            key::run();
            if (key::changed() && key::state() == key::state_t::pressed) return true;
            // TODO: idle CPU
        } while (!timer::elapsed());
        return false;
    }

    void display_countdown(uint8_t count) {
        if (count > 10) count = 9;

        uint8_t ticks = timer::TICKS_PER_SEC / FLASH_SPEED;

        display::show_char(count ? '0' + count : ' ');
        timer::enable(count ? timer::TICKS_PER_SEC : FLASH_SPEED);
        for (;;) {
            key::run();
            if (timer::elapsed()) {
                if (count && --count) {
                    timer::enable(timer::TICKS_PER_SEC);
                    display::show_char('0' + count);
                } else {
                    timer::enable(FLASH_SPEED);
                    display::show_char(ticks & 1 ? '0' + count : ' ');
                    if (--ticks == 0) return;
                }
            }
            // TODO: idle CPU
        }
    }

    void display_flashing_digit(uint8_t value) {
        if (value > 10) value = 9;
        wait_key_released();

        uint8_t tick = 0;
        timer::enable(FLASH_SPEED);
        display::show_char('0' + value);
        for (;;) {
            key::run();
            if (key::changed() && key::state() == key::state_t::pressed) break;
            if (timer::elapsed()) {
                timer::enable(FLASH_SPEED);
                display::show_char(tick & 1 ? '0' + value : ' ');
                ++tick;
                if (tick == 20) break;
            }
            // TODO: idle CPU
        }
        display::off();
    }

    bool display_char(char code) {
        display::show_char(code);

        timer::enable(timer::TICKS_PER_SEC >> 1);
        if (wait_key_pressed_or_timer_elapsed()) return true;
        display::off();
        timer::enable(1);
        return wait_key_pressed_or_timer_elapsed();
    }

    bool display_string(const char* text) {
        char code = *text++;
        while (code) {
            if (display_char(code)) return true;
            code = *text++;
        }
        return false;
    }
}
