// Video options (Windows): file, the part to play (start, length), zoom and pan to frame it, frame rate, a round
// thumbnail of the first frame and the preparation progress. Values go to the app 0.5 s after the last change (the
// clip is re-encoded); the thumbnail after 0.15 s.
import './VideoEditor.scss';
import { useEffect, useRef, useState } from 'react';
import Button from '../../../components/Button/Button.jsx';
import Progress from '../../../components/Progress/Progress.jsx';
import Row from '../../../components/Row/Row.jsx';
import Slider from '../../../components/Slider/Slider.jsx';
import TextField from '../../../components/TextField/TextField.jsx';
import { send, useStore } from '../../../bridge.js';
import { fmtTime } from '../../../util.js';

export default function VideoEditor({ cfg }) {
  const video = useStore((s) => s.video);
  const progress = useStore((s) => (s.ST ? s.ST.videoProgress : -1));
  const fromCfg = () => ({ start: cfg.VideoStart, length: cfg.VideoLength, zoom: cfg.VideoZoom, panx: cfg.VideoPanX, pany: cfg.VideoPanY, fps: cfg.VideoFps });
  const [v, setV] = useState(fromCfg);
  const edited = useRef(0);
  const timers = useRef({});
  // follow the settings unless the user just moved a slider
  useEffect(() => { if (Date.now() - edited.current > 1500) setV(fromCfg()); },
    [cfg.VideoStart, cfg.VideoLength, cfg.VideoZoom, cfg.VideoPanX, cfg.VideoPanY, cfg.VideoFps]);
  const input = (k) => (x) => {
    const nv = { ...v, [k]: x };
    setV(nv);
    edited.current = Date.now();
    clearTimeout(timers.current.apply);
    timers.current.apply = setTimeout(() => send({ t: 'video', start: nv.start, length: nv.length, zoom: nv.zoom, panx: nv.panx, pany: nv.pany, fps: Math.round(nv.fps) }), 500);
    clearTimeout(timers.current.thumb);
    timers.current.thumb = setTimeout(() => send({ t: 'thumb', start: nv.start, zoom: nv.zoom, panx: nv.panx, pany: nv.pany }), 150);
  };
  const dur = video.duration || 0;
  const info = dur > 0 ? `Source ${fmtTime(dur)} · clip ${fmtTime(v.start)}–${fmtTime(Math.min(dur, v.start + v.length))}` : video.info;
  return (
    <div className="video-editor">
      <div className="pathrow">
        <TextField readOnly value={cfg.VideoPath} />
        <Button accent onClick={() => send({ t: 'browse', what: 'video' })}>Browse…</Button>
      </div>
      <div className="videogrid">
        <div>
          <Row kind="compact" label="Start"><Slider value={v.start} min={0} max={dur > 0 ? Math.max(0, dur - 1) : 30} step={0.1} onInput={input('start')} format={fmtTime} /></Row>
          <Row kind="compact" label="Length"><Slider value={v.length} min={1} max={dur > 0 ? Math.max(1, Math.min(30, dur)) : 30} step={0.1} onInput={input('length')} format={(x) => x.toFixed(1) + ' s'} /></Row>
          <Row kind="compact" label="Zoom"><Slider value={v.zoom} min={1} max={4} step={0.01} onInput={input('zoom')} format={(x) => x.toFixed(1) + '×'} /></Row>
          <Row kind="compact" label="Pan X"><Slider value={v.panx} min={-1} max={1} step={0.01} onInput={input('panx')} /><span /></Row>
          <Row kind="compact" label="Pan Y"><Slider value={v.pany} min={-1} max={1} step={0.01} onInput={input('pany')} /><span /></Row>
          <Row kind="compact" label="FPS"><Slider value={v.fps} min={5} max={30} step={1} onInput={input('fps')} format={(x) => String(Math.round(x))} /></Row>
        </div>
        <div className="thumbcol">
          <div className="vthumb">{video.thumb != null && <img src={'thumb/' + video.thumb + '.jpg'} alt="" />}</div>
          <div className="note vinfo">{info}</div>
        </div>
      </div>
      {progress >= 0 && <Progress frac={progress} />}
    </div>
  );
}
