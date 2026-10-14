// The window: title bar, navigation, the current page, the preview column; the frameless window's resize edges,
// keys (F11 full screen, Escape) and the message dialog.
import './App.scss';
import { useEffect, useRef, useState } from 'react';
import TitleBar from './components/TitleBar/TitleBar.jsx';
import Nav from './components/Nav/Nav.jsx';
import Dialog from './components/Dialog/Dialog.jsx';
import ResizeEdges from './components/ResizeEdges/ResizeEdges.jsx';
import { closeAllCombos } from './components/Combo/Combo.jsx';
import Dashboard from './pages/Dashboard/Dashboard.jsx';
import Modes from './pages/Modes/Modes.jsx';
import Device from './pages/Device/Device.jsx';
import Settings from './pages/Settings/Settings.jsx';
import Preview from './preview/Preview.jsx';
import { getStore, send, update, useStore } from './bridge.js';

const PAGES = { dashboard: Dashboard, modes: Modes, device: Device, settings: Settings };

/** The page area; its thin scrollbar shows while scrolling (and on hover). */
function Pages() {
  const page = useStore((s) => s.page);
  const ready = useStore((s) => !!s.S);
  const el = useRef(null);
  const [scrolling, setScrolling] = useState(false);
  const timer = useRef(0);
  useEffect(() => { el.current.scrollTop = 0; }, [page]);
  const Page = PAGES[page] || Dashboard;
  return (
    <div className={'pages' + (scrolling ? ' scrolling' : '')} ref={el}
      onScroll={() => { setScrolling(true); clearTimeout(timer.current); timer.current = setTimeout(() => setScrolling(false), 1750); closeAllCombos(); }}>
      {ready && <Page />}
    </div>
  );
}

export default function App() {
  const navcol = useStore((s) => !!(s.S && s.S.cfg.NavCollapsed));
  const full = useStore((s) => !!(s.ST && s.ST.full));
  const alert = useStore((s) => s.alert);
  const dragging = useStore((s) => s.dnd.active);
  useEffect(() => {
    const key = (e) => {
      if (e.key === 'F11') { e.preventDefault(); send({ t: 'win', a: 'full' }); }
      else if (e.key === 'Escape') {
        const s = getStore();
        if (s.alert != null) update({ alert: null });
        else if (s.ST && s.ST.full) send({ t: 'win', a: 'full' });
        closeAllCombos();
      }
    };
    const menu = (e) => { if (!window.TTLCD_DEVTOOLS && !e.target.closest('input')) e.preventDefault(); };
    document.addEventListener('keydown', key);
    document.addEventListener('contextmenu', menu);
    return () => { document.removeEventListener('keydown', key); document.removeEventListener('contextmenu', menu); };
  }, []);
  return (
    // while a file is dragged over the window everything but the preview greys out (the drop target is the preview)
    <div className={'app' + (navcol ? ' navcol' : '') + (full ? ' full' : '') + (dragging ? ' dragging-file' : '')}>
      <TitleBar />
      <div className="main">
        <div className="bloom" />
        <Nav />
        <Pages />
        <Preview />
      </div>
      {!full && <ResizeEdges />}
      <Dialog text={alert} onClose={() => update({ alert: null })} />
    </div>
  );
}
