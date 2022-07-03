/*
 * File:   game.ts
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

import { ui } from "./game_ui";
import { vm } from "./game_vm";
import { key, KeyListener } from "./key";
import { timer, TimerListener } from "./timer";

const STEER_TICKS = 5;


const demo_loop = async () => {
    if (await ui.display_string("Press to play ")) return true;
    let loops = 20;
    
    vm.reset(50);
    vm.tick_event();
    timer.enable( vm.wait_ticks() - STEER_TICKS );

    return new Promise<boolean>(
        (resolve, reject) => {
            let tick = true;
            const end = (rc: boolean) => {
                key.unregister(keyListener);
                timer.unregister(timerListener);
                resolve(rc);
            };
            const keyListener: KeyListener = (pressed) => {
                if (pressed) end(true);
            };
            key.register(keyListener);
            const timerListener: TimerListener = (event) => {
                if (tick) {
                    tick = false;

                    const steer = vm.may_steer_safely() && Math.random() < 0.3;
                    if (steer) {
                        if (!vm.steer_event()) {
                            end(false);
                            return;
                        }
    
                        --loops;
                        if (!loops) {
                            end(false);
                            return;
                        }
                    }

                    timer.enable( STEER_TICKS );
                } else {
                    tick = true;
                    vm.tick_event();
                    timer.enable( vm.wait_ticks() - STEER_TICKS );
                }
            };
            timer.register(timerListener);
        }
    );
};

const do_tick = () => {
    vm.tick_event();
    timer.enable( vm.wait_ticks() );
};

const play_game = async () => {
    do_tick();
    return new Promise<void>(
        (resolve, reject) => {
            const end = () => {
                key.unregister(keyListener);
                timer.unregister(timerListener);
                resolve();
            };
            const keyListener: KeyListener = (pressed) => {
                if (pressed && !vm.steer_event()) end();
            };
            key.register(keyListener);
            const timerListener: TimerListener = (event) => {
                if (event === 'elapsed') do_tick();
                else end();
            };
            timer.register(timerListener);
        }
    );
}

const game_loop = async () => {
    vm.reset();
    await ui.display_countdown(3);
    for (;;) {
        await play_game();
        await ui.display_flashing_digit( vm.remaining_cars() );
        if (vm.game_over()) break;
    }
};

const score_loop = async () => {
    const text = `Score ${vm.score()} Press to play again `;
    while (! await ui.display_string(text)) ;
}

export const game = {

    run: async () => {
        while (! await demo_loop());
        await ui.key_released();
        for (;;) {
            await game_loop();
            await ui.key_released();
            await score_loop();
            await ui.key_released();
        }
    }

};
