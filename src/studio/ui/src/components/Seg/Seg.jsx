// Segmented toggle: the options sit in a rounded track and one red pill slides (eased) to the selected option.
import './Seg.scss';
import { useLayoutEffect, useRef, useState } from 'react';
import { useOptimistic } from '../../optimistic.js';

/** options: [[value, label], ...]; values are compared as strings. */
export default function Seg({ value, options, onChange }) {
  const [shown, setShown] = useOptimistic(String(value));
  const track = useRef(null);
  const [pill, setPill] = useState(null); // {x, w, anim}
  const first = useRef(true);

  const place = (anim) => {
    const b = track.current && track.current.querySelector('button.sel');
    if (!b || b.offsetWidth <= 0) { setPill(null); return; }
    setPill({ x: b.offsetLeft, w: b.offsetWidth, anim });
  };
  useLayoutEffect(() => { place(!first.current); first.current = false; }, [shown, options.length]);
  useLayoutEffect(() => {
    const ro = new ResizeObserver(() => place(false));
    ro.observe(track.current);
    return () => ro.disconnect();
  }, []);

  return (
    <div className="seg" ref={track}>
      <i className={'pill' + (pill && pill.anim ? ' anim' : '')}
        style={pill ? { transform: `translateX(${pill.x}px)`, width: pill.w, opacity: 1 } : { opacity: 0 }} />
      {options.map(([v, label]) => (
        <button type="button" key={String(v)} className={String(v) === shown ? 'sel' : ''}
          onClick={() => { if (String(v) !== shown) { setShown(String(v)); onChange(v); } }}>{label}</button>
      ))}
    </div>
  );
}
