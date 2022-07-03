import { display } from "Game/display";
import { key } from "Game/key";
import { useEffect, useState } from "react";
import GameUi from "./GameUi";

const Game = () => {
    const [pressed, setPressed] = useState(false);
    const [segments, setSegments] = useState('');
    useEffect(() => {
        display.register(setSegments);
        key.setUpdater(setPressed);
        return () => {
            display.unregister();
            key.clearUpdater();
        }
    }, [setPressed, setSegments]);
    return (
        <GameUi
            pressed={pressed}
            segments={segments}
            onPress={() => key.onKeyPress()}
            onRelease={() => key.onKeyRelease()}
        />
    );
}

export default Game;
