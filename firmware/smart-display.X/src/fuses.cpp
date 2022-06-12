/* 
 * File:   fuses.cpp
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
#include <avr/cpufunc.h>

#include <stdint.h>

#include "fuses.hpp"

static uint8_t state;

namespace fuses {

    void init() {
        // ensure PC0..PC3 is INPUT
        PORTC.DIRCLR = 0x0F;
        // INVEN = 1 (pin -> GND when soldered), PULLUPEN = 1 (enabled)
        PORTC.PIN0CTRL = PORT_INVEN_bm | PORT_PULLUPEN_bm;
        // INVEN = 1 (pin -> GND when soldered), PULLUPEN = 1 (enabled)
        PORTC.PIN1CTRL = PORT_INVEN_bm | PORT_PULLUPEN_bm;
        // INVEN = 1 (pin -> GND when soldered), PULLUPEN = 1 (enabled)
        PORTC.PIN2CTRL = PORT_INVEN_bm | PORT_PULLUPEN_bm;
        // INVEN = 1 (pin -> GND when soldered), PULLUPEN = 1 (enabled)
        PORTC.PIN3CTRL = PORT_INVEN_bm | PORT_PULLUPEN_bm;
        // ensure stable port state
        _NOP();

        state = PORTC.IN & 0x0F;

        PORTC.PIN0CTRL = 0;
        PORTC.PIN1CTRL = 0;
        PORTC.PIN2CTRL = 0;
        PORTC.PIN3CTRL = 0;
    }

    bool get_state(id fuse_id) {
        switch (fuse_id) {
            case id::fuse0: return state & 0x01;
            case id::fuse1: return state & 0x02;
            case id::fuse2: return state & 0x04;
            case id::fuse3: return state & 0x08;
            default: return 0;
        }
    }

}
