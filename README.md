STRANGER SEGS
=============

An hardware platform to play with 7-segment displays.

This project was submitted to the montly challenge of the Element14 community (April 2022).
This repository contains all the artifacts developed for the challenge.

What you need
-------------

* The schematics and the PCB were produced with [KiCad 5.1.12](https://www.kicad.org).

* The firmware of the two applications was developed with [Microchip MPLAB X IDE 6.00](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide) and the [Microchip AVR-GCC 5.4.0](https://www.microchip.com/en-us/tools-resources/develop/microchip-studio/gcc-compilers).

    - The first application is a **minimalist** racing game which may be played with a key and a display.

    - The second application is the firmware of a smart display module. Each module may be chained to others or composed in arbitrary display trees.

* The application to feed the display chain with data can be compiled with any **GCC** toolchain for a POSIX system. The build system is `make`. Built and tested on **macOS**, with [VSCode](https://code.visualstudio.com) as the IDE. It may work with Linux and [Cygwin](https://www.cygwin.com) for Windows.

### To build the firmware

1. First open the project with MPLAB X (some `makefile`s must be regenerated by the IDE).

2. Choose the exact part which you wish to use. The firmware should work with any **tinyavr 0** or **tinyavr 1** with enough RAM and flash to run the application. The game requires ~3KB of flash. The smart module less than 2KB. The use of RAM is negligible. The hardware module requires a **20-Pin VQFN** package.

3. Install the Device Family Pack of the part you wish to use.

4. Finally, build the application.

### To program the firmware into a module

To be programmed, the hardware module requires a [6-pin Tag-Connect cable](https://www.tag-connect.com/product-category/products/cables/6-pin-target) and a compatible programming tool, such as the **Atmel-ICE** or the **PicKit4**.

Since the **tinyavr** used have a **UPDI** interface, you may also use a simple USB-to-serial adapter with [pymcuprog](https://github.com/microchip-pic-avr-tools/pymcuprog).

### To build the driver application

A simple `make` should suffice.

Other resources
---------------

The project is described in two blog posts in the E14 community.

* The [first post](#);

* The [second post](#).
