/* 
 * File:   serial.hpp
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

#ifndef SERIAL_HPP_INCLUDED
#define SERIAL_HPP_INCLUDED

namespace serial {

    /**
     * Initialises the hardware resources related to the communication ports.
     * GPIO: PA1, PA2, PA4.
     * USART: USART0.
     * CCL: LUT0.
     * Timer A: TCA0.
     */
	void init();

    /**
     * Checks if any communication error occurred.
     * Since the protocol doesn't use any handshake, protocol errors
     * are limited to message framing and RX buffer overflows.
     * Any error condition is maintained until the protocol timeout has elapsed.
     * Normal communication is then resumed.
     * @return true if any error occurred, false otherwise.
     */
	bool has_errors();

    /**
     * Checks whether a character to display was received.
     * A character is received only once per message.
     * @return true if a character code was received, false otherwise.
     */
	bool has_data();

    /**
     * Gets the last received character code.
     * The protocol assumes that 7-bit character codes are transmitted with
     * 8-bit message units. When the 8th bit is set, this means that
     * the display decimal point must be turned on, off otherwise.
     * @return The last received character code.
     */
	uint8_t get_data();

    /**
     * In self-similar mode, computes the effective
     * character code to show on this module's display.
     * @param code The received character code.
     * @param map The segment map corresponding to the received character code.
     * @return The character code to display.
     */
    inline uint8_t get_root_char(uint8_t code, uint8_t map) {
        return map & 1 ? code : ' ';
    }

    /**
     * In self-similar mode, enqueues a message for the chain of display
     * connected to this module.
     * This message is sent through the UART TX module.
     * Standard message is forwarded through the CCL data path.
     * @param code The received character code.
     * @param map The segment map corresponding to the received character code.
     */
	void enqueue_mapped_chars(uint8_t code, uint8_t map);

}

#endif /* SERIAL_HPP_INCLUDED */
