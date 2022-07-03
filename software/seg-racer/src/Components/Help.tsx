import { useState } from "react";
import styles from "./Help.module.css";

const Arrow = ({show}: {show?: boolean}) => {
    const arrow = show ? String.fromCharCode(0x025BC) : String.fromCharCode(0x025B6);
    return (
        <span className={styles.arrow}>{arrow}</span>
    );
};

const Help = () => {
    const [show, setShow] = useState(false);
    const classNames = [styles.text];
    if (!show) classNames.push(styles.hidden);

    return (
        <div className={styles.help}>
            <h2 className={styles.h2} onClick={() => setShow(!show)}>
                <Arrow show={show} /> How to play
            </h2>
            <div className={classNames.join(' ')}>
                <p className={styles.p}>The lit up segment is your car.<br/>
                The car loops in the circuit.<br/>
                When you press the key or the spacebar, the car tries to steer into the middle segment.<br />
                The car may steer only when it's just before the middle segment (consider the driving direction).<br />
                If you steer at the right moment, your score increases.<br />
                If you steer at the wrong moment, the car crashes.<br />
                You have only three cars until the game is over.</p>
                <p className={styles.p}>Press the key or the spacebar to play.</p>
            </div>
        </div>
    );
};

export default Help;
