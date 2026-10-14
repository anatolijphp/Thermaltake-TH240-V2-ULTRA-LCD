// Navigation: full width with names, or an icon rail (the names fade). Footer: model and serial.
import './Nav.scss';
import Icon from '../Icon/Icon.jsx';
import { send, showPage, useStore } from '../../bridge.js';

const PAGES = [['dashboard', 'Dashboard'], ['modes', 'Display modes'], ['device', 'Device'], ['settings', 'Settings']];

export default function Nav() {
  const page = useStore((s) => s.page);
  const collapsed = useStore((s) => !!(s.S && s.S.cfg.NavCollapsed));
  const serial = useStore((s) => (s.ST ? s.ST.navSerial : ''));
  return (
    <nav className="nav">
      <button type="button" className="iconbtn nav-toggle" title={collapsed ? 'Expand menu' : 'Collapse menu'}
        onClick={() => send({ t: 'set', k: 'NavCollapsed', v: !collapsed })}><Icon name="menu" /></button>
      {PAGES.map(([p, label]) => (
        <button type="button" key={p} className={'navitem' + (p === page ? ' sel' : '')} title={label} onClick={() => showPage(p)}>
          <i className="bar" /><span className="ic"><Icon name={p === 'modes' ? 'modes' : p} /></span><span className="tx">{label}</span>
        </button>
      ))}
      <div className="nav-footer">
        <div className="rgbtick" />
        <div>TH240 V2 Ultra · CL-W383</div>
        <div>{serial}</div>
      </div>
    </nav>
  );
}
