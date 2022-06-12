/* 
 * File:   timer.hpp
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

#ifndef TIMER_HPP_INCLUDED
#define	TIMER_HPP_INCLUDED

#include <stdint.h>

namespace timer {
    /**
     * The number of ticks in one second.
     */
    constexpr uint8_t TICKS_PER_SEC = 50;

    /**
     * Initialises the timer state and peripherals.
     * The timer starts to count ticks and seconds.
     */
    void init();

    /**
     * Clears the timer state.
     * All counters are resets to zero and the timeout is canceled.
     */
    void reset();

    /**
     * Gets the number of elapsed ticks.
     * This counter is only 8-bit wide so it wraps around often.
     * @return The number of elapsed ticks.
     */
    uint8_t ticks();

    /**
     * Gets the number of elapsed seconds.
     * @return The number of elapsed seconds.
     */
    uint16_t seconds();

    /**
     * Configures and enables a timeout in the future.
     * @param ticks The number of ticks to wait.
     */
    void enable(uint8_t ticks);

    /**
     * Disables the timeout.
     */
    void disable();

    /**
     * Checks if the timeout timer has elapsed.
     * A disabled timer never elapses.
     * @return true if the timer has elapsed, false otherwise.
     */
    bool elapsed();

    /**
     * Gets the number of ticks remaining before the timeout timer elapses.
     * @return The number of ticks remaining or 0 if elapsed or disabled.
     */
    uint8_t remaining_ticks();
}

#endif	/* TIMER_HPP_INCLUDED */
