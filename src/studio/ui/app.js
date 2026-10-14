// TT LCD Studio page: draws what the Rust side sends (window.ttlcd.*) and posts the user's changes back as JSON
// through window.ipc.postMessage. No frameworks; the same page runs in WebView2 (Windows) and WebKitGTK (Linux).
'use strict';

const $ = (id) => document.getElementById(id);
const $$ = (sel, root) => Array.from((root || document).querySelectorAll(sel));
window.addEventListener('error', (e) => send({ t: 'jserr', m: e.message + ' @' + e.lineno }));
const send = (msg) => { try { window.ipc.postMessage(JSON.stringify(msg)); } catch (e) { /* not in the app */ } };
const parseV = (s) => { try { return JSON.parse(s); } catch (e) { return s; } };
const hex6 = (v) => '#' + ((v >>> 0) & 0xFFFFFF).toString(16).padStart(6, '0').toUpperCase();

// ---------------- icons (inline SVG, 24x24, stroked with currentColor) ----------------
const ICONS = {
  menu: '<path d="M4 6.5h16M4 12h16M4 17.5h16"/>',
  dashboard: '<rect x="3" y="3" width="18" height="18" rx="2"/><path d="M6.5 13h2.5l2-5 2.5 9 2-4h2"/>',
  modes: '<rect x="3.5" y="3.5" width="7" height="7" rx="1"/><rect x="13.5" y="3.5" width="7" height="7" rx="1"/><rect x="3.5" y="13.5" width="7" height="7" rx="1"/><rect x="13.5" y="13.5" width="7" height="7" rx="1"/>',
  device: '<circle cx="12" cy="12" r="4"/><path d="M12 2.5v2M12 19.5v2M2.5 12h2M19.5 12h2M5.3 5.3l1.4 1.4M17.3 17.3l1.4 1.4M5.3 18.7l1.4-1.4M17.3 6.7l1.4-1.4"/>',
  settings: '<circle cx="12" cy="12" r="3"/><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 1 1-2.83 2.83l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 1 1-4 0v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 1 1-2.83-2.83l.06-.06A1.65 1.65 0 0 0 4.68 15a1.65 1.65 0 0 0-1.51-1H3a2 2 0 1 1 0-4h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 1 1 2.83-2.83l.06.06A1.65 1.65 0 0 0 9 4.68a1.65 1.65 0 0 0 1-1.51V3a2 2 0 1 1 4 0v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 1 1 2.83 2.83l-.06.06A1.65 1.65 0 0 0 19.4 9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 1 1 0 4h-.09a1.65 1.65 0 0 0-1.51 1z"/>',
  gamepad: '<path d="M7.5 6h9a5 5 0 0 1 4.9 5.9l-.9 4.6a2.6 2.6 0 0 1-4.5 1.2L14.5 16h-5L8 17.7a2.6 2.6 0 0 1-4.5-1.2l-.9-4.6A5 5 0 0 1 7.5 6z"/><path d="M7 11h3M8.5 9.5v3"/><circle cx="15.5" cy="10.3" r=".6" fill="currentColor"/><circle cx="17.3" cy="12.3" r=".6" fill="currentColor"/>',
  minimize: '<path d="M5 12h14"/>',
  fullscreen: '<path d="M4 9V4h5M20 9V4h-5M4 15v5h5M20 15v5h-5"/>',
  restore: '<path d="M9 4v5H4M15 4v5h5M9 20v-5H4M15 20v-5h5"/>',
  close: '<path d="M6 6l12 12M18 6L6 18"/>',
  power: '<path d="M17.7 6.8a8 8 0 1 1-11.4 0"/><path d="M12 3v8"/>',
  monitor: '<rect x="2.5" y="4" width="19" height="12.5" rx="1.5"/><path d="M8.5 20.5h7M12 16.5v4"/>',
  mirror: '<rect x="3" y="9" width="12" height="10" rx="1"/><path d="M8 9V5.5a1 1 0 0 1 1-1h11a1 1 0 0 1 1 1v8.5a1 1 0 0 1-1 1h-5"/>',
  image: '<rect x="3" y="3" width="18" height="18" rx="2"/><circle cx="8.5" cy="8.5" r="1.8"/><path d="M21 15.5l-5-5L5 21"/>',
  play: '<path d="M7 4.5l12 7.5-12 7.5z"/>',
  video: '<rect x="2" y="6" width="13" height="12" rx="2"/><path d="M15 10.5l6-3.5v10l-6-3.5"/>',
  palette: '<path d="M12 3a9 9 0 1 0 0 18c.9 0 1.5-.7 1.5-1.5 0-.4-.2-.8-.4-1.1-.3-.3-.4-.7-.4-1.1 0-.8.7-1.5 1.5-1.5H16a5 5 0 0 0 5-5c0-4.3-4-7.8-9-7.8z"/><circle cx="7.5" cy="11.5" r="1.1" fill="currentColor"/><circle cx="10" cy="7.3" r="1.1" fill="currentColor"/><circle cx="14.6" cy="7.3" r="1.1" fill="currentColor"/>',
  clock: '<circle cx="12" cy="12" r="9"/><path d="M12 7v5l3.2 2"/>',
  aura: '<path class="f" d="M 12,1 L 23,22 L 18.5,22 L 12,9.5 L 5.5,22 L 1,22 Z M 12,13 L 15.6,20 L 8.4,20 Z"/>',
  thermo: '<path d="M14 14.8V4.5a2 2 0 0 0-4 0v10.3a4 4 0 1 0 4 0z"/><path d="M12 9v7.5"/>',
  cpu: '<rect x="5.5" y="5.5" width="13" height="13" rx="1.5"/><rect x="9" y="9" width="6" height="6"/><path d="M9.5 2.5v3M14.5 2.5v3M9.5 18.5v3M14.5 18.5v3M2.5 9.5h3M2.5 14.5h3M18.5 9.5h3M18.5 14.5h3"/>',
  ram: '<rect x="2.5" y="7" width="19" height="10" rx="1"/><path d="M6.5 10.5v3M10 10.5v3M14 10.5v3M17.5 10.5v3M5.5 17v2.5M18.5 17v2.5"/>',
  rotate: '<path d="M20 12a8 8 0 1 1-2.6-5.9"/><path d="M20 3.5v5h-5"/>',
  check: '<path d="M5 12.5l4.5 4.5L19 7.5"/>',
  plus: '<path d="M12 5v14M5 12h14"/>',
  pause: '<rect class="f" x="6" y="4.5" width="4" height="15" rx="1"/><rect class="f" x="14" y="4.5" width="4" height="15" rx="1"/>',
  resume: '<path class="f" d="M7 4.5l13 7.5-13 7.5z"/>',
  chevron: '<path d="M6 9l6 6 6-6"/>',
  x: '<path d="M7 7l10 10M17 7L7 17"/>',
};
function icon(name) {
  let body = ICONS[name] || '';
  body = body.replace(/class="f"/g, 'fill="currentColor" stroke="none"');
  return '<svg class="i" viewBox="0 0 24 24">' + body + '</svg>';
}
function setIcon(el, name) { if (el.dataset.iconNow !== name) { el.dataset.iconNow = name; el.innerHTML = icon(name); } }

// ---------------- state ----------------
let S = null;          // full state from Rust
let ST = null;         // status (every 500 ms)
let fonts = [];
let page = 'dashboard';
const drags = { Clock: null, Overlay: null };   // LineDrag from Rust while a reading is dragged

const MODE_TILES = [
  ['Off', 'Off', 'power'], ['VirtualDisplay', 'Wallpaper Engine', 'monitor'], ['Monitor', 'Screen mirror', 'mirror'],
  ['Image', 'Image', 'image'], ['Gif', 'GIF', 'play'], ['Video', 'Video', 'video'], ['Color', 'Colour', 'palette'],
  ['Clock', 'Clock', 'clock'], ['Aura', 'Aura Sync', 'aura'],
];
const GAUGES = [
  ['coolant', 'DashWater', 'COOLANT', 'thermo', '#50B4FF'],
  ['cpu_temp', 'DashCpuTemp', 'CPU TEMP', 'thermo', '#FFAA3C'],
  ['cpu_load', 'DashCpuLoad', 'CPU LOAD', 'cpu', '#FF4B6E'],
  ['ram', 'DashRam', 'RAM', 'ram', '#5BE38A'],
];

// ---------------- segmented toggles ----------------
function initSeg(seg) {
  const pill = document.createElement('i');
  pill.className = 'pill';
  seg.prepend(pill);
  seg.querySelectorAll('button').forEach((b) => b.addEventListener('click', () => {
    if (b.classList.contains('sel')) return;
    setSeg(seg, b.dataset.v, true);
    const v = parseV(b.dataset.v);
    if (seg.dataset.k) send({ t: 'set', k: seg.dataset.k, v });
    else if (seg.dataset.style) send({ t: 'style', k: seg.dataset.style, v });
    else if (seg.id === 'rotSeg') send({ t: 'rot', v });
  }));
  new ResizeObserver(() => placePill(seg, false)).observe(seg);
}
function setSeg(seg, value, animate) {
  const v = String(value);
  let changed = false;
  seg.querySelectorAll('button').forEach((b) => {
    const on = b.dataset.v === v;
    if (on !== b.classList.contains('sel')) changed = true;
    b.classList.toggle('sel', on);
  });
  if (changed || animate) placePill(seg, animate !== false && changed);
}
function placePill(seg, animate) {
  const pill = seg.querySelector('.pill');
  const b = seg.querySelector('button.sel');
  if (!b || b.offsetWidth <= 0) { pill.style.opacity = 0; return; }
  const show = pill.style.opacity === '1';
  pill.classList.toggle('anim', !!animate && show);
  pill.style.transform = 'translateX(' + b.offsetLeft + 'px)';
  pill.style.width = b.offsetWidth + 'px';
  pill.style.opacity = 1;
}

// ---------------- sliders (thin track, gradient fill, diamond thumb) ----------------
class Slider {
  constructor(el, onInput) {
    this.el = el;
    this.min = +el.dataset.min; this.max = +el.dataset.max; this.step = +(el.dataset.step || 1);
    this.unit = el.dataset.unit || '';
    this.value = this.min;
    this.dragging = false;
    this.onInput = onInput;
    this.label = el.nextElementSibling && el.nextElementSibling.classList.contains('val') ? el.nextElementSibling : null;
    el.innerHTML = '<div class="tr"></div><div class="fi"></div><div class="th"></div>';
    this.fi = el.querySelector('.fi'); this.th = el.querySelector('.th');
    el.addEventListener('pointerdown', (e) => {
      if (e.button !== 0) return;
      this.dragging = true;
      el.setPointerCapture(e.pointerId);
      this.fromX(e.clientX);
    });
    el.addEventListener('pointermove', (e) => { if (this.dragging) this.fromX(e.clientX); });
    const end = () => { this.dragging = false; };
    el.addEventListener('pointerup', end);
    el.addEventListener('pointercancel', end);
    el.addEventListener('lostpointercapture', end);
    el.addEventListener('wheel', (e) => { e.preventDefault(); this.input(this.value + (e.deltaY < 0 ? this.step : -this.step)); }, { passive: false });
    this.draw();
  }
  fromX(x) {
    const r = this.el.getBoundingClientRect();
    const f = Math.max(0, Math.min(1, (x - r.left) / Math.max(1, r.width)));
    this.input(this.min + f * (this.max - this.min));
  }
  snap(v) {
    v = Math.max(this.min, Math.min(this.max, v));
    v = Math.round((v - this.min) / this.step) * this.step + this.min;
    return +v.toFixed(4);
  }
  input(v) {
    v = this.snap(v);
    if (v === this.value) return;
    this.value = v;
    this.draw();
    if (this.onInput) this.onInput(v);
  }
  set(v) { if (this.dragging) return; this.value = this.snap(v); this.draw(); }
  setRange(min, max) { this.min = min; this.max = Math.max(min, max); this.value = this.snap(this.value); this.draw(); }
  draw() {
    const f = this.max > this.min ? (this.value - this.min) / (this.max - this.min) : 0;
    this.fi.style.width = (f * 100) + '%';
    this.th.style.left = (f * 100) + '%';
    if (this.label && this.unit !== null) this.label.textContent = this.text();
  }
  text() { return (Number.isInteger(this.value) ? this.value : this.value.toFixed(1)) + this.unit; }
}
const sliders = [];

// ---------------- combo box ----------------
function makeCombo(el, onPick) {
  el.innerHTML = '<div class="cur"></div><span class="chev">' + icon('chevron') + '</span>';
  const cur = el.querySelector('.cur');
  const c = { items: [], value: null, list: null };
  const close = () => { if (c.list) { c.list.remove(); c.list = null; } };
  c.close = close;
  el.addEventListener('click', (e) => {
    if (c.list) { if (!c.list.contains(e.target)) close(); return; }
    closeCombos();
    const list = document.createElement('div');
    list.className = 'list';
    c.items.forEach((it) => {
      const d = document.createElement('div');
      d.textContent = it.label;
      if (it.font) d.style.fontFamily = '"' + it.font + '"';
      if (it.v === c.value) d.className = 'sel';
      d.addEventListener('click', (ev) => { ev.stopPropagation(); close(); c.set(it.v); onPick(it.v); });
      list.appendChild(d);
    });
    el.appendChild(list);
    c.list = list;
    const sel = list.querySelector('.sel');
    if (sel) list.scrollTop = sel.offsetTop - 140;
  });
  c.set = (v) => {
    c.value = v;
    const it = c.items.find((i) => i.v === v);
    cur.textContent = it ? it.label : (v == null ? '' : String(v));
    cur.style.fontFamily = it && it.font ? '"' + it.font + '"' : '';
  };
  c.setItems = (items) => { c.items = items; c.set(c.value); };
  combos.push(c);
  return c;
}
const combos = [];
function closeCombos() { combos.forEach((c) => c.close()); }
document.addEventListener('pointerdown', (e) => { if (!e.target.closest('.combo')) closeCombos(); });

// ---------------- dashboard gauges ----------------
function gaugeArc(frac) {
  if (!(frac >= 0.004)) return '';
  frac = Math.min(1, frac);
  const c = 78, r = c - 11 / 2 - 6;
  const a0 = 135 * Math.PI / 180, a1 = (135 + 270 * Math.min(frac, 0.9999)) * Math.PI / 180;
  const p = (a) => (c + r * Math.cos(a)).toFixed(2) + ',' + (c + r * Math.sin(a)).toFixed(2);
  return 'M' + p(a0) + ' A' + r + ',' + r + ' 0 ' + (270 * frac > 180 ? 1 : 0) + ' 1 ' + p(a1);
}
function buildGauges() {
  const host = $('gauges');
  GAUGES.forEach(([key, , label, ic, color]) => {
    const card = document.createElement('div');
    card.className = 'card';
    card.dataset.gauge = key;
    card.innerHTML = '<svg class="gauge" viewBox="0 0 156 156">' +
      '<path class="track" d="' + gaugeArc(1) + '"/>' +
      '<path class="arc" stroke="' + color + '" style="filter:drop-shadow(0 0 6px ' + color + ')"/>' +
      '<svg x="70.5" y="34" width="15" height="15" viewBox="0 0 24 24" style="color:' + color + '">' +
      icon(ic).replace(/^<svg[^>]*>/, '').replace(/<\/svg>$/, '').replace(/<path/g, '<path fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round"').replace(/<rect/g, '<rect fill="none" stroke="currentColor" stroke-width="1.8"') + '</svg>' +
      '<text class="num" x="78" y="84">--</text>' +
      '<text class="glabel" x="78" y="138" fill="' + color + '">' + label + '</text></svg>';
    host.appendChild(card);
  });
  new ResizeObserver(() => send({ t: 'gw', w: host.clientWidth + 14 })).observe(host);
}
function applyDash(d) {
  if (!S) return;
  const cards = $$('#gauges > .card');
  let shown = 0;
  cards.forEach((card) => {
    const g = GAUGES.find((x) => x[0] === card.dataset.gauge);
    const on = !!S.cfg[g[1]];
    card.hidden = !on;
    if (on) shown++;
    const v = d[g[0]];
    card.querySelector('.num').textContent = v.text;
    card.querySelector('.arc').setAttribute('d', gaugeArc(Math.max(0, Math.min(1, v.frac))));
  });
  $('gauges').style.gridTemplateColumns = 'repeat(' + Math.max(1, d.cols || 2) + ', minmax(0, 1fr))';
  $('dashNote').textContent = d.note;
  $('cpuTempRow').hidden = !d.cpu_temp_hint;
}

// ---------------- mode tiles ----------------
let tilesKey = '';
function buildTiles() {
  const key = S.modes.join(',');
  if (key === tilesKey) return;
  tilesKey = key;
  const host = $('tiles');
  host.innerHTML = '';
  MODE_TILES.filter((t) => S.modes.includes(t[0])).forEach(([m, label, ic]) => {
    const b = document.createElement('button');
    b.className = 'tile';
    b.dataset.mode = m;
    b.innerHTML = '<span class="ic">' + icon(ic) + '</span><span class="tx">' + label + '</span>';
    b.addEventListener('click', () => { if (S.cfg.Mode !== m) send({ t: 'set', k: 'Mode', v: m }); });
    host.appendChild(b);
  });
  layoutTiles();
}
function layoutTiles() {
  const host = $('tiles');
  const n = host.children.length;
  if (!n || host.clientWidth <= 0) return;
  let cols = Math.max(3, Math.min(n, Math.floor((host.clientWidth + 10) / 128)));
  const rows = Math.ceil(n / cols);
  cols = Math.ceil(n / rows);
  host.style.gridTemplateColumns = 'repeat(' + cols + ', minmax(0, 1fr))';
}

// ---------------- reading lines (drag and drop) ----------------
function buildLines(kind) {
  const host = kind === 'Clock' ? $('ClkItems') : $('OvItems');
  const L = S.lists[kind];
  const d = drags[kind];
  const lines = d ? d.display : L.lines;
  const sel = S.sel && S.sel.list === kind ? S.sel.id : null;
  host.innerHTML = '';
  lines.forEach((line, i) => {
    const row = document.createElement('div');
    row.className = 'lrow';
    row.dataset.row = i;
    row.innerHTML = '<span class="n">' + (i + 1) + '</span><span class="chips"></span>';
    const chips = row.querySelector('.chips');
    line.forEach((id) => {
      const def = L.defs.find((x) => x.id === id);
      if (!def) return;
      const on = !!L.on[id];
      const c = document.createElement('span');
      c.className = 'rchip' + (on ? ' on' : '') + (sel === id && !d ? ' sel' : '') + (d && d.id === id ? ' preview' : '');
      c.dataset.id = id;
      c.title = 'Click to style it, drag to move it';
      c.innerHTML = '<span class="dot" title="' + (on ? 'Shown on the screen: click to hide' : 'Hidden: click to show') + '"><i></i></span><span></span>';
      c.lastChild.textContent = def.name;
      c.addEventListener('pointerdown', (e) => pressChip(e, kind, id, def.name, c));
      chips.appendChild(c);
    });
    if (line.length === 0) {
      const b = document.createElement('span');
      b.className = 'blank' + (d && d.id === -1 && i === d.new_row_idx ? ' preview' : '');
      b.innerHTML = 'empty line<span class="x" title="Remove this empty line">' + icon('x') + '</span>';
      b.querySelector('.x').addEventListener('click', (e) => { e.stopPropagation(); if (!d) send({ t: 'removeBlank', list: kind, index: i }); });
      chips.appendChild(b);
    }
    host.appendChild(row);
  });
  const pal = document.createElement('div');
  pal.className = 'lrow';
  pal.innerHTML = '<span class="n"></span><span class="addblank" title="Drag between two lines to leave an empty line there">+&nbsp;&nbsp;Empty line</span>';
  pal.querySelector('.addblank').addEventListener('pointerdown', (e) => pressChip(e, kind, -1, 'Empty line', e.currentTarget));
  host.appendChild(pal);
}

let press = null;   // pointer pressed on a reading chip: {kind, id, name, x, y, dot, dragging, last}
function pressChip(e, kind, id, name, el) {
  if (e.button !== 0) return;
  e.preventDefault();
  press = { kind, id, name, x: e.clientX, y: e.clientY, dot: !!e.target.closest('.dot'), dragging: false, last: '' };
}
document.addEventListener('pointermove', (e) => {
  if (!press) return;
  if (!press.dragging) {
    if (Math.abs(e.clientX - press.x) < 5 && Math.abs(e.clientY - press.y) < 5) return;
    press.dragging = true;
    send({ t: 'dragBegin', list: press.kind, id: press.id });
    const g = $('ghost');
    g.textContent = press.name;
    g.hidden = false;
    (press.kind === 'Clock' ? $('ClkItems') : $('OvItems')).classList.add('dragging');
  }
  const g = $('ghost');
  g.style.left = (e.clientX - 20) + 'px';
  g.style.top = (e.clientY - 16) + 'px';
  dropTarget(press, e.clientX, e.clientY);
});
document.addEventListener('pointerup', () => {
  if (!press) return;
  const p = press;
  press = null;
  if (p.dragging) {
    $('ghost').hidden = true;
    $$('.lines.dragging').forEach((x) => x.classList.remove('dragging'));
    send({ t: 'dragEnd', list: p.kind, drop: true });
    return;
  }
  if (p.id < 0) return;
  if (p.dot) send({ t: 'toggle', list: p.kind, id: p.id });
  else send({ t: 'select', list: p.kind, id: p.id });
});

// Works out the drop target from the preview on screen (rows map back to base lines around the preview row).
function dropTarget(p, x, y) {
  const d = drags[p.kind];
  if (!d) return;
  const host = p.kind === 'Clock' ? $('ClkItems') : $('OvItems');
  const rows = $$('.lrow[data-row]', host);
  const toBase = (r) => (d.new_row_idx >= 0 && r > d.new_row_idx ? r - 1 : r);
  let tNew = true, tLine = d.base.length, tIndex = 0;
  for (let r = 0; r < rows.length; r++) {
    const rc = rows[r].getBoundingClientRect();
    const mid = rc.top + rc.height / 2;
    if (Math.abs(y - mid) <= rc.height * 0.32) {
      if (r === d.new_row_idx) { tNew = true; tLine = d.new_row_idx; }
      else if (p.id === -1) { tNew = true; tLine = toBase(r) + (y >= mid ? 1 : 0); }
      else {
        tNew = false; tLine = toBase(r);
        const chips = $$('.rchip', rows[r]).filter((c) => +c.dataset.id !== p.id);
        tIndex = chips.filter((c) => { const b = c.getBoundingClientRect(); return b.left + b.width / 2 < x; }).length;
      }
      break;
    }
    if (y < mid) { tNew = true; tLine = r === d.new_row_idx + 1 && d.new_row_idx >= 0 ? d.new_row_idx : toBase(r); break; }
  }
  const key = tNew + ':' + tLine + ':' + (tNew ? 0 : tIndex);
  if (key === p.last) return;
  p.last = key;
  send({ t: 'dragTarget', list: p.kind, tNew, tLine, tIndex });
}

// ---------------- swatches ----------------
let swatchesBuilt = false;
function buildSwatches() {
  if (swatchesBuilt) return;
  swatchesBuilt = true;
  $$('.swatches').forEach((host) => {
    const target = host.dataset.swatch;
    const pick = (v) => (target === 'bg' ? send({ t: 'set', k: 'ColorRgb', v }) : send({ t: 'style', k: 'Rgb', v }));
    S.palette.forEach((v) => {
      const b = document.createElement('button');
      b.className = 'sw';
      b.style.background = hex6(v);
      b.title = hex6(v);
      b.dataset.v = v;
      b.addEventListener('click', () => pick(v));
      host.appendChild(b);
    });
    const c = document.createElement('span');
    c.className = 'sw custom';
    c.title = 'Custom colour…';
    c.innerHTML = '<input type="color">';
    let timer = 0;
    c.firstChild.addEventListener('input', (e) => {
      clearTimeout(timer);
      const v = parseInt(e.target.value.slice(1), 16);
      timer = setTimeout(() => pick(v), 60);
    });
    host.appendChild(c);
  });
}
function markSwatch(host, v) {
  let found = false;
  host.querySelectorAll('.sw').forEach((b) => {
    const on = b.dataset.v !== undefined && +b.dataset.v === v;
    if (on) found = true;
    b.classList.toggle('cur', on);
  });
  const custom = host.querySelector('.custom');
  custom.classList.toggle('cur', !found);
  const inp = custom.querySelector('input');
  if (document.activeElement !== inp) inp.value = hex6(v).toLowerCase();
}

// ---------------- video (Windows) ----------------
const vid = { start: 0, length: 30, zoom: 1, panx: 0, pany: 0, fps: 30, duration: 0, editedAt: 0 };
const vidSliders = {};
let vidTimer = 0, thumbTimer = 0;
function fmtTime(s) { return Math.floor(s / 60) + ':' + String(Math.floor(s) % 60).padStart(2, '0') + '.' + (Math.floor(s * 10) % 10); }
function videoLabels() {
  $('vStartT').textContent = fmtTime(vid.start);
  $('vLengthT').textContent = vid.length.toFixed(1) + ' s';
  $('vZoomT').textContent = vid.zoom.toFixed(1) + '×';
  $('vFpsT').textContent = String(Math.round(vid.fps));
  if (vid.duration > 0) $('vInfo').textContent = 'Source ' + fmtTime(vid.duration) + ' · clip ' + fmtTime(vid.start) + '–' + fmtTime(Math.min(vid.duration, vid.start + vid.length));
}
function videoInput(key, v) {
  vid[key] = v;
  vid.editedAt = Date.now();
  videoLabels();
  clearTimeout(vidTimer);
  vidTimer = setTimeout(() => send({ t: 'video', start: vid.start, length: vid.length, zoom: vid.zoom, panx: vid.panx, pany: vid.pany, fps: Math.round(vid.fps) }), 500);
  clearTimeout(thumbTimer);
  thumbTimer = setTimeout(() => send({ t: 'thumb', start: vid.start, zoom: vid.zoom, panx: vid.panx, pany: vid.pany }), 150);
}
window.ttlcd_video = null;

// ---------------- pages ----------------
function showPage(name) {
  page = name;
  $$('.navitem').forEach((n) => n.classList.toggle('sel', n.dataset.page === name));
  $$('.page').forEach((p) => { p.hidden = p.id !== 'page-' + name; });
  $('pages').scrollTop = 0;
  send({ t: 'nav', page: name });
}
function scrollToCentre(el, done) {
  const sv = $('pages');
  const from = sv.scrollTop;
  const y = el.getBoundingClientRect().top - sv.getBoundingClientRect().top;
  const to = Math.max(0, Math.min(sv.scrollHeight - sv.clientHeight, from + y + el.offsetHeight / 2 - sv.clientHeight / 2));
  if (Math.abs(to - from) < 1) { done(); return; }
  const t0 = performance.now();
  const step = (now) => {
    const t = Math.min(1, (now - t0) / 350);
    sv.scrollTop = from + (to - from) * (1 - Math.pow(1 - t, 3));
    if (t < 1) requestAnimationFrame(step); else done();
  };
  requestAnimationFrame(step);
}
function flashCard(card) {
  card.classList.remove('flashing');
  void card.offsetWidth;
  card.classList.add('flashing');
  setTimeout(() => card.classList.remove('flashing'), 2500);
}

// ---------------- apply state from Rust ----------------
function applyState(s) {
  S = s;
  const c = s.cfg, P = s.plat;
  document.body.classList.toggle('linux', P.linux);
  $('app').classList.toggle('navcol', !!c.NavCollapsed);
  $('navToggle').title = c.NavCollapsed ? 'Expand menu' : 'Collapse menu';

  // bound settings: switches and chips (bool), segments, sliders
  $$('.switch[data-k], .chip[data-k]').forEach((el) => el.classList.toggle('on', !!c[el.dataset.k]));
  $$('.seg[data-k]').forEach((el) => setSeg(el, c[el.dataset.k]));
  sliders.forEach((sl) => { if (sl.el.dataset.k) sl.set(c[sl.el.dataset.k] / (+(sl.el.dataset.scale || 1))); });
  $$('[data-path]').forEach((el) => { el.value = c[el.dataset.path] || ''; el.title = el.value; });
  $$('[data-text]').forEach((el) => { if (document.activeElement !== el) el.value = c[el.dataset.text] || ''; });
  $$('[data-region]').forEach((el) => { if (document.activeElement !== el) el.value = c[el.dataset.region]; el.disabled = !c.UseRegion; });
  $('regionRow').style.opacity = c.UseRegion ? 1 : 0.45;
  $('startupSwitch').classList.toggle('on', !!s.autostart);
  setSeg($('rotSeg'), s.userRot);

  // modes
  buildTiles();
  $$('#tiles .tile').forEach((t) => {
    t.classList.toggle('sel', t.dataset.mode === c.Mode);
    const dis = t.dataset.mode === 'Aura' && !s.auraReady && c.Mode !== 'Aura';
    t.classList.toggle('disabled', dis);
    t.title = t.dataset.mode === 'Aura' && !s.auraReady ? 'Needs armory-crate (the Linux Aura Sync service). Install it and enable armory-crated' : '';
  });
  $('optTitle').textContent = s.optTitle;
  $('optDesc').textContent = s.optDesc;
  Object.keys(s.panels).forEach((k) => { const el = $(k); if (el) el.hidden = !s.panels[k]; });
  buildSwatches();
  markSwatch($$('.swatches[data-swatch=bg]')[0], c.ColorRgb);
  buildLines('Clock');
  buildLines('Overlay');

  // text style of the selected reading
  if (s.sel) {
    const st = s.sel.style;
    $('stTitle').textContent = s.sel.title;
    fontCombo.set(st.Font);
    $$('.chip[data-style]').forEach((el) => el.classList.toggle('on', !!st[el.dataset.style]));
    $$('.switch[data-style]').forEach((el) => el.classList.toggle('on', !!st[el.dataset.style]));
    $$('.seg[data-style]').forEach((el) => {
      const k = el.dataset.style;
      setSeg(el, k === 'LabelMode' ? (!st.Label ? 'none' : st.LabelAbove ? 'above' : 'beside') : st[k]);
    });
    sliders.forEach((sl) => { if (sl.el.dataset.style) sl.set(st[sl.el.dataset.style]); });
    markSwatch($$('.swatches[data-swatch=text]')[0], st.Rgb);
    $('stUnits').hidden = !s.sel.temp;
    $('stReading').hidden = !s.sel.reading;
  }

  // monitor list, virtual display, Aura, sensors (Windows parts)
  monitorCombo.setItems(s.monitors.map((m, i) => ({ v: i, label: m })));
  monitorCombo.set(Math.min(c.MonitorIndex, s.monitors.length - 1));
  ['vdStatus1', 'vdStatus2'].forEach((id) => { $(id).textContent = s.vd.text; });
  $('vdInstall').disabled = s.vd.installed || s.vd.busy;
  $('vdUninstall').disabled = !s.vd.installed || s.vd.busy;
  $('vdCard').hidden = !P.windows;
  $('auraStatus').parentElement.hidden = !P.windows;
  $('auraStatus').textContent = s.aura.text;
  $('auraInstall').disabled = s.aura.installed || s.aura.busy;
  $('auraUninstall').disabled = !s.aura.installed || s.aura.busy;
  $('svcStatus').textContent = s.svc.text;
  $('svcInstall').parentElement.querySelectorAll('.btn').forEach((b) => { b.hidden = !P.sensorsManaged; });
  $('svcInstall').disabled = s.svc.installed || s.svc.busy;
  $('svcUninstall').disabled = !s.svc.installed || s.svc.busy;
  $('clearCache').hidden = !P.video;

  // game mode
  const detect = P.gameDetection;
  ['gmOn', 'gmDetectRow', 'gmExtraRow', 'gmIgnoreRow'].forEach((id) => { $(id).hidden = !detect; });
  const gmOn = !detect || c.GameDetect;
  $('gmOptions').style.opacity = gmOn ? 1 : 0.45;
  $('gmOptions').style.pointerEvents = gmOn ? '' : 'none';
  $('gmFpsRow').hidden = c.GameAction !== 'LowerFps';

  // platform texts
  $('dashSub').textContent = 'Live readings. Coolant temperature comes from the cooler\'s own sensor; the rest are read from ' + (P.linux ? 'the system.' : 'Windows.');
  $('connSub').textContent = 'The cooler as ' + (P.linux ? 'the system' : 'Windows') + ' sees it over USB.';
  if (P.linux) $('sensorsSub').textContent = 'CPU temperature is read from the kernel\'s sensor driver (k10temp, zenpower or coretemp); nothing has to be installed.';
  $('startupText').textContent = P.linux ? 'Start with the desktop session (in the tray)' : 'Start with Windows (in tray)';

  // video sliders follow the settings unless the user just moved them
  if (Date.now() - vid.editedAt > 1500) {
    vid.start = c.VideoStart; vid.length = c.VideoLength; vid.zoom = c.VideoZoom; vid.panx = c.VideoPanX; vid.pany = c.VideoPanY; vid.fps = c.VideoFps;
    Object.keys(vidSliders).forEach((k) => vidSliders[k].set(vid[k]));
  }
  videoLabels();

  // live preview turned by -mount: the LCD as you see it on this mount
  $('pvImg').style.transform = 'rotate(' + (-s.mount) + 'deg)';
  layoutTiles();
}

function applyStatus(st) {
  ST = st;
  $('titleStatus').textContent = st.title;
  $('gameBtn').classList.toggle('on', st.forceGame);
  $('gmForce').classList.toggle('on', st.forceGame);
  const col = st.connected ? '#2BE36B' : '#FF1F4B';
  $('connDot').style.background = col;
  $('connDot').style.boxShadow = '0 0 10px ' + col;
  $('connText').textContent = st.conn;
  $('fpsBig').textContent = st.fps;
  $('modeText').textContent = st.modeText;
  $('devStatus').textContent = st.devStatus;
  $('devSerial').textContent = st.serial;
  $('devFps').textContent = st.devFps;
  $('navSerial').textContent = st.navSerial;
  $('gmStatus').textContent = st.gameStatus;
  $('cacheInfo').textContent = st.cacheInfo;
  if (S) {
    $('svcStatus').textContent = S.svc.busy ? S.svc.text : st.svc;
    if (S.cfg.Mode === 'Aura' && !S.aura.busy) $('auraStatus').textContent = st.aura;
  }
  const hov = $('pvHover');
  hov.classList.toggle('paused', st.paused);
  hov.title = st.paused ? 'Resume' : 'Pause';
  setIcon($('pauseIcon'), st.paused ? 'resume' : 'pause');
  const vp = $('vProgress');
  vp.hidden = !(st.videoProgress >= 0 && S && S.cfg.Mode === 'Video');
  vp.firstChild.style.width = Math.max(0, st.videoProgress) * 100 + '%';
  const it = $('idleTemp');
  it.hidden = !(idleView && st.idleTemp != null);
  if (st.idleTemp != null) $('idleTempNum').textContent = st.idleTemp;
  $('app').classList.toggle('full', st.full);
  const fb = $('fullBtn');
  setIcon(fb, st.full ? 'restore' : 'fullscreen');
  fb.title = st.full ? 'Exit full screen (F11 / Esc)' : 'Full screen (F11)';
}

// ---------------- idle view ----------------
let idleView = false, idleSeq = null;
function applyIdle(I) {
  idleView = I.view;
  $('pvIdle').classList.toggle('on', I.view);
  $('pvTitle').textContent = I.view ? 'IDLE IMAGE' : 'LIVE PREVIEW';
  ['pvImg', 'livePanel', 'pvHover'].forEach((id) => { $(id).hidden = I.view; });
  ['idleImg', 'idlePanel', 'idleHit'].forEach((id) => { $(id).hidden = !I.view; });
  const img = $('idleImg');
  if (I.img !== idleSeq) {
    idleSeq = I.img;
    if (I.img == null) img.removeAttribute('src'); else img.src = 'idle/' + I.img + '.jpg';
  }
  img.style.transform = 'rotate(' + I.angle + 'deg)';
  $('idleEmpty').hidden = !(I.view && I.img == null);
  $('idleTitle').textContent = I.title;
  $('idleEdit').hidden = !I.editing;
  $('idleInfo').textContent = I.info;
  idleZoom.set(I.zoom);
  $('idleZoomT').textContent = I.zoom.toFixed(1) + '×';
  $('idleStore').hidden = !I.editing;
  $('idleStore').disabled = !I.fits || I.busy;
  $('idleCancel').hidden = !I.editing;
  $('idleRead').hidden = I.editing;
  ['idleBrowse', 'idleRead', 'idleCancel'].forEach((id) => { $(id).disabled = I.busy; });
  $('idleProgress').hidden = !I.busy;
  const p = Math.max(0, Math.min(100, I.progress));
  $('idleProgress').firstChild.style.width = p + '%';
  const len = 2 * Math.PI * 168;
  $('idleRing').firstElementChild.style.strokeDasharray = (I.busy && p > 0 ? (p >= 100 ? len + 1 : len * p / 100) : 0) + ' 1100';
  $('idleStatus').textContent = I.status;
  $('idleHit').classList.toggle('editing', I.editing);
  if (ST) applyStatus(ST);
}

// ---------------- live frames ----------------
let frameLoading = false, frameWant = 0, frameShown = 0;
function loadFrame() {
  if (frameLoading || frameWant === frameShown) return;
  frameLoading = true;
  const seq = frameWant;
  const img = $('pvImg');
  img.onload = img.onerror = () => { frameLoading = false; frameShown = seq; loadFrame(); };
  img.src = 'frame/' + seq + '.jpg';
}

// ---------------- the API the Rust side calls ----------------
window.ttlcd = {
  fonts(list) { fonts = list; fontCombo.setItems(list.map((f) => ({ v: f, label: f, font: f }))); },
  state(s) { applyState(s); },
  status(st) { applyStatus(st); },
  dash(d) { applyDash(d); },
  idle(i) { applyIdle(i); },
  frame(seq) { frameWant = seq; loadFrame(); },
  drag(kind, d) { drags[kind] = d; if (S) buildLines(kind); },
  video(v) {
    if (v.duration !== undefined) {
      vid.duration = v.duration;
      if (v.duration > 0) {
        vidSliders.start.setRange(0, Math.max(0, v.duration - 1));
        vidSliders.length.setRange(1, Math.max(1, Math.min(30, v.duration)));
      }
      videoLabels();
    }
    if (v.info) $('vInfo').textContent = v.info;
    if (v.thumb !== undefined) { if (v.thumb == null) $('vThumb').removeAttribute('src'); else $('vThumb').src = 'thumb/' + v.thumb + '.jpg'; }
  },
  alert(text) { $('dialogText').textContent = text; $('dialog').hidden = false; },
  page(name) { showPage(name); },
};

// ---------------- wiring ----------------
let fontCombo, monitorCombo, idleZoom;
function init() {
  $$('[data-icon]').forEach((el) => setIcon(el, el.dataset.icon));
  $$('.chip').forEach((el) => el.insertAdjacentHTML('afterbegin', '<span class="ci ci-plus">' + icon('plus') + '</span><span class="ci ci-check">' + icon('check') + '</span>'));
  const st = document.createElement('style');
  st.textContent = '.chip .ci-check{display:none}.chip.on .ci-check{display:block}.chip.on .ci-plus{display:none}';
  document.head.appendChild(st);
  $$('.card').forEach((c) => c.insertAdjacentHTML('beforeend', '<div class="flash"></div>'));
  $$('.seg').forEach(initSeg);

  // sliders
  $$('.slider').forEach((el) => {
    let on;
    if (el.dataset.k) {
      const scale = +(el.dataset.scale || 1);
      on = (v) => send({ t: 'set', k: el.dataset.k, v: Math.round(v * scale) });
    } else if (el.dataset.style) on = (v) => send({ t: 'style', k: el.dataset.style, v });
    else if (el.dataset.video) on = (v) => videoInput(el.dataset.video, v);
    else if (el.id === 'idleZoom') on = (v) => { $('idleZoomT').textContent = v.toFixed(1) + '×'; send({ t: 'idle', a: 'zoom', v }); };
    const s = new Slider(el, on);
    if (el.dataset.video || el.id === 'idleZoom') s.unit = null;
    if (el.dataset.video) vidSliders[el.dataset.video] = s;
    if (el.id === 'idleZoom') idleZoom = s;
    sliders.push(s);
  });

  // switches and chips
  $$('.switch[data-k], .chip[data-k]').forEach((el) => el.addEventListener('click', (e) => {
    e.preventDefault();
    const v = !el.classList.contains('on');
    el.classList.toggle('on', v);
    send({ t: 'set', k: el.dataset.k, v });
  }));
  $$('.switch[data-style], .chip[data-style]').forEach((el) => el.addEventListener('click', (e) => {
    e.preventDefault();
    const v = !el.classList.contains('on');
    el.classList.toggle('on', v);
    send({ t: 'style', k: el.dataset.style, v });
  }));
  $('startupSwitch').addEventListener('click', (e) => { e.preventDefault(); const v = !$('startupSwitch').classList.contains('on'); $('startupSwitch').classList.toggle('on', v); send({ t: 'autostart', v }); });
  $('gmForce').addEventListener('click', (e) => { e.preventDefault(); send({ t: 'game', v: !$('gmForce').classList.contains('on') }); });
  $('gameBtn').addEventListener('click', () => send({ t: 'game', v: !$('gameBtn').classList.contains('on') }));
  $('pvIdle').addEventListener('click', (e) => { e.preventDefault(); send({ t: 'idle', a: 'view', v: !idleView }); });

  // text fields
  $$('[data-text]').forEach((el) => el.addEventListener('change', () => send({ t: 'set', k: el.dataset.text, v: el.value.trim() })));
  $$('[data-region]').forEach((el) => el.addEventListener('change', () => {
    const v = {};
    $$('[data-region]').forEach((x) => { v[x.dataset.region] = parseInt(x.value, 10); });
    send({ t: 'region', x: v.RegionX, y: v.RegionY, w: v.RegionW, h: v.RegionH });
  }));
  $$('[data-browse]').forEach((el) => el.addEventListener('click', () => send({ t: 'browse', what: el.dataset.browse })));

  fontCombo = makeCombo($('txtFont'), (v) => send({ t: 'style', k: 'Font', v }));
  monitorCombo = makeCombo($('monitorBox'), (v) => send({ t: 'set', k: 'MonitorIndex', v }));

  // navigation
  $$('.navitem').forEach((n) => n.addEventListener('click', () => showPage(n.dataset.page)));
  $('navToggle').addEventListener('click', () => send({ t: 'set', k: 'NavCollapsed', v: !(S && S.cfg.NavCollapsed) }));
  $('vdGoDevice').addEventListener('click', () => showPage('device'));
  $('cpuTempLink').addEventListener('click', (e) => {
    e.preventDefault();
    showPage('settings');
    requestAnimationFrame(() => scrollToCentre($('sensorsCard'), () => flashCard($('sensorsCard'))));
  });

  // buttons that run the command-line tool, and the rest
  const cli = (id, args, what) => $(id).addEventListener('click', () => send({ t: 'cli', args, what }));
  cli('vdInstall', 'display install', 'vd'); cli('vdUninstall', 'display uninstall', 'vd');
  cli('auraInstall', 'aura install', 'aura'); cli('auraUninstall', 'aura uninstall', 'aura');
  cli('svcInstall', 'service install', 'svc'); cli('svcUninstall', 'service uninstall', 'svc');
  $('clearCache').addEventListener('click', () => send({ t: 'clearCache' }));
  $('openConfig').addEventListener('click', () => send({ t: 'openFolder' }));
  $('rotateBtn').addEventListener('click', () => send({ t: 'rotate' }));
  $('pvHover').addEventListener('click', () => send({ t: 'pause' }));
  $('idleBrowse').addEventListener('click', () => send({ t: 'idle', a: 'choose' }));
  $('idleRead').addEventListener('click', () => send({ t: 'idle', a: 'read' }));
  $('idleStore').addEventListener('click', () => send({ t: 'idle', a: 'store' }));
  $('idleCancel').addEventListener('click', () => send({ t: 'idle', a: 'cancel' }));
  $('dialogOk').addEventListener('click', () => { $('dialog').hidden = true; });

  // idle picture: drag to pan, wheel to zoom
  const hit = $('idleHit');
  let last = null;
  hit.addEventListener('pointerdown', (e) => {
    if (!hit.classList.contains('editing') || e.button !== 0) return;
    last = { x: e.clientX, y: e.clientY };
    hit.setPointerCapture(e.pointerId);
  });
  hit.addEventListener('pointermove', (e) => {
    if (!last) return;
    const k = 480 / Math.max(1, hit.clientWidth);
    send({ t: 'idle', a: 'pan', dx: (e.clientX - last.x) * k, dy: (e.clientY - last.y) * k });
    last = { x: e.clientX, y: e.clientY };
  });
  const endPan = () => { last = null; };
  hit.addEventListener('pointerup', endPan);
  hit.addEventListener('lostpointercapture', endPan);
  hit.addEventListener('wheel', (e) => {
    e.preventDefault();
    if (!hit.classList.contains('editing')) return;
    idleZoom.input(Math.max(1, Math.min(5, idleZoom.value * (e.deltaY < 0 ? 1.1 : 1 / 1.1))));
  }, { passive: false });

  // window: title bar drag, buttons, resize edges, keys
  $('titlebar').addEventListener('mousedown', (e) => {
    if (e.button !== 0 || e.target.closest('.tb-btns')) return;
    if (e.detail === 2) send({ t: 'win', a: 'full' }); else send({ t: 'win', a: 'drag' });
  });
  $('minBtn').addEventListener('click', () => send({ t: 'win', a: 'min' }));
  $('fullBtn').addEventListener('click', () => send({ t: 'win', a: 'full' }));
  $('closeBtn').addEventListener('click', () => send({ t: 'win', a: 'close' }));
  $$('.rz').forEach((el) => el.addEventListener('mousedown', (e) => { if (e.button === 0) { e.preventDefault(); send({ t: 'win', a: 'resize', d: el.dataset.rz }); } }));
  document.addEventListener('keydown', (e) => {
    if (e.key === 'F11') { e.preventDefault(); send({ t: 'win', a: 'full' }); }
    else if (e.key === 'Escape') {
      if (!$('dialog').hidden) $('dialog').hidden = true;
      else if (ST && ST.full) send({ t: 'win', a: 'full' });
      closeCombos();
    }
  });
  document.addEventListener('contextmenu', (e) => { if (!e.target.closest('input')) e.preventDefault(); });

  // overlay scrollbar: shown while scrolling
  let scrollTimer = 0;
  $('pages').addEventListener('scroll', () => {
    $('pages').classList.add('scrolling');
    clearTimeout(scrollTimer);
    scrollTimer = setTimeout(() => $('pages').classList.remove('scrolling'), 1750);
    closeCombos();
  });

  buildGauges();
  new ResizeObserver(layoutTiles).observe($('tiles'));
  send({ t: 'ready' });
}
document.addEventListener('DOMContentLoaded', init);
