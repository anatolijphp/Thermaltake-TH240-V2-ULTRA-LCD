// What Studio shows and decides, without any UI toolkit: texts, which option panels a mode needs, rotation math and the
// dashboard readings. Both front ends render these; panel names match the element names in MainWindow.xaml.
using System;
using System.Collections.Generic;

namespace TtLcd
{
    class StudioModel
    {
        readonly StudioCore core;
        public StudioModel(StudioCore core) { this.core = core; }
        Engine E { get { return core.Engine; } }

        // ---- modes ----
        public static string ModeDescription(Mode m)
        {
            switch (m)
            {
                case Mode.VirtualDisplay: return "A virtual 480×480 monitor is mirrored to the cooler. In Wallpaper Engine, choose that monitor and pick a wallpaper for it.";
                case Mode.Monitor: return "Mirrors one of your monitors, or a part of it, to the cooler.";
                case Mode.Image: return "Shows a still picture on the cooler.";
                case Mode.Gif: return "Plays an animated GIF on the cooler in a loop.";
                case Mode.Video: return "Plays a clip of up to 30 seconds in a loop. Choose the part to play, then zoom and pan to frame it.";
                case Mode.Color: return "Fills the screen with one colour.";
                case Mode.Clock: return "Time, date and sensor readings, arranged and styled below.";
                case Mode.Aura: return "Adds the cooler screen to Armoury Crate as an Aura Sync device (\"TT LCD CL-W383\"). Its effects and colours drive an abstract visualiser on the LCD. Installing needs administrator rights and restarts the ASUS lighting service.";
                case Mode.Off: return "The screen is blanked.";
                default: return "";
            }
        }

        public static string OptionsTitle(Mode m) { return (Engine.Label(m) + " options").ToUpperInvariant(); }

        // Which option panels the mode (and overlay choice) needs. selectedVisible: a reading is selected for styling
        // and belongs to what this mode shows.
        public static List<KeyValuePair<string, bool>> Panels(Config cfg, bool selectedVisible)
        {
            var m = cfg.Mode;
            bool overlay = m != Mode.Clock && m != Mode.Off;
            bool overlayOn = cfg.OverlayTime || cfg.OverlayWaterTemp || cfg.OverlayCpuTemp;
            Func<string, bool, KeyValuePair<string, bool>> p = (n, on) => new KeyValuePair<string, bool>(n, on);
            return new List<KeyValuePair<string, bool>>
            {
                p("OptVirtualDisplay", m == Mode.VirtualDisplay),
                p("OptMonitor", m == Mode.Monitor),
                p("OptImage", m == Mode.Image),
                p("OptGif", m == Mode.Gif),
                p("OptVideo", m == Mode.Video),
                p("OptOff", m == Mode.Off),
                p("OptAura", m == Mode.Aura),
                p("OptClock", m == Mode.Clock),
                p("OptScaling", m == Mode.Image || m == Mode.Gif || m == Mode.Monitor),
                p("OptBg", m == Mode.Image || m == Mode.Gif || m == Mode.Color || m == Mode.Clock),
                p("OptOverlay", overlay),
                p("OvDetails", overlayOn),
                p("OptText", selectedVisible),
            };
        }

        // ---- rotation ----
        // The live rotation at which the picture is upright on this mount.
        public static int Mount(Config c) { return c.PreviewMount < 0 ? c.Rotation : c.PreviewMount; }
        // Rotation as the user sees it: 0° = upright on this mount (the stored Rotation is the software turn that
        // achieves it, e.g. 270° here).
        public static int UserRotation(Config c) { return ((c.Rotation - Mount(c)) % 360 + 360) % 360; }
        // Stored Rotation for a user rotation (0/90/180/270 relative to upright).
        public static int RotationFor(Config c, int user) { return (Mount(c) + user) % 360; }
        public static int NextUserRotation(Config c) { return (UserRotation(c) + 90) % 360; }
        // The cooler draws its idle picture turned like the live frames: Rotation - mount from upright.
        public static double IdlePreviewAngle(Config c) { return UserRotation(c); }

        // First run: the current live rotation is "upright on this mount" for the preview.
        public void InitMount()
        {
            if (core.Config.PreviewMount < 0) { int r0 = core.Config.Rotation; core.Change(c => c.PreviewMount = r0); }
        }

        // First connect: the cooler's current rotation is taken as the one that matches today's live rotation
        // (calibration for Engine.HwFor). Returns true when the idle preview's angle may have changed.
        int hwRotShown = -2;
        public bool CheckHwRotation()
        {
            int hw = E.HwRotation;
            if (hw >= 0 && core.Config.HwUpright < 0) core.Change(c => c.HwUpright = hw);
            int key = hw * 1000 + core.Config.Rotation;
            if (key == hwRotShown) return false;
            hwRotShown = key;
            return true;
        }

        // ---- status texts ----
        public string ConnText { get { return E.Connected ? "Cooler connected" : "Cooler not connected"; } }
        public string FpsText { get { return E.Connected ? string.Format("{0:F0} FPS", E.Fps) : ""; } }
        public string ModeText { get { return E.Connected ? E.RenderStatus : E.DeviceStatus; } }
        public string DeviceStatus { get { return E.Connected ? "Connected" : E.DeviceStatus; } }
        public string SerialText { get { return string.IsNullOrEmpty(E.Serial) ? "—" : E.Serial; } }
        public string NavSerial { get { return string.IsNullOrEmpty(E.Serial) ? "" : "S/N " + E.Serial; } }
        public string DevFps { get { return string.Format("{0:F1} fps", E.Fps); } }

        public string TitleStatus
        {
            get
            {
                return (E.Connected ? "● " : "○ ") + Engine.Label(E.Config.Mode)
                    + (E.Suspended ? " · cooler showing its idle screen" : E.Paused ? " · paused" : "")
                    + (E.Game == Engine.ManualGame ? " · game mode" : E.Game != null ? " · game: " + E.Game : "");
            }
        }

        // Live game-mode status line (Settings).
        public string GameStatus()
        {
            var c = E.Config;
            string g = E.Game;
            if (g == null)
            {
                if (!core.Platform.GameDetection) return "Automatic game detection needs Windows; game mode can be switched on by hand.";
                if (!c.GameDetect) return "Game detection is off.";
                int known = core.Platform.KnownGameCount;
                return "No game running." + (c.GameKnownList && known >= 0 ? string.Format(" Windows knows {0} games on this PC.", known) : "");
            }
            string act = c.GameAction == GameAction.LowerFps ? "streaming at " + c.GameFps + " fps"
                       : c.GameAction == GameAction.IdleScreen ? "streaming stopped, the cooler shows its idle screen" : "no change";
            string who = g == Engine.ManualGame ? "Game mode on (forced)" : "Game running: " + g;
            return who + " · " + act + ", sensors every " + c.GameSensorMs / 1000 + " s.";
        }

        public static string CacheInfo()
        {
            return string.Format("Video cache: {0:F1} MB · Settings: {1}", Video.CacheBytes() / 1048576.0, Config.Folder);
        }

        // ---- dashboard ----
        public class Gauge { public string Text; public double Frac; }
        public class Dash { public Gauge Coolant, CpuTemp, CpuLoad, Ram; public int CpuTempC; public string Note; public bool ShowCpuTempHint; }

        public Dash Readings()
        {
            var cfg = E.Config;
            double water = E.Coolant;
            bool wOk = water >= -5 && water <= 99;
            int ct = CpuTemp.C(), ram = Mem.PercentUsed(), load = Cpu.Percent();
            return new Dash
            {
                Coolant = new Gauge { Text = wOk ? Math.Round(water) + "°" : "--", Frac = wOk ? (water - 20) / 30 : 0 },
                CpuTemp = new Gauge { Text = ct > 0 ? ct + "°" : "n/a", Frac = ct > 0 ? (ct - 20) / 75.0 : 0 },
                CpuLoad = new Gauge { Text = load + "%", Frac = load / 100.0 },
                Ram = new Gauge { Text = ram + "%", Frac = ram / 100.0 },
                CpuTempC = ct,
                Note = DashNote(cfg, ct),
                ShowCpuTempHint = ct <= 0 && core.Platform.CpuTempNeedsSetup,
            };
        }

        // "Coolant every 2 s · CPU temperature every 1.5 s · CPU load and RAM every 1 s", from measured intervals:
        // coolant = time between the last two readings from the cooler; CPU temperature = the helper's own update
        // timestamps (Windows) or the dashboard refresh (Linux, read directly); CPU load / RAM = the 1 s refresh.
        long helperTicks; int helperPeriodMs;
        string DashNote(Config cfg, int cpuTemp)
        {
            var parts = new List<string>();
            if (cfg.DashWater)
            {
                long age = E.CoolantAtMs < 0 ? -1 : Engine.NowMs - E.CoolantAtMs;
                int period = E.CoolantPeriodMs;
                if (age < 0) parts.Add("Coolant: waiting for the cooler");
                else if (period > 0 && age > 3 * period + 2000) parts.Add("Coolant: no reading for " + Secs(age));
                else if (!Render.WaterValid(E.Coolant)) parts.Add("Coolant sensor reads open");
                else parts.Add("Coolant every " + (period > 0 ? Secs(period) : "…"));
            }
            if (cfg.DashCpuTemp)
            {
                if (Os.Linux) parts.Add(cpuTemp <= 0 ? "CPU temperature: not available" : "CPU temperature every 1.5 s");
                else
                {
                    long t = Sensors.HelperUpdatedTicks();
                    if (t != 0 && t != helperTicks)
                    {
                        if (helperTicks != 0) helperPeriodMs = (int)((t - helperTicks) / TimeSpan.TicksPerMillisecond);
                        helperTicks = t;
                    }
                    parts.Add(cpuTemp <= 0 ? "CPU temperature: not available"
                        : "CPU temperature every " + (helperPeriodMs > 0 ? Secs(helperPeriodMs) : "…"));
                }
            }
            var fast = new List<string>();
            if (cfg.DashCpuLoad) fast.Add("CPU load");
            if (cfg.DashRam) fast.Add("RAM");
            if (fast.Count > 0) parts.Add(string.Join(" and ", fast) + " every 1 s");
            return parts.Count == 0 ? "" : "Updates: " + string.Join(" · ", parts) + ".";
        }

        public static string Secs(long ms)
        {
            double s = ms / 1000.0;
            return (s < 10 ? s.ToString("0.#") : Math.Round(s).ToString("0")) + " s";
        }

        // Dashboard gauge grid: as many columns as fit (~200 px each), balanced so every row is full (4 -> 2x2 or 4x1).
        public static int GaugeColumns(int count, double width)
        {
            if (count <= 0) return 1;
            int fit = Math.Max(1, Math.Min(count, (int)(width / 200)));
            int rows = (count + fit - 1) / fit;
            return (count + rows - 1) / rows;
        }

        public static readonly int[] Palette = { 0x000000, 0xFFFFFF, 0xFF2B45, 0xFF7A00, 0xFFD400, 0x2BE36B, 0x00D1FF, 0x2B6BFF, 0x8A3BFF, 0xFF3BD2 };
    }
}
