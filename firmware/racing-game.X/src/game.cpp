/* 
 * File:   game.cpp
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

#include "cpu.hpp"
#include "display.hpp"
#include "game.hpp"
#include "game_ui.hpp"
#include "game_vm.hpp"
#include "key.hpp"
#include "timer.hpp"
#include "utilities.hpp"

static const char start_text[] PROGMEM = {
    'P', 'r', 'e', 's', 's', ' ',
    't', 'o', ' ',
    'p', 'l', 'a', 'y', ' ',
};
static const char score_prefix[] PROGMEM = {
    'S', 'c', 'o', 'r', 'e', ' ',
};
static const char score_suffix[] PROGMEM = {
    ' ',
    'P', 'r', 'e', 's', 's', ' ',
    't', 'o', ' ',
    'p', 'l', 'a', 'y', ' ',
    'a', 'g', 'a', 'i', 'n', ' ',
};

inline void demo_loop() {
    str_builder str;
    str.append_progmem(start_text, sizeof(start_text));

    for (;;) {
        if (ui::display_string( str.text() )) return;
        uint8_t loops = 20;

        vm::reset(50);
        vm::tick_event();
        timer::enable( vm::wait_ticks() );
        while (loops) {
            key::run();
            if (key::changed() && key::state() == key::state_t::pressed) return;

            if (timer::remaining_ticks() < 5) {
                if (vm::may_steer_safely() && ((timer::ticks() & 0xF) < 4)) {
                    if (!vm::steer_event()) break;

                    timer::enable( vm::wait_ticks() );

                    --loops;
                }
            }

            if (timer::elapsed()) {
                vm::tick_event();
                timer::enable( vm::wait_ticks() );
            }
            // TODO: idle CPU
        }
    }
}

inline void play_game() {
    vm::tick_event();
    timer::enable( vm::wait_ticks() );
    for (;;) {
        key::run();
        if (key::changed() && key::state() == key::state_t::pressed) {
            if (!vm::steer_event()) return;
        }
        if (timer::elapsed()) {
            vm::tick_event();
            timer::enable( vm::wait_ticks() );
        }
        // TODO: idle CPU
    }
}

inline void game_loop() {
    vm::reset();
    ui::display_countdown(3);
    for (;;) {
        play_game();
        ui::display_flashing_digit( vm::remaining_cars() );
        if (vm::game_over()) break;
    }
}

inline void score_loop() {
    str_builder str;

    str.append_progmem(score_prefix, sizeof(score_prefix));
    str.append_uint( vm::score() );
    str.append_progmem(score_suffix, sizeof(score_suffix));

    while (!ui::display_string( str.text() )) ;
}

namespace game {

    void init() {
        cpu::irq_roundrobin();
        timer::init();
        key::init();
        display::init();
    }

    void run() {
        demo_loop();
        ui::wait_key_released();
        for (;;) {
            game_loop();
            ui::wait_key_released();
            score_loop();
            ui::wait_key_released();
        }
    }
}
