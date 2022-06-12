/* 
 * File:   display.hpp
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

#ifndef DISPLAY_HPP_INCLUDED
#define	DISPLAY_HPP_INCLUDED

#include <stdint.h>

namespace display {

    namespace segment {
        constexpr uint8_t a  = 0x01;
        constexpr uint8_t b  = 0x02;
        constexpr uint8_t c  = 0x04;
        constexpr uint8_t d  = 0x08;
        constexpr uint8_t e  = 0x10;
        constexpr uint8_t f  = 0x20;
        constexpr uint8_t g  = 0x40;
        constexpr uint8_t dp = 0x80;
    }

    /**
     * Initialises the hardware resources related to the 7-seg display.
     * GPIO: PA6, PA7, PB0, PB1, PB2, PB3, PB4, PB5.
     * Timer B: TCB0.
     */
    void init();

    /**
     * Turns off all segments.
     */
    void off();

    /**
     * Displays a character.
     * Supported character codes are displayed.
     * Unsupported character codes leave the display off.
     * If the character code has the msb set, the display dot is turned on.
     * 
     * @param code The ASCII code of the character to display.
     */
    void show_char(char code);

    /**
     * Displays an arbitrary set of segments.
     * Each segment is associated to a bit of the given map.
     * When the bit is 1, the segment is on, off otherwise.
     * @see segments for a list of valid segment bits.
     * 
     * @param map The map of the segments to display.
     */
    void show_segments(uint8_t map);

    /**
     * Maps an ASCII character code to a set of segments.
     * If the given character has the msb set, the display dot is turned on.
     * If the character cannot be mapped, a blank map is returned.
     * 
     * @param code An ASCII character code.
     * @return The corresponding segment map.
     */
    uint8_t char_to_segs(char code);

};

#endif	/* DISPLAY_HPP_INCLUDED */
