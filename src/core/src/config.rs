//! User settings, stored as XML in %APPDATA%\ttlcd\config.xml (Linux: ~/.config/ttlcd/config.xml) in exactly the
//! layout the C# version's XmlSerializer wrote, so existing settings carry over in both directions.
//! Field names are the XML element names (and what `ttlcd set <name>` and the Studio page use).
#![allow(non_snake_case)]

use crate::xml;
use serde::Serialize;
use std::path::PathBuf;

macro_rules! enum_str {
    ($name:ident { $($v:ident),* $(,)? }) => {
        #[derive(Clone, Copy, Debug, PartialEq, Eq, Hash, Serialize)]
        pub enum $name { $($v),* }
        impl $name {
            pub const ALL: &'static [$name] = &[$($name::$v),*];
            pub fn name(self) -> &'static str { match self { $($name::$v => stringify!($v)),* } }
            pub fn parse(s: &str) -> Option<$name> {
                let s = s.trim();
                $(if s.eq_ignore_ascii_case(stringify!($v)) { return Some($name::$v); })*
                None
            }
        }
        impl std::fmt::Display for $name {
            fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result { f.write_str(self.name()) }
        }
    };
}

enum_str!(Mode { VirtualDisplay, Monitor, Image, Gif, Video, Color, Clock, Aura, Pump, Off });
enum_str!(Overlay { None, Clock, ClockCpu });
enum_str!(TextEffect { None, Shadow, Outline, Glow });
// What to do while a game runs: nothing, lower the frame rate, or stop streaming so the cooler shows its idle screen.
enum_str!(GameAction { None, LowerFps, IdleScreen });
// Legacy line layouts (before drag-and-drop lines); only read to convert an old order string without '|'.
enum_str!(TextLayout { Auto, Line, Stack, Custom });

/// A value that can live in config.xml, on the command line (`ttlcd set`) and in the Studio page's JSON.
pub trait Field: Sized {
    fn to_xml(&self) -> String;
    fn from_xml(s: &str) -> Option<Self>;
    fn from_cli(s: &str) -> Option<Self> {
        Self::from_xml(s)
    }
    fn from_json(v: &serde_json::Value) -> Option<Self>;
    fn show(&self, _name: &str) -> String {
        self.to_xml()
    }
}

impl Field for i32 {
    fn to_xml(&self) -> String {
        self.to_string()
    }
    fn from_xml(s: &str) -> Option<i32> {
        s.trim().parse().ok()
    }
    fn from_cli(s: &str) -> Option<i32> {
        let s = s.trim();
        match s.strip_prefix('#') {
            Some(h) => i32::from_str_radix(h, 16).ok(),
            None => s.parse().ok(),
        }
    }
    fn from_json(v: &serde_json::Value) -> Option<i32> {
        v.as_f64().map(|f| f.round() as i32)
    }
    fn show(&self, name: &str) -> String {
        if name.ends_with("Rgb") { format!("#{:06X}", self) } else { self.to_string() }
    }
}

impl Field for f64 {
    fn to_xml(&self) -> String {
        // like .NET's round-trip formatting: 30, 0.5, 1.25
        let s = format!("{}", self);
        if s == "-0" { "0".into() } else { s }
    }
    fn from_xml(s: &str) -> Option<f64> {
        s.trim().parse().ok()
    }
    fn from_json(v: &serde_json::Value) -> Option<f64> {
        v.as_f64()
    }
}

impl Field for bool {
    fn to_xml(&self) -> String {
        if *self { "true" } else { "false" }.into()
    }
    fn from_xml(s: &str) -> Option<bool> {
        match s.trim() {
            "true" | "True" | "1" => Some(true),
            "false" | "False" | "0" => Some(false),
            _ => None,
        }
    }
    fn from_cli(s: &str) -> Option<bool> {
        let s = s.trim().to_ascii_lowercase();
        Some(matches!(s.as_str(), "1" | "on" | "yes" | "true"))
    }
    fn from_json(v: &serde_json::Value) -> Option<bool> {
        v.as_bool()
    }
    fn show(&self, _: &str) -> String {
        if *self { "True" } else { "False" }.into()
    }
}

impl Field for String {
    fn to_xml(&self) -> String {
        self.clone()
    }
    fn from_xml(s: &str) -> Option<String> {
        Some(s.to_owned())
    }
    fn from_json(v: &serde_json::Value) -> Option<String> {
        v.as_str().map(str::to_owned)
    }
}

macro_rules! enum_field {
    ($($t:ident),*) => {$(
        impl Field for $t {
            fn to_xml(&self) -> String { self.name().into() }
            fn from_xml(s: &str) -> Option<$t> { $t::parse(s) }
            fn from_json(v: &serde_json::Value) -> Option<$t> { v.as_str().and_then($t::parse) }
        }
    )*};
}
enum_field!(Mode, Overlay, TextEffect, GameAction, TextLayout);

/// Text style of one reading (time, date, water, CPU temperature, CPU load) on the clock face or the overlay.
#[derive(Clone, Debug, Serialize, PartialEq)]
pub struct ElementStyle {
    pub Id: i32,
    pub Font: String,
    pub Bold: bool,
    pub Italic: bool,
    pub Effect: TextEffect,
    pub Rgb: i32,
    /// colour by value (cool cyan -> hot red)
    pub ByTemp: bool,
    /// "Coolant: 31°C" vs "31°C"
    pub Label: bool,
    /// label small above the value instead of beside it
    pub LabelAbove: bool,
    pub Fahrenheit: bool,
    /// px on the 480 px screen
    pub Size: i32,
}

impl Default for ElementStyle {
    fn default() -> Self {
        ElementStyle {
            Id: 0,
            Font: "Segoe UI".into(),
            Bold: true,
            Italic: false,
            Effect: TextEffect::Shadow,
            Rgb: 0xFFFFFF,
            ByTemp: false,
            Label: true,
            LabelAbove: false,
            Fahrenheit: false,
            Size: 54,
        }
    }
}

impl ElementStyle {
    fn from_node(n: &xml::Node) -> ElementStyle {
        let mut s = ElementStyle::default();
        macro_rules! rd { ($($f:ident),*) => {$( if let Some(v) = n.child(stringify!($f)).and_then(|c| Field::from_xml(&c.text)) { s.$f = v; } )*}; }
        rd!(Id, Font, Bold, Italic, Effect, Rgb, ByTemp, Label, LabelAbove, Fahrenheit, Size);
        s
    }

    fn write(&self, o: &mut String) {
        o.push_str("    <ElementStyle>\n");
        macro_rules! wr { ($($f:ident),*) => {$( o.push_str(&format!("      <{0}>{1}</{0}>\n", stringify!($f), xml::escape(&self.$f.to_xml()))); )*}; }
        wr!(Id, Font, Bold, Italic, Effect, Rgb, ByTemp, Label, LabelAbove, Fahrenheit, Size);
        o.push_str("    </ElementStyle>\n");
    }

    /// Apply one field from the Studio page ({"Size": 60}).
    pub fn set_json(&mut self, name: &str, v: &serde_json::Value) -> bool {
        macro_rules! st { ($($f:ident),*) => { match name { $( stringify!($f) => match Field::from_json(v) { Some(x) => { self.$f = x; true } None => false }, )* _ => false } }; }
        st!(Font, Bold, Italic, Effect, Rgb, ByTemp, Label, LabelAbove, Fahrenheit, Size)
    }
}

macro_rules! config {
    ($( $(#[$m:meta])* $name:ident : $ty:ty = $def:expr ),* $(,)?) => {
        #[derive(Clone, Debug, Serialize)]
        pub struct Config {
            $( $(#[$m])* pub $name: $ty, )*
            /// Per-reading text styles; a reading without an entry uses the older global text settings.
            pub ClockStyles: Option<Vec<ElementStyle>>,
            pub OverlayStyles: Option<Vec<ElementStyle>>,
        }

        impl Default for Config {
            fn default() -> Self { Config { $( $name: $def.into(), )* ClockStyles: None, OverlayStyles: None } }
        }

        impl Config {
            /// Names of the plain settings, in file order (what `ttlcd config` lists).
            pub const FIELDS: &'static [&'static str] = &[$( stringify!($name) ),*];

            fn read_fields(&mut self, root: &xml::Node) {
                $( if let Some(v) = root.child(stringify!($name)).and_then(|c| Field::from_xml(&c.text)) { self.$name = v; } )*
            }

            fn write_fields(&self, o: &mut String) {
                $(
                    let v = self.$name.to_xml();
                    if v.is_empty() { o.push_str(&format!("  <{} />\n", stringify!($name))); }
                    else { o.push_str(&format!("  <{0}>{1}</{0}>\n", stringify!($name), xml::escape(&v))); }
                )*
            }

            /// `ttlcd set <name> <value>`: case-insensitive name; returns the canonical name and the value as shown.
            pub fn set_cli(&mut self, name: &str, value: &str) -> Result<(&'static str, String), String> {
                $(
                    if name.eq_ignore_ascii_case(stringify!($name)) {
                        let v: $ty = Field::from_cli(value).ok_or_else(|| format!("invalid value '{}' for {}", value, stringify!($name)))?;
                        let shown = v.show(stringify!($name));
                        self.$name = v;
                        return Ok((stringify!($name), shown));
                    }
                )*
                Err(format!("unknown setting '{}' (see 'ttlcd config')", name))
            }

            /// Change from the Studio page ({"Mode": "Clock"}); false if the name or value isn't valid.
            pub fn set_json(&mut self, name: &str, v: &serde_json::Value) -> bool {
                $(
                    if name == stringify!($name) {
                        return match Field::from_json(v) { Some(x) => { self.$name = x; true } None => false };
                    }
                )*
                false
            }

            /// (name, value) pairs as `ttlcd config` prints them.
            pub fn listing(&self) -> Vec<(&'static str, String)> {
                vec![$( (stringify!($name), self.$name.show(stringify!($name))) ),*]
            }
        }
    };
}

config! {
    Mode: Mode = Mode::VirtualDisplay,
    ImagePath: String = "",
    GifPath: String = "",
    VideoPath: String = "",
    VideoStart: f64 = 0.0,
    VideoLength: f64 = 30.0,
    VideoZoom: f64 = 1.0,
    VideoPanX: f64 = 0.0,
    VideoPanY: f64 = 0.0,
    VideoFps: i32 = 30,
    MonitorIndex: i32 = 0,
    UseRegion: bool = false,
    RegionX: i32 = 0,
    RegionY: i32 = 0,
    RegionW: i32 = 480,
    RegionH: i32 = 480,
    Fps: i32 = 30,
    Quality: i32 = 80,
    /// software rotation, degrees clockwise
    Rotation: i32 = 0,
    /// The live rotation at which the picture looks upright on this mount (taken from Rotation on first run). The
    /// preview turns frames by -PreviewMount, so it always shows the LCD as you see it, even after rotating.
    PreviewMount: i32 = -1,
    /// The cooler's stored rotation (0x11) that shows its idle screen upright when Rotation == PreviewMount (taken from
    /// the cooler on first connect). Engine::hw_for derives the cooler's rotation from Rotation.
    HwUpright: i32 = -1,
    /// The cooler (model::device_key) whose calibration from model::KNOWN_MOUNTS was applied; "" = none yet. Applied
    /// once per cooler, so turning the picture afterwards is kept.
    CalibratedFor: String = "",
    Brightness: i32 = 100,
    /// false = fill/crop, true = fit with borders
    FitContain: bool = false,
    /// background for Color/Clock modes
    ColorRgb: i32 = 0x000000,
    /// Pump mode: where its speed comes from: "" = the CPU fan, a motherboard fan's id ("nct6799/fan7"), or
    /// "manual" = PumpRpm
    PumpFan: String = "",
    /// Pump mode: the speed when set by hand (or no fan readings), rpm; and the coolant's colour
    PumpRpm: i32 = 2800,
    PumpColor: i32 = 0x00D1FF,
    /// Pump mode: the LED light round the pump head follows Aura Sync (else a flickering red)
    PumpAura: bool = false,
    /// Pump mode: the view tilted from straight down, degrees towards the picture's right (PumpViewX) and bottom
    /// (PumpViewY), together at most 30 (render::pump3d::MAX_TILT)
    PumpViewX: i32 = 0,
    PumpViewY: i32 = 0,
    TextRgb: i32 = 0xFFFFFF,
    /// legacy; replaced by the Overlay* flags below
    Overlay: Overlay = Overlay::None,
    /// Universal overlay drawn on every display mode: time / CPU temp / water temp.
    OverlayTime: bool = false,
    OverlayCpuTemp: bool = false,
    OverlayWaterTemp: bool = false,
    BlankOnExit: bool = true,
    CloseToTray: bool = true,
    /// Studio: icon-only navigation rail
    NavCollapsed: bool = false,
    /// Game mode: reduce load while a game runs (see games.rs for the detection sources)
    GameDetect: bool = true,
    GameFullscreen: bool = true,
    GameKnownList: bool = true,
    GameBorderless: bool = false,
    GameExtra: String = "",
    GameIgnore: String = "",
    GameAction: GameAction = GameAction::LowerFps,
    GameFps: i32 = 5,
    GameSensorMs: i32 = 10000,
    /// last picture stored as the cooler's idle (standby) screen
    IdlePath: String = "",
    /// last animation stored as the cooler's loading (boot) screen
    BootPath: String = "",
    ShowTime: bool = true,
    ShowDate: bool = true,
    ShowWaterTemp: bool = true,
    ShowCpuTemp: bool = false,
    ShowCpuLoad: bool = false,
    ClockFont: i32 = 120,
    DateSize: i32 = 31,
    TempSize: i32 = 54,
    LoadSize: i32 = 31,
    /// 0 left, 1 centre, 2 right (clock face and overlay)
    TextAlign: i32 = 1,
    /// clock face vertical shift (px, + = down)
    TextOffsetY: i32 = 0,
    /// clock face gap between lines (px)
    LineSpacing: i32 = 12,
    FontName: String = "Segoe UI",
    FontBold: bool = true,
    FontItalic: bool = false,
    TextEffect: TextEffect = TextEffect::Shadow,
    ShowLabels: bool = true,
    Fahrenheit: bool = false,
    ColorByTemp: bool = false,
    /// 0 bottom, 1 top, 2 centre
    OverlayPos: i32 = 0,
    OverlaySize: i32 = 28,
    OverlayBar: bool = false,
    /// Lines of readings, arranged by drag and drop: render ids, ',' within a line, '|' ends a line.
    ClockOrder: String = "0|1|2|3|4|",
    OverlayOrder: String = "0,2,3|",
    ClockLayout: TextLayout = TextLayout::Stack,
    OverlayLayout: TextLayout = TextLayout::Auto,
    ClockBreaks: i32 = 0x1F,
    OverlayBreaks: i32 = 0,
    /// the firmware's dim-after-idle flag (cmd 0x1E)
    DeviceSleep: bool = true,
    DashWater: bool = true,
    DashCpuLoad: bool = true,
    DashCpuTemp: bool = false,
    DashRam: bool = true,
    DashClock: bool = true,
}

/// %APPDATA%\ttlcd (Windows) or $XDG_CONFIG_HOME/ttlcd, ~/.config/ttlcd (Linux), created on demand.
pub fn folder() -> PathBuf {
    let d = base_dir(false).join("ttlcd");
    let _ = std::fs::create_dir_all(&d);
    d
}

/// %LOCALAPPDATA%\ttlcd (Windows) or $XDG_DATA_HOME/ttlcd, ~/.local/share/ttlcd (Linux): caches.
pub fn local_folder() -> PathBuf {
    base_dir(true).join("ttlcd")
}

fn base_dir(local: bool) -> PathBuf {
    #[cfg(windows)]
    {
        let v = if local { "LOCALAPPDATA" } else { "APPDATA" };
        std::env::var_os(v).map(PathBuf::from).unwrap_or_else(|| PathBuf::from("."))
    }
    #[cfg(not(windows))]
    {
        let (var, sub) = if local { ("XDG_DATA_HOME", ".local/share") } else { ("XDG_CONFIG_HOME", ".config") };
        match std::env::var_os(var) {
            Some(v) if !v.is_empty() => PathBuf::from(v),
            _ => PathBuf::from(std::env::var_os("HOME").unwrap_or_default()).join(sub),
        }
    }
}

pub fn file_path() -> PathBuf {
    folder().join("config.xml")
}

impl Config {
    pub fn load() -> Config {
        std::fs::read_to_string(file_path()).ok().and_then(|s| Config::from_xml(&s)).unwrap_or_default() // corrupt -> defaults
    }

    pub fn from_xml(s: &str) -> Option<Config> {
        let root = xml::parse(s.trim_start_matches('\u{feff}'))?;
        if root.name != "Config" {
            return None;
        }
        let mut c = Config::default();
        c.read_fields(&root);
        let styles = |name: &str| root.child(name).map(|n| n.children.iter().filter(|e| e.name == "ElementStyle").map(ElementStyle::from_node).collect());
        c.ClockStyles = styles("ClockStyles");
        c.OverlayStyles = styles("OverlayStyles");
        Some(c)
    }

    pub fn to_xml(&self) -> String {
        let mut o = String::from("<?xml version=\"1.0\"?>\n<Config xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n");
        self.write_fields(&mut o);
        for (name, list) in [("ClockStyles", &self.ClockStyles), ("OverlayStyles", &self.OverlayStyles)] {
            if let Some(l) = list {
                o.push_str(&format!("  <{}>\n", name));
                for s in l {
                    s.write(&mut o);
                }
                o.push_str(&format!("  </{}>\n", name));
            }
        }
        o.push_str("</Config>");
        o
    }

    /// Atomic save: write a temp file, then replace.
    pub fn save(&self) -> std::io::Result<()> {
        let path = file_path();
        let tmp = path.with_extension("xml.tmp");
        std::fs::write(&tmp, self.to_xml())?;
        std::fs::rename(&tmp, &path)
    }

    /// The style of a reading; a reading without an entry uses the older global text settings.
    pub fn style(&self, overlay: bool, id: i32) -> ElementStyle {
        let list = if overlay { &self.OverlayStyles } else { &self.ClockStyles };
        if let Some(s) = list.as_ref().and_then(|l| l.iter().find(|s| s.Id == id)) {
            return s.clone();
        }
        ElementStyle {
            Id: id,
            Font: if self.FontName.is_empty() { "Segoe UI".into() } else { self.FontName.clone() },
            Bold: self.FontBold,
            Italic: self.FontItalic,
            Effect: self.TextEffect,
            Rgb: self.TextRgb,
            ByTemp: self.ColorByTemp,
            Label: self.ShowLabels,
            LabelAbove: false,
            Fahrenheit: self.Fahrenheit,
            Size: if overlay {
                self.OverlaySize
            } else {
                match id {
                    0 => self.ClockFont,
                    1 => self.DateSize,
                    4 => self.LoadSize,
                    _ => self.TempSize,
                }
            },
        }
    }

    /// Stores a style, replacing the reading's previous one.
    pub fn set_style(&mut self, overlay: bool, st: ElementStyle) {
        let list = if overlay { &mut self.OverlayStyles } else { &mut self.ClockStyles };
        let l = list.get_or_insert_with(Vec::new);
        l.retain(|s| s.Id != st.Id);
        l.push(st);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn round_trip_matches_dotnet_layout() {
        let mut c = Config::default();
        c.Mode = Mode::Aura;
        c.GifPath = "/home/u/a & b.gif".into();
        c.VideoZoom = 1.5;
        c.set_style(true, ElementStyle { Id: 0, Font: "Noto Kufi Arabic".into(), Size: 118, ..Default::default() });
        let x = c.to_xml();
        assert!(x.contains("  <Mode>Aura</Mode>\n"));
        assert!(x.contains("  <ImagePath />\n"));
        assert!(x.contains("<VideoZoom>1.5</VideoZoom>"));
        assert!(x.contains("<VideoLength>30</VideoLength>"));
        assert!(x.contains("a &amp; b.gif"));
        let back = Config::from_xml(&x).unwrap();
        assert_eq!(back.GifPath, c.GifPath);
        assert_eq!(back.Mode, Mode::Aura);
        assert_eq!(back.OverlayStyles.as_ref().unwrap()[0].Font, "Noto Kufi Arabic");
        assert_eq!(back.to_xml(), x);
    }

    #[test]
    fn cli_set() {
        let mut c = Config::default();
        assert_eq!(c.set_cli("mode", "clock").unwrap().0, "Mode");
        assert_eq!(c.Mode, Mode::Clock);
        c.set_cli("ColorRgb", "#FF0000").unwrap();
        assert_eq!(c.ColorRgb, 0xFF0000);
        assert!(c.set_cli("nope", "1").is_err());
    }
}
