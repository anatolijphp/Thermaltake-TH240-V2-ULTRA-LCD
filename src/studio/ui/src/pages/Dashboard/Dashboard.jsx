// Dashboard: chips to choose the gauges, the gauges (balanced columns, from the app), the update note and the
// CPU temperature hint that jumps to Settings → Sensors.
import './Dashboard.scss';
import { useEffect, useMemo, useRef } from 'react';
import Card from '../../components/Card/Card.jsx';
import Chip from '../../components/Chip/Chip.jsx';
import Gauge from '../../components/Gauge/Gauge.jsx';
import { jumpTo, send, setCfg, useStore } from '../../bridge.js';
import { throttle } from '../../util.js';

const GAUGES = [
  ['coolant', 'DashWater', 'COOLANT', 'thermo', '#50B4FF'],
  ['cpu_temp', 'DashCpuTemp', 'CPU TEMP', 'thermo', '#FFAA3C'],
  ['cpu_load', 'DashCpuLoad', 'CPU LOAD', 'cpu', '#FF4B6E'],
  ['ram', 'DashRam', 'RAM', 'ram', '#5BE38A'],
];
const CHIPS = [['DashWater', 'Coolant temperature'], ['DashCpuTemp', 'CPU temperature'], ['DashCpuLoad', 'CPU load'], ['DashRam', 'RAM usage']];

export default function Dashboard() {
  const S = useStore((s) => s.S);
  const D = useStore((s) => s.D);
  const grid = useRef(null);
  // the app balances the gauge columns for this width
  const gw = useMemo(() => throttle((w) => send({ t: 'gw', w }), 100), []);
  useEffect(() => {
    const ro = new ResizeObserver(() => gw(grid.current.clientWidth + 14));
    ro.observe(grid.current);
    return () => ro.disconnect();
  }, [gw]);
  const c = S.cfg;
  return (
    <section className="page dashboard">
      <h1>Dashboard</h1>
      <p className="sub">Live readings. Coolant temperature comes from the cooler's own sensor; the rest are read from {S.plat.linux ? 'the system.' : 'Windows.'}</p>
      <Card caption="SENSORS" sub="Choose which gauges the dashboard shows.">
        <div className="wrap">
          {CHIPS.map(([k, label]) => <Chip key={k} on={c[k]} onChange={(v) => setCfg(k, v)}>{label}</Chip>)}
        </div>
      </Card>
      <div className="gauges" ref={grid} style={{ gridTemplateColumns: `repeat(${Math.max(1, (D && D.cols) || 2)}, minmax(0, 1fr))` }}>
        {GAUGES.filter((g) => c[g[1]]).map(([key, , label, icon, color]) => (
          <Card key={key} className="gauge-card">
            <Gauge label={label} icon={icon} color={color} text={D ? D[key].text : '--'} frac={D ? D[key].frac : 0} />
          </Card>
        ))}
      </div>
      <div className="note dash-note">{D ? D.note : ''}</div>
      {D && D.cpu_temp_hint && (
        <div className="note cpu-hint">CPU temperature needs the sensors service:{' '}
          <a href="#" onClick={(e) => { e.preventDefault(); jumpTo('settings', 'sensorsCard'); }}>set it up in Settings → Sensors</a></div>
      )}
    </section>
  );
}
