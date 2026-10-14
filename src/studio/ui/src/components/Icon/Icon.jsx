// Inline SVG icons, 24x24, stroked with currentColor (no icon font: they must look the same on Windows and Linux).
import './Icon.scss';

const F = { fill: 'currentColor', stroke: 'none' };
const ICONS = {
  menu: <path d="M4 6.5h16M4 12h16M4 17.5h16" />,
  dashboard: <><rect x="3" y="3" width="18" height="18" rx="2" /><path d="M6.5 13h2.5l2-5 2.5 9 2-4h2" /></>,
  modes: <><rect x="3.5" y="3.5" width="7" height="7" rx="1" /><rect x="13.5" y="3.5" width="7" height="7" rx="1" /><rect x="3.5" y="13.5" width="7" height="7" rx="1" /><rect x="13.5" y="13.5" width="7" height="7" rx="1" /></>,
  device: <><circle cx="12" cy="12" r="4" /><path d="M12 2.5v2M12 19.5v2M2.5 12h2M19.5 12h2M5.3 5.3l1.4 1.4M17.3 17.3l1.4 1.4M5.3 18.7l1.4-1.4M17.3 6.7l1.4-1.4" /></>,
  settings: <><circle cx="12" cy="12" r="3" /><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 1 1-2.83 2.83l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 1 1-4 0v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 1 1-2.83-2.83l.06-.06A1.65 1.65 0 0 0 4.68 15a1.65 1.65 0 0 0-1.51-1H3a2 2 0 1 1 0-4h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 1 1 2.83-2.83l.06.06A1.65 1.65 0 0 0 9 4.68a1.65 1.65 0 0 0 1-1.51V3a2 2 0 1 1 4 0v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 1 1 2.83 2.83l-.06.06A1.65 1.65 0 0 0 19.4 9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 1 1 0 4h-.09a1.65 1.65 0 0 0-1.51 1z" /></>,
  gamepad: <><path d="M7.5 6h9a5 5 0 0 1 4.9 5.9l-.9 4.6a2.6 2.6 0 0 1-4.5 1.2L14.5 16h-5L8 17.7a2.6 2.6 0 0 1-4.5-1.2l-.9-4.6A5 5 0 0 1 7.5 6z" /><path d="M7 11h3M8.5 9.5v3" /><circle cx="15.5" cy="10.3" r=".6" {...F} /><circle cx="17.3" cy="12.3" r=".6" {...F} /></>,
  minimize: <path d="M5 12h14" />,
  fullscreen: <path d="M4 9V4h5M20 9V4h-5M4 15v5h5M20 15v5h-5" />,
  restore: <path d="M9 4v5H4M15 4v5h5M9 20v-5H4M15 20v-5h5" />,
  close: <path d="M6 6l12 12M18 6L6 18" />,
  power: <><path d="M17.7 6.8a8 8 0 1 1-11.4 0" /><path d="M12 3v8" /></>,
  monitor: <><rect x="2.5" y="4" width="19" height="12.5" rx="1.5" /><path d="M8.5 20.5h7M12 16.5v4" /></>,
  mirror: <><rect x="3" y="9" width="12" height="10" rx="1" /><path d="M8 9V5.5a1 1 0 0 1 1-1h11a1 1 0 0 1 1 1v8.5a1 1 0 0 1-1 1h-5" /></>,
  image: <><rect x="3" y="3" width="18" height="18" rx="2" /><circle cx="8.5" cy="8.5" r="1.8" /><path d="M21 15.5l-5-5L5 21" /></>,
  play: <path d="M7 4.5l12 7.5-12 7.5z" />,
  video: <><rect x="2" y="6" width="13" height="12" rx="2" /><path d="M15 10.5l6-3.5v10l-6-3.5" /></>,
  palette: <><path d="M12 3a9 9 0 1 0 0 18c.9 0 1.5-.7 1.5-1.5 0-.4-.2-.8-.4-1.1-.3-.3-.4-.7-.4-1.1 0-.8.7-1.5 1.5-1.5H16a5 5 0 0 0 5-5c0-4.3-4-7.8-9-7.8z" /><circle cx="7.5" cy="11.5" r="1.1" {...F} /><circle cx="10" cy="7.3" r="1.1" {...F} /><circle cx="14.6" cy="7.3" r="1.1" {...F} /></>,
  clock: <><circle cx="12" cy="12" r="9" /><path d="M12 7v5l3.2 2" /></>,
  // an impeller: hub and four swept blades in a round case
  pump: <><circle cx="12" cy="12" r="10" /><circle cx="12" cy="12" r="2.2" /><path d="M12 9.8c0-3 1.6-5 4.2-5.6M14.2 12c3 0 5 1.6 5.6 4.2M12 14.2c0 3-1.6 5-4.2 5.6M9.8 12c-3 0-5-1.6-5.6-4.2" /></>,
  aura: <path {...F} d="M 12,1 L 23,22 L 18.5,22 L 12,9.5 L 5.5,22 L 1,22 Z M 12,13 L 15.6,20 L 8.4,20 Z" />,
  thermo: <><path d="M14 14.8V4.5a2 2 0 0 0-4 0v10.3a4 4 0 1 0 4 0z" /><path d="M12 9v7.5" /></>,
  cpu: <><rect x="5.5" y="5.5" width="13" height="13" rx="1.5" /><rect x="9" y="9" width="6" height="6" /><path d="M9.5 2.5v3M14.5 2.5v3M9.5 18.5v3M14.5 18.5v3M2.5 9.5h3M2.5 14.5h3M18.5 9.5h3M18.5 14.5h3" /></>,
  ram: <><rect x="2.5" y="7" width="19" height="10" rx="1" /><path d="M6.5 10.5v3M10 10.5v3M14 10.5v3M17.5 10.5v3M5.5 17v2.5M18.5 17v2.5" /></>,
  rotate: <><path d="M20 12a8 8 0 1 1-2.6-5.9" /><path d="M20 3.5v5h-5" /></>,
  check: <path d="M5 12.5l4.5 4.5L19 7.5" />,
  plus: <path d="M12 5v14M5 12h14" />,
  pause: <><rect {...F} x="6" y="4.5" width="4" height="15" rx="1" /><rect {...F} x="14" y="4.5" width="4" height="15" rx="1" /></>,
  resume: <path {...F} d="M7 4.5l13 7.5-13 7.5z" />,
  chevron: <path d="M6 9l6 6 6-6" />,
  x: <path d="M7 7l10 10M17 7L7 17" />,
  drop: <><path d="M12 3v11" /><path d="M7.5 9.5L12 14l4.5-4.5" /><path d="M4 15v3.5A1.5 1.5 0 0 0 5.5 20h13a1.5 1.5 0 0 0 1.5-1.5V15" /></>,
  ban: <><circle cx="12" cy="12" r="9" /><path d="M5.6 5.6l12.8 12.8" /></>,
};

/** <Icon name="clock" /> as an <svg> sized 1em; `nested` draws it inside another SVG at (x, y, size). */
export default function Icon({ name, className, x, y, size, color }) {
  const body = ICONS[name] || null;
  if (size !== undefined) {
    return <svg className="i" x={x} y={y} width={size} height={size} viewBox="0 0 24 24" style={{ color }}>{body}</svg>;
  }
  return <svg className={'i' + (className ? ' ' + className : '')} viewBox="0 0 24 24">{body}</svg>;
}
