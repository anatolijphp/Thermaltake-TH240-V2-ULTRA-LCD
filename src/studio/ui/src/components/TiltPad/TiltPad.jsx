// TiltPad: picks a direction and an angle at once, up to `max` degrees from straight down. The dot is where the eye
// is: drag it (or click) inside the ring; the centre is straight down, the ring is `max`. Like Slider, the dot
// follows the pointer at once and onInput(x, y) gets the value at most every 60 ms while dragging (the app
// re-renders the cooler frame per value), and always the last one. Double-click goes back to straight down.
import './TiltPad.scss';
import { useEffect, useRef, useState } from 'react';

const clamp = (x, y, max) => {
  const m = Math.hypot(x, y);
  return m > max ? [(x * max) / m, (y * max) / m] : [x, y];
};
const round = ([x, y]) => [Math.round(x), Math.round(y)];

/** "Straight down", "20° right", "15° left, 10° down" */
export const describe = (x, y) => {
  const part = (v, pos, neg) => (v ? `${Math.abs(v)}° ${v > 0 ? pos : neg}` : null);
  const parts = [part(x, 'right', 'left'), part(y, 'down', 'up')].filter(Boolean);
  return parts.length ? parts.join(', ') : 'Straight down';
};

export default function TiltPad({ x, y, max = 30, onInput, id }) {
  const [local, setLocal] = useState(() => round(clamp(x, y, max)));
  const st = useRef({ dragging: false, timer: null, pending: false, value: [x, y] });
  const el = useRef(null);

  // follow the app's value, except while the user is moving the dot
  useEffect(() => {
    const s = st.current;
    if (!s.dragging && !s.timer) { s.value = round(clamp(x, y, max)); setLocal(s.value); }
  }, [x, y, max]);

  const flush = () => {
    const s = st.current;
    if (!s.pending) return;
    s.pending = false;
    onInput && onInput(s.value[0], s.value[1]);
  };
  const input = (v) => {
    const s = st.current;
    v = round(clamp(v[0], v[1], max));
    if (v[0] === s.value[0] && v[1] === s.value[1]) return;
    s.value = v;
    setLocal(v);
    s.pending = true;
    if (!s.timer) {
      flush();
      s.timer = setTimeout(() => { s.timer = null; flush(); }, 60);
    }
  };
  const fromPoint = (cx, cy) => {
    const r = el.current.getBoundingClientRect();
    const half = Math.max(1, r.width / 2);
    input([((cx - r.left - half) / half) * max, ((cy - r.top - half) / half) * max]);
  };
  const end = () => { st.current.dragging = false; flush(); };
  const [lx, ly] = local;
  const at = (v) => 50 + (v / max) * 50 + '%';
  return (
    <div className="tiltpad">
      <div className="pad" ref={el} id={id} title="Drag to tilt the view; double-click for straight down"
        onPointerDown={(e) => { if (e.button !== 0) return; st.current.dragging = true; e.currentTarget.setPointerCapture(e.pointerId); fromPoint(e.clientX, e.clientY); }}
        onPointerMove={(e) => { if (st.current.dragging) fromPoint(e.clientX, e.clientY); }}
        onPointerUp={end} onPointerCancel={end} onLostPointerCapture={end}
        onDoubleClick={() => input([0, 0])}>
        <div className="ring half" />
        <div className="cross h" /><div className="cross v" />
        <div className="line" style={{ width: Math.hypot(lx, ly) / max * 50 + '%', transform: `rotate(${Math.atan2(ly, lx)}rad)` }} />
        <div className="dot" style={{ left: at(lx), top: at(ly) }} />
      </div>
      <span className="val">{describe(lx, ly)}</span>
    </div>
  );
}
