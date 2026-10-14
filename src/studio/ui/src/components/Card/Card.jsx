// Card with the cut top-right corner and the red tick; `flash` blinks its outline (a link jumped here).
import './Card.scss';
import { forwardRef } from 'react';

const Card = forwardRef(function Card({ caption, sub, children, flash, className, id, style }, ref) {
  return (
    <div className={'card' + (flash ? ' flashing' : '') + (className ? ' ' + className : '')} id={id} ref={ref} style={style}>
      {caption && <div className="caption">{caption}</div>}
      {sub && <p className="sub tight">{sub}</p>}
      {children}
      <div className="flash" />
    </div>
  );
});
export default Card;
