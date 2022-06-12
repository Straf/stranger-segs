/* 
 * File:   game_vm.cpp
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

#include <avr/pgmspace.h>

#include "cpu.hpp"
#include "display.hpp"
#include "game_vm.hpp"
#include "utilities.hpp"

enum vm_result: uint8_t {
    vm_pause = 0,
    vm_exec,
    vm_stop,
};

enum vm_opcode: uint8_t {
    op_jump = 0,  // PC = next_pc(), fetch, exec tick

    op_a,     // turn on segment a, increment PC, steer (vm_next != 0), pause
    op_b,     // turn on segment b, increment PC, steer (vm_next != 0), pause
    op_c,     // turn on segment c, increment PC, steer (vm_next != 0), pause
    op_d,     // turn on segment d, increment PC, steer (vm_next != 0), pause
    op_e,     // turn on segment e, increment PC, steer (vm_next != 0), pause
    op_f,     // turn on segment f, increment PC, steer (vm_next != 0), pause
    op_g,     // turn on segment g, increment PC, steer (vm_next != 0), pause

    // on failed steer: decrement cars, reset speed, PC = 0, fetch, stop VM
    // on successful steer: increment score, recompute speed, PC = next_pc(), fetch, exec_tick
};

constexpr uint8_t vm_instruction(const vm_opcode on_tick, const int8_t next_pc) {
    return (on_tick << 5) | next_pc;
}

inline vm_opcode get_opcode(uint8_t instruction) {
    return static_cast<vm_opcode>(instruction >> 5);
}

inline uint8_t get_next_pc(uint8_t instruction) {
    return instruction & 0x01F;
}

const uint8_t vm_program[] PROGMEM = {
/*  0: */ vm_instruction(op_b,     7), // on steer, goto 7
/*  1: */ vm_instruction(op_c,     0),
/*  2: */ vm_instruction(op_d,     0),
/*  3: */ vm_instruction(op_e,    21), // on steer, goto 21
/*  4: */ vm_instruction(op_f,     0),
/*  5: */ vm_instruction(op_a,     0),
/*  6: */ vm_instruction(op_jump,  0), // goto 0
/*  7: */ vm_instruction(op_g,     0),
/*  8: */ vm_instruction(op_e,     0),
/*  9: */ vm_instruction(op_d,     0),
/* 10: */ vm_instruction(op_c,    17), // on steer, goto 17
/* 11: */ vm_instruction(op_b,     0),
/* 12: */ vm_instruction(op_a,     0),
/* 13: */ vm_instruction(op_f,    25), // on steer, goto 25
/* 14: */ vm_instruction(op_e,     0),
/* 15: */ vm_instruction(op_d,     0),
/* 16: */ vm_instruction(op_jump, 10), // goto 10
/* 17: */ vm_instruction(op_g,     0),
/* 18: */ vm_instruction(op_f,     0),
/* 19: */ vm_instruction(op_a,     0),
/* 20: */ vm_instruction(op_jump,  0), // goto 0
/* 21: */ vm_instruction(op_g,     0),
/* 22: */ vm_instruction(op_b,     0),
/* 23: */ vm_instruction(op_a,     0),
/* 24: */ vm_instruction(op_jump, 13), // goto 13
/* 25: */ vm_instruction(op_g,     0),
/* 26: */ vm_instruction(op_jump,  1), // goto 1
};

static struct {
    uint16_t score;
    uint8_t cars;  // number of cars before game over
    uint8_t speed; // abstract scale from 0 to 255
    uint8_t ticks; // ticks to wait before executing another instruction

    uint8_t pc;
    uint8_t instruction;

    void update_ticks() {
        if (speed < 80) {
            ticks = 25 - (speed >> 2); // 25..5
        } else if (speed < 128) {
            ticks = 10 - (speed >> 4); // 5..2
        } else {
            ticks = 2;
        }
    }

    inline void update_speed() {
        if (speed == 255) return;
        ++speed;
        update_ticks();
    }

    inline void fetch() {
        instruction = pgm_read_byte(vm_program + pc);
    }

    void reset(uint8_t start_speed) {
        score = 0;
        cars = 3;
        speed = start_speed;
        update_ticks();
        pc = 0;
    }

    vm_result exec() {
        const vm_opcode op = get_opcode(instruction);

        switch (op) {
            case op_jump:
                pc = get_next_pc(instruction);
                fetch();
                return vm_exec;
            case op_a:
                display::show_segments(display::segment::a);
                pc += 1;
                return vm_pause;
            case op_b:
                display::show_segments(display::segment::b);
                pc += 1;
                return vm_pause;
            case op_c:
                display::show_segments(display::segment::c);
                pc += 1;
                return vm_pause;
            case op_d:
                display::show_segments(display::segment::d);
                pc += 1;
                return vm_pause;
            case op_e:
                display::show_segments(display::segment::e);
                pc += 1;
                return vm_pause;
            case op_f:
                display::show_segments(display::segment::f);
                pc += 1;
                return vm_pause;
            case op_g:
                display::show_segments(display::segment::g);
                pc += 1;
                return vm_pause;
        }

        // invalid opcode!
        // display an 'E' (for ERROR) with the dot turned on and wait forever.
        display::show_char('E' + 128);
        for (;;) cpu::idle();
    }

    vm_result steer() {
        pc = get_next_pc(instruction);
        if (pc) {
            score += 1;
            update_speed();
            fetch();
            return vm_exec;
        } else {
            if (cars > 0) --cars;
            speed = 0;
            update_ticks();
            return vm_stop;
        }
    }

} state;

namespace vm {

    void reset(uint8_t start_speed) {
        state.reset(start_speed);
    }

    uint8_t wait_ticks() {
        return state.ticks;
    }

    void tick_event() {
        state.fetch();

        vm_result result;
        do {
            result = state.exec();
        } while (result == vm_exec);
    }

    bool steer_event() {
        switch (state.steer()) {
            case vm_stop:
                return false; // stop VM
            case vm_exec:
                tick_event();
                // PASSTHRU
            default:
                return true; // run VM
        }
    }

    bool game_over() {
        return state.cars == 0;
    }

    uint8_t remaining_cars() {
        return state.cars;
    }

    uint16_t score() {
        return state.score;
    }

    bool may_steer_safely() {
        return get_next_pc(state.instruction);
    }
}
