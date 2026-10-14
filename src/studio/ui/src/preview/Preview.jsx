// Preview column: the round LCD inside a rotating RGB ring. Live = what is streamed now (turned by -mount, so it
// shows the LCD as you see it), with pause/play on hover and the connection card. Idle = the picture the cooler
// shows by itself (stored in its flash): choose, drag/zoom to place, upload with read-back, or read it back.
import './Preview.scss';
import { useEffect, useMemo, useRef } from 'react';
import Button from '../components/Button/Button.jsx';
import Icon from '../components/Icon/Icon.jsx';
import Progress from '../components/Progress/Progress.jsx';
import Slider from '../components/Slider/Slider.jsx';
import Switch from '../components/Switch/Switch.jsx';
import { onFrame, send, useStore } from '../bridge.js';
import { throttle } from '../util.js';

/** Live frames: one request at a time, always the newest (frames arrive up to 25 per second). */
function LiveImage({ mount, hidden }) {
  const img = useRef(null);
  useEffect(() => {
    let loading = false, want = 0, shown = 0;
    const load = () => {
      if (loading || want === shown || !img.current) return;
      loading = true;
      const seq = want;
      img.current.onload = img.current.onerror = () => { loading = false; shown = seq; load(); };
      img.current.src = 'frame/' + seq + '.jpg';
    };
    onFrame((seq) => { want = seq; load(); });
    return () => onFrame(null);
  }, []);
  return <img ref={img} className="disc live" alt="" hidden={hidden} style={{ transform: `rotate(${-mount}deg)` }} />;
}

function IdleHit({ editing, zoom }) {
  const hit = useRef(null);
  const last = useRef(null);
  const pan = useRef({ dx: 0, dy: 0 });
  // moves add up and go to the app at most every 40 ms
  const sendPan = useMemo(() => throttle(() => {
    const p = pan.current;
    if (p.dx || p.dy) { send({ t: 'idle', a: 'pan', dx: p.dx, dy: p.dy }); pan.current = { dx: 0, dy: 0 }; }
  }, 40), []);
  const zoomRef = useRef(zoom);
  zoomRef.current = zoom;
  useEffect(() => {
    const node = hit.current;
    const wheel = (e) => {
      e.preventDefault();
      if (!node.classList.contains('editing')) return;
      const z = Math.max(1, Math.min(5, zoomRef.current * (e.deltaY < 0 ? 1.1 : 1 / 1.1)));
      zoomRef.current = z;
      send({ t: 'idle', a: 'zoom', v: z });
    };
    node.addEventListener('wheel', wheel, { passive: false });
    return () => node.removeEventListener('wheel', wheel);
  }, []);
  return (
    <div ref={hit} className={'disc idle-hit' + (editing ? ' editing' : '')}
      onPointerDown={(e) => { if (!editing || e.button !== 0) return; last.current = { x: e.clientX, y: e.clientY }; e.currentTarget.setPointerCapture(e.pointerId); }}
      onPointerMove={(e) => {
        if (!last.current) return;
        const k = 480 / Math.max(1, hit.current.clientWidth);
        pan.current.dx += (e.clientX - last.current.x) * k; pan.current.dy += (e.clientY - last.current.y) * k;
        last.current = { x: e.clientX, y: e.clientY };
        sendPan();
      }}
      onPointerUp={() => { last.current = null; }} onLostPointerCapture={() => { last.current = null; }} />
  );
}

export default function Preview() {
  const S = useStore((s) => s.S);
  const ST = useStore((s) => s.ST);
  const I = useStore((s) => s.I);
  const dnd = useStore((s) => s.dnd);
  const view = !!(I && I.view);
  const mount = S ? S.mount : 0;
  const busy = !!(I && I.busy);
  const p = I ? Math.max(0, Math.min(100, I.progress)) : 0;
  const ringLen = 2 * Math.PI * 168;
  const col = ST && ST.connected ? '#2BE36B' : '#FF1F4B';
  return (
    <aside className="preview">
      <div className="caption pvtitle">{view ? 'IDLE IMAGE' : 'LIVE PREVIEW'}</div>
      <div className="pvinner">
        <div className="pvtop">
          <Switch on={view} onChange={(v) => send({ t: 'idle', a: 'view', v })} title="Show and change the picture the cooler displays by itself">Idle</Switch>
          <button type="button" className="rotate" title="Rotate 90° (saved; turns the live picture and the idle screen)" onClick={() => send({ t: 'rotate' })}>
            <Icon name="rotate" /></button>
        </div>
        <div className="lcd">
          <div className="glow" /><div className="ring" />
          <div className="screen" />
          <LiveImage mount={mount} hidden={view} />
          {view && I.img != null && <img className="disc" alt="" src={'idle/' + I.img + '.jpg'} style={{ transform: `rotate(${I.angle}deg)` }} />}
          {/* upload / read progress around the preview, from the top, clockwise */}
          <svg className={'idle-ring' + (busy && p > 0 ? ' on' : '')} width="344" height="344" viewBox="0 0 344 344">
            <circle cx="172" cy="172" r="168" style={{ strokeDasharray: `${busy && p > 0 ? (p >= 100 ? ringLen + 1 : ringLen * p / 100) : 0} 1100` }} />
          </svg>
          {view && ST && ST.idleTemp != null && (
            <div className="idle-temp"><span>{ST.idleTemp}</span><span className="deg">°</span><span>C</span></div>
          )}
          {view && I.img == null && <div className="idle-empty">No idle image yet</div>}
          {!view && (
            <div className={'disc pv-hover' + (ST && ST.paused ? ' paused' : '')} title={ST && ST.paused ? 'Resume' : 'Pause'} onClick={() => send({ t: 'pause' })}>
              <div className="pause-btn"><Icon name={ST && ST.paused ? 'resume' : 'pause'} /></div>
            </div>
          )}
          {view && <IdleHit editing={I.editing} zoom={I.zoom} />}
          {dnd.active && (
            <div className={'disc drop-zone' + (dnd.ok ? '' : ' bad')}>
              <div className="drop-ic"><Icon name={dnd.ok ? 'drop' : 'ban'} /></div>
              <div className="drop-label">{dnd.label}</div>
              {dnd.name && <div className="drop-name">{dnd.name}</div>}
            </div>
          )}
        </div>

        {!view && (
          <div className="pcard">
            <div className="connrow"><span className="conn-dot" style={{ background: col, boxShadow: '0 0 10px ' + col }} />
              <span className="conn-text">{ST ? ST.conn : ''}</span><span className="fps">{ST ? ST.fps : ''}</span></div>
            <div className="mode-text">{ST ? ST.modeText : ''}</div>
          </div>
        )}

        {view && (
          <div>
            <div className="pcard">
              <div className="strong">{I.title}</div>
              <div className="note">The cooler shows this by itself when the PC isn't driving the screen (boot, sleep, powered off).</div>
              {I.editing && (
                <div>
                  <div className="zoomrow"><span className="lbl">Zoom</span>
                    <Slider value={I.zoom} min={1} max={5} step={0.01} onInput={(v) => send({ t: 'idle', a: 'zoom', v })} format={(v) => v.toFixed(1) + '×'} /></div>
                  <div className="status idle-info">{I.info}</div>
                </div>
              )}
              {busy && <Progress cyan frac={p / 100} />}
              <div className="note idle-status">{I.status}</div>
            </div>
            <div className="wrap idle-btns">
              <Button disabled={busy} onClick={() => send({ t: 'idle', a: 'choose' })}>Choose image…</Button>
              {!I.editing && <Button disabled={busy} onClick={() => send({ t: 'idle', a: 'read' })}>Read from cooler</Button>}
              {I.editing && <Button accent disabled={!I.fits || busy} onClick={() => send({ t: 'idle', a: 'store' })}>Upload to cooler</Button>}
              {I.editing && <Button disabled={busy} onClick={() => send({ t: 'idle', a: 'cancel' })}>Cancel</Button>}
            </div>
          </div>
        )}
      </div>
    </aside>
  );
}
