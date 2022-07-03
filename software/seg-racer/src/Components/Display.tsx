const defaultSize = '50px';

interface Colors {
    /** Color of top segment. */
    a?: string;
    /** Color of top right segment. */
    b?: string;
    /** Color of bottom right segment. */
    c?: string;
    /** Color of bottom segment. */
    d?: string;
    /** Color of bottom left segment. */
    e?: string;
    /** Color of top left segment. */
    f?: string;
    /** Color of center segment. */
    g?: string;
    /** Color of (decimal) point. */
    p?: string;
 }

export interface DisplayProps {
    /** The width of the display's enclosing rectangle. Will be 50px when undefined. */
    width?: string;
    /** The height of the display's enclosing rectangle. Will be 50px when undefined. */
    height?: string;
    /** The LED color. Will be "red" when undefined. */
    color?: string;
    /** The background (and LED off) color. Will be "black" when undefined. */
    fill?: string;
    /**
     * A string of segment ID to turn on.
     * Examples: 0 = "abcdef", 1 = "bc", 2 = "abdeg", 3 = "abcdg", 4 = "bcfg", 5 = "acdfg", decimal point = "p".
     */
    segments?: string;
}

const Display = (props: DisplayProps) => {
    const width = props.width ?? defaultSize;
    const height = props.height ?? defaultSize;
    const box = {
        width,
        height,
        viewBox: '0 0 210 320',
    }

    const on = props.color ?? 'red';
    const off = props.fill ?? 'black';
    const colors: Colors = {};
    const segments = props.segments ?? '';
    for (let i = 0; i < segments.length; ++i) {
        const c = segments.charAt(i);
        switch (c) {
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'p':
                colors[c] = on;
                break;
        }
    }

    return (<svg {...box}>
        <rect x="0" y="0" width="210" height="320" fill={off} />
        <path d="M45,32L60,17L150,17L165,32L150,47L60,47L45,32Z" fill={colors.a ?? off} />
        <path d="M169,36L184,51L184,141L169,156L154,141L154,51L169,36Z" fill={colors.b ?? off} />
        <path d="M169,164L184,179L184,269L169,284L154,269L154,179L169,164Z" fill={colors.c ?? off} />
        <path d="M165,288L150,303L60,303L45,288L60,273L150,273L165,288Z" fill={colors.d ?? off} />
        <path d="M41,284L26,269L26,179L41,164L56,179L56,269L41,284Z" fill={colors.e ?? off} />
        <path d="M41,156L26,141L26,51L41,36L56,51L56,141L41,156Z" fill={colors.f ?? off} />
        <path d="M45,160L60,145L150,145L165,160L150,175L60,175L45,160Z" fill={colors.g ?? off} />
        <circle cx="190" cy="288" r="10" fill={colors.p ?? off} />
    </svg>);
}

export default Display;
