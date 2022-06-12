/* 
 * File:   serial.h
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

#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

/* --------------------------------------------------------------------- */

#include <stddef.h>
#include <termios.h>

#include <string>

/* --------------------------------------------------------------------- */

namespace serial {

	enum class parity {
		none,
		even,
		odd
	};

	enum class handshake {
		none,
		xon_xoff,
		hardware
	};

	struct options {
		speed_t   speed;
		int       nbits;
		parity    parity;
		int       nstops;
		handshake handshake;

		options();

		int bits_per_char();
		int ms_per_char();
		long ms_per_message(int nchars);
	};

	struct buffer {
		void* ptr;    // pointer to the memory area where to read/write bytes
		int   offset; // start offset inside the memory area
		int   size;   // max size available starting from the offset
	};

	class port {
		std::string    device_path;
		int            device_fh;
		options        options;
		struct termios saved;

	public:
		port();
		~port();

		inline void set_path(const char* device_path) {
			this->device_path = device_path;
		}

		inline const char* get_path() const {
			return device_path.c_str();
		}

		inline void set_options(const serial::options& options) {
			this->options = options;
		};

		inline const serial::options& get_options() const {
			return options;
		}

		bool open();
		void close();

		ssize_t read(const buffer& buffer);
		ssize_t write(const buffer& buffer, size_t size);

	};

}

#endif /* SERIAL_H_INCLUDED */
