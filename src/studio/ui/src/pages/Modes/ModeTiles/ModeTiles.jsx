// Mode tiles: as many columns as fit (~128 px each), balanced so the rows are evenly filled. Only the modes this
// system supports; Aura is greyed out on Linux until armory-crate is installed (unless it's the current mode).
import './ModeTiles.scss';
import { useLayoutEffect, useRef, useState } from 'react';
import Icon from '../../../components/Icon/Icon.jsx';
import { setCfg } from '../../../bridge.js';
import { useOptimistic } from '../../../optimistic.js';

const TILES = [
  ['Off', 'Off', 'power'], ['VirtualDisplay', 'Wallpaper Engine', 'monitor'], ['Monitor', 'Screen mirror', 'mirror'],
  ['Image', 'Image', 'image'], ['Gif', 'GIF', 'play'], ['Video', 'Video', 'video'], ['Color', 'Colour', 'palette'],
  ['Clock', 'Clock', 'clock'], ['Aura', 'Aura Sync', 'aura'], ['Pump', 'Pump', 'pump'],
];

export default function ModeTiles({ modes, mode, auraReady }) {
  const [shown, setShown] = useOptimistic(mode);
  const host = useRef(null);
  const [cols, setCols] = useState(3);
  const tiles = TILES.filter((t) => modes.includes(t[0]));
  useLayoutEffect(() => {
    const layout = () => {
      const n = tiles.length, w = host.current.clientWidth;
      if (!n || w <= 0) return;
      let c = Math.max(3, Math.min(n, Math.floor((w + 10) / 128)));
      const rows = Math.ceil(n / c);
      setCols(Math.ceil(n / rows));
    };
    layout();
    const ro = new ResizeObserver(layout);
    ro.observe(host.current);
    return () => ro.disconnect();
  }, [tiles.length]);
  return (
    <div className="tiles" ref={host} style={{ gridTemplateColumns: `repeat(${cols}, minmax(0, 1fr))` }}>
      {tiles.map(([m, label, icon]) => {
        const off = m === 'Aura' && !auraReady;
        return (
          <button type="button" key={m} className={'tile' + (m === shown ? ' sel' : '') + (off && mode !== 'Aura' ? ' disabled' : '')}
            title={off ? 'Needs armory-crate (the Linux Aura Sync service). Install it and enable armory-crated' : ''}
            onClick={() => { if (m !== shown) { setShown(m); setCfg('Mode', m); } }}>
            <span className="ic"><Icon name={icon} /></span><span className="tx">{label}</span>
          </button>
        );
      })}
    </div>
  );
}
