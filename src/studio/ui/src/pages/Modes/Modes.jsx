// Display modes: the mode tiles and one options card whose panels follow the mode (the app decides which: S.panels).
import './Modes.scss';
import Button from '../../components/Button/Button.jsx';
import Card from '../../components/Card/Card.jsx';
import Combo from '../../components/Combo/Combo.jsx';
import Row from '../../components/Row/Row.jsx';
import Seg from '../../components/Seg/Seg.jsx';
import Slider, { unit } from '../../components/Slider/Slider.jsx';
import Swatches from '../../components/Swatches/Swatches.jsx';
import Switch from '../../components/Switch/Switch.jsx';
import TextField from '../../components/TextField/TextField.jsx';
import ModeTiles from './ModeTiles/ModeTiles.jsx';
import LineEditor from './LineEditor/LineEditor.jsx';
import VideoEditor from './VideoEditor/VideoEditor.jsx';
import TextStyle from './TextStyle/TextStyle.jsx';
import { send, setCfg, showPage, useStore } from '../../bridge.js';

const ALIGN = [[0, 'Left'], [1, 'Centre'], [2, 'Right']];

function PathPicker({ value, what }) {
  return (
    <div className="pathrow">
      <TextField readOnly value={value} />
      <Button accent onClick={() => send({ t: 'browse', what })}>Browse…</Button>
    </div>
  );
}

function Region({ cfg }) {
  const keys = ['RegionX', 'RegionY', 'RegionW', 'RegionH'];
  const commit = (k) => (text) => {
    const v = Object.fromEntries(keys.map((x) => [x, cfg[x]]));
    v[k] = parseInt(text, 10);
    send({ t: 'region', x: v.RegionX, y: v.RegionY, w: v.RegionW, h: v.RegionH });
  };
  return (
    <div className="hrow region" style={{ opacity: cfg.UseRegion ? 1 : 0.45 }}>
      {keys.map((k) => (
        <span key={k} className="hrow"><span className="lbl">{k.slice(6)}</span>
          <TextField className="num" value={String(cfg[k])} disabled={!cfg.UseRegion} onCommit={commit(k)} /></span>
      ))}
    </div>
  );
}

export default function Modes() {
  const S = useStore((s) => s.S);
  const ST = useStore((s) => s.ST);
  const c = S.cfg, P = S.panels, plat = S.plat;
  return (
    <section className="page modes">
      <h1>Display modes</h1>
      <p className="sub">Choose what the cooler screen shows. Changes apply instantly.</p>
      <ModeTiles modes={S.modes} mode={c.Mode} auraReady={S.auraReady} />
      <Card caption={S.optTitle} sub={S.optDesc}>
        {P.OptVirtualDisplay && (
          <div className="hrow"><span className="lbl">{S.vd.text}</span><Button onClick={() => showPage('device')}>Manage virtual display</Button></div>
        )}
        {P.OptMonitor && (
          <div>
            <Row kind="2" label="Monitor">
              <Combo items={S.monitors.map((m, i) => ({ v: i, label: m }))} value={Math.min(c.MonitorIndex, S.monitors.length - 1)}
                onPick={(v) => setCfg('MonitorIndex', v)} />
            </Row>
            <Switch on={c.UseRegion} onChange={(v) => setCfg('UseRegion', v)}>Only a region of the monitor</Switch>
            <Region cfg={c} />
          </div>
        )}
        {P.OptImage && <PathPicker value={c.ImagePath} what="image" />}
        {P.OptGif && <PathPicker value={c.GifPath} what="gif" />}
        {P.OptVideo && <VideoEditor cfg={c} />}
        {P.OptAura && plat.windows && (
          <div className="hrow">
            <span className="lbl minw">{S.aura.busy ? S.aura.text : c.Mode === 'Aura' && ST ? ST.aura : S.aura.text}</span>
            <Button accent disabled={S.aura.installed || S.aura.busy} onClick={() => send({ t: 'cli', args: 'aura install', what: 'aura' })}>Install plugin</Button>
            <Button disabled={!S.aura.installed || S.aura.busy} onClick={() => send({ t: 'cli', args: 'aura uninstall', what: 'aura' })}>Uninstall</Button>
          </div>
        )}
        {P.OptPump && (
          <div>
            <Row kind="2" label="Speed from">
              <Combo items={[{ v: '', label: 'CPU fan (automatic)' }, ...(S.fans || []).map((f) => ({ v: f.id, label: f.label })), { v: 'manual', label: 'Set by hand' }]}
                value={c.PumpFan} onPick={(v) => setCfg('PumpFan', v)} />
            </Row>
            {ST && ST.pump && (
              <p className="note pump-now">{ST.pump.source ? `Now ${ST.pump.rpm} rpm from ${ST.pump.source}.`
                : ST.pump.fans ? 'Set by hand.' : 'This system reports no fan speeds here, so the speed is set by hand.'}</p>
            )}
            {(c.PumpFan === 'manual' || (ST && ST.pump && !ST.pump.source)) && (
              <Row label="Pump rpm"><Slider value={c.PumpRpm} min={0} max={4000} step={50} onInput={(v) => setCfg('PumpRpm', v)} /></Row>
            )}
            <Row kind="2" label="Coolant"><Swatches palette={S.palette} value={c.PumpColor} onPick={(v) => setCfg('PumpColor', v)} /></Row>
            <Row kind="2" label="Light">
              <Switch on={c.PumpAura} onChange={(v) => setCfg('PumpAura', v)}>From Aura Sync (otherwise red)</Switch>
            </Row>
          </div>
        )}
        {P.OptClock && (
          <div>
            <div className="hair" />
            <div className="caption section">PLACEMENT</div>
            <p className="sub tight">Where the readings sit. Everything is kept inside the round screen: text that would cross the edge is shrunk automatically.</p>
            <Row label="Alignment"><div className="span"><Seg value={c.TextAlign} options={ALIGN} onChange={(v) => setCfg('TextAlign', v)} /></div></Row>
            <Row label="Vertical offset"><Slider value={c.TextOffsetY} min={-160} max={160} step={2} onInput={(v) => setCfg('TextOffsetY', v)} format={unit(' px')} /></Row>
            <Row label="Line spacing"><Slider value={c.LineSpacing} min={-20} max={60} step={1} onInput={(v) => setCfg('LineSpacing', v)} format={unit(' px')} /></Row>
            <div className="hair" />
            <div className="caption section">ELEMENTS</div>
            <p className="sub tight">Each row is a line on the screen. Drag a reading onto a line to put it beside the others, or between lines to give it its own line. The dot turns a reading on or off; click a reading to style it.</p>
            <LineEditor kind="Clock" />
          </div>
        )}
        {P.OptScaling && (
          <div className="spaced"><Row kind="2" label="Scaling">
            <Seg value={c.FitContain} options={[[false, 'Fill'], [true, 'Fit']]} onChange={(v) => setCfg('FitContain', v)} /></Row></div>
        )}
        {P.OptBg && (
          <div className="spaced"><Row kind="2" label="Background">
            <Swatches palette={S.palette} value={c.ColorRgb} onPick={(v) => setCfg('ColorRgb', v)} /></Row></div>
        )}
        {P.OptOverlay && (
          <div>
            <div className="hair" />
            {P.OvDetails && (
              <div>
                <div className="caption section">OVERLAY · PLACEMENT</div>
                <p className="sub tight">Time and temperatures drawn on top of this mode.</p>
                <Row kind="2" label="Vertical"><Seg value={c.OverlayPos} options={[[1, 'Top'], [2, 'Centre'], [0, 'Bottom']]} onChange={(v) => setCfg('OverlayPos', v)} /></Row>
                <Row kind="2" label="Alignment"><Seg value={c.TextAlign} options={ALIGN} onChange={(v) => setCfg('TextAlign', v)} /></Row>
              </div>
            )}
            <div className="caption section">OVERLAY · ELEMENTS</div>
            <p className="sub tight">Each row is a line. Drag readings between lines to arrange them. The dot turns a reading on or off; click a reading to style it.</p>
            <LineEditor kind="Overlay" />
          </div>
        )}
        {P.OptText && S.sel && <TextStyle sel={S.sel} palette={S.palette} />}
      </Card>
    </section>
  );
}
