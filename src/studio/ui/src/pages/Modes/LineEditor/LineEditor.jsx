// A reading list (clock face or overlay) shown as the screen's lines: each row holds the chips of one line. Click the
// dot to turn a reading on or off, click the name to style it, drag a chip onto a line to put it beside the others,
// or between lines to give it a line of its own; drag "Empty line" between two lines to leave a blank row. The line
// logic lives in the app (LineDrag): while dragging, it sends the lines as they would be (`drags[kind]`).
import './LineEditor.scss';
import { useEffect, useRef, useState } from 'react';
import { createPortal } from 'react-dom';
import Icon from '../../../components/Icon/Icon.jsx';
import { send, useStore } from '../../../bridge.js';

export default function LineEditor({ kind }) {
  const L = useStore((s) => s.S.lists[kind]);
  const selAll = useStore((s) => s.S.sel);
  const d = useStore((s) => s.drags[kind]);
  const host = useRef(null);
  const press = useRef(null); // pointer pressed on a chip: {id, name, x, y, dot, dragging, last}
  const [ghost, setGhost] = useState(null); // {name, x, y}
  const sel = selAll && selAll.list === kind ? selAll.id : null;
  const lines = d ? d.display : L.lines;
  const drag = useRef(d);
  drag.current = d;

  useEffect(() => {
    const move = (e) => {
      const p = press.current;
      if (!p) return;
      if (!p.dragging) {
        if (Math.abs(e.clientX - p.x) < 5 && Math.abs(e.clientY - p.y) < 5) return;
        p.dragging = true;
        send({ t: 'dragBegin', list: kind, id: p.id });
      }
      setGhost({ name: p.name, x: e.clientX, y: e.clientY });
      dropTarget(p, e.clientX, e.clientY);
    };
    const up = () => {
      const p = press.current;
      if (!p) return;
      press.current = null;
      if (p.dragging) {
        setGhost(null);
        send({ t: 'dragEnd', list: kind, drop: true });
        return;
      }
      if (p.id < 0) return;
      send(p.dot ? { t: 'toggle', list: kind, id: p.id } : { t: 'select', list: kind, id: p.id });
    };
    document.addEventListener('pointermove', move);
    document.addEventListener('pointerup', up);
    return () => { document.removeEventListener('pointermove', move); document.removeEventListener('pointerup', up); };
  }, [kind]);

  // works out the drop target from the lines on screen (rows map back to base lines around the preview row)
  function dropTarget(p, x, y) {
    const dd = drag.current;
    if (!dd) return;
    const rows = Array.from(host.current.querySelectorAll('.lrow[data-row]'));
    const toBase = (r) => (dd.new_row_idx >= 0 && r > dd.new_row_idx ? r - 1 : r);
    let tNew = true, tLine = dd.base.length, tIndex = 0;
    for (let r = 0; r < rows.length; r++) {
      const rc = rows[r].getBoundingClientRect();
      const mid = rc.top + rc.height / 2;
      if (Math.abs(y - mid) <= rc.height * 0.32) {
        if (r === dd.new_row_idx) { tNew = true; tLine = dd.new_row_idx; }
        else if (p.id === -1) { tNew = true; tLine = toBase(r) + (y >= mid ? 1 : 0); }
        else {
          tNew = false; tLine = toBase(r);
          const chips = Array.from(rows[r].querySelectorAll('.rchip')).filter((c) => +c.dataset.id !== p.id);
          tIndex = chips.filter((c) => { const b = c.getBoundingClientRect(); return b.left + b.width / 2 < x; }).length;
        }
        break;
      }
      if (y < mid) { tNew = true; tLine = r === dd.new_row_idx + 1 && dd.new_row_idx >= 0 ? dd.new_row_idx : toBase(r); break; }
    }
    const key = tNew + ':' + tLine + ':' + (tNew ? 0 : tIndex);
    if (key === p.last) return;
    p.last = key;
    send({ t: 'dragTarget', list: kind, tNew, tLine, tIndex });
  }

  const pressChip = (e, id, name) => {
    if (e.button !== 0) return;
    e.preventDefault();
    press.current = { id, name, x: e.clientX, y: e.clientY, dot: !!e.target.closest('.dot'), dragging: false, last: '' };
  };

  return (
    <div className={'lines' + (ghost ? ' dragging' : '')} ref={host}>
      {lines.map((line, i) => (
        <div className="lrow" data-row={i} key={i}>
          <span className="n">{i + 1}</span>
          <span className="chips">
            {line.map((id) => {
              const def = L.defs.find((x) => x.id === id);
              if (!def) return null;
              const on = !!L.on[id];
              return (
                <span key={id} data-id={id} title="Click to style it, drag to move it"
                  className={'rchip' + (on ? ' on' : '') + (sel === id && !d ? ' sel' : '') + (d && d.id === id ? ' preview' : '')}
                  onPointerDown={(e) => pressChip(e, id, def.name)}>
                  <span className="dot" title={on ? 'Shown on the screen: click to hide' : 'Hidden: click to show'}><i /></span>
                  <span>{def.name}</span>
                </span>
              );
            })}
            {line.length === 0 && (
              <span className={'blank' + (d && d.id === -1 && i === d.new_row_idx ? ' preview' : '')}>
                empty line
                <span className="x" title="Remove this empty line"
                  onClick={(e) => { e.stopPropagation(); if (!d) send({ t: 'removeBlank', list: kind, index: i }); }}><Icon name="x" /></span>
              </span>
            )}
          </span>
        </div>
      ))}
      <div className="lrow">
        <span className="n" />
        <span className="addblank" title="Drag between two lines to leave an empty line there"
          onPointerDown={(e) => pressChip(e, -1, 'Empty line')}>+&nbsp;&nbsp;Empty line</span>
      </div>
      {/* in <body>: cards clip their content (cut corner) */}
      {ghost && createPortal(<div className="line-ghost" style={{ left: ghost.x - 20, top: ghost.y - 16 }}>{ghost.name}</div>, document.body)}
    </div>
  );
}
