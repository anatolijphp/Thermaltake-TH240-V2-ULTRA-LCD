//! Clock face and overlay readings on the round panel. Every line is placed inside the circle (radius FIT_R around the
//! centre) and the whole block shrinks until each line fits the chord at its height. Lines are arranged by drag and
//! drop and saved as "0,2|3|" (ids, ',' within a line, '|' ends a line).
use super::{paint, rgb, rgba, text::fonts, LocalTime};
use crate::config::{Config, ElementStyle, TextEffect, TextLayout};
use tiny_skia::{Color, FillRule, LineJoin, Path, Pixmap, Rect, Stroke, Transform};

const CX: f32 = 240.0;
const CY: f32 = 240.0;
const FIT_R: f32 = 226.0;

pub const ID_TIME: i32 = 0;
pub const ID_DATE: i32 = 1;
pub const ID_WATER: i32 = 2;
pub const ID_CPU_TEMP: i32 = 3;
pub const ID_LOAD: i32 = 4;
pub const CLOCK_IDS: [i32; 5] = [ID_TIME, ID_DATE, ID_WATER, ID_CPU_TEMP, ID_LOAD];
pub const OVERLAY_IDS: [i32; 3] = [ID_TIME, ID_WATER, ID_CPU_TEMP];

pub fn water_valid(c: f64) -> bool {
    (-5.0..=99.0).contains(&c)
}

pub fn deg(c: f64, fahrenheit: bool) -> String {
    if fahrenheit { format!("{}°F", (c * 9.0 / 5.0 + 32.0).round()) } else { format!("{}°C", c.round()) }
}

/// cool -> hot over lo..hi: cyan, green, yellow, red
pub fn heat(v: f64, lo: f64, hi: f64) -> Color {
    let k = ((v - lo) / (hi - lo)).clamp(0.0, 1.0);
    let h = 3.0 * (1.0 - k);
    let i = h as i32;
    let f = h - i as f64;
    let (r, g, b) = match i {
        0 => (1.0, f, 0.0),
        1 => (1.0 - f, 1.0, 0.0),
        2 => (0.0, 1.0, f),
        _ => (0.0, 1.0, 1.0),
    };
    rgba((r * 255.0) as u8, (g * 255.0) as u8, (b * 255.0) as u8, 255)
}

// ---- line arrangement ----

/// The readings' lines: "0,2|3|" = line 1: time + water, line 2: CPU. Saved strings always contain '|'. Older configs
/// (plain order + layout setting) are converted: Stack = one reading per line, Custom = its "new line after" mask,
/// Line / Auto = all on one line. A reading missing from the string gets its own line at the end.
pub fn lines(order: &str, legacy: TextLayout, breaks: i32, all: &[i32]) -> Vec<Vec<i32>> {
    let mut out: Vec<Vec<i32>> = Vec::new();
    let mut seen = Vec::new();
    if order.contains('|') {
        let parts: Vec<&str> = order.split('|').collect();
        for part in &parts[..parts.len() - 1] {
            let mut line = Vec::new();
            for p in part.split(',') {
                if let Ok(id) = p.trim().parse::<i32>() {
                    if all.contains(&id) && !seen.contains(&id) {
                        seen.push(id);
                        line.push(id);
                    }
                }
            }
            out.push(line);
        }
        trim_empty_ends(&mut out);
        for &id in all {
            if !seen.contains(&id) {
                out.push(vec![id]);
            }
        }
        return out;
    }
    let ids = parse_order(order, all);
    let mut cur = Vec::new();
    for (i, &id) in ids.iter().enumerate() {
        cur.push(id);
        let brk = legacy == TextLayout::Stack || (legacy == TextLayout::Custom && (breaks >> id & 1) != 0);
        if brk || i == ids.len() - 1 {
            out.push(std::mem::take(&mut cur));
        }
    }
    out
}

/// "0,2|" + "|" (empty line) + "3|"  ->  "0,2||3|"
pub fn lines_to_string(lines: &[Vec<i32>]) -> String {
    let mut s = String::new();
    for l in lines {
        s.push_str(&l.iter().map(|i| i.to_string()).collect::<Vec<_>>().join(","));
        s.push('|');
    }
    s
}

/// Empty lines only make sense between readings; at the top or bottom they would just shift the block.
pub fn trim_empty_ends(lines: &mut Vec<Vec<i32>>) {
    while lines.first().is_some_and(|l| l.is_empty()) {
        lines.remove(0);
    }
    while lines.last().is_some_and(|l| l.is_empty()) {
        lines.pop();
    }
}

/// "2,0,3" -> [2,0,3] followed by any of `all` missing from it.
pub fn parse_order(order: &str, all: &[i32]) -> Vec<i32> {
    let mut r = Vec::new();
    for p in order.split(',') {
        if let Ok(id) = p.trim().parse::<i32>() {
            if all.contains(&id) && !r.contains(&id) {
                r.push(id);
            }
        }
    }
    for &id in all {
        if !r.contains(&id) {
            r.push(id);
        }
    }
    r
}

// ---- layout ----

/// One reading ("Coolant: 31°C") with its own text style.
struct Seg {
    id: i32,
    text: String,
    col: Color,
    size: f32,
    st: ElementStyle,
    above: Option<String>, // label drawn small above the value
}

impl Seg {
    fn new(id: i32, text: String, st: ElementStyle, col: Color) -> Seg {
        Seg { id, text, col, size: (st.Size as f32).max(8.0), st, above: None }
    }
    fn label_size(&self) -> f32 {
        self.size * 0.42
    }
    fn height(&self) -> f32 {
        self.size * 1.2 + if self.above.is_some() { self.label_size() * 1.15 } else { 0.0 }
    }
}

struct Row<'a> {
    segs: Vec<&'a Seg>,
    blank: f32, // height (as a font size) of an intentionally empty line
}

impl Row<'_> {
    fn size(&self) -> f32 {
        self.segs.iter().fold(self.blank, |m, s| m.max(s.size))
    }
    fn height(&self) -> f32 {
        self.segs.iter().fold(self.blank * 1.2, |m, s| m.max(s.height()))
    }
}

struct Placed {
    path: Path,
    col: Color,
    fx: TextEffect,
    size: f32,
}

fn rows_from<'a>(items: &'a [Seg], lines: &[Vec<i32>]) -> Vec<Row<'a>> {
    let blank = if items.is_empty() { 40.0 } else { items.iter().map(|i| i.size).sum::<f32>() / items.len() as f32 };
    let mut rows = Vec::new();
    for line in lines {
        let segs: Vec<&Seg> = line.iter().filter_map(|id| items.iter().find(|s| s.id == *id)).collect();
        let empty = line.is_empty();
        if !segs.is_empty() || empty {
            rows.push(Row { segs, blank: if empty { blank } else { 0.0 } });
        }
    }
    while rows.first().is_some_and(|r| r.segs.is_empty()) {
        rows.remove(0);
    }
    while rows.last().is_some_and(|r| r.segs.is_empty()) {
        rows.pop();
    }
    rows
}

fn union(a: Option<Rect>, b: Rect) -> Rect {
    match a {
        None => b,
        Some(a) => Rect::from_ltrb(a.left().min(b.left()), a.top().min(b.top()), a.right().max(b.right()), a.bottom().max(b.bottom())).unwrap_or(a),
    }
}

fn bounds(p: &Path) -> Rect {
    p.compute_tight_bounds().unwrap_or(p.bounds())
}

fn effect_pad(fx: TextEffect) -> f32 {
    match fx {
        TextEffect::Glow => 0.16,
        TextEffect::Outline => 0.06,
        _ => 0.04,
    }
}

/// Draws the rows; if a line doesn't fit inside the circle, everything is shrunk until it does.
/// v_anchor: -1 = block top at anchor_y, 0 = centred on it, 1 = block bottom at it.
fn draw_readings(p: &mut Pixmap, cfg: &Config, rows: &[Row], gap: f32, anchor_y: f32, v_anchor: i32, bar: bool) {
    if rows.is_empty() {
        return;
    }
    let mut s = 1.0f32;
    loop {
        let (placed, ink, fits) = place(cfg, rows, s, gap, anchor_y, v_anchor);
        if fits || s < 0.25 {
            if let (true, Some(ink)) = (bar, ink) {
                if let Some(r) = Rect::from_xywh(0.0, ink.top() - 12.0, 480.0, ink.height() + 24.0) {
                    p.fill_rect(r, &paint(rgba(0, 0, 0, 150)), Transform::identity(), None);
                }
            }
            for pl in &placed {
                draw_path(p, pl.fx, &pl.path, pl.col, pl.size);
            }
            return;
        }
        s *= 0.94;
    }
}

/// Lays rows out at scale s; returns the outlines, the ink box and whether every line lies inside the circle.
fn place(cfg: &Config, rows: &[Row], s: f32, gap: f32, anchor_y: f32, v_anchor: i32) -> (Vec<Placed>, Option<Rect>, bool) {
    let f = fonts();
    let mut total = gap * s * (rows.len() as f32 - 1.0);
    for r in rows {
        total += r.height() * s;
    }
    let mut y = match v_anchor {
        a if a < 0 => anchor_y,
        a if a > 0 => anchor_y - total,
        _ => anchor_y - total / 2.0,
    };
    let mut lines: Vec<(Vec<Placed>, Rect)> = Vec::new();
    let mut halves = Vec::new();
    for r in rows {
        let row_h = r.height() * s;
        if r.segs.is_empty() {
            y += row_h + gap * s; // blank line: space only
            continue;
        }
        // readings side by side, each in its own font, vertically centred in the row; a label "above" sits small
        // over its value, both centred on the wider of the two
        let mut segs: Vec<Placed> = Vec::new();
        let mut x = 0f32;
        let mut pad = 0f32;
        for sg in &r.segs {
            let size = sg.size * s;
            let mut top = y + (row_h - sg.height() * s) / 2.0;
            let (fam, b, it) = (sg.st.Font.as_str(), sg.st.Bold, sg.st.Italic);
            let mut lp = None;
            if let Some(lbl) = &sg.above {
                lp = f.path(fam, b, it, sg.label_size() * s, x, top, lbl);
                top += sg.label_size() * s * 1.15;
            }
            let Some(mut path) = f.path(fam, b, it, size, x, top, &sg.text) else { continue };
            let pb = bounds(&path);
            let mut right = pb.right();
            if let Some(l) = lp {
                let lb = bounds(&l);
                let w = pb.width().max(lb.width());
                let l = l.clone().transform(Transform::from_translate(pb.left() + (w - lb.width()) / 2.0 - lb.left(), 0.0)).unwrap_or(l);
                path = path.clone().transform(Transform::from_translate((w - pb.width()) / 2.0, 0.0)).unwrap_or(path);
                right = pb.left() + w;
                segs.push(Placed { path: l, col: sg.col, fx: sg.st.Effect, size: sg.label_size() * s });
            }
            if pb.width() > 0.0 {
                x = right + size.max(r.size() * s * 0.5) * 0.6;
            }
            pad = pad.max(size * effect_pad(sg.st.Effect));
            segs.push(Placed { path, col: sg.col, fx: sg.st.Effect, size });
        }
        let mut b: Option<Rect> = None;
        for sp in &segs {
            let pb = bounds(&sp.path);
            if pb.width() > 0.0 {
                b = Some(union(b, pb));
            }
        }
        let Some(b) = b else {
            y += row_h + gap * s;
            continue;
        };
        let dy = (b.top() - pad - CY).abs().max((b.bottom() + pad - CY).abs());
        let half = if dy < FIT_R { (FIT_R * FIT_R - dy * dy).sqrt() - pad } else { 0.0 };
        lines.push((segs, b));
        halves.push(half);
        y += row_h + gap * s;
    }
    // left/right alignment share one edge: the innermost chord end over all lines
    let mut edge_l = f32::MIN;
    let mut edge_r = f32::MAX;
    for h in &halves {
        edge_l = edge_l.max(CX - h);
        edge_r = edge_r.min(CX + h);
    }
    let mut fits = true;
    let mut ink: Option<Rect> = None;
    let mut placed = Vec::new();
    for ((segs, b), half) in lines.into_iter().zip(halves) {
        let left = match cfg.TextAlign {
            0 => edge_l,
            2 => edge_r - b.width(),
            _ => CX - b.width() / 2.0,
        };
        if b.width() > 2.0 * half || left < CX - half - 0.5 || left + b.width() > CX + half + 0.5 {
            fits = false;
        }
        let t = Transform::from_translate(left - b.left(), 0.0);
        for sp in segs {
            let path = sp.path.clone().transform(t).unwrap_or(sp.path);
            placed.push(Placed { path, ..sp });
        }
        if let Some(nb) = Rect::from_xywh(left, b.top(), b.width(), b.height()) {
            ink = Some(union(ink, nb));
        }
    }
    (placed, ink, fits)
}

/// size = font size in px (effect strengths scale with it)
fn draw_path(p: &mut Pixmap, fx: TextEffect, path: &Path, c: Color, size: f32) {
    let id = Transform::identity();
    match fx {
        TextEffect::Shadow => {
            let d = (size * 0.03).max(2.0);
            p.fill_path(path, &paint(rgba(0, 0, 0, 170)), FillRule::Winding, Transform::from_translate(d, d), None);
        }
        TextEffect::Outline => {
            let st = Stroke { width: (size * 0.08).max(3.0), line_join: LineJoin::Round, ..Default::default() };
            p.stroke_path(path, &paint(rgba(0, 0, 0, 255)), &st, id, None);
        }
        TextEffect::Glow => {
            for i in (1..=6).rev() {
                let mut g = c;
                g.set_alpha((16 + (6 - i) * 9) as f32 / 255.0);
                let st = Stroke { width: (size * 0.045 * i as f32).max(2.0), line_join: LineJoin::Round, ..Default::default() };
                p.stroke_path(path, &paint(g), &st, id, None);
            }
        }
        TextEffect::None => {}
    }
    p.fill_path(path, &paint(c), FillRule::Winding, id, None);
}

/// Sensor values the readings show.
#[derive(Clone, Copy, Debug)]
pub struct Values {
    pub water: f64,    // NaN / out of range when unknown
    pub cpu_temp: i32, // <= 0 when unknown
    pub cpu_load: i32,
}

/// The reading's text and colour in its style ("Coolant: 31°C" or "31°C", °C/°F, colour by value).
fn reading(cfg: &Config, overlay: bool, id: i32, v: &Values, now: &LocalTime) -> Seg {
    let st = cfg.style(overlay, id);
    let c = rgb(st.Rgb);
    match id {
        ID_TIME => Seg::new(id, now.hhmm(), st, c),
        ID_DATE => Seg::new(id, now.date(), st, c),
        ID_WATER => {
            let col = if st.ByTemp { heat(v.water, 25.0, 45.0) } else { c };
            labelled(id, "Coolant", deg(v.water, st.Fahrenheit), st, col)
        }
        ID_CPU_TEMP => {
            let col = if st.ByTemp { heat(v.cpu_temp as f64, 35.0, 90.0) } else { c };
            labelled(id, "CPU", deg(v.cpu_temp as f64, st.Fahrenheit), st, col)
        }
        _ => {
            let col = if st.ByTemp { heat(v.cpu_load as f64, 0.0, 100.0) } else { c };
            labelled(id, "Load", format!("{}%", v.cpu_load), st, col)
        }
    }
}

/// Label beside ("Coolant: 31°C"), above (small "Coolant" over "31°C") or none.
fn labelled(id: i32, label: &str, value: String, st: ElementStyle, c: Color) -> Seg {
    if !st.Label {
        return Seg::new(id, value, st, c);
    }
    if st.LabelAbove {
        let mut s = Seg::new(id, value, st, c);
        s.above = Some(label.to_owned());
        return s;
    }
    Seg::new(id, format!("{label}: {value}"), st, c)
}

/// Overlay readings at the top, centre or bottom of the round panel.
pub fn draw_overlay(p: &mut Pixmap, cfg: &Config, v: &Values) {
    let now = super::local_now();
    let mut items = Vec::new();
    if cfg.OverlayTime {
        items.push(reading(cfg, true, ID_TIME, v, &now));
    }
    if cfg.OverlayWaterTemp && water_valid(v.water) {
        items.push(reading(cfg, true, ID_WATER, v, &now));
    }
    if cfg.OverlayCpuTemp && v.cpu_temp > 0 {
        items.push(reading(cfg, true, ID_CPU_TEMP, v, &now));
    }
    let gap = items.iter().map(|i| i.size).fold(0.0, f32::max) * 0.1;
    let ls = lines(&cfg.OverlayOrder, cfg.OverlayLayout, cfg.OverlayBreaks, &OVERLAY_IDS);
    let (y, a) = match cfg.OverlayPos {
        1 => (62.0, -1),
        2 => (CY, 0),
        _ => (480.0 - 62.0, 1),
    };
    draw_readings(p, cfg, &rows_from(&items, &ls), gap, y, a, cfg.OverlayBar);
}

/// Configurable clock / readings face, around the centre (plus the vertical offset).
pub fn draw_clock(p: &mut Pixmap, cfg: &Config, v: &Values) {
    let now = super::local_now();
    let mut items = Vec::new();
    if cfg.ShowTime {
        items.push(reading(cfg, false, ID_TIME, v, &now));
    }
    if cfg.ShowDate {
        items.push(reading(cfg, false, ID_DATE, v, &now));
    }
    if cfg.ShowWaterTemp && water_valid(v.water) {
        items.push(reading(cfg, false, ID_WATER, v, &now));
    }
    if cfg.ShowCpuTemp && v.cpu_temp > 0 {
        items.push(reading(cfg, false, ID_CPU_TEMP, v, &now));
    }
    if cfg.ShowCpuLoad {
        items.push(reading(cfg, false, ID_LOAD, v, &now));
    }
    let ls = lines(&cfg.ClockOrder, cfg.ClockLayout, cfg.ClockBreaks, &CLOCK_IDS);
    draw_readings(p, cfg, &rows_from(&items, &ls), cfg.LineSpacing as f32, CY + cfg.TextOffsetY as f32, 0, false);
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn line_strings() {
        assert_eq!(lines("0,2|3|", TextLayout::Auto, 0, &OVERLAY_IDS), vec![vec![0, 2], vec![3]]);
        assert_eq!(lines("0,2||3|", TextLayout::Auto, 0, &OVERLAY_IDS), vec![vec![0, 2], vec![], vec![3]]);
        assert_eq!(lines("2,0", TextLayout::Stack, 0, &OVERLAY_IDS), vec![vec![2], vec![0], vec![3]]);
        assert_eq!(lines("2,0", TextLayout::Auto, 0, &OVERLAY_IDS), vec![vec![2, 0, 3]]);
        assert_eq!(lines_to_string(&[vec![0, 2], vec![], vec![3]]), "0,2||3|");
    }
}
