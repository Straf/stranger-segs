import { display } from './display';

export const vm_pause = 0;
export const vm_exec = 1;
export const vm_stop = 2;

export enum vm_result {
    vm_pause = 0,
    vm_exec,
    vm_stop,
}

export const op_jump = 0;
export const op_a = 1;
export const op_b = 2;
export const op_c = 3;
export const op_d = 4;
export const op_e = 5;
export const op_f = 6;
export const op_g = 7;

enum vm_opcode {
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
}

const vm_instruction = (on_tick: vm_opcode, next_pc: number) => (on_tick << 5) | next_pc;

const get_opcode = (instruction: number): vm_opcode => instruction >> 5;

const get_next_pc = (instruction: number): number => instruction & 0x01F;

const vm_program: number[] = [
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
];

class game_state {
    score: number = 0;
    cars: number = 0;  // number of cars before game over
    speed: number = 0; // abstract scale from 0 to 255
    ticks: number = 0; // ticks to wait before executing another instruction

    pc: number = 0;
    instruction: number = 0;

    update_ticks(): void {
        if (this.speed < 80) {
            this.ticks = 25 - (this.speed >> 2); // 25..5
        } else if (this.speed < 128) {
            this.ticks = 10 - (this.speed >> 4); // 5..2
        } else {
            this.ticks = 2;
        }
    }

    update_speed(): void {
        if (this.speed === 255) return;
        ++this.speed;
        this.update_ticks();
    }

    fetch(): void {
        this.instruction = vm_program[this.pc];
    }

    reset(start_speed: number): void {
        this.score = 0;
        this.cars = 3;
        this.speed = start_speed;
        this.update_ticks();
        this.pc = 0;
    }

    exec(): vm_result {
        const op = get_opcode(this.instruction);

        switch (op) {
            case op_jump:
                this.pc = get_next_pc(this.instruction);
                this.fetch();
                return vm_exec;
            case op_a:
                display.show_segments('a');
                this.pc += 1;
                return vm_pause;
            case op_b:
                display.show_segments('b');
                this.pc += 1;
                return vm_pause;
            case op_c:
                display.show_segments('c');
                this.pc += 1;
                return vm_pause;
            case op_d:
                display.show_segments('d');
                this.pc += 1;
                return vm_pause;
            case op_e:
                display.show_segments('e');
                this.pc += 1;
                return vm_pause;
            case op_f:
                display.show_segments('f');
                this.pc += 1;
                return vm_pause;
            case op_g:
                display.show_segments('g');
                this.pc += 1;
                return vm_pause;
            default:
                console.error(`Invalid VM opcode: ${this.instruction}`);
                return vm_pause;
        }
    }

    steer(): vm_result {
        this.pc = get_next_pc(this.instruction);
        if (this.pc) {
            this.score += 1;
            this.update_speed();
            this.fetch();
            return vm_exec;
        } else {
            if (this.cars > 0) --this.cars;
            this.speed = 0;
            this.update_ticks();
            return vm_stop;
        }
    }

};

const state = new game_state();

export const vm = {

    reset: (start_speed: number = 0): void => state.reset(start_speed),

    wait_ticks: (): number => state.ticks,

    tick_event: (): void => {
        state.fetch();
        let result: vm_result;
        do {
            result = state.exec();
        } while (result === vm_exec);
    },

    steer_event: (): boolean => {
        switch (state.steer()) {
            case vm_stop:
                return false; // stop VM
            case vm_exec:
                vm.tick_event();
                return true; // run VM
            default:
                return true; // run VM
        }
    },

    game_over: (): boolean => state.cars === 0,

    remaining_cars: (): number => state.cars,

    score: (): number => state.score,

    may_steer_safely: (): boolean => !!get_next_pc(state.instruction),

};
