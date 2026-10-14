// Title bar of the frameless window: logo with the red slash, centred status, game mode toggle, minimise, full screen,
// close, and the flowing RGB line. Dragging it moves the window; a double click toggles full screen.
import './TitleBar.scss';
import Icon from '../Icon/Icon.jsx';
import { send, useStore } from '../../bridge.js';
import { useOptimistic } from '../../optimistic.js';

export default function TitleBar() {
  const ST = useStore((s) => s.ST);
  const [game, setGame] = useOptimistic(!!(ST && ST.forceGame));
  const full = !!(ST && ST.full);
  const down = (e) => {
    if (e.button !== 0 || e.target.closest('.tb-btns')) return;
    send({ t: 'win', a: e.detail === 2 ? 'full' : 'drag' });
  };
  return (
    <header className="titlebar" onMouseDown={down}>
      <svg className="logo-bg" width="270" height="48" aria-hidden="true">
        <polygon points="0,0 252,0 228,48 0,48" fill="#0D0D0E" />
        <polygon className="slash" points="252,0 264,0 240,48 228,48" fill="#FF1F4B" />
      </svg>
      <div className="logo"><b>TT</b><span>LCD STUDIO</span></div>
      <div className="title-status">{ST ? ST.title : ''}</div>
      <div className="tb-btns">
        <button type="button" className={'tb tbtoggle' + (game ? ' on' : '')} title="Game mode: force on (for games that aren't detected)"
          onClick={() => { setGame(!game); send({ t: 'game', v: !game }); }}><Icon name="gamepad" /></button>
        <button type="button" className="tb" title="Minimise" onClick={() => send({ t: 'win', a: 'min' })}><Icon name="minimize" /></button>
        <button type="button" className="tb" title={full ? 'Exit full screen (F11 / Esc)' : 'Full screen (F11)'} onClick={() => send({ t: 'win', a: 'full' })}>
          <Icon name={full ? 'restore' : 'fullscreen'} /></button>
        <button type="button" className="tb" title="Close" onClick={() => send({ t: 'win', a: 'close' })}><Icon name="close" /></button>
      </div>
      <div className="rgbline" />
    </header>
  );
}
