// Text style of the selected reading (each reading has its own): font, bold/italic, size, effect, colour, and for
// temperatures the units, for readings the label and colour by value.
import './TextStyle.scss';
import { useMemo } from 'react';
import Chip from '../../../components/Chip/Chip.jsx';
import Combo from '../../../components/Combo/Combo.jsx';
import Row from '../../../components/Row/Row.jsx';
import Seg from '../../../components/Seg/Seg.jsx';
import Slider, { unit } from '../../../components/Slider/Slider.jsx';
import Swatches from '../../../components/Swatches/Swatches.jsx';
import Switch from '../../../components/Switch/Switch.jsx';
import { setStyle, useStore } from '../../../bridge.js';

export default function TextStyle({ sel, palette }) {
  const fonts = useStore((s) => s.fonts);
  const items = useMemo(() => fonts.map((f) => ({ v: f, label: f, font: f })), [fonts]);
  const st = sel.style;
  const label = !st.Label ? 'none' : st.LabelAbove ? 'above' : 'beside';
  return (
    <div className="text-style">
      <div className="hair" />
      <div className="caption section">{sel.title}</div>
      <p className="sub tight">How the selected reading looks. Each reading has its own style.</p>
      <Row label="Font">
        <div className="fontrow span">
          <Combo items={items} value={st.Font} onPick={(v) => setStyle('Font', v)} />
          <Chip inline on={st.Bold} onChange={(v) => setStyle('Bold', v)}>Bold</Chip>
          <Chip inline on={st.Italic} onChange={(v) => setStyle('Italic', v)}>Italic</Chip>
        </div>
      </Row>
      <Row label="Size"><Slider value={st.Size} min={12} max={220} step={1} onInput={(v) => setStyle('Size', v)} format={unit(' px')} /></Row>
      <Row label="Effect">
        <div className="span"><Seg value={st.Effect} onChange={(v) => setStyle('Effect', v)}
          options={[['None', 'None'], ['Shadow', 'Shadow'], ['Outline', 'Outline'], ['Glow', 'Neon glow']]} /></div>
      </Row>
      <Row label="Colour"><div className="span"><Swatches palette={palette} value={st.Rgb} onPick={(v) => setStyle('Rgb', v)} /></div></Row>
      {sel.temp && (
        <Row label="Units"><div className="span"><Seg value={st.Fahrenheit} onChange={(v) => setStyle('Fahrenheit', v)} options={[[false, '°C'], [true, '°F']]} /></div></Row>
      )}
      {sel.reading && (
        <Row label="Reading" top>
          <div className="wrap gap10 span">
            <Seg value={label} onChange={(v) => setStyle('LabelMode', v)} options={[['none', 'No label'], ['beside', 'Coolant: 31°C'], ['above', 'Label above']]} />
            <Switch on={st.ByTemp} onChange={(v) => setStyle('ByTemp', v)}>Colour by value</Switch>
          </div>
        </Row>
      )}
    </div>
  );
}
