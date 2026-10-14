// Small helpers shared by the components.

/** Calls fn at once, then at most every `ms` while calls keep coming; the last call is always delivered. */
export function throttle(fn, ms) {
  let timer = null, pending = null;
  const run = () => {
    timer = null;
    if (pending) { const a = pending; pending = null; fn(...a); timer = setTimeout(run, ms); }
  };
  return (...a) => { pending = a; if (!timer) run(); };
}

/** 0xRRGGBB -> "#RRGGBB" */
export const hex6 = (v) => '#' + ((v >>> 0) & 0xFFFFFF).toString(16).padStart(6, '0').toUpperCase();

/** seconds -> "m:ss.t" */
export const fmtTime = (s) => Math.floor(s / 60) + ':' + String(Math.floor(s) % 60).padStart(2, '0') + '.' + (Math.floor(s * 10) % 10);

/** class names from an object of flags: cx('a', { b: true, c: false }) -> "a b" */
export function cx(...parts) {
  const out = [];
  for (const p of parts) {
    if (!p) continue;
    if (typeof p === 'string') out.push(p);
    else for (const k of Object.keys(p)) if (p[k]) out.push(k);
  }
  return out.join(' ');
}
