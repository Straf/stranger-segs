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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "serial.h"

/* --------------------------------------------------------------------- */
/* compatibility macros */

#if defined( CRTSCTS )
#define HANDSHAKE_HW CRTSCTS
#elif defined( CNEW_RTSCTS )
#define HANDSHAKE_HW CNEW_RTSCTS
#else
#error "Hardware handshake not supported on this platform."
#endif

namespace serial {

	options::options() {
		speed = 19200;
		nbits = 8;
		parity = parity::none;
		nstops = 1;
		handshake = handshake::none;
	}

	int options::bits_per_char() {
		return 1 + nbits + (parity != parity::none ? 1 : 0) + nstops;
	}

	int options::ms_per_char() {
		return (int)(bits_per_char() * 1000.0 / speed + 0.5);
	}

	long options::ms_per_message(int nchars) {
		return (long)(bits_per_char() * nchars * 1000.0 / speed + 0.5);
	}

	port::port() {
		device_fh = -1;
	}

	port::~port() {
		close();
	}

	bool port::open() {
		device_fh = ::open(device_path.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
		if (device_fh != -1) {
			if (fcntl(device_fh, F_SETFL, 0) == -1) {
				perror("Error while setting serial FD");
			}

			if (tcgetattr(device_fh, &saved) == -1) {
				perror("Error while getting options for serial device");
			} else {
				struct termios new_options = saved;

//				cfmakeraw(&new_options);
				cfsetispeed(&new_options, options.speed);
				cfsetospeed(&new_options, options.speed);
				new_options.c_cflag |= (CLOCAL | CREAD);
				new_options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);	// RAW mode
				new_options.c_oflag |= (OPOST | ONLCR);

				new_options.c_cflag &= ~CSIZE; // Mask the character size bits
				switch (options.nbits) {
					case 5:
						new_options.c_cflag |= CS5;
						break;
					case 6:
						new_options.c_cflag |= CS6;
						break;
					case 7:
						new_options.c_cflag |= CS7;
						break;
					default:	// 8 bits
						new_options.c_cflag |= CS8;
						break;
				}

				switch (options.parity) {
					case parity::none:
						new_options.c_cflag &= ~PARENB;
						break;
					case parity::even:
						new_options.c_cflag |= PARENB;
						new_options.c_cflag &= ~PARODD;
						new_options.c_iflag |= (INPCK | ISTRIP);
						break;
					case parity::odd:
						new_options.c_cflag |= PARENB;
						new_options.c_cflag |= PARODD;
						new_options.c_iflag |= (INPCK | ISTRIP);
						break;
				}

				switch (options.nstops) {
					case 2:
						new_options.c_cflag |= CSTOPB;
						break;
					default:	// 1 stop bit
						new_options.c_cflag &= ~CSTOPB;
						break;
				}

				switch (options.handshake) {
					case handshake::none:
						new_options.c_cflag &= ~CRTSCTS;
						new_options.c_iflag &= ~(IXON | IXOFF | IXANY);
						break;
					case handshake::xon_xoff:
						new_options.c_cflag &= ~CRTSCTS;
						new_options.c_iflag |= (IXON | IXOFF | IXANY);
						break;
					case handshake::hardware:
						new_options.c_cflag |= CRTSCTS;
						new_options.c_iflag &= ~(IXON | IXOFF | IXANY);
						break;
				}

				if (tcsetattr(device_fh, TCSANOW, &new_options) == -1) {
					perror("Error while setting options for serial device");
				} else {
					return true;
				}
			}

			close();
		} else {
			perror(device_path.c_str());
		}
		return 0;
	}

	void port::close() {
		if (device_fh != -1) {
			tcdrain(device_fh);
			sleep(1); // workaround for USB-serial adapters
			tcsetattr(device_fh, TCSANOW, &saved);
			::close(device_fh);
			device_fh = -1;
		}
	}

	ssize_t port::read(const buffer& buffer) {
		int avail;

		if (::ioctl(device_fh, FIONREAD, &avail) == -1) {
			perror("*** Error while getting avail data from serial device");
			return -1;
		} else if (avail > 0) {
			size_t  size = (avail < buffer.size ? avail : buffer.size);
			return ::read(device_fh, ((char*)buffer.ptr) + buffer.offset, size);
		} else {
			return avail;
		}
	}

	ssize_t port::write(const buffer& buffer, size_t size) {
		ssize_t written;
		do {
			ssize_t count = ::write(device_fh, ((char*)buffer.ptr) + buffer.offset, size);
			if (count == -1) return -1;
			written += count;
		} while (written < size);

		return size;
	}

}
