// Device: brightness, rotation and sleep (stored in the cooler), the virtual display (Windows) and the connection.
import './Device.scss';
import Button from '../../components/Button/Button.jsx';
import Card from '../../components/Card/Card.jsx';
import Row from '../../components/Row/Row.jsx';
import Seg from '../../components/Seg/Seg.jsx';
import Slider, { unit } from '../../components/Slider/Slider.jsx';
import Switch from '../../components/Switch/Switch.jsx';
import { send, setCfg, useStore } from '../../bridge.js';

export default function Device() {
  const S = useStore((s) => s.S);
  const ST = useStore((s) => s.ST);
  const c = S.cfg;
  const os = S.plat.linux ? 'the system' : 'Windows';
  return (
    <section className="page device">
      <h1>Device</h1>
      <p className="sub">Screen settings that live on the cooler.</p>
      <Card caption="SCREEN" sub="Brightness, orientation and sleep. These are stored in the cooler.">
        <Row label="Brightness"><Slider value={c.Brightness} min={0} max={100} step={1} onInput={(v) => setCfg('Brightness', v)} format={unit('%')} /></Row>
        <Row label="Rotation"><div className="span">
          <Seg value={S.userRot} options={[[0, '0°'], [90, '90°'], [180, '180°'], [270, '270°']]} onChange={(v) => send({ t: 'rot', v })} /></div></Row>
        <Row label="Sleep"><div className="span"><Switch on={c.DeviceSleep} onChange={(v) => setCfg('DeviceSleep', v)}>Dim the screen when idle</Switch></div></Row>
        <div className="note">Rotation turns everything on the cooler to match how the pump is mounted: the live picture and the cooler's own idle and boot screens. Also the arrow on the preview.</div>
      </Card>
      {S.plat.windows && (
        <Card caption="VIRTUAL DISPLAY (WALLPAPER ENGINE)" sub="Adds a 480×480 monitor that Wallpaper Engine can draw on. Requires administrator rights; uninstall removes the driver completely.">
          <div className="hrow"><span className="lbl minw">{S.vd.text}</span>
            <Button accent disabled={S.vd.installed || S.vd.busy} onClick={() => send({ t: 'cli', args: 'display install', what: 'vd' })}>Install</Button>
            <Button disabled={!S.vd.installed || S.vd.busy} onClick={() => send({ t: 'cli', args: 'display uninstall', what: 'vd' })}>Uninstall</Button></div>
        </Card>
      )}
      <Card caption="CONNECTION" sub={`The cooler as ${os} sees it over USB.`}>
        <Row kind="2" label="Status"><span>{ST ? ST.devStatus : ''}</span></Row>
        <Row kind="2" label="Serial"><span>{ST ? ST.serial : ''}</span></Row>
        <Row kind="2" label="Frame rate"><span>{ST ? ST.devFps : ''}</span></Row>
      </Card>
    </section>
  );
}
