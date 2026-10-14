// Drop-down list (fonts, monitors). Items: [{ v, label, font? }]. The open list lives in <body> (cards clip their
// content), placed under the box (or above it when there's no room). Closes on outside press, Escape or page scroll.
import './Combo.scss';
import { useEffect, useLayoutEffect, useRef, useState } from 'react';
import { createPortal } from 'react-dom';
import Icon from '../Icon/Icon.jsx';
import { useOptimistic } from '../../optimistic.js';

export const closeAllCombos = () => window.dispatchEvent(new Event('ttlcd-close-combos'));

export default function Combo({ items, value, onPick }) {
  const [pos, setPos] = useState(null); // open: {left, top, width, up}
  const [shown, setShown] = useOptimistic(value);
  const root = useRef(null);
  const list = useRef(null);
  useEffect(() => {
    const close = () => setPos(null);
    const down = (e) => {
      if (root.current && !root.current.contains(e.target) && !(list.current && list.current.contains(e.target))) setPos(null);
    };
    window.addEventListener('ttlcd-close-combos', close);
    window.addEventListener('resize', close);
    document.addEventListener('pointerdown', down);
    return () => {
      window.removeEventListener('ttlcd-close-combos', close);
      window.removeEventListener('resize', close);
      document.removeEventListener('pointerdown', down);
    };
  }, []);
  useLayoutEffect(() => {
    const sel = pos && list.current && list.current.querySelector('.sel');
    if (sel) list.current.scrollTop = sel.offsetTop - 140;
  }, [pos]);
  const toggle = () => {
    if (pos) { setPos(null); return; }
    closeAllCombos();
    const r = root.current.getBoundingClientRect();
    const up = window.innerHeight - r.bottom < 200 && r.top > window.innerHeight - r.bottom;
    setPos({ left: r.left, width: r.width, top: up ? undefined : r.bottom, bottom: up ? window.innerHeight - r.top : undefined, up });
  };
  const cur = items.find((i) => i.v === shown);
  const fontOf = (it) => (it && it.font ? { fontFamily: `"${it.font}"` } : undefined);
  return (
    <div className={'combo' + (pos ? ' open' : '')} ref={root} onClick={toggle}>
      <div className="cur" style={fontOf(cur)}>{cur ? cur.label : shown == null ? '' : String(shown)}</div>
      <span className="chev"><Icon name="chevron" /></span>
      {pos && createPortal(
        <div className="combo-list" ref={list}
          style={{ left: pos.left, minWidth: pos.width, top: pos.top, bottom: pos.bottom, maxHeight: Math.min(320, (pos.up ? window.innerHeight - pos.bottom : window.innerHeight - pos.top) - 12) }}>
          {items.map((it) => (
            <div key={String(it.v)} className={it.v === shown ? 'sel' : ''} style={fontOf(it)}
              onClick={() => { setPos(null); setShown(it.v); onPick(it.v); }}>{it.label}</div>
          ))}
        </div>, document.body)}
    </div>
  );
}
