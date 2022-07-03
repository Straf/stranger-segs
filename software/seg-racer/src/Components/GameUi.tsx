import { useEffect, useRef } from "react";
import Display, { DisplayProps } from "./Display";
import styles from "./GameUi.module.css";
import Help from "./Help";
import Switch, { SwitchProps } from "./Switch";

export type GameUiProps =
    Pick<DisplayProps, 'segments'> &
    Pick<SwitchProps, 'onPress' | 'onRelease' | 'pressed'>;

const GameUi = ({ pressed, segments, onPress, onRelease }: GameUiProps) => {
    const ref = useRef<HTMLDivElement>(null);
    useEffect(() => ref.current?.focus());

    return (
        <div ref={ref}
             className={`hidden-focus ${styles.column}`}
             tabIndex={-1}
             onKeyDown={(event) => {
                if (event.key === ' ') onPress?.();
             }}
             onKeyUp={(event) => {
                if (event.key === ' ') onRelease?.();
             }}>
            <h1 className={styles.h1}>Seg Racer</h1>
            <p>Emulation of a minimalistic electronic game.</p>
            <div className={styles.game}>
                <Display width="150px" height="150px" segments={segments} />
                <Switch pressed={pressed} onPress={onPress} onRelease={onRelease} />
            </div>
            <Help />
        </div>
    );
}

export default GameUi;
