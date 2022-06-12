/* 
 * File:   protocol.cpp
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

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "protocol.h"
#include "serial.h"

/* --------------------------------------------------------------------- */

constexpr size_t BUFFER_MAXSIZE = 4096;

static char output_data[BUFFER_MAXSIZE];

static void copy_text(serial::buffer& output, const protocol::options& opts) {
	size_t length = 0;
	const char* in = opts.input_text;

	if (opts.is_animated()) {
		int fill = opts.animation_window - 1;
		while (fill && length < BUFFER_MAXSIZE) {
			output_data[length++] = ' ';
			--fill;
		}
	}

	while (*in && length < BUFFER_MAXSIZE) {
		output_data[length++] = *in++;
	}

	if (opts.is_animated()) {
		int fill = opts.animation_window;
		while (fill && length < BUFFER_MAXSIZE) {
			output_data[length++] = ' ';
			--fill;
		}
	}

	output.ptr = output_data;
	output.offset = 0;
	output.size = length;
}

static void process_text(serial::buffer& output, const protocol::options& opts) {
	bool plain = false;
	size_t length = 0;
	const char* in = opts.input_text;

	if (opts.is_animated()) {
		int fill = opts.animation_window - 1;
		while (fill && length < BUFFER_MAXSIZE) {
			output_data[length++] = ' ';
			--fill;
		}
	}

	while (*in && length < BUFFER_MAXSIZE) {
		if (isalnum(*in)) {
			output_data[length] = *in;
			plain = true;
			++length;
		} else if (isspace(*in)) {
			output_data[length] = *in;
			plain = true;
			++length;
		} else if (*in == '.' && plain) {
			output_data[length-1] |= 0x80;
			plain = false;
		} else {
			output_data[length] = *in;
			plain = false;
			++length;
		}
		++in;
	}

	if (opts.is_animated()) {
		int fill = opts.animation_window;
		while (fill && length < BUFFER_MAXSIZE) {
			output_data[length++] = ' ';
			--fill;
		}
	}

	output.ptr = output_data;
	output.offset = 0;
	output.size = length;
}

static int sleep_millis(long ms) {
	if (ms < 0) {
		errno = EINVAL;
		return -1;
	}

	struct timespec ts = {
		ms / 1000,          // tv_sec
		ms % 1000 * 1000000 // tv_nsec
	};

	int res;
	do {
		res = nanosleep(&ts, &ts);
	} while (res && errno == EINTR);

	return res;
}

namespace protocol {

	options::options() {
		input_text = NULL;
		raw = false;
		animation_window = 0;
		animation_timing_ms = 100;
	}

	void process(serial::buffer& output, const options& opts) {
		if (opts.raw) {
			copy_text(output, opts);
		} else {
			process_text(output, opts);
		}
	}

	void send(serial::port& port, const options& opts) {
		serial::buffer buffer;
		protocol::process(buffer, opts);

		if (buffer.size > opts.animation_window
		&& opts.is_animated()) {
			auto serial_options = port.get_options();
			long wait_ms = serial_options.ms_per_message(opts.animation_window) + END_OF_MESSAGE_MS;
			if (wait_ms < opts.animation_timing_ms) wait_ms = opts.animation_timing_ms;

			int begin = 0;
			int end = buffer.size - opts.animation_window;
			while (begin <= end) {
				buffer.offset = begin;
				buffer.size = opts.animation_window;

				if (port.write(buffer, buffer.size) < buffer.size) {
					perror("Couldn't write data to serial device");
				}

				sleep_millis(wait_ms);
				++begin;
			}
		} else {
			if (port.write(buffer, buffer.size) < buffer.size) {
				perror("Couldn't write data to serial device");
			}
		}

	}
}
