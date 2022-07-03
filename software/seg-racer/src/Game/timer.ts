/* 
 * File:   timer.ts
 * Author: Gabriele Falcioni
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

export type TimerEvent = 'elapsed' | 'disabled';
export type TimerListener = (event: TimerEvent) => void;

export const TICKS_PER_SECOND = 50;

let listeners: TimerListener[] = [];
let base_time = Date.now();

let _timer: ReturnType<typeof setTimeout> | undefined = undefined;
let _elapsed: boolean = false;
let _ticks: number = 0;
let _start: number = 0;

const clear = () => {
    clearTimeout(_timer);
    _timer = undefined;
    _elapsed = false;
};

export const timer = {

    register: (listener: TimerListener): void => {
        listeners = [...listeners, listener];
    },

    unregister: (listener: TimerListener): void => {
        listeners = listeners.filter((item) => item !== listener);
    },

    reset: (): void => {
        base_time =  Date.now();
    },

    ticks: (): number => Math.round((Date.now() - base_time) * 1000 / TICKS_PER_SECOND) % 256,

    seconds: (): number => Math.round((Date.now() - base_time) / 1000),

    enable: (ticks: number): void => {
        clear();
        _ticks = ticks;
        _timer = setTimeout(() => {
            clear();
            _elapsed = true;
            listeners.forEach((item) => item('elapsed'));
        }, ticks * 1000 / TICKS_PER_SECOND);
        _start = Date.now();
    },

    disable: (): void => {
        clear();
        listeners.forEach((item) => item('disabled'));
    },

    elapsed: (): boolean => !!_timer && _elapsed,

    remaining_ticks: (): number => {
        const elapsed = Math.round((Date.now() - _start) * 1000 / TICKS_PER_SECOND);
        return elapsed < _ticks ? _ticks - elapsed : 0;
    },

    elapsed_ticks: (): number => {
        const elapsed = Math.round((Date.now() - _start) * 1000 / TICKS_PER_SECOND);
        return elapsed < _ticks ? elapsed : _ticks;
    },
};
