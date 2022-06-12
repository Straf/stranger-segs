/* 
 * File:   game_vm.hpp
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

#ifndef GAME_VM_HPP_DEFINED
#define	GAME_VM_HPP_DEFINED

#include <stdint.h>

namespace vm {

    /**
     * Resets the state of game VM.
     * Must be called each time a new game begins.
     * @param start_speed The initial speed value. Default value is 0.
     */
    void reset(uint8_t start_speed = 0);

    /**
     * Gets the number of ticks to wait before the next tick event.
     * @return The number of ticks to wait.
     */
    uint8_t wait_ticks();

    /**
     * Reacts to a tick event.
     */
    void tick_event();

    /**
     * Reacts to a steer event.
     * May stop the game if the car crashes.
     * @return true if the game continues, false if the game stops.
     */
    bool steer_event();

    /**
     * Checks if game is over (too many crashes).
     * @return true if game is over, false if game may resume.
     */
    bool game_over();

    /**
     * Gets the number of remaining cars before game over.
     * @return The number of remaining cars.
     */
    uint8_t remaining_cars();

    /**
     * Gets the score earned in the game until now.
     * The score is the number of steering successfully executed.
     * @return The player's score.
     */
    uint16_t score();

    /**
     * Checks if player may steer safely.
     * Used in demo mode to send synthetic steer events.
     * @return true if may steer safely, false otherwise.
     */
    bool may_steer_safely();

}

#endif	/* GAME_VM_HPP_DEFINED */
