//! The setup's payload: the application files appended to the setup stub exe.
//!
//! Layout of a packed setup (all integers little-endian):
//!
//! ```text
//! [stub exe .............................]  TTLcdStudio-Setup.exe as built; the PE loader never looks past it
//! [file data ............................]  one record per file: raw deflate stream (method 1) or the bytes (method 0)
//! [index ................................]  per file: u16 path length, path (UTF-8, '/' separators, relative),
//!                                           u8 method, u64 data offset (from payload start), u64 stored length,
//!                                           u64 size, u32 CRC-32 (IEEE) of the original bytes
//! [zero padding to make the file length a multiple of 8]
//! [trailer, 40 bytes ....................]  b"TTLCDPK1", u32 format version (1), u32 file count,
//!                                           u64 payload start (= stub length), u64 index offset (from payload start),
//!                                           u64 index length
//! ```
//!
//! The setup finds the trailer at the end of its own file. A later Authenticode signature is appended after it (the
//! padding keeps the signature 8-byte aligned); `logical_end` skips it by reading the PE security directory.
use std::io::{self, Read, Seek, SeekFrom};
use std::path::Path;

pub const MAGIC: &[u8; 8] = b"TTLCDPK1";
pub const VERSION: u32 = 1;
pub const TRAILER_LEN: u64 = 40;
const METHOD_STORED: u8 = 0;
const METHOD_DEFLATE: u8 = 1;

#[derive(Clone, Debug, PartialEq)]
pub struct Entry {
    /// Relative path with '/' separators.
    pub path: String,
    pub method: u8,
    /// Offset of the stored bytes from the payload start.
    pub offset: u64,
    pub stored: u64,
    pub size: u64,
    pub crc: u32,
}

#[derive(Debug)]
pub struct Payload {
    /// Absolute file offset of the payload, which is also the stub's length.
    pub start: u64,
    pub entries: Vec<Entry>,
}

fn crc_table() -> &'static [u32; 256] {
    static TABLE: std::sync::OnceLock<[u32; 256]> = std::sync::OnceLock::new();
    TABLE.get_or_init(|| {
        let mut t = [0u32; 256];
        for (i, v) in t.iter_mut().enumerate() {
            let mut c = i as u32;
            for _ in 0..8 {
                c = if c & 1 != 0 { 0xEDB8_8320 ^ (c >> 1) } else { c >> 1 };
            }
            *v = c;
        }
        t
    })
}

/// CRC-32 (IEEE 802.3, as in zip/PNG).
pub fn crc32(data: &[u8]) -> u32 {
    let t = crc_table();
    !data.iter().fold(!0u32, |c, &b| t[((c ^ b as u32) & 0xFF) as usize] ^ (c >> 8))
}

/// A path is safe to extract when it is relative and stays inside the target folder.
pub fn safe_path(p: &str) -> bool {
    !p.is_empty()
        && !p.starts_with('/')
        && !p.contains('\\')
        && !p.contains(':')
        && !p.contains('\0')
        && p.split('/').all(|c| !c.is_empty() && c != "." && c != "..")
}

/// Every file under `dir`, recursively, as (relative '/' path, contents), sorted by path.
pub fn collect_dir(dir: &Path) -> io::Result<Vec<(String, Vec<u8>)>> {
    fn walk(base: &Path, dir: &Path, out: &mut Vec<(String, Vec<u8>)>) -> io::Result<()> {
        for e in std::fs::read_dir(dir)? {
            let e = e?;
            let p = e.path();
            let t = e.file_type()?;
            if t.is_dir() {
                walk(base, &p, out)?;
            } else if t.is_file() || (t.is_symlink() && p.is_file()) {
                let rel = p.strip_prefix(base).map_err(io::Error::other)?;
                let parts: Vec<String> = rel.components().map(|c| c.as_os_str().to_string_lossy().into_owned()).collect();
                out.push((parts.join("/"), std::fs::read(&p)?));
            }
        }
        Ok(())
    }
    let mut out = Vec::new();
    walk(dir, dir, &mut out)?;
    out.sort_by(|a, b| a.0.cmp(&b.0));
    Ok(out)
}

/// The packed setup: `stub` followed by the payload with `files`.
pub fn pack(stub: &[u8], files: &[(String, Vec<u8>)]) -> io::Result<Vec<u8>> {
    let start = stub.len() as u64;
    let mut out = stub.to_vec();
    let mut index = Vec::new();
    for (path, data) in files {
        if !safe_path(path) || path.len() > u16::MAX as usize {
            return Err(io::Error::new(io::ErrorKind::InvalidInput, format!("bad path in payload: {path}")));
        }
        let packed = miniz_oxide::deflate::compress_to_vec(data, 9);
        let (method, bytes) =
            if packed.len() < data.len() { (METHOD_DEFLATE, &packed[..]) } else { (METHOD_STORED, &data[..]) };
        let offset = out.len() as u64 - start;
        out.extend_from_slice(bytes);
        index.extend_from_slice(&(path.len() as u16).to_le_bytes());
        index.extend_from_slice(path.as_bytes());
        index.push(method);
        index.extend_from_slice(&offset.to_le_bytes());
        index.extend_from_slice(&(bytes.len() as u64).to_le_bytes());
        index.extend_from_slice(&(data.len() as u64).to_le_bytes());
        index.extend_from_slice(&crc32(data).to_le_bytes());
    }
    let index_offset = out.len() as u64 - start;
    out.extend_from_slice(&index);
    while (out.len() as u64 + TRAILER_LEN) % 8 != 0 {
        out.push(0);
    }
    out.extend_from_slice(MAGIC);
    out.extend_from_slice(&VERSION.to_le_bytes());
    out.extend_from_slice(&(files.len() as u32).to_le_bytes());
    out.extend_from_slice(&start.to_le_bytes());
    out.extend_from_slice(&index_offset.to_le_bytes());
    out.extend_from_slice(&(index.len() as u64).to_le_bytes());
    Ok(out)
}

fn read_at<R: Read + Seek>(r: &mut R, pos: u64, len: usize) -> io::Result<Vec<u8>> {
    r.seek(SeekFrom::Start(pos))?;
    let mut b = vec![0u8; len];
    r.read_exact(&mut b)?;
    Ok(b)
}

fn u16_at(b: &[u8], i: usize) -> u16 {
    u16::from_le_bytes(b[i..i + 2].try_into().unwrap())
}
fn u32_at(b: &[u8], i: usize) -> u32 {
    u32::from_le_bytes(b[i..i + 4].try_into().unwrap())
}
fn u64_at(b: &[u8], i: usize) -> u64 {
    u64::from_le_bytes(b[i..i + 8].try_into().unwrap())
}

/// The end of the file's own data: before an Authenticode signature (the PE security directory), if there is one.
pub fn logical_end<R: Read + Seek>(r: &mut R) -> io::Result<u64> {
    let len = r.seek(SeekFrom::End(0))?;
    let pe = (|| -> io::Result<Option<u64>> {
        let dos = read_at(r, 0, 64)?;
        if &dos[..2] != b"MZ" {
            return Ok(None);
        }
        let nt = u32_at(&dos, 0x3C) as u64;
        let hdr = read_at(r, nt, 24 + 2)?;
        if &hdr[..4] != b"PE\0\0" {
            return Ok(None);
        }
        let opt = nt + 24;
        let dirs = match u16_at(&hdr, 24) {
            0x10B => opt + 96,  // PE32
            0x20B => opt + 112, // PE32+
            _ => return Ok(None),
        };
        let sec = read_at(r, dirs + 4 * 8, 8)?; // IMAGE_DIRECTORY_ENTRY_SECURITY: file offset + size
        let (off, size) = (u32_at(&sec, 0) as u64, u32_at(&sec, 4) as u64);
        Ok((off != 0 && size != 0 && off + size <= len).then_some(off))
    })()
    .unwrap_or(None);
    Ok(pe.unwrap_or(len))
}

/// Finds and reads the payload index. Ok(None): the file carries no payload.
pub fn read_payload<R: Read + Seek>(r: &mut R) -> io::Result<Option<Payload>> {
    let end = logical_end(r)?;
    // the trailer ends the data; tolerate a few bytes of alignment padding a signing tool may have added
    let mut trailer = None;
    for pad in 0..8u64 {
        if end < TRAILER_LEN + pad {
            break;
        }
        let t = read_at(r, end - pad - TRAILER_LEN, TRAILER_LEN as usize)?;
        if &t[..8] == MAGIC {
            trailer = Some((t, end - pad - TRAILER_LEN));
            break;
        }
    }
    let Some((t, trailer_pos)) = trailer else { return Ok(None) };
    let bad = |m: &str| io::Error::new(io::ErrorKind::InvalidData, format!("damaged setup payload: {m}"));
    if u32_at(&t, 8) != VERSION {
        return Err(bad("unknown format version"));
    }
    let count = u32_at(&t, 12) as usize;
    let start = u64_at(&t, 16);
    let index_offset = u64_at(&t, 24);
    let index_len = u64_at(&t, 32);
    if start.checked_add(index_offset).and_then(|v| v.checked_add(index_len)).is_none_or(|v| v > trailer_pos) {
        return Err(bad("index out of range"));
    }
    let index = read_at(r, start + index_offset, index_len as usize)?;
    let mut entries = Vec::with_capacity(count);
    let mut i = 0usize;
    for _ in 0..count {
        if i + 2 > index.len() {
            return Err(bad("index truncated"));
        }
        let n = u16_at(&index, i) as usize;
        i += 2;
        if i + n + 29 > index.len() {
            return Err(bad("index truncated"));
        }
        let path = String::from_utf8(index[i..i + n].to_vec()).map_err(|_| bad("path is not UTF-8"))?;
        i += n;
        let e = Entry {
            path,
            method: index[i],
            offset: u64_at(&index, i + 1),
            stored: u64_at(&index, i + 9),
            size: u64_at(&index, i + 17),
            crc: u32_at(&index, i + 25),
        };
        i += 29;
        if !safe_path(&e.path) {
            return Err(bad(&format!("unsafe path {}", e.path)));
        }
        if e.offset.checked_add(e.stored).is_none_or(|v| v > index_offset) {
            return Err(bad(&format!("{} out of range", e.path)));
        }
        entries.push(e);
    }
    Ok(Some(Payload { start, entries }))
}

/// One file's original bytes, checked against its CRC.
pub fn extract<R: Read + Seek>(r: &mut R, p: &Payload, e: &Entry) -> io::Result<Vec<u8>> {
    let stored = read_at(r, p.start + e.offset, e.stored as usize)?;
    let data = match e.method {
        METHOD_STORED => stored,
        METHOD_DEFLATE => miniz_oxide::inflate::decompress_to_vec_with_limit(&stored, e.size as usize)
            .map_err(|err| io::Error::new(io::ErrorKind::InvalidData, format!("{}: {err:?}", e.path)))?,
        _ => return Err(io::Error::new(io::ErrorKind::InvalidData, format!("{}: unknown method", e.path))),
    };
    if data.len() as u64 != e.size || crc32(&data) != e.crc {
        return Err(io::Error::new(io::ErrorKind::InvalidData, format!("{} is damaged (checksum)", e.path)));
    }
    Ok(data)
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::io::Cursor;

    fn sample() -> Vec<(String, Vec<u8>)> {
        vec![
            ("TTLcdStudio.exe".into(), b"MZ fake exe ".repeat(5000)),
            ("driver/MttVDD.inf".into(), b"[Version]\r\nSignature=\"$Windows NT$\"\r\n".to_vec()),
            ("empty.txt".into(), Vec::new()),
            ("firmware/random.bin".into(), (0..4096u32).map(|i| (i.wrapping_mul(2654435761) >> 13) as u8).collect()),
        ]
    }

    #[test]
    fn crc32_known_value() {
        assert_eq!(crc32(b"123456789"), 0xCBF4_3926);
        assert_eq!(crc32(b""), 0);
    }

    #[test]
    fn round_trip() {
        let stub = b"MZ this is the stub".to_vec();
        let files = sample();
        let packed = pack(&stub, &files).unwrap();
        assert_eq!(packed.len() % 8, 0);
        assert_eq!(&packed[..stub.len()], &stub[..]);
        let mut r = Cursor::new(&packed);
        let p = read_payload(&mut r).unwrap().expect("payload");
        assert_eq!(p.start, stub.len() as u64);
        assert_eq!(p.entries.len(), files.len());
        for ((path, data), e) in files.iter().zip(&p.entries) {
            assert_eq!(&e.path, path);
            assert_eq!(&extract(&mut r, &p, e).unwrap(), data);
        }
        // the repetitive exe compresses, the random-ish firmware is stored
        assert_eq!(p.entries[0].method, METHOD_DEFLATE);
        assert!(p.entries[0].stored < p.entries[0].size);
    }

    #[test]
    fn no_payload() {
        let mut r = Cursor::new(b"MZ just a stub, no trailer here at all........".to_vec());
        assert!(read_payload(&mut r).unwrap().is_none());
        let mut r = Cursor::new(Vec::new());
        assert!(read_payload(&mut r).unwrap().is_none());
    }

    #[test]
    fn detects_damage() {
        let packed = pack(b"stub", &sample()).unwrap();
        let mut bad = packed.clone();
        bad[10] ^= 0x55; // inside the first file's data
        let mut r = Cursor::new(&bad);
        let p = read_payload(&mut r).unwrap().unwrap();
        assert!(extract(&mut r, &p, &p.entries[0]).is_err());
    }

    #[test]
    fn tolerates_signature_padding() {
        // a signing tool may pad before appending its certificate; without a PE header the padding stays at the end
        let mut packed = pack(b"stub", &sample()).unwrap();
        packed.extend_from_slice(&[0u8; 3]);
        let mut r = Cursor::new(&packed);
        assert_eq!(read_payload(&mut r).unwrap().unwrap().entries.len(), 4);
    }

    #[test]
    fn skips_authenticode_signature() {
        // a minimal PE32+ header whose security directory points at a "certificate" appended after the payload
        let mut stub = vec![0u8; 0x200];
        stub[..2].copy_from_slice(b"MZ");
        stub[0x3C..0x40].copy_from_slice(&0x80u32.to_le_bytes());
        stub[0x80..0x84].copy_from_slice(b"PE\0\0");
        stub[0x98..0x9A].copy_from_slice(&0x20Bu16.to_le_bytes());
        let mut signed = pack(&stub, &sample()).unwrap();
        let cert_at = signed.len() as u32;
        signed[0x128..0x12C].copy_from_slice(&cert_at.to_le_bytes());
        signed[0x12C..0x130].copy_from_slice(&24u32.to_le_bytes());
        signed.extend_from_slice(&[0xAB; 24]);
        let mut r = Cursor::new(&signed);
        assert_eq!(logical_end(&mut r).unwrap(), cert_at as u64);
        let p = read_payload(&mut r).unwrap().unwrap();
        assert_eq!(p.start, 0x200);
        assert_eq!(extract(&mut r, &p, &p.entries[1]).unwrap(), sample()[1].1);
    }

    #[test]
    fn rejects_unsafe_paths() {
        for p in ["../x", "/abs", "a/../b", "c:/x", "a\\b", "", "a//b", "./a"] {
            assert!(!safe_path(p), "{p}");
            assert!(pack(b"", &[(p.to_string(), vec![1])]).is_err());
        }
        assert!(safe_path("driver/x64/MttVDD.dll"));
    }

    #[test]
    fn collects_a_folder() {
        let dir = std::env::temp_dir().join(format!("ttlcd-pack-test-{}", std::process::id()));
        let _ = std::fs::remove_dir_all(&dir);
        std::fs::create_dir_all(dir.join("sub/deeper")).unwrap();
        std::fs::write(dir.join("a.txt"), b"a").unwrap();
        std::fs::write(dir.join("sub/deeper/b.bin"), b"bb").unwrap();
        let files = collect_dir(&dir).unwrap();
        std::fs::remove_dir_all(&dir).unwrap();
        assert_eq!(files, vec![("a.txt".to_string(), b"a".to_vec()), ("sub/deeper/b.bin".to_string(), b"bb".to_vec())]);
    }
}
