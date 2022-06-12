/* 
 * File:   game_ui.hpp
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

#ifndef GAME_UI_HPP_DEFINED
#define	GAME_UI_HPP_DEFINED

#include <stdint.h>

namespace ui {

    /**
     * Loops until the key is released.
     * Does not reset the key changed flag.
     */
    extern void wait_key_released();

    /**
     * Waits for key pressed or timer elapsed.
     * @return true if key pressed, false if timer elapsed.
     */
    extern bool wait_key_pressed_or_timer_elapsed();

    /**
     * Displays a countdown from the given value to 0.
     * Key is ignored.
     * @param count The number to start from.
     *              Value is clamped to the interval [0..9].
     */
    extern void display_countdown(uint8_t count);

    /**
     * Displays a single digit flashing.
     * The animation ends when the key is pressed or after a while.
     * @param value The number to display.
     *              Value is clamped to the interval [0..9].
     */
    extern void display_flashing_digit(uint8_t value);

    /**
     * Displays a character for half a second.
     * Key is checked.
     * @param code The character code to display.
     * @return true if the key was pressed, false otherwise.
     */
    extern bool display_char(char code);

    /**
     * Displays each character of a string in sequence for half a second
     * until the key is pressed or the string is terminated.
     * Key is checked.
     * @param text The text to display as a NUL-terminated string.
     *             String must be in RAM.
     * @return true if the key was pressed, false otherwise.
     */
    extern bool display_string(const char* text);
}

#endif	/* GAME_UI_HPP_DEFINED */
