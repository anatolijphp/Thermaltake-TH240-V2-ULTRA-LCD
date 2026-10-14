//! Video clips (max 30 s) as 480x480 JPEG frames with crop/zoom/pan, cached on disk. Decoding uses Windows Media
//! Foundation (built into Windows, no codecs to install, see win/mf.rs); other systems have no decoder (as before).
use crate::render::{self, Picture};
use std::io::{self, Read, Write};
use std::path::PathBuf;

pub const MAX_SECONDS: f64 = 30.0;

#[derive(Clone, Debug, Default)]
pub struct Clip {
    pub frames: Vec<Vec<u8>>, // upright 480x480 JPEGs
    pub frame_ms: i32,
    pub source_seconds: f64,
}

#[derive(Clone, Debug)]
pub struct Options {
    pub path: String,
    pub start: f64,    // seconds
    pub length: f64,   // seconds, clamped to 30
    pub fps: i32,      // output frame rate, clamped 1..30
    pub zoom: f64,     // 1 = cover, >1 zooms in
    pub pan_x: f64,    // -1..1, which part of the cropped area to keep
    pub pan_y: f64,
    pub quality: i32,
}

impl Options {
    pub fn from_config(c: &crate::config::Config) -> Options {
        Options {
            path: c.VideoPath.clone(),
            start: c.VideoStart,
            length: c.VideoLength,
            fps: c.VideoFps,
            zoom: c.VideoZoom,
            pan_x: c.VideoPanX,
            pan_y: c.VideoPanY,
            quality: c.Quality,
        }
    }

    pub fn key(&self) -> String {
        let s = format!(
            "{}|{}|{:.2}|{:.2}|{}|{:.2}|{:.2}|{:.2}|{}",
            self.path,
            crate::util::mtime_key(&self.path),
            self.start,
            self.length,
            self.fps,
            self.zoom,
            self.pan_x,
            self.pan_y,
            self.quality
        );
        format!("{:016X}", crate::util::fnv64(&s))
    }
}

pub fn cache_dir() -> PathBuf {
    crate::config::local_folder().join("video")
}

pub fn supported() -> bool {
    cfg!(windows)
}

/// The clip, from the cache or decoded. progress: 0..1; return false from it to cancel (then Ok(None)).
pub fn load(o: &Options, progress: &mut dyn FnMut(f64) -> bool) -> io::Result<Option<Clip>> {
    let mut o = o.clone();
    o.length = o.length.clamp(0.5, MAX_SECONDS);
    o.fps = o.fps.clamp(1, 30);
    let cache = cache_dir().join(format!("{}.bin", o.key()));
    if let Some(c) = read_cache(&cache) {
        return Ok(Some(c));
    }
    let clip = decode(&o, progress)?;
    if let Some(c) = &clip {
        if !c.frames.is_empty() {
            write_cache(&cache, c);
        }
    }
    Ok(clip)
}

fn decode(o: &Options, progress: &mut dyn FnMut(f64) -> bool) -> io::Result<Option<Clip>> {
    #[cfg(windows)]
    {
        let mut clip = Clip { frame_ms: 1000 / o.fps, ..Default::default() };
        let step = 1.0 / o.fps as f64;
        let next = std::cell::Cell::new(o.start);
        let end = o.start + o.length;
        let mut cancelled = false;
        let mut r = crate::win::mf::Reader::open(&o.path)?;
        clip.source_seconds = r.duration;
        if o.start > 0.0 {
            r.seek(o.start);
        }
        r.read_frames(&mut |ts| ts >= next.get() - step / 2.0 || ts >= end, &mut |pic, ts| {
            if ts >= end {
                return false;
            }
            // the same source frame may cover several output slots (low-fps source)
            let jpg = render::jpeg(&crop_to_480(&pic, o), o.quality, false);
            while next.get() <= ts + step / 2.0 && next.get() < end {
                clip.frames.push(jpg.clone());
                next.set(next.get() + step);
            }
            if !progress(((ts - o.start) / o.length).min(1.0)) {
                cancelled = true;
                return false;
            }
            ts < end
        })?;
        Ok(if cancelled { None } else { Some(clip) })
    }
    #[cfg(not(windows))]
    {
        let _ = (o, progress);
        Err(io::Error::other("videos need the Windows version"))
    }
}

/// Single upright frame at time t (editor preview, idle picture from a video) and the source length.
pub fn thumbnail(o: &Options, t: f64) -> io::Result<(Option<tiny_skia::Pixmap>, f64)> {
    #[cfg(windows)]
    {
        let mut r = crate::win::mf::Reader::open(&o.path)?;
        let dur = r.duration;
        let mut result = None;
        r.seek(t);
        r.read_frames(&mut |ts| ts >= t - 0.05, &mut |pic, _| {
            result = Some(crop_to_480(&pic, o));
            false
        })?;
        Ok((result, dur))
    }
    #[cfg(not(windows))]
    {
        let _ = (o, t);
        Err(io::Error::other("videos need the Windows version"))
    }
}

pub fn duration(path: &str) -> io::Result<f64> {
    #[cfg(windows)]
    {
        Ok(crate::win::mf::Reader::open(path)?.duration)
    }
    #[cfg(not(windows))]
    {
        let _ = path;
        Err(io::Error::other("videos need the Windows version"))
    }
}

/// Cover-crop to a square, then zoom in and pan within the remaining area.
pub fn crop_to_480(src: &Picture, o: &Options) -> tiny_skia::Pixmap {
    let (w, h) = (src.width() as f64, src.height() as f64);
    let side = w.min(h) / o.zoom.max(1.0);
    let (max_x, max_y) = (w - side, h - side);
    let x = max_x / 2.0 * (1.0 + o.pan_x.clamp(-1.0, 1.0));
    let y = max_y / 2.0 * (1.0 + o.pan_y.clamp(-1.0, 1.0));
    let (xi, yi, si) = (x.round() as u32, y.round() as u32, side.round().max(1.0) as u32);
    let sub = image::imageops::crop_imm(src, xi.min(src.width() - 1), yi.min(src.height() - 1), si, si).to_image();
    render::compose(Some(&sub), false, 0)
}

pub fn clear_cache() {
    let _ = std::fs::remove_dir_all(cache_dir());
}

pub fn cache_bytes() -> u64 {
    std::fs::read_dir(cache_dir())
        .map(|d| d.flatten().filter_map(|e| e.metadata().ok()).map(|m| m.len()).sum())
        .unwrap_or(0)
}

// cache file: "VDCL" | frameMs i32 | sourceSeconds f64 | count i32 | (len i32, jpeg)*  (same as the C# version)
fn read_cache(file: &PathBuf) -> Option<Clip> {
    let mut f = std::fs::File::open(file).ok()?;
    let mut b = Vec::new();
    f.read_to_end(&mut b).ok()?;
    let rd = |o: usize| -> Option<i32> { Some(i32::from_le_bytes(b.get(o..o + 4)?.try_into().ok()?)) };
    if rd(0)? != 0x4C43_4456 {
        return None;
    }
    let frame_ms = rd(4)?;
    let source_seconds = f64::from_le_bytes(b.get(8..16)?.try_into().ok()?);
    let n = rd(16)? as usize;
    let mut o = 20;
    let mut frames = Vec::with_capacity(n);
    for _ in 0..n {
        let len = rd(o)? as usize;
        frames.push(b.get(o + 4..o + 4 + len)?.to_vec());
        o += 4 + len;
    }
    Some(Clip { frames, frame_ms, source_seconds })
}

fn write_cache(file: &PathBuf, c: &Clip) {
    let dir = cache_dir();
    if std::fs::create_dir_all(&dir).is_err() {
        return;
    }
    // keep the cache small: only the most recent few clips
    if let Ok(rd) = std::fs::read_dir(&dir) {
        let mut old: Vec<_> = rd.flatten().filter(|e| e.path().extension().is_some_and(|x| x == "bin")).collect();
        old.sort_by_key(|e| std::cmp::Reverse(e.metadata().and_then(|m| m.modified()).ok()));
        for e in old.iter().skip(4) {
            let _ = std::fs::remove_file(e.path());
        }
    }
    let Ok(mut f) = std::fs::File::create(file) else { return };
    let mut w = Vec::new();
    w.extend_from_slice(&0x4C43_4456i32.to_le_bytes());
    w.extend_from_slice(&c.frame_ms.to_le_bytes());
    w.extend_from_slice(&c.source_seconds.to_le_bytes());
    w.extend_from_slice(&(c.frames.len() as i32).to_le_bytes());
    for fr in &c.frames {
        w.extend_from_slice(&(fr.len() as i32).to_le_bytes());
        w.extend_from_slice(fr);
    }
    let _ = f.write_all(&w);
}
