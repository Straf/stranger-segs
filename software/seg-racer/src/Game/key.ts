/*
 * File:   key.ts
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

export type KeyListener = (state: boolean) => void;
export type KeyUpdater = (state: boolean) => void;
const none = () => {};

let _updater: KeyUpdater = none;
let listeners: KeyListener[] = [];
let state: boolean = false;

export const key = {

    setUpdater: (updater: KeyUpdater) => {
        _updater = updater;
    },

    clearUpdater: () => {
        _updater = none;
    },

    register: (listener: KeyListener): void => {
        listeners = [...listeners, listener];
    },

    unregister: (listener: KeyListener): void => {
        listeners = listeners.filter((item) => item !== listener);
    },

    onKeyPress: (): void => {
        state = true;
        _updater?.(true);
        listeners.forEach((item) => item(true));
    },

    onKeyRelease: (): void => {
        state = false;
        _updater?.(false);
        listeners.forEach((item) => item(false));
    },

    pressed: (): boolean => state,
    released: (): boolean => !state,
};
