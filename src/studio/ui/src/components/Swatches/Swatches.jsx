// Round colour swatches from the palette plus a rainbow one that opens the system colour picker (the colour follows
// while dragging in the picker, at most every 60 ms).
import './Swatches.scss';
import { useMemo, useRef } from 'react';
import { hex6, throttle } from '../../util.js';
import { useOptimistic } from '../../optimistic.js';

export default function Swatches({ palette, value, onPick }) {
  const [shown, setShown] = useOptimistic(value);
  const pick = useRef(onPick);
  pick.current = onPick;
  const pickT = useMemo(() => throttle((v) => pick.current(v), 60), []);
  const inPalette = palette.includes(shown);
  return (
    <div className="swatches">
      {palette.map((v) => (
        <button type="button" key={v} className={'sw' + (v === shown ? ' cur' : '')} style={{ background: hex6(v) }} title={hex6(v)}
          onClick={() => { setShown(v); onPick(v); }} />
      ))}
      <span className={'sw custom' + (inPalette ? '' : ' cur')} title="Custom colour…">
        <input type="color" value={hex6(shown).toLowerCase()}
          onChange={(e) => { const v = parseInt(e.target.value.slice(1), 16); setShown(v); pickT(v); }} />
      </span>
    </div>
  );
}
