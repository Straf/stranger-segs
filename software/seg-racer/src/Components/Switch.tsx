const defaultSize = "50px";

export interface SwitchProps {
    /** The width of the switch's enclosing rectangle. Will be 50px when undefined. */
    width?: string;
    /** The height of the switch's enclosing rectangle. Will be 50px when undefined. */
    height?: string;
    /** The switch color. Will be 'grey' when undefined. */
    color?: string;
    /** Draw the switch down when true, up when false. */
    pressed?: boolean;
    /** Function to call when a mouseDown event is detected on the switch's shape. */
    onPress?: () => void;
    /** Function to call when a mouseUp event is detected on the switch's shape. */
    onRelease?: () => void;
}

const defs = (
    <defs>
        <filter id="switchFrame" filterUnits="userSpaceOnUse" x="-60" y="-60" width="120" height="120">
            <desc>Produces a 3D lighting effect.</desc>
            <feGaussianBlur in="SourceAlpha" stdDeviation="2" result="blur"/>
            <feOffset in="blur" dx="0" dy="2" result="offsetBlur"/>
            <feSpecularLighting in="blur" surfaceScale="5" specularConstant=".75"
                    specularExponent="20" lightingColor="#bbbbbb"
                    result="specOut">
                <fePointLight x="-5000" y="-10000" z="20000"/>
            </feSpecularLighting>
            <feComposite in="specOut" in2="SourceAlpha" operator="in" result="specOut"/>
            <feComposite in="SourceGraphic" in2="specOut" operator="arithmetic"
                k1="0" k2="1" k3="1" k4="0" result="litPaint"/>
            <feMerge>
                <feMergeNode in="offsetBlur"/>
                <feMergeNode in="litPaint"/>
            </feMerge>
        </filter>
        <filter id="switchPressed" filterUnits="userSpaceOnUse" x="-60" y="-60" width="120" height="120">
            <feGaussianBlur in="SourceAlpha" stdDeviation="8" result="blur"></feGaussianBlur>
            <feComposite in2="SourceAlpha" operator="arithmetic" k2="-1" k3="1" result="shadowDiff" />
            <feFlood floodColor="#000" floodOpacity="0.75" />
            <feComposite in2="shadowDiff" operator="in" />
            <feComposite in2="SourceGraphic" operator="over" result="firstfilter" />
            <feGaussianBlur in="firstfilter" stdDeviation="8" result="blur2" />
            <feComposite in2="firstfilter" operator="arithmetic" k2="-1" k3="1" result="shadowDiff" />
            <feFlood floodColor="#000" floodOpacity="0.75" />
            <feComposite in2="shadowDiff" operator="in" />
            <feComposite in2="firstfilter" operator="over" />
        </filter>
    </defs>
);

const Switch = (props: SwitchProps) => {
    const width = props.width ?? defaultSize;
    const height = props.height ?? defaultSize;
    const box = {
        width,
        height,
        viewBox: '-60 -60 120 120',
    }

    const fill = props.color ?? 'grey';
    const colors = {
        fill,
        stroke: 'black',
        strokeWidth: '2',
    }

    const pressed = props.pressed
        ? { filter: 'url(#switchPressed)' }
        : {};

    const handlers = {
        pointerEvents: "visible",
        onMouseDown: props.onPress,
        onMouseUp: props.onRelease,
    };

    const side = props.pressed ? 48 : 50;
    return (
        <svg {...box} >
            {defs}
            <rect x="-55" y="-55" width="110" height="110" filter="url(#switchFrame)"
                fill="grey" stroke="black" />
            <rect x={-side} y={-side} width={side * 2} height={side * 2}
                {...colors} {...pressed} {...handlers} />
        </svg>
    );
}

export default Switch;
