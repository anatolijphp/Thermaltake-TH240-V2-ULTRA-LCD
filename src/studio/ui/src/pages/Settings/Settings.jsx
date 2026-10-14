// Settings: streaming quality, game mode, the CPU temperature source, and how the app starts and closes.
import './Settings.scss';
import { useEffect, useRef, useState } from 'react';
import Button from '../../components/Button/Button.jsx';
import Card from '../../components/Card/Card.jsx';
import Chip from '../../components/Chip/Chip.jsx';
import Row from '../../components/Row/Row.jsx';
import Seg from '../../components/Seg/Seg.jsx';
import Slider, { unit } from '../../components/Slider/Slider.jsx';
import Switch from '../../components/Switch/Switch.jsx';
import TextField from '../../components/TextField/TextField.jsx';
import { send, setCfg, useStore } from '../../bridge.js';

/** Smoothly scrolls the page so `el` sits in the middle of the viewport, then calls done. */
function scrollToCentre(el, done) {
  const sv = el.closest('.pages');
  const from = sv.scrollTop;
  const y = el.getBoundingClientRect().top - sv.getBoundingClientRect().top;
  const to = Math.max(0, Math.min(sv.scrollHeight - sv.clientHeight, from + y + el.offsetHeight / 2 - sv.clientHeight / 2));
  if (Math.abs(to - from) < 1) { done(); return; }
  const t0 = performance.now();
  const step = (now) => {
    const t = Math.min(1, (now - t0) / 350);
    sv.scrollTop = from + (to - from) * (1 - Math.pow(1 - t, 3));
    if (t < 1) requestAnimationFrame(step); else done();
  };
  requestAnimationFrame(step);
}

export default function Settings() {
  const S = useStore((s) => s.S);
  const ST = useStore((s) => s.ST);
  const flash = useStore((s) => s.flash);
  const sensors = useRef(null);
  const [flashing, setFlashing] = useState(false);
  // the dashboard's hint jumps here: centre the sensors card and blink it
  useEffect(() => {
    if (!flash || flash.id !== 'sensorsCard' || Date.now() - flash.at > 1000) return;
    requestAnimationFrame(() => scrollToCentre(sensors.current, () => {
      setFlashing(false);
      requestAnimationFrame(() => setFlashing(true));
      setTimeout(() => setFlashing(false), 2500);
    }));
  }, [flash]);
  const c = S.cfg, P = S.plat, linux = P.linux;
  const detect = P.gameDetection;
  const gmOn = !detect || c.GameDetect;
  const chipTitles = linux ? {
    fs: 'Full-screen games: Steam, Proton/Wine, Lutris or Heroic games in a full-screen window (X11/XWayland)',
    known: 'Any game started by Steam (also Proton) or running under Wine, full-screen or not',
    any: 'Any app covering its whole monitor in the foreground (X11/XWayland); also triggers on full-screen video',
  } : {
    fs: 'Exclusive full-screen Direct3D games (what Windows reports to hold back notifications)',
    known: 'Games the Xbox Game Bar has recorded on this PC (includes borderless and windowed games)',
    any: 'Any app covering its whole monitor in the foreground; also triggers on full-screen video',
  };
  return (
    <section className="page settings">
      <h1>Settings</h1>
      <p className="sub">Streaming quality and how the app starts.</p>
      <Card caption="STREAMING" sub="How often and how sharp the pictures sent to the cooler are. Lower values use less CPU.">
        <Row kind="wide" label="Frame rate (FPS)" title="Screen mirror, Wallpaper Engine and Aura Sync">
          <Slider value={c.Fps} min={5} max={30} step={1} onInput={(v) => setCfg('Fps', v)} format={unit('')} /></Row>
        <Row kind="wide" label="JPEG quality"><Slider value={c.Quality} min={40} max={95} step={1} onInput={(v) => setCfg('Quality', v)} format={unit('')} /></Row>
      </Card>
      <Card caption="GAME MODE" sub="Lowers the load while you play: fewer frames to the cooler and slower sensor polling, or no streaming at all (the cooler then shows its own idle screen).">
        {detect && <Switch className="gap" on={c.GameDetect} onChange={(v) => setCfg('GameDetect', v)}>Detect running games</Switch>}
        <p className="sub tight">For a game that isn't detected, the gamepad button in the title bar turns game mode on by hand.</p>
        <div className="gm-options" style={{ opacity: gmOn ? 1 : 0.45, pointerEvents: gmOn ? '' : 'none' }}>
          {detect && (
            <Row kind="wide" label="Detect" top>
              <div className="wrap span">
                <Chip on={c.GameFullscreen} onChange={(v) => setCfg('GameFullscreen', v)} title={chipTitles.fs}>Full-screen games</Chip>
                <Chip on={c.GameKnownList} onChange={(v) => setCfg('GameKnownList', v)} title={chipTitles.known}>{linux ? 'Steam and Wine games' : 'Games known to Windows'}</Chip>
                <Chip on={c.GameBorderless} onChange={(v) => setCfg('GameBorderless', v)} title={chipTitles.any}>Any full-screen window</Chip>
              </div>
            </Row>
          )}
          {detect && (
            <Row kind="wide" label="Also these apps"><div className="span field">
              <TextField value={c.GameExtra} onCommit={(v) => setCfg('GameExtra', v.trim())} title="App names, comma separated, for example: eldenring.exe, cs2" /></div></Row>
          )}
          <Row kind="wide" label="While a game runs"><div className="span">
            <Seg value={c.GameAction} options={[['LowerFps', 'Lower frame rate'], ['IdleScreen', 'Show idle screen']]} onChange={(v) => setCfg('GameAction', v)} /></div></Row>
          {c.GameAction === 'LowerFps' && (
            <Row kind="wide" label="Game frame rate"><Slider value={c.GameFps} min={1} max={30} step={1} onInput={(v) => setCfg('GameFps', v)} format={unit(' fps')} /></Row>
          )}
          <Row kind="wide" label="Sensor updates">
            <Slider value={c.GameSensorMs / 1000} min={2} max={60} step={1} onInput={(v) => setCfg('GameSensorMs', Math.round(v * 1000))} format={unit(' s')} /></Row>
          {detect && (
            <Row kind="wide" label="Ignore these apps"><div className="span field">
              <TextField value={c.GameIgnore} onCommit={(v) => setCfg('GameIgnore', v.trim())} title="App names never treated as games, comma separated, for example: BongoCat" /></div></Row>
          )}
        </div>
        <div className="status">{ST ? ST.gameStatus : ''}</div>
      </Card>
      <Card caption="SENSORS" ref={sensors} flash={flashing} id="sensorsCard"
        sub={linux ? 'CPU temperature is read from the kernel\'s sensor driver (k10temp, zenpower or coretemp); nothing has to be installed.'
          : 'CPU temperature comes from TOWEI\'s sensor driver, which needs administrator rights. The TT LCD sensors service runs it at every boot, so nothing has to be enabled again.'}>
        <div className="hrow">
          <span className="lbl minw">{S.svc.busy ? S.svc.text : ST ? ST.svc : S.svc.text}</span>
          {P.sensorsManaged && <>
            <Button accent disabled={S.svc.installed || S.svc.busy} onClick={() => send({ t: 'cli', args: 'service install', what: 'svc' })}>Install service</Button>
            <Button disabled={!S.svc.installed || S.svc.busy} onClick={() => send({ t: 'cli', args: 'service uninstall', what: 'svc' })}>Uninstall service</Button>
          </>}
        </div>
      </Card>
      <Card caption="APPLICATION" sub="How the app starts and closes, and the files it keeps.">
        <Switch className="gap2" on={S.autostart} onChange={(v) => send({ t: 'autostart', v })}>
          {linux ? 'Start with the desktop session (in the tray)' : 'Start with Windows (in tray)'}</Switch>
        <Switch className="gap2" on={c.BlankOnExit} onChange={(v) => setCfg('BlankOnExit', v)}>Blank the screen when the app exits</Switch>
        <Switch on={c.CloseToTray} onChange={(v) => setCfg('CloseToTray', v)}>Keep running in the tray when the window closes</Switch>
        <div className="hair" />
        <div className="hrow top16">
          {P.video && <Button onClick={() => send({ t: 'clearCache' })}>Clear video cache</Button>}
          <Button onClick={() => send({ t: 'openFolder' })}>Open settings folder</Button>
        </div>
        <div className="note">{ST ? ST.cacheInfo : ''}</div>
      </Card>
    </section>
  );
}
