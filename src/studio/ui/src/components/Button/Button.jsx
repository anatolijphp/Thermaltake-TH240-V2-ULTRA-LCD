// Flat buttons: plain, accent (red), or a square icon button.
import './Button.scss';
import Icon from '../Icon/Icon.jsx';

export default function Button({ accent, icon, className, children, ...rest }) {
  if (icon) {
    return <button type="button" className={'iconbtn' + (className ? ' ' + className : '')} {...rest}><Icon name={icon} /></button>;
  }
  return <button type="button" className={'btn' + (accent ? ' accent' : '') + (className ? ' ' + className : '')} {...rest}>{children}</button>;
}
