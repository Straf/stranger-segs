/* 
 * File:   display.ts
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

type DisplayUpdater = (segments: string) => void;

let update: DisplayUpdater | undefined = undefined;

const digits: string[] = [
    'abcdef',   // 0
    'bc',       // 1
    'abdeg',    // 2
    'abcdg',    // 3
    'bcfg',     // 4
    'acdfg',    // 5
    'acdefg',   // 6
    'abc',      // 7
    'abcdefg',  // 8
    'abcdfg',   // 9
];

const lower: string[] = [
    'abcefg',   // A
    'cdefg',    // b
    'deg',      // c
    'bcdeg',    // d
    'adefg',    // E
    'aefg',     // F
    'acdef',    // G
    'cefg',     // h
    'e',        // i
    'bcde',     // J
    '',         // k
    'def',      // L
    'abcef',    // M
    'ceg',      // n
    'cdeg',     // o
    'abefg',    // P
    'abcfg',    // q
    'eg',       // r
    'acdfg',    // S
    'defg',     // t
    'cde',      // u
    '',         // v
    '',         // w
    '',         // x
    'bcdfg',    // y
    '',         // z
];

const upper: string[] = [
    'abcefg',   // A
    'cdefg',    // b
    'adef',     // C
    'bcdeg',    // d
    'adefg',    // E
    'aefg',     // F
    'acdef',    // G
    'bcefg',    // H
    'ef',       // I
    'bcde',     // J
    '',         // k
    'def',      // L
    'abcef',    // M
    'ceg',      // n
    'abcdef',   // O
    'abefg',    // P
    'abcfg',    // q
    'eg',       // r
    'acdfg',    // S
    'defg',     // t
    'bcdef',    // U
    '',         // v
    '',         // w
    '',         // x
    'bcdfg',    // y
    '',         // z
];

const codeDigit0 = '0'.charCodeAt(0);
const codeDigit9 = '9'.charCodeAt(0);
const codeLowerA = 'a'.charCodeAt(0);
const codeLowerZ = 'z'.charCodeAt(0);
const codeUpperA = 'A'.charCodeAt(0);
const codeUpperZ = 'Z'.charCodeAt(0);

export const display = {

    register: (handler: DisplayUpdater): void => {
        update = handler;
    },

    unregister: (): void => {
        update = undefined;
    },

    off: (): void => update?.(''),

    show_char(char: string) {
        display.show_segments( display.char_to_segs(char) );
    },

    show_segments: (segments: string): void => {
        update?.(segments);
    },

    char_to_segs: (char: string): string => {
        const code = char.charCodeAt(0);
        if (codeDigit0 <= code && code <= codeDigit9) {
            return digits[code - codeDigit0];
        } else if (codeUpperA <= code && code <= codeUpperZ) {
            return upper[code - codeUpperA];
        } else if (codeLowerA <= code && code <= codeLowerZ) {
            return lower[code - codeLowerA];
        } else switch (char) {
            case '-': return 'g';
            case '_': return 'd';
            case '.': return 'p';
        }
        return '';
    }

};
