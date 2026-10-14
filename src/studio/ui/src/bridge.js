// The link to the Rust side. Messages go out as JSON through window.ipc.postMessage; the Rust side calls
// window.ttlcd.* with the state, status, dashboard, idle view and so on, which land in a small store that
// components read with useStore(selector). Live frames (25 per second) bypass the store: see onFrame.
import { useSyncExternalStore } from 'react';

export const send = (msg) => { try { window.ipc.postMessage(JSON.stringify(msg)); } catch (e) { /* not in the app */ } };

let snap = {
  S: null,            // full state (settings, panels, lists, platform)
  ST: null,           // status, every 500 ms
  D: null,            // dashboard readings, every second
  I: null,            // idle view
  fonts: [],
  drags: { Clock: null, Overlay: null }, // LineDrag while a reading is dragged
  video: { duration: 0, info: '', thumb: null },
  alert: null,
  page: 'dashboard',
  dnd: { active: false }, // a file dragged over the window: {active, ok, target: 'mode'|'idle', label, name}
};
const listeners = new Set();
export function update(patch) {
  snap = { ...snap, ...(typeof patch === 'function' ? patch(snap) : patch) };
  listeners.forEach((l) => l());
}
const subscribe = (l) => { listeners.add(l); return () => listeners.delete(l); };
/** Re-renders when the selected part of the store changes (compare by identity). */
export const useStore = (sel) => useSyncExternalStore(subscribe, () => sel(snap));
export const getStore = () => snap;

let frameHandler = null;
/** The live preview registers here; called with each new frame number. */
export const onFrame = (fn) => { frameHandler = fn; };

export function showPage(name) {
  update({ page: name });
  send({ t: 'nav', page: name });
}

window.addEventListener('error', (e) => send({ t: 'jserr', m: e.message + ' @' + e.lineno }));

// the API the Rust side calls
window.ttlcd = {
  fonts: (list) => update({ fonts: list }),
  state: (s) => update({ S: s }),
  status: (st) => update({ ST: st }),
  dash: (d) => update({ D: d }),
  idle: (i) => update({ I: i }),
  frame: (seq) => frameHandler && frameHandler(seq),
  drag: (kind, d) => update((s) => ({ drags: { ...s.drags, [kind]: d } })),
  video: (v) => update((s) => ({ video: { ...s.video, ...v } })),
  alert: (text) => update({ alert: text }),
  dnd: (d) => update({ dnd: d }),
  page: (name) => showPage(name),
};

/** Change one setting / one style field of the selected reading. */
export const setCfg = (k, v) => send({ t: 'set', k, v });
export const setStyle = (k, v) => send({ t: 'style', k, v });

/** Show a page and flash one of its cards (`id`), e.g. the dashboard's hint jumping to Settings → Sensors. */
export function jumpTo(page, id) {
  showPage(page);
  update({ flash: { id, at: Date.now() } });
}
