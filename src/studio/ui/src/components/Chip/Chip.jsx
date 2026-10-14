// Compact on/off pill for "what to show" groups: + when off, ✓ and a red glow when on.
import './Chip.scss';
import Icon from '../Icon/Icon.jsx';
import { useOptimistic } from '../../optimistic.js';

export default function Chip({ on, onChange, children, title, inline }) {
  const [shown, setShown] = useOptimistic(!!on);
  return (
    <button type="button" className={'chip' + (shown ? ' on' : '') + (inline ? ' inline' : '')} title={title}
      onClick={() => { setShown(!shown); onChange(!shown); }}>
      <span className="ci"><Icon name={shown ? 'check' : 'plus'} /></span>{children}
    </button>
  );
}
