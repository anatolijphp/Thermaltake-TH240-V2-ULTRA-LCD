// Message box for command output (installer errors and the like).
import './Dialog.scss';
import Button from '../Button/Button.jsx';

export default function Dialog({ text, onClose }) {
  if (text == null) return null;
  return (
    <div className="dialog-back" onClick={onClose}>
      <div className="dlg" onClick={(e) => e.stopPropagation()}>
        <div className="dlgtitle">TT LCD Studio</div>
        <pre>{text}</pre>
        <Button accent onClick={onClose}>OK</Button>
      </div>
    </div>
  );
}
