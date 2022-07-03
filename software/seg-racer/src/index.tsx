import Game from 'Components/Game';
import { game } from "Game/game";
import { StrictMode } from 'react';
import { createRoot } from 'react-dom/client';
import './index.css';
import * as serviceWorker from './serviceWorker';

const root = createRoot(document.getElementById('root')!);
root.render(
    <StrictMode>
        <Game />
    </StrictMode>,
)

// If you want your app to work offline and load faster, you can change
// unregister() to register() below. Note this comes with some pitfalls.
// Learn more about service workers: https://bit.ly/CRA-PWA
serviceWorker.unregister();

setTimeout(() => {
    game.run();
}, 1000);
