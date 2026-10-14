// Text input that sends its value when edited (on change / leaving the field) and isn't overwritten while focused.
import './TextField.scss';
import { useEffect, useRef, useState } from 'react';

export default function TextField({ value, onCommit, readOnly, title, className, disabled }) {
  const [text, setText] = useState(value ?? '');
  const el = useRef(null);
  useEffect(() => { if (document.activeElement !== el.current) setText(value ?? ''); }, [value]);
  return (
    <input type="text" ref={el} className={'text' + (className ? ' ' + className : '')} value={text} readOnly={readOnly} disabled={disabled}
      title={title ?? (readOnly ? text : undefined)}
      onChange={(e) => setText(e.target.value)}
      onBlur={() => { if (!readOnly && text !== (value ?? '')) onCommit(text); }}
      onKeyDown={(e) => { if (e.key === 'Enter') e.currentTarget.blur(); }} />
  );
}
