// iOS-style switch: the knob slides and the red track fades in; only the switch and its text are clickable.
import './Switch.scss';
import { useOptimistic } from '../../optimistic.js';

export default function Switch({ on, onChange, children, title, className, id }) {
  const [shown, setShown] = useOptimistic(!!on);
  const click = (e) => { e.preventDefault(); setShown(!shown); onChange(!shown); };
  return (
    <label className={'switch' + (shown ? ' on' : '') + (className ? ' ' + className : '')} title={title} id={id} onClick={click}>
      <span className="track"><span className="knob" /></span>
      {children !== undefined && <span className="txt">{children}</span>}
    </label>
  );
}
