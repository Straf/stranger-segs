/* 
 * File:   utilities.hpp
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

#ifndef UTILITIES_HPP_DEFINED
#define	UTILITIES_HPP_DEFINED

#include <stdint.h>

class str_builder {

private:
    char buf[48];
    uint8_t len;

public:
    inline str_builder() {
        clear();
    }

    /**
     * Gets the text contents.
     * @return The text contents as a NUL-terminated C string.
     */
    inline const char* text() {
        return buf;
    }

    /**
     * Gets the text size.
     * @return The text size.
     */
    inline uint8_t length() {
        return len;
    }

    /**
     * Clears the text content.
     */
    void clear();

    /**
     * Appends to current contents some text from PROGMEM.
     * @param ptr A pointer to the text to append.
     * @param size The size of the text to append.
     */
    void append_progmem(const void *ptr, size_t size);

    /**
     * Appends to current contents an unsigned integer.
     * The number is rendered in radix = 10.
     * @param value The unsigned integer to append.
     */
    void append_uint(unsigned int value);

};

#endif	/* UTILITIES_HPP_DEFINED */

