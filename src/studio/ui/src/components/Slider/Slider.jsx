// Slider: thin track, gradient fill, diamond thumb. The knob follows the pointer at once; onInput gets the value at
// most every 60 ms while dragging (the app re-renders the cooler frame per value) and always the last one.
// Renders the track and, with `format`, its value cell (two cells of a Row).
import './Slider.scss';
import { useEffect, useRef, useState } from 'react';

const snapTo = (v, min, max, step) => {
  v = Math.max(min, Math.min(max, v));
  return +(Math.round((v - min) / step) * step + min).toFixed(4);
};

export default function Slider({ value, min, max, step = 1, onInput, format, valueId }) {
  const [local, setLocal] = useState(value);
  const st = useRef({ dragging: false, timer: null, pending: false, value });
  const el = useRef(null);
  const v = snapTo(local, min, max, step);

  // follow the app's value, except while the user is moving the knob (no jump back to a stale value)
  useEffect(() => {
    const s = st.current;
    if (!s.dragging && !s.timer) { s.value = value; setLocal(value); }
  }, [value]);

  const flush = () => {
    const s = st.current;
    if (!s.pending) return;
    s.pending = false;
    onInput && onInput(s.value);
  };
  const input = (nv) => {
    const s = st.current;
    nv = snapTo(nv, min, max, step);
    if (nv === s.value) return;
    s.value = nv;
    setLocal(nv);
    s.pending = true;
    if (!s.timer) {
      flush();
      s.timer = setTimeout(() => { s.timer = null; flush(); }, 60);
    }
  };
  const fromX = (x) => {
    const r = el.current.getBoundingClientRect();
    input(min + Math.max(0, Math.min(1, (x - r.left) / Math.max(1, r.width))) * (max - min));
  };
  // wheel needs a non-passive listener to stop the page from scrolling
  useEffect(() => {
    const node = el.current;
    const wheel = (e) => { e.preventDefault(); input(st.current.value + (e.deltaY < 0 ? step : -step)); };
    node.addEventListener('wheel', wheel, { passive: false });
    return () => node.removeEventListener('wheel', wheel);
  });

  const f = max > min ? (v - min) / (max - min) : 0;
  const end = () => { st.current.dragging = false; flush(); };
  return (
    <>
      <div className="slider" ref={el}
        onPointerDown={(e) => { if (e.button !== 0) return; st.current.dragging = true; e.currentTarget.setPointerCapture(e.pointerId); fromX(e.clientX); }}
        onPointerMove={(e) => { if (st.current.dragging) fromX(e.clientX); }}
        onPointerUp={end} onPointerCancel={end} onLostPointerCapture={end}>
        <div className="tr" /><div className="fi" style={{ width: f * 100 + '%' }} /><div className="th" style={{ left: f * 100 + '%' }} />
      </div>
      {format && <span className="val" id={valueId}>{format(v)}</span>}
    </>
  );
}

/** "12 px", "80" — integers plain, others with one decimal */
export const unit = (u) => (v) => (Number.isInteger(v) ? v : v.toFixed(1)) + u;
