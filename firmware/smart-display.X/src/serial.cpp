/* 
 * File:   serial.cpp
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
#include <avr/interrupt.h>

#include "serial.hpp"

/**
 * The protocol speed in bits per seconds.
 */
constexpr uint16_t UART_BPS = 19200;

/**
 The protocol timeout interval in milliseconds.
 */
constexpr uint16_t PROTOCOL_TIMEOUT_MS = 50;

constexpr uint16_t uart_baud(uint16_t bps) {
    // original formula from Microchip TB3216:
    // return ( (F_CPU * 64 / (16 * (float)bps) + 0.5);
	return (uint16_t)((float)F_CPU * 4 / bps + 0.5);
}

constexpr uint16_t tca_top(uint16_t ms) { // assumes CLKSEL = DIV16
	return (uint16_t)((float)F_CPU * ms / 16000 + 0.5);
}

inline void setup_timer() {
    // CLKSEL = 4 (CLK_PER/DIV16), ENABLE = 0 (disabled)
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc;
    // TOP ~= 50 ms
    TCA0.SINGLE.PER = tca_top(PROTOCOL_TIMEOUT_MS);
    // OVF = 1 (INT cleared)
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
    // OVF = 1 (INT enabled)
    TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
}

inline void start_timer() {
    TCA0.SINGLE.CNT = 0;
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc | TCA_SINGLE_ENABLE_bm;
}

inline void stop_timer() {
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc;
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}

inline void reset_timer() {
    TCA0.SINGLE.CNT = 0;
}

inline void setup_ports() {
	// Configure port MUX to use UART's alternate pins.
	PORTMUX.CTRLB |= PORTMUX_USART0_ALTERNATE_gc;
	// Configure as output: PA1 (UART TX alternate), PA4 (CCLO output).
	PORTA.DIR |= PIN1_bm | PIN4_bm;
	// Configure PA4 (CCLO output) to output 1.
	PORTA.OUTSET |= PIN4_bm;
	// Configure as input: PA2 (UART RX alternate), PA4 (CCLO output).
	PORTA.DIR &= ~PIN2_bm;
}

inline void setup_ccl() {
	CCL.TRUTH0 = 0xF0; // IN2=0 -> OUT=0, IN2=1 -> OUT=1 (EXT mirrors RX)
	CCL.LUT0CTRLC = CCL_INSEL2_IO_gc;
	CCL.LUT0CTRLB = CCL_INSEL1_MASK_gc | CCL_INSEL0_MASK_gc;
	CCL.LUT0CTRLA = CCL_OUTEN_bm | CCL_ENABLE_bm;
	CCL.CTRLA = CCL_RUNSTDBY_bm;
}

inline void start_ccl() {
	CCL.CTRLA = CCL_RUNSTDBY_bm | CCL_ENABLE_bm;
}

inline void stop_ccl() {
	CCL.CTRLA = CCL_RUNSTDBY_bm;
}

inline void setup_usart() {
    // give USART RXC IRQ the maximum priority to minimise latency
    CPUINT.LVL1VEC = USART0_RXC_vect_num;

	USART0.BAUD = uart_baud(UART_BPS);
	USART0.CTRLC = USART_CHSIZE_8BIT_gc;
	USART0.CTRLB = USART_TXEN_bm | USART_RXEN_bm;
	USART0.CTRLA = USART_RXCIE_bm | USART_TXCIE_bm;
}

static volatile bool first;
static volatile bool error;
static volatile bool changed;

static volatile uint8_t temp;
static volatile uint8_t rx_code;

static uint8_t tx_code;
static uint8_t tx_map;
static uint8_t tx_index;

static bool inited;

ISR(TCA0_OVF_vect) {
    stop_ccl();
	stop_timer();
	first = true;
	error = false;
	changed = false;
}

ISR(USART0_TXC_vect) {
    USART0.STATUS |= USART_TXCIF_bm;
    // bit 7 is ignored: no message unit is transmitted for the DP state
    if (tx_index < 6) {
        ++tx_index;
        USART0.TXDATAL = tx_map & (1 << tx_index) ? tx_code : ' ';
    }
}

ISR(USART0_RXC_vect) {
    start_ccl();
	reset_timer();
	if (error) {
        temp = USART0.RXDATAL;	// empties RX buffer and forces clear status bits
    } else if (USART0.RXDATAH & (USART_BUFOVF_bm | USART_FERR_bm)) {
        temp = USART0.RXDATAL;	// empties RX buffer and forces clear status bits
        error = true;
    } else if (first) {
    	start_timer();
        rx_code = USART0.RXDATAL;
        changed = true;
        first = false;
    } else {
        temp = USART0.RXDATAL;	// empties RX buffer and forces clear status bits
    }
}

namespace serial {

	void init() {
        // ensure that initialisation is done once
        if (inited) return;

		first = true;
		error = false;
		changed = false;

		setup_timer();
		setup_ports();
		setup_ccl();
		setup_usart();

        inited = true;
	}

	bool has_errors() {
		return error;
	}

	bool has_data() {
		return changed;
	}

	uint8_t get_data() {
		changed = false;
		return rx_code;
	}

	void enqueue_mapped_chars(uint8_t code, uint8_t map) {
        tx_code = code;
        tx_map = map;
        tx_index = 1;
        while (!(USART0.STATUS & USART_DREIF_bm)) ;
        USART0.TXDATAL = map & (1 << 1) ? code : ' ';
    }
}
