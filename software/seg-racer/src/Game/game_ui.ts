/* 
 * File:   game_ui.ts
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

import { display } from "./display";
import { key, KeyListener } from "./key";
import { TICKS_PER_SECOND, timer, TimerListener } from "./timer";

const FLASH_SPEED = 5;

const display_digit = (count: number) => display.show_char('0123456789'.charAt(count));

export const ui = {

    for_ticks: (ticks: number) => new Promise<void>(
        (resolve, reject) => {
            const listener: TimerListener = (event) => {
                timer.unregister(listener);
                resolve();
            };
            timer.register(listener);
        }
    ),
    
    key_released: async () => new Promise<void>(
        (resolve, reject) => {
            if (key.released()) {
                resolve();
                return;
            }
            const listener = (pressed: boolean) => {
                if (!pressed) {
                    key.unregister(listener);
                    resolve();
                }
            };
            key.register(listener);
        }
    ),

    key_pressed_or_timer_elapsed: async () => new Promise<boolean>(
        (resolve, reject) => {
            if (key.pressed() || timer.elapsed()) {
                resolve(key.pressed());
                return;
            }
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
                end(false);
            };
            timer.register(timerListener);
        }
    ),

    display_countdown: async (count: number) => new Promise<void>(
        (resolve, reject) => {
            if (count > 10) count = 9;
            let ticks = TICKS_PER_SECOND / FLASH_SPEED;
            const end = () => {
                timer.unregister(listener);
                resolve();
            };
            const listener: TimerListener = (event) => {
                if (event === 'disabled') {
                    end();
                } else if (count && --count) {
                    timer.enable(TICKS_PER_SECOND);
                    display_digit(count);
                } else {
                    timer.enable(FLASH_SPEED);
                    display.show_char(ticks & 1 ? '0' : ' ');
                    if (--ticks === 0) end();
                }
            };
            timer.register(listener);
            display_digit(count);
            timer.enable(count ? TICKS_PER_SECOND : FLASH_SPEED);
        }
    ),

    display_flashing_digit: async (value: number) => {
        if (value > 10) value = 9;
        await ui.key_released();
        return new Promise<void>(
            (resolve, reject) => {
                let tick = 0;
                const end = () => {
                    timer.unregister(listener);
                    display.off();
                    resolve();
                };
                const listener: TimerListener = (event) => {
                    if (event === 'disabled' || tick === 20 || key.pressed()) {
                        end();
                        return;
                    }

                    if (tick & 1) {
                        display_digit(value);
                    } else {
                        display.show_char(' ');
                    }

                    ++tick;
                    if (tick === 20) {
                        end();
                        return;
                    }

                    timer.enable(FLASH_SPEED);
                };
                timer.register(listener);
                display_digit(value);
                timer.enable(FLASH_SPEED);
            }
        );
    },

    display_char: async (char: string) => {
        display.show_char(char);
        timer.enable(TICKS_PER_SECOND / 2);
        if (await ui.key_pressed_or_timer_elapsed()) return true;
        display.off();
        timer.enable(1);
        return await ui.key_pressed_or_timer_elapsed();
    },

    display_string: async (text: string) => {
        for (let i = 0; i < text.length; ++i) {
            if (await ui.display_char(text.charAt(i))) return true;
        }
        return false;
    }

};
