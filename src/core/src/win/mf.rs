//! Media Foundation source reader (built into Windows, no codecs to install): video frames as RGBA pictures, with the
//! decoder's padding cropped to the display aperture. Port of the Reader in Video.cs.
use crate::render::Picture;
use std::io;
use windows::core::{GUID, HSTRING};
use windows::Win32::Media::MediaFoundation::*;
use windows::Win32::System::Com::StructuredStorage::PROPVARIANT;
use windows::Win32::System::Com::{CoInitializeEx, COINIT_MULTITHREADED};

const FIRST_VIDEO: u32 = MF_SOURCE_READER_FIRST_VIDEO_STREAM.0 as u32;
const ALL_STREAMS: u32 = MF_SOURCE_READER_ALL_STREAMS.0 as u32;
const MEDIA_SOURCE: u32 = MF_SOURCE_READER_MEDIASOURCE.0 as u32;
const END_OF_STREAM: u32 = MF_SOURCE_READERF_ENDOFSTREAM.0 as u32;
const TYPE_CHANGED: u32 = MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED.0 as u32;

fn e(err: windows::core::Error) -> io::Error {
    io::Error::other(err.message())
}

fn startup() -> io::Result<()> {
    use std::sync::OnceLock;
    static STARTED: OnceLock<Result<(), String>> = OnceLock::new();
    unsafe {
        let _ = CoInitializeEx(None, COINIT_MULTITHREADED); // per thread; RPC_E_CHANGED_MODE is fine
    }
    STARTED.get_or_init(|| unsafe { MFStartup(MF_VERSION, MFSTARTUP_FULL).map_err(|x| x.message()) }).clone().map_err(io::Error::other)
}

pub struct Reader {
    reader: IMFSourceReader,
    width: i32,
    height: i32,
    stride: i32,
    vis: (i32, i32, i32, i32), // x, y, w, h of the visible picture
    pub duration: f64,
}

impl Reader {
    pub fn open(path: &str) -> io::Result<Reader> {
        startup()?;
        unsafe {
            let mut attr: Option<IMFAttributes> = None;
            MFCreateAttributes(&mut attr, 2).map_err(e)?;
            let attr = attr.ok_or_else(|| io::Error::other("MFCreateAttributes"))?;
            attr.SetUINT32(&MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, 1).map_err(e)?;
            let reader = MFCreateSourceReaderFromURL(&HSTRING::from(path), &attr).map_err(e)?;
            reader.SetStreamSelection(ALL_STREAMS, false).map_err(e)?;
            reader.SetStreamSelection(FIRST_VIDEO, true).map_err(e)?;
            let t = MFCreateMediaType().map_err(e)?;
            t.SetGUID(&MF_MT_MAJOR_TYPE, &MFMediaType_Video).map_err(e)?;
            t.SetGUID(&MF_MT_SUBTYPE, &MFVideoFormat_RGB32).map_err(e)?;
            reader.SetCurrentMediaType(FIRST_VIDEO, None, &t).map_err(e)?;
            let mut r = Reader { reader, width: 0, height: 0, stride: 0, vis: (0, 0, 0, 0), duration: 0.0 };
            r.read_format()?;
            if let Ok(pv) = r.reader.GetPresentationAttribute(MEDIA_SOURCE, &MF_PD_DURATION) {
                if let Ok(v) = u64::try_from(&pv).map(|v| v as i64).or_else(|_| i64::try_from(&pv)) {
                    r.duration = v as f64 / 1e7;
                }
            }
            Ok(r)
        }
    }

    fn read_format(&mut self) -> io::Result<()> {
        unsafe {
            let t = self.reader.GetCurrentMediaType(FIRST_VIDEO).map_err(e)?;
            let size = t.GetUINT64(&MF_MT_FRAME_SIZE).map_err(e)?;
            self.width = (size >> 32) as i32;
            self.height = (size & 0xFFFF_FFFF) as i32;
            self.stride = t.GetUINT32(&MF_MT_DEFAULT_STRIDE).map(|s| s as i32).unwrap_or(self.width * 4);
            // decoders pad to 16-pixel blocks (e.g. 600 -> 608); the aperture is the real picture
            self.vis = (0, 0, self.width, self.height);
            let mut area = [0u8; 16];
            let mut n = 0u32;
            if t.GetBlob(&MF_MT_MINIMUM_DISPLAY_APERTURE, &mut area, Some(&mut n)).is_ok() && n == 16 {
                // MFVideoArea: MFOffset x {WORD fract; short value}, MFOffset y, SIZE {int cx, cy}
                let ox = i16::from_le_bytes([area[2], area[3]]) as i32;
                let oy = i16::from_le_bytes([area[6], area[7]]) as i32;
                let cx = i32::from_le_bytes(area[8..12].try_into().unwrap());
                let cy = i32::from_le_bytes(area[12..16].try_into().unwrap());
                if cx > 0 && cy > 0 && ox >= 0 && oy >= 0 && ox + cx <= self.width && oy + cy <= self.height {
                    self.vis = (ox, oy, cx, cy);
                }
            }
            if std::env::var_os("TTLCD_DEBUG").is_some() {
                println!("video {}x{} stride {} aperture {:?}", self.width, self.height, self.stride, self.vis);
            }
        }
        Ok(())
    }

    pub fn seek(&mut self, seconds: f64) {
        unsafe {
            let pv = PROPVARIANT::from((seconds * 1e7) as i64);
            let _ = self.reader.SetCurrentPosition(&GUID::zeroed(), &pv);
        }
    }

    /// want(ts): decode this frame?  got(picture, ts): return false to stop.
    pub fn read_frames(&mut self, want: &mut dyn FnMut(f64) -> bool, got: &mut dyn FnMut(Picture, f64) -> bool) -> io::Result<()> {
        loop {
            let (mut flags, mut ts) = (0u32, 0i64);
            let mut sample: Option<IMFSample> = None;
            unsafe {
                self.reader.ReadSample(FIRST_VIDEO, 0, None, Some(&mut flags), Some(&mut ts), Some(&mut sample)).map_err(e)?;
            }
            if flags & TYPE_CHANGED != 0 {
                self.read_format()?;
            }
            let Some(sample) = sample else {
                if flags & END_OF_STREAM != 0 {
                    return Ok(());
                }
                continue;
            };
            let sec = ts as f64 / 1e7;
            if !want(sec) {
                if sec > 0.0 && flags & END_OF_STREAM != 0 {
                    return Ok(());
                }
                continue;
            }
            let pic = self.copy(&sample)?;
            if !got(pic, sec) || flags & END_OF_STREAM != 0 {
                return Ok(());
            }
        }
    }

    fn copy(&self, sample: &IMFSample) -> io::Result<Picture> {
        let (vx, vy, vw, vh) = self.vis;
        let mut out = vec![0u8; (vw.max(1) * vh.max(1) * 4) as usize];
        unsafe {
            let buf = sample.ConvertToContiguousBuffer().map_err(e)?;
            let mut p: *mut u8 = std::ptr::null_mut();
            let mut cur = 0u32;
            buf.Lock(&mut p, None, Some(&mut cur)).map_err(e)?;
            let abs = self.stride.unsigned_abs() as usize;
            let row = vw as usize * 4;
            for y in 0..vh as usize {
                // negative stride = bottom-up image
                let src_row = if self.stride < 0 { self.height as usize - 1 - (vy as usize + y) } else { vy as usize + y };
                let off = src_row * abs + vx as usize * 4;
                if off + row > cur as usize {
                    break;
                }
                let src = std::slice::from_raw_parts(p.add(off), row);
                let dst = &mut out[y * row..(y + 1) * row];
                for (d, s) in dst.chunks_exact_mut(4).zip(src.chunks_exact(4)) {
                    d[0] = s[2]; // BGRX -> RGBA
                    d[1] = s[1];
                    d[2] = s[0];
                    d[3] = 255;
                }
            }
            let _ = buf.Unlock();
        }
        image::RgbaImage::from_raw(vw.max(1) as u32, vh.max(1) as u32, out).ok_or_else(|| io::Error::other("bad frame size"))
    }
}
