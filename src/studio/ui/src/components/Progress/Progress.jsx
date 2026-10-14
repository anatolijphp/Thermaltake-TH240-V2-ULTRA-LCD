// Thin progress bar (red), or the thicker cyan one of the idle upload.
import './Progress.scss';

export default function Progress({ frac, cyan }) {
  return <div className={'progress' + (cyan ? ' cyan' : '')}><i style={{ width: Math.max(0, Math.min(1, frac)) * 100 + '%' }} /></div>;
}
