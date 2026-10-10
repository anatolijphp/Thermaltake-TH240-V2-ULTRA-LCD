// User settings for the tray app, stored as XML in %APPDATA%\ttlcd\config.xml.
using System;
using System.IO;
using System.Xml.Serialization;
using Microsoft.Win32;

namespace TtLcd
{
    public enum Mode { VirtualDisplay, Monitor, Image, Gif, Video, Color, Clock, Aura, Off }
    public enum Overlay { None, Clock, ClockCpu }
    public enum TextEffect { None, Shadow, Outline, Glow }
    // What to do while a game runs: nothing, lower the frame rate, or stop streaming so the cooler shows its idle screen.
    public enum GameAction { None, LowerFps, IdleScreen }
    // How readings are split into lines: Auto = fewest lines that fit, Line = all on one line,
    // Stack = one per line, Custom = new line after the readings chosen in the break mask.
    public enum TextLayout { Auto, Line, Stack, Custom }

    // Text style of one reading (time, date, water, CPU temperature, CPU load) on the clock face or the overlay.
    public class ElementStyle
    {
        public int Id;
        public string Font = "Segoe UI";
        public bool Bold = true, Italic = false;
        public TextEffect Effect = TextEffect.Shadow;
        public int Rgb = 0xFFFFFF;
        public bool ByTemp = false;     // colour by value (cool cyan -> hot red)
        public bool Label = true;       // "Coolant: 31°C" vs "31°C"
        public bool LabelAbove = false; // label small above the value instead of beside it
        public bool Fahrenheit = false;
        public int Size = 54;           // px on the 480 px screen
        public ElementStyle Clone() { return (ElementStyle)MemberwiseClone(); }
    }

    public class Config
    {
        public Mode Mode = Mode.VirtualDisplay;
        public string ImagePath = "";
        public string GifPath = "";
        public string VideoPath = "";
        public double VideoStart = 0, VideoLength = 30;
        public double VideoZoom = 1, VideoPanX = 0, VideoPanY = 0;
        public int VideoFps = 30;
        public int MonitorIndex = 0;
        public bool UseRegion = false;
        public int RegionX = 0, RegionY = 0, RegionW = 480, RegionH = 480;
        public int Fps = 30;
        public int Quality = 80;
        public int Rotation = 0;          // software rotation, degrees clockwise
        // The live rotation at which the picture looks upright on this mount (taken from Rotation on first run). The
        // preview turns frames by -PreviewMount, so it always shows the LCD as you see it, even after rotating.
        public int PreviewMount = -1;
        // The cooler's stored rotation (0x11) that shows its idle screen upright when Rotation == PreviewMount
        // (taken from the cooler on first connect). Engine.HwFor derives the cooler's rotation from Rotation.
        public int HwUpright = -1;
        public int Brightness = 100;
        public bool FitContain = false;   // false = fill/crop, true = fit with borders
        public int ColorRgb = 0x000000;   // background for Color/Clock modes
        public int TextRgb = 0xFFFFFF;
        public Overlay Overlay = Overlay.None;   // legacy; replaced by the Overlay* flags below
        // Universal overlay drawn on every display mode: toggle time / CPU temp (motherboard) / water temp (cooler).
        public bool OverlayTime = false;
        public bool OverlayCpuTemp = false;
        public bool OverlayWaterTemp = false;
        public bool BlankOnExit = true;
        public bool CloseToTray = true;
        public bool NavCollapsed = false;   // Studio: icon-only navigation rail
        // Game mode: reduce load while a game runs (see Games.cs for the detection sources)
        public bool GameDetect = true;
        public bool GameFullscreen = true;  // exclusive full-screen Direct3D
        public bool GameKnownList = true;   // games known to the Xbox Game Bar
        public bool GameBorderless = false; // any foreground window covering its monitor (also full-screen video)
        public string GameExtra = "";       // extra exe names, comma separated
        public string GameIgnore = "";      // exe names never treated as games (e.g. always-on "games" like BongoCat)
        public GameAction GameAction = GameAction.LowerFps;
        public int GameFps = 5;
        public int GameSensorMs = 10000;    // sensor polling interval while gaming
        public string IdlePath = "";      // last picture stored as the cooler's idle (standby) screen
        public string BootPath = "";      // last animation stored as the cooler's loading (boot) screen
        // Clock mode: which elements to draw, and the big-element font size (px).
        public bool ShowTime = true;
        public bool ShowDate = true;
        public bool ShowWaterTemp = true;   // from the cooler's own sensor
        public bool ShowCpuTemp = false;    // best-effort, from Windows (ACPI thermal zone)
        public bool ShowCpuLoad = false;
        public int ClockFont = 120;         // time size (px)
        public int DateSize = 31, TempSize = 54, LoadSize = 31;
        public int TextAlign = 1;           // 0 left, 1 centre, 2 right (clock face and overlay)
        public int TextOffsetY = 0;         // clock face vertical shift (px, + = down)
        public int LineSpacing = 12;        // clock face gap between lines (px)
        // LCD text style, shared by the clock face and the overlay
        public string FontName = "Segoe UI";
        public bool FontBold = true, FontItalic = false;
        public TextEffect TextEffect = TextEffect.Shadow;
        public bool ShowLabels = true;      // "Coolant 31°C" vs "31°C"
        public bool Fahrenheit = false;
        public bool ColorByTemp = false;    // temperatures coloured cool (cyan) -> hot (red)
        public int OverlayPos = 0;          // 0 bottom, 1 top, 2 centre
        public int OverlaySize = 28;
        public bool OverlayBar = false;     // dark band behind the overlay
        // Lines of readings, arranged by drag and drop: Render.Id* values, ',' within a line, '|' ends a line.
        public string ClockOrder = "0|1|2|3|4|", OverlayOrder = "0,2,3|";
        // Legacy (before drag-and-drop lines): only read to convert an old order string without '|'.
        public TextLayout ClockLayout = TextLayout.Stack, OverlayLayout = TextLayout.Auto;
        public int ClockBreaks = 0x1F, OverlayBreaks = 0;
        public bool DeviceSleep = true;     // the firmware's dim-after-idle flag (cmd 0x18)
        // Dashboard mode: which sensor gauges to show (water temp is from the cooler; the rest from Windows).
        public bool DashWater = true;
        public bool DashCpuLoad = true;
        public bool DashCpuTemp = false;
        public bool DashRam = true;
        public bool DashClock = true;

        static readonly string Dir = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "ttlcd");
        static readonly string FilePath = Path.Combine(Dir, "config.xml");
        public static string Folder { get { Directory.CreateDirectory(Dir); return Dir; } }
        const string RunKey = @"Software\Microsoft\Windows\CurrentVersion\Run";
        const string RunName = "TTLcd";

        public Config Clone() { return (Config)MemberwiseClone(); } // style arrays are replaced, never edited in place

        // Per-reading text styles; a reading without an entry uses the older global text settings.
        public ElementStyle[] ClockStyles, OverlayStyles;

        public ElementStyle Style(bool overlay, int id)
        {
            var list = overlay ? OverlayStyles : ClockStyles;
            if (list != null) foreach (var s in list) if (s.Id == id) return s;
            return new ElementStyle
            {
                Id = id, Font = string.IsNullOrEmpty(FontName) ? "Segoe UI" : FontName, Bold = FontBold, Italic = FontItalic,
                Effect = TextEffect, Rgb = TextRgb, ByTemp = ColorByTemp, Label = ShowLabels, Fahrenheit = Fahrenheit,
                Size = overlay ? OverlaySize : id == 0 ? ClockFont : id == 1 ? DateSize : id == 4 ? LoadSize : TempSize,
            };
        }

        // Stores a style (as a new array, so configs cloned earlier stay unchanged).
        public void SetStyle(bool overlay, ElementStyle st)
        {
            var list = new System.Collections.Generic.List<ElementStyle>(overlay ? OverlayStyles ?? new ElementStyle[0] : ClockStyles ?? new ElementStyle[0]);
            list.RemoveAll(s => s.Id == st.Id);
            list.Add(st);
            if (overlay) OverlayStyles = list.ToArray(); else ClockStyles = list.ToArray();
        }

        public static Config Load()
        {
            try
            {
                if (File.Exists(FilePath))
                    using (var f = File.OpenRead(FilePath))
                        return (Config)new XmlSerializer(typeof(Config)).Deserialize(f);
            }
            catch (Exception) { } // corrupt file -> defaults
            return new Config();
        }

        public void Save()
        {
            Directory.CreateDirectory(Dir);
            string tmp = FilePath + ".tmp";
            using (var f = File.Create(tmp)) new XmlSerializer(typeof(Config)).Serialize(f, this);
            if (File.Exists(FilePath)) File.Delete(FilePath);
            File.Move(tmp, FilePath);
        }

        public static bool StartWithWindows
        {
            get
            {
                using (var k = Registry.CurrentUser.OpenSubKey(RunKey))
                    return k != null && k.GetValue(RunName) != null;
            }
            set
            {
                using (var k = Registry.CurrentUser.CreateSubKey(RunKey))
                {
                    if (value) k.SetValue(RunName, "\"" + System.Reflection.Assembly.GetEntryAssembly().Location + "\" --tray");
                    else k.DeleteValue(RunName, false);
                }
            }
        }
    }
}
