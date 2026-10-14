//! Pictures the cooler keeps in its own flash and shows without the PC:
//!  - idle (standby): shown when no frames arrive (PC asleep / no software running)
//!  - loading (boot animation, boot.pak): played while the PC boots
//! The idle picture is written only on the LX800 firmware, where the write is verified by read-back. Uploads to the
//! original Thermaltake firmware erased its flash once (see CLAUDE.md), so they are refused, and so is the boot
//! animation: the LX800 firmware's boot file upload was never verified.
use crate::lcd::{Lcd, STANDBY_FILE, STANDBY_MAX_LX};
use crate::render;
use std::io;

fn is_video(p: &str) -> bool {
    let e = p.rsplit('.').next().unwrap_or("").to_ascii_lowercase();
    crate::idle::VIDEO_EXT.contains(&e.as_str())
}

/// Upright 480x480 JPEG of a picture, a GIF's first frame or a video's first frame, turned by `rotation`.
pub fn standby_jpeg(path: &str, rotation: i32, quality: i32) -> io::Result<Vec<u8>> {
    let frame = if is_video(path) {
        let o = crate::video::Options { path: path.into(), start: 0.0, length: 1.0, fps: 1, zoom: 1.0, pan_x: 0.0, pan_y: 0.0, quality };
        crate::video::thumbnail(&o, 0.0)?.0.ok_or_else(|| io::Error::other("could not decode the video"))?
    } else {
        let pic = crate::media::load_picture(path).ok_or_else(|| io::Error::other(format!("cannot read {path}")))?;
        render::compose(Some(&pic), false, 0)
    };
    Ok(render::jpeg(&render::rotate(&frame, rotation), quality, false))
}

/// Store the idle picture (LX800 firmware): the panel's red/blue swap is baked in, the 950 KB cap is enforced
/// (re-encoding to fit), and the write is read back and byte-compared. Returns whether the read-back matched.
/// progress: writing 0..80 %, reading back 80..100 %.
pub fn write_standby(lcd: &Lcd, jpeg: &[u8], progress: &mut dyn FnMut(i32)) -> io::Result<bool> {
    if !lcd.swap_red_blue {
        return Err(io::Error::other("storing an idle image is disabled on this firmware (an upload erased the flash once)"));
    }
    let data = fit_jpeg(&lcd.for_panel(jpeg), STANDBY_MAX_LX)?;
    lcd.upload_file_lx(STANDBY_FILE, &data, &mut |p| progress(p * 80 / 100))?;
    let back = lcd.read_file_lx(STANDBY_FILE, &mut |p| progress(80 + p / 5))?;
    Ok(back == data)
}

/// Re-encode the JPEG at lower quality until it fits max_bytes.
fn fit_jpeg(jpeg: &[u8], max_bytes: usize) -> io::Result<Vec<u8>> {
    if jpeg.len() <= max_bytes {
        return Ok(jpeg.to_vec());
    }
    let mut q = 85;
    while q >= 30 {
        if let Some(r) = render::recompress(jpeg, q, false) {
            if r.len() <= max_bytes {
                return Ok(r);
            }
        }
        q -= 10;
    }
    Err(io::Error::other(format!("idle picture is too large for the cooler ({} KB limit)", max_bytes / 1024)))
}

/// boot.pak (the TT RGB Plus boot animation archive): one folder record followed by an all-zero terminator record.
///  record header (16 bytes, LE): i32 check, i32 a, i32 b, u16 nameLen (incl. NUL), u16 flags
///    check = -(a + b + nameLen + (flags & 0xFFF))
///  folder: a = total record size, b = 16 * fileCount, flags = 0x1000 | fileCount
///          header, name, fileCount file headers, then (name, data) per file
///  file:   a = offset of its name from the folder header, b = size, flags = 0
///  files are ordered by name length, then name.
/// Kept for reference and the format tests; nothing uploads it (see the module notes).
pub fn pack(root: &str, files: &[(String, Vec<u8>)]) -> Vec<u8> {
    let z = |s: &str| {
        let mut v = s.as_bytes().to_vec();
        v.push(0);
        v
    };
    let mut sorted: Vec<&(String, Vec<u8>)> = files.iter().collect();
    sorted.sort_by(|a, b| a.0.len().cmp(&b.0.len()).then(a.0.cmp(&b.0)));
    let count = sorted.len() as i32;
    let root_name = z(root);
    let start = 16 + root_name.len() as i32 + 16 * count;
    let body: i32 = sorted.iter().map(|f| (z(&f.0).len() + f.1.len()) as i32).sum();
    let mut w = Vec::new();
    let header = |w: &mut Vec<u8>, a: i32, b: i32, name_len: i32, flags: i32| {
        w.extend_from_slice(&(-(a + b + name_len + (flags & 0xFFF))).to_le_bytes());
        w.extend_from_slice(&a.to_le_bytes());
        w.extend_from_slice(&b.to_le_bytes());
        w.extend_from_slice(&(name_len as u16).to_le_bytes());
        w.extend_from_slice(&(flags as u16).to_le_bytes());
    };
    header(&mut w, start + body, 16 * count, root_name.len() as i32, 0x1000 | count);
    w.extend_from_slice(&root_name);
    let mut off = start;
    for f in &sorted {
        let nl = z(&f.0).len() as i32;
        header(&mut w, off, f.1.len() as i32, nl, 0);
        off += nl + f.1.len() as i32;
    }
    for f in &sorted {
        w.extend_from_slice(&z(&f.0));
        w.extend_from_slice(&f.1);
    }
    w.extend_from_slice(&[0u8; 14]);
    w.extend_from_slice(&0x1000u16.to_le_bytes()); // terminator
    w
}

#[cfg(test)]
mod tests {
    #[test]
    fn pack_layout() {
        let p = super::pack("R", &[("b_.jpg".into(), vec![1, 2]), ("a.jpg".into(), vec![3])]);
        // folder header: a = total, b = 32, nameLen 2, flags 0x1002
        let i = |o: usize| i32::from_le_bytes(p[o..o + 4].try_into().unwrap());
        assert_eq!(i(8), 32);
        assert_eq!(u16::from_le_bytes([p[14], p[15]]), 0x1002);
        assert_eq!(i(0), -(i(4) + 32 + 2 + 2));
        // shorter name first
        assert_eq!(&p[16 + 2 + 32..16 + 2 + 32 + 6], b"a.jpg\0");
        assert_eq!(p.len(), i(4) as usize + 16);
    }
}
