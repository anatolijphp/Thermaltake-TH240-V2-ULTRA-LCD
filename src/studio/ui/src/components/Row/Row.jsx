// One settings row: label | control | (value). `kind`: "2" (label + control), "3" (label + control + value),
// "wide" (wider label), "compact" (video editor). `top` aligns to the top for wrapping controls.
import './Row.scss';

export default function Row({ label, kind = '3', top, title, children, id, hidden, style }) {
  return (
    <div className={'row row-' + kind + (top ? ' top' : '')} id={id} hidden={hidden} style={style}>
      <span className="lbl" title={title}>{label}</span>
      {children}
    </div>
  );
}
