/* 
 * File:   fuses.hpp
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

#ifndef FUSES_HPP_INCLUDED
#define	FUSES_HPP_INCLUDED

#include <stdint.h>

namespace fuses {
    enum class id: uint8_t {
        fuse0,
        fuse1,
        fuse2,
        fuse3, // shared with the key module
    };

    /**
     * Initialises the hardware resources related to the external fuses.
     * GPIO: PC0, PC3 (shared with the key module).
     * Please, call this function before the key module is initialised.
     * Notice: fuse state is read only once at the application's startup.
     * Fuses never change while the application is running.
     */
    void init();

    /**
     * Gets the state of a fuse.
     * Notice: the fuse3 reads as soldered when the key is pressed.
     * @param fuse_id The ID of the fuse to check.
     * @return true if the fuse is soldered, false otherwise.
     */
    bool get_state(id fuse_id);

}

#endif	/* FUSES_HPP_INCLUDED */

