/* 
 * File:   main.cpp
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
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define APP_VERSION "1.0.0"

#include "protocol.h"
#include "serial.h"

/* --------------------------------------------------------------------- */
/* driver (shell) */

static void print_version(const char* tool_name) {
	printf("%s %s\n", tool_name, APP_VERSION);
}

static void print_usage(const char* tool_name, int help_mode) {
	fprintf(stderr,
			"Usage: %s\t[-hrV] [-f FRAMING] [-s BIT_RATE] [-t TIMING] [-w WINDOW]\n"
		    "\t\tserial_device\n"
		    "\t\ttext_string\n",
		   tool_name);
	if (help_mode) {
		fprintf(stderr,
				"\n"
				"Sends a text string or the contents of a file to a serial device.\n"
				"\n"
				"positional arguments:\n"
				"serial_device\tthe path to a serial device (example: /dev/cu.usbserial)\n"
				"text_string\tthe string to send\n"
				"\n"
				"optional arguments:\n"
				"-V, --version\tshow program's version number and exit\n"
				"-h, --help\tshow this help message and exit\n"
				"-r, --raw\tdo not preprocess text before sending\n"
				"-s BIT_RATE, --speed BIT_RATE\n"
				"\t\tthe transmission speed in bps (default: 19200)\n"
				"-f FRAMING, --framing FRAMING\n"
				"\t\tthe character framing (default: 8N1)\n"
				"-t TIMING_MS, --timing TIMING_MS\n"
				"\t\tthe timing between two animation frames in milliseconds (default: 100)\n"
				"-w SIZE, --window SIZE\n"
				"\t\tthe size of the animation window (default: 0 - no animation)\n"
				);
	}
}

static bool parse_framing(serial::options& opts, const char* framing, const char* tool_name) {
	using namespace serial;

	if (strlen(framing) == 3) {
		switch (framing[0]) {
			case '5':
				opts.nbits = 5;
				break;
			case '6':
				opts.nbits = 6;
				break;
			case '7':
				opts.nbits = 7;
				break;
			case '8':
				opts.nbits = 8;
				break;
			default:
				fprintf(stderr,
						"%s: error: '%c' is an invalid number of data bits: "
						"please specify any of 5, 6, 7, 8\n",
						tool_name,
						framing[0]);
				return false;
		}
		switch (toupper(framing[1])) {
			case 'E':
				opts.parity = parity::even;
				break;
			case 'N':
				opts.parity = parity::none;
				break;
			case 'O':
				opts.parity = parity::odd;
				break;
			default:
				fprintf(stderr,
						"%s: error: '%c' is an invalid parity: "
						"please specify any of E(ven), N(one), O(dd)\n",
						tool_name,
						framing[1]);
				return false;
		}
		switch (framing[2]) {
			case '1':
				opts.nstops = 1;
				break;
			case '2':
				opts.nstops = 2;
				break;
			default:
				fprintf(stderr,
						"%s: error: '%c' is an invalid number of stop bits: "
						"please specify any of 1, 2\n",
						tool_name,
						framing[2]);
				return false;
		}
		return true;
	} else {
		fprintf(stderr,
				"%s: error: '%s' is an invalid framing specifier\n",
				tool_name,
				framing);
		return false;
	}
}

static bool parse_speed(serial::options& opts, const char* value, const char* tool_name) {
	intmax_t bit_rate = strtoimax(value, NULL, 10);
	if (strlen(value) > 0 && bit_rate > 0 && bit_rate <= 115200) {
		opts.speed = (speed_t)bit_rate;
		return true;
	} else {
		fprintf(stderr,
				"%s: error: '%s' is an invalid bit rate, "
				"please specify an unsigned integer less or equal to 115200 bps\n",
				tool_name,
				value);
		return false;
	}
}

static bool parse_animation_timing(protocol::options& opts, const char* value, const char* tool_name) {
	intmax_t timing_ms = strtoimax(value, NULL, 10);
	if (strlen(value) > 0 && timing_ms > 0 && timing_ms <= 1000) {
		opts.animation_timing_ms = timing_ms;
		return true;
	} else {
		fprintf(stderr,
				"%s: error: '%s' is an invalid timing, "
				"please specify an unsigned integer less or equal to 1000 ms\n",
				tool_name,
				value);
		return false;
	}
}

static bool parse_animation_window(protocol::options& opts, const char* value, const char* tool_name) {
	intmax_t window = strtoimax(value, NULL, 10);
	if (strlen(value) > 0 && window > 0 && window <= 128) {
		opts.animation_window = window;
		return true;
	} else {
		fprintf(stderr,
				"%s: error: '%s' is an invalid animation window, "
				"please specify an unsigned integer less or equal to 128\n",
				tool_name,
				value);
		return false;
	}
}

static bool parse_arguments(
		serial::port& port,
		protocol::options& protocol_options,
		int argc,
		char* argv[]) {
	static const char* short_options = "f:hrs:t:Vw:";
	static struct option long_options[] = {
		{ "framing", required_argument, NULL, 'f' },
		{ "help", no_argument, NULL, 'h' },
		{ "raw", no_argument, NULL, 'r' },
		{ "speed", required_argument, NULL, 's' },
		{ "timing", required_argument, NULL, 'w' },
		{ "version", no_argument, NULL, 'V' },
		{ "window", required_argument, NULL, 'w' },
		{ NULL, 0, NULL, 0 }
	};

	const char* tool_name = argv[0];
	int opt;

	serial::options port_options;

	while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
		switch (opt) {
			case 'f':	// --framing
				if ( ! parse_framing(port_options, optarg, tool_name)) return false;
				break;
			case 'h':	// --help
				print_usage(tool_name, 1);
				return false;
			case 'r':	// --raw
				protocol_options.raw = true;
				break;
			case 's':	// --speed
				if ( ! parse_speed(port_options, optarg, tool_name)) return false;
				break;
			case 't':	// --timing
				if ( ! parse_animation_timing(protocol_options, optarg, tool_name)) return false;
				break;
			case 'V':	// --version
				print_version(tool_name);
				return false;
			case 'w':	// --window
				if ( ! parse_animation_window(protocol_options, optarg, tool_name)) return false;
				break;
			default:
				return false;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc == 2) {
		port.set_path(argv[0]);
		port.set_options(port_options);

		protocol_options.input_text = argv[1];

		return true;
	} else {
		print_usage(tool_name, 0);
		fprintf(stderr,
				"%s: error: the following arguments are required: %s\n",
				tool_name,
				argc == 1 ? "text_string" : "serial_device");
		return false;
	}
}

/* --------------------------------------------------------------------- */
/* driver (main) */

int main(int argc, char * argv[]) {
	const char* tool_name = argv[0];

	serial::port port;
	protocol::options options;

	if (parse_arguments(port, options, argc, argv)) {
		if (port.open()) {
			print_version(tool_name);
			printf("Connected to %s\n", port.get_path());

			protocol::send(port, options);

			port.close();
		}
	}

	return 0;
}
