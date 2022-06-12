/*
 * File:   smart_display.cpp
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

#include <stdint.h>

#include "cpu.hpp"
#include "display.hpp"
#include "fuses.hpp"
#include "serial.hpp"
#include "timer.hpp"

constexpr uint8_t FLASH_SPEED = 5;
constexpr uint8_t ERR_CHAR = 'E';

inline void error_loop() {
    uint8_t tick = 0;
    timer::enable(FLASH_SPEED);
    display::show_char(ERR_CHAR);
    do {
        if (timer::elapsed()) {
            timer::enable(FLASH_SPEED);
            display::show_char(tick & 1 ? ERR_CHAR : ' ');
            ++tick;
        }
        // TODO: idle CPU
    } while (serial::has_errors());
    display::off();
}

namespace smart_display {

    void init() {
        cpu::irq_roundrobin();
        timer::init();
        fuses::init();
        display::init();
        serial::init();
    }

    void run() {
        bool root_node = fuses::get_state(fuses::id::fuse0);

        for (;;) {
            if (serial::has_errors()) {
                error_loop();
            } else if (serial::has_data()) {
                if (root_node) {
                    uint8_t code = serial::get_data();
                    uint8_t map = display::char_to_segs(code);
                    serial::enqueue_mapped_chars(code, map);
                    display::show_char(serial::get_root_char(code, map));
                } else {
                    display::show_char(serial::get_data());
                }
            }
        }
    }
    
}
