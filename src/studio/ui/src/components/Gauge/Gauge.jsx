// Round 270° gauge: track + value arc (open at the bottom, where the label sits), icon and value in the middle.
import './Gauge.scss';
import Icon from '../Icon/Icon.jsx';

function arc(frac) {
  if (!(frac >= 0.004)) return '';
  frac = Math.min(1, frac);
  const c = 78, r = c - 11 / 2 - 6;
  const a0 = 135 * Math.PI / 180, a1 = (135 + 270 * Math.min(frac, 0.9999)) * Math.PI / 180;
  const p = (a) => (c + r * Math.cos(a)).toFixed(2) + ',' + (c + r * Math.sin(a)).toFixed(2);
  return 'M' + p(a0) + ' A' + r + ',' + r + ' 0 ' + (270 * frac > 180 ? 1 : 0) + ' 1 ' + p(a1);
}

export default function Gauge({ label, icon, color, text, frac }) {
  return (
    <svg className="gauge" viewBox="0 0 156 156">
      <path className="track" d={arc(1)} />
      <path className="arc" d={arc(Math.max(0, Math.min(1, frac)))} stroke={color} style={{ filter: `drop-shadow(0 0 6px ${color})` }} />
      <Icon name={icon} x={70.5} y={34} size={15} color={color} />
      <text className="num" x="78" y="84">{text}</text>
      <text className="glabel" x="78" y="138" fill={color}>{label}</text>
    </svg>
  );
}
