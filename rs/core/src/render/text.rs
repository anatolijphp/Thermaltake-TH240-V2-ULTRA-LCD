//! Text as vector paths, like GDI+'s GraphicsPath.AddString: system fonts found through fontdb (no fontconfig or
//! DirectWrite needed), shaped with rustybuzz, glyph outlines turned into tiny-skia paths.
use std::collections::HashMap;
use std::sync::{Arc, Mutex, OnceLock};
use tiny_skia::{Path, PathBuilder};

struct Face {
    data: Arc<Vec<u8>>,
    index: u32,
}

pub struct Fonts {
    db: fontdb::Database,
    faces: Mutex<HashMap<(String, bool, bool), Option<Arc<Face>>>>,
    files: Mutex<HashMap<std::path::PathBuf, Arc<Vec<u8>>>>,
}

/// Sans-serif fallbacks for a family the system doesn't have ("Segoe UI" on Linux, as fontconfig would pick).
const FALLBACK: &[&str] = &["Segoe UI", "DejaVu Sans", "Ubuntu", "Noto Sans", "Liberation Sans", "Cantarell", "Arial"];

pub fn fonts() -> &'static Fonts {
    static F: OnceLock<Fonts> = OnceLock::new();
    F.get_or_init(|| {
        let mut db = fontdb::Database::new();
        db.load_system_fonts();
        #[cfg(windows)]
        if let Some(local) = std::env::var_os("LOCALAPPDATA") {
            db.load_fonts_dir(std::path::PathBuf::from(local).join("Microsoft\\Windows\\Fonts")); // per-user installs
        }
        Fonts { db, faces: Mutex::new(HashMap::new()), files: Mutex::new(HashMap::new()) }
    })
}

impl Fonts {
    /// Installed font families, sorted (the Studio's font list).
    pub fn families(&self) -> Vec<String> {
        let mut v: Vec<String> = self.db.faces().filter_map(|f| f.families.first().map(|(n, _)| n.clone())).collect();
        v.sort_by_key(|s| s.to_lowercase());
        v.dedup();
        v
    }

    pub fn has_family(&self, name: &str) -> bool {
        self.db.faces().any(|f| f.families.iter().any(|(n, _)| n.eq_ignore_ascii_case(name)))
    }

    fn face(&self, family: &str, bold: bool, italic: bool) -> Option<Arc<Face>> {
        let key = (family.to_owned(), bold, italic);
        if let Some(f) = self.faces.lock().unwrap().get(&key) {
            return f.clone();
        }
        let mut names: Vec<&str> = Vec::new();
        if !family.is_empty() && self.has_family(family) {
            names.push(family);
        }
        let mut found = None;
        // Linux: a family that isn't installed gets fontconfig's substitute (what the C# version drew with)
        #[cfg(target_os = "linux")]
        if names.is_empty() {
            found = fc_match(&format!("{}:weight={}:slant={}", if family.is_empty() { "sans-serif" } else { family }, if bold { "bold" } else { "regular" }, if italic { "italic" } else { "roman" }))
                .and_then(|(p, i)| self.load_file(&p, i));
        }
        names.extend(FALLBACK.iter().filter(|n| self.has_family(n)));
        for n in names {
            if found.is_some() {
                break;
            }
            let q = fontdb::Query {
                families: &[fontdb::Family::Name(n)],
                weight: if bold { fontdb::Weight::BOLD } else { fontdb::Weight::NORMAL },
                stretch: fontdb::Stretch::Normal,
                style: if italic { fontdb::Style::Italic } else { fontdb::Style::Normal },
            };
            if let Some(id) = self.db.query(&q) {
                found = self.load(id);
                if found.is_some() {
                    break;
                }
            }
        }
        if found.is_none() {
            found = self.db.faces().next().and_then(|f| self.load(f.id)); // anything at all
        }
        self.faces.lock().unwrap().insert(key, found.clone());
        found
    }

    fn load_file(&self, p: &std::path::Path, index: u32) -> Option<Arc<Face>> {
        let data = {
            let mut files = self.files.lock().unwrap();
            match files.get(p) {
                Some(d) => d.clone(),
                None => {
                    let d = Arc::new(std::fs::read(p).ok()?);
                    files.insert(p.to_path_buf(), d.clone());
                    d
                }
            }
        };
        rustybuzz::Face::from_slice(&data, index)?;
        Some(Arc::new(Face { data, index }))
    }

    /// Faces tried for characters the chosen font lacks (fontconfig's sans-serif first on Linux).
    fn fallbacks(&self, bold: bool) -> Vec<Arc<Face>> {
        let key = (String::from("\u{0}fallback"), bold, false);
        let mut v = Vec::new();
        if let Some(Some(f)) = self.faces.lock().unwrap().get(&key) {
            v.push(f.clone());
        }
        if v.is_empty() {
            #[cfg(target_os = "linux")]
            let first = fc_match(if bold { "sans-serif:weight=bold" } else { "sans-serif" }).and_then(|(p, i)| self.load_file(&p, i));
            #[cfg(not(target_os = "linux"))]
            let first: Option<Arc<Face>> = None;
            let first = first.or_else(|| self.face("", bold, false));
            self.faces.lock().unwrap().insert(key, first.clone());
            v.extend(first);
        }
        for n in ["Segoe UI Symbol", "Segoe UI Emoji", "DejaVu Sans", "Noto Sans Symbols", "Noto Sans Symbols2"] {
            if self.has_family(n) {
                if let Some(f) = self.face(n, bold, false) {
                    v.push(f);
                }
            }
        }
        v
    }

    fn load(&self, id: fontdb::ID) -> Option<Arc<Face>> {
        let info = self.db.face(id)?;
        let data = match &info.source {
            fontdb::Source::File(p) => {
                let mut files = self.files.lock().unwrap();
                match files.get(p) {
                    Some(d) => d.clone(),
                    None => {
                        let d = Arc::new(std::fs::read(p).ok()?);
                        files.insert(p.clone(), d.clone());
                        d
                    }
                }
            }
            fontdb::Source::Binary(b) => Arc::new(b.as_ref().as_ref().to_vec()),
            #[allow(unreachable_patterns)]
            _ => return None,
        };
        rustybuzz::Face::from_slice(&data, info.index)?;
        Some(Arc::new(Face { data, index: info.index }))
    }

    /// The text as one path: `size` = em size in px, (x, top) = top-left of the line (baseline at top + ascent),
    /// like AddString with StringFormat.GenericTypographic. None for empty text or no usable font.
    pub fn path(&self, family: &str, bold: bool, italic: bool, size: f32, x: f32, top: f32, text: &str) -> Option<Path> {
        if text.is_empty() {
            return None;
        }
        let primary = self.face(family, bold, italic)?;
        let pface = rustybuzz::Face::from_slice(&primary.data, primary.index)?;
        let ascent = pface.tables().os2.map(|o| o.windows_ascender() as f32).filter(|a| *a > 0.0).unwrap_or(pface.ascender() as f32);
        let baseline = top + ascent * size / pface.units_per_em() as f32;
        let mut pb = PathBuilder::new();
        let mut pen = x;
        for (face, run) in self.runs(&primary, bold, text) {
            let Some(rf) = rustybuzz::Face::from_slice(&face.data, face.index) else { continue };
            let k = size / rf.units_per_em() as f32;
            let mut buf = rustybuzz::UnicodeBuffer::new();
            buf.push_str(&run);
            let glyphs = rustybuzz::shape(&rf, &[], buf);
            for (info, pos) in glyphs.glyph_infos().iter().zip(glyphs.glyph_positions()) {
                let mut ob = Outline { pb: &mut pb, k, x: pen + pos.x_offset as f32 * k, y: baseline - pos.y_offset as f32 * k };
                rf.outline_glyph(ttf_parser::GlyphId(info.glyph_id as u16), &mut ob);
                pen += pos.x_advance as f32 * k;
            }
        }
        pb.finish()
    }

    /// Splits text into runs drawn with the primary face, or a fallback face for characters it lacks.
    fn runs(&self, primary: &Arc<Face>, bold: bool, text: &str) -> Vec<(Arc<Face>, String)> {
        let has = |f: &Face, c: char| rustybuzz::Face::from_slice(&f.data, f.index).is_some_and(|ff| ff.glyph_index(c).is_some());
        let mut fb: Option<Vec<Arc<Face>>> = None;
        let mut out: Vec<(Arc<Face>, String)> = Vec::new();
        for c in text.chars() {
            let face = if c.is_whitespace() || has(primary, c) {
                primary.clone()
            } else {
                let list = fb.get_or_insert_with(|| self.fallbacks(bold));
                list.iter().find(|f| has(f, c)).cloned().unwrap_or_else(|| primary.clone())
            };
            match out.last_mut() {
                Some((f, s)) if Arc::ptr_eq(f, &face) => s.push(c),
                _ => out.push((face, c.to_string())),
            }
        }
        out
    }

    /// Advance width of the text in px (for word wrapping).
    pub fn width(&self, family: &str, bold: bool, size: f32, text: &str) -> f32 {
        let Some(f) = self.face(family, bold, false) else { return 0.0 };
        let Some(face) = rustybuzz::Face::from_slice(&f.data, f.index) else { return 0.0 };
        let mut buf = rustybuzz::UnicodeBuffer::new();
        buf.push_str(text);
        let g = rustybuzz::shape(&face, &[], buf);
        g.glyph_positions().iter().map(|p| p.x_advance as f32).sum::<f32>() * size / face.units_per_em() as f32
    }

    /// Line height (ascent + descent) in px, as GDI+ lays out wrapped text.
    pub fn line_height(&self, family: &str, bold: bool, size: f32) -> f32 {
        let Some(f) = self.face(family, bold, false) else { return size * 1.2 };
        let Some(face) = rustybuzz::Face::from_slice(&f.data, f.index) else { return size * 1.2 };
        let (a, d) = match face.tables().os2 {
            Some(o) if o.windows_ascender() > 0 => (o.windows_ascender() as f32, o.windows_descender().unsigned_abs() as f32),
            _ => (face.ascender() as f32, face.descender().unsigned_abs() as f32),
        };
        (a + d) * size / face.units_per_em() as f32
    }
}

struct Outline<'a> {
    pb: &'a mut PathBuilder,
    k: f32,
    x: f32,
    y: f32,
}

impl Outline<'_> {
    fn p(&self, x: f32, y: f32) -> (f32, f32) {
        (self.x + x * self.k, self.y - y * self.k)
    }
}

impl ttf_parser::OutlineBuilder for Outline<'_> {
    fn move_to(&mut self, x: f32, y: f32) {
        let (a, b) = self.p(x, y);
        self.pb.move_to(a, b);
    }
    fn line_to(&mut self, x: f32, y: f32) {
        let (a, b) = self.p(x, y);
        self.pb.line_to(a, b);
    }
    fn quad_to(&mut self, x1: f32, y1: f32, x: f32, y: f32) {
        let (a, b) = self.p(x1, y1);
        let (c, d) = self.p(x, y);
        self.pb.quad_to(a, b, c, d);
    }
    fn curve_to(&mut self, x1: f32, y1: f32, x2: f32, y2: f32, x: f32, y: f32) {
        let (a, b) = self.p(x1, y1);
        let (c, d) = self.p(x2, y2);
        let (e, f) = self.p(x, y);
        self.pb.cubic_to(a, b, c, d, e, f);
    }
    fn close(&mut self) {
        self.pb.close();
    }
}

/// fontconfig's best match for a pattern ("Segoe UI:weight=bold"): (file, face index). Cached; None without fc-match.
#[cfg(target_os = "linux")]
fn fc_match(pattern: &str) -> Option<(std::path::PathBuf, u32)> {
    static CACHE: OnceLock<Mutex<HashMap<String, Option<(std::path::PathBuf, u32)>>>> = OnceLock::new();
    let cache = CACHE.get_or_init(Default::default);
    if let Some(v) = cache.lock().unwrap().get(pattern) {
        return v.clone();
    }
    let r = std::process::Command::new("fc-match")
        .args(["-f", "%{file}|%{index}", pattern])
        .output()
        .ok()
        .and_then(|o| {
            let s = String::from_utf8_lossy(&o.stdout).into_owned();
            let (f, i) = s.split_once('|')?;
            Some((std::path::PathBuf::from(f), i.trim().parse().unwrap_or(0)))
        })
        .filter(|(f, _)| f.exists());
    cache.lock().unwrap().insert(pattern.to_owned(), r.clone());
    r
}
