// Invisible resize edges and corners of the frameless window (hidden in full screen).
import './ResizeEdges.scss';
import { send } from '../../bridge.js';

const DIRS = ['North', 'South', 'West', 'East', 'NorthWest', 'NorthEast', 'SouthWest', 'SouthEast'];

export default function ResizeEdges() {
  return DIRS.map((d) => (
    <div key={d} className={'rz rz-' + d}
      onMouseDown={(e) => { if (e.button === 0) { e.preventDefault(); send({ t: 'win', a: 'resize', d }); } }} />
  ));
}
