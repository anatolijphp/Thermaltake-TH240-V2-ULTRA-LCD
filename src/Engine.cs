// Two threads:
//  - renderer: produces frames for the configured mode (always runs, so the UI preview works without the cooler)
//  - sender:   owns the USB connection and pushes the newest frame, dropping stale ones
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;

namespace TtLcd
{
    public class Frame
    {
        public byte[] Jpeg;   // exactly what is sent (already rotated)
        public int Rotation;  // rotation applied, so a preview can undo it
        public long Seq;
    }

    public class Engine
    {
        volatile Config cfg;
        volatile bool paused, quit;
        volatile Frame latest;
        readonly AutoResetEvent newFrame = new AutoResetEvent(false);
        Thread renderThread, sendThread;
        long seq;

        public volatile string RenderStatus = "Starting";
        public volatile string DeviceStatus = "Connecting...";
        public volatile bool Connected;
        public volatile string Serial = "";
        public double Fps;            // frames actually sent per second
        public double VideoProgress;  // 0..1 while a video is being prepared, -1 otherwise
        public volatile int CoolantHundredths = int.MinValue;   // pump sensor, or MinValue when unread
        // The cooler's own rotation setting (cmd 0x11 / query 0x81, byte 1 of its flash config record). It turns the
        // firmware's own screens (idle, boot) but not the frames we stream, which are rotated in software.
        // It follows the one Rotation setting, so the idle screen turns with the live picture (see HwFor).
        public volatile int HwRotation = -1;          // degrees, -1 until read after connecting

        // The cooler's rotation that matches live rotation R. Its +90° turns counter-clockwise while ours turns
        // clockwise (tested 2026-10-10), so: hw = HwUpright - (R - PreviewMount). -1 = not calibrated yet.
        public static int HwFor(Config c)
        {
            if (c.HwUpright < 0 || c.PreviewMount < 0) return -1;
            return ((c.HwUpright - (c.Rotation - c.PreviewMount)) % 360 + 360) % 360;
        }
        public double Coolant { get { int v = CoolantHundredths; return v == int.MinValue ? double.NaN : v / 100.0; } }
        // When the last water reading arrived and the measured time between the last two (for the dashboard note)
        static readonly Stopwatch clock = Stopwatch.StartNew();
        public static long NowMs { get { return clock.ElapsedMilliseconds; } }
        public long CoolantAtMs = -1;
        public int CoolantPeriodMs;

        public Engine(Config c) { cfg = c; VideoProgress = -1; }
        public Config Config { get { return cfg; } set { cfg = value; } }
        public bool Paused { get { return paused; } set { paused = value; } }
        // Stop sending frames altogether (not even black ones), so the firmware falls back to its own idle screen.
        volatile bool suspended;
        public bool Suspended { get { return suspended; } set { suspended = value; if (value) Fps = 0; newFrame.Set(); } }
        public Frame Latest { get { return latest; } }
        public string Status { get { return Connected ? RenderStatus : DeviceStatus; } }

        [DllImport("winmm.dll")] static extern int timeBeginPeriod(int ms);
        [DllImport("winmm.dll")] static extern int timeEndPeriod(int ms);

        public void Start()
        {
            timeBeginPeriod(1); // default 15.6 ms sleep granularity caps pacing well below 30 fps
            renderThread = new Thread(RenderLoop) { IsBackground = true, Name = "lcd-render" };
            sendThread = new Thread(SendLoop) { IsBackground = true, Name = "lcd-send" };
            var gameThread = new Thread(GameLoop) { IsBackground = true, Name = "lcd-games", Priority = ThreadPriority.BelowNormal };
            renderThread.Start();
            sendThread.Start();
            gameThread.Start();
        }

        // ================= game mode =================
        // Name of the running game (null = none), checked every few seconds while detection is enabled.
        public volatile string Game;
        public const string ManualGame = "manual";
        // Force game mode on (for games detection misses). Not saved: it ends when the app restarts.
        volatile bool forceGame;
        readonly AutoResetEvent gameWake = new AutoResetEvent(false);
        public bool ForceGame { get { return forceGame; } set { forceGame = value; gameWake.Set(); } }
        public bool Gaming { get { var c = cfg; return Game != null && c.GameAction != GameAction.None; } }
        bool GameIdle(Config c) { return Game != null && c.GameAction == GameAction.IdleScreen; }

        void GameLoop()
        {
            int sensorMs = -1;
            while (!quit)
            {
                var c = cfg;
                string g = null;
                if (c.GameDetect) try { g = GameDetect.Detect(c); } catch (Exception) { }
                if (g == null && forceGame) g = ManualGame;
                Game = g;
                // slow the CPU-temperature helper down while gaming (0 = its default)
                int want = g != null ? Math.Max(1000, c.GameSensorMs) : 0;
                if (want != sensorMs) { Sensors.RequestInterval(want); sensorMs = want; }
                newFrame.Set();
                gameWake.WaitOne(3000); // next check, or at once when game mode is forced on/off
            }
        }

        public void Stop()
        {
            quit = true;
            newFrame.Set();
            gameWake.Set();
            if (renderThread != null) renderThread.Join(3000);
            if (sendThread != null) sendThread.Join(4000);
            timeEndPeriod(1);
        }

        void Publish(byte[] jpeg, int rotation)
        {
            latest = new Frame { Jpeg = jpeg, Rotation = rotation, Seq = Interlocked.Increment(ref seq) };
            newFrame.Set();
        }

        // ================= renderer =================
        string imgKey; Image img;
        string gifKey; List<KeyValuePair<Bitmap, int>> gif; int gifIdx;
        ScreenSource screen; Rectangle screenRect;
        string videoKey, failedVideoKey; VideoClip video; int videoIdx; Thread videoLoader;

        void RenderLoop()
        {
            var sw = Stopwatch.StartNew();
            bool blank = false;
            while (!quit)
            {
                var c = cfg;
                try
                {
                    if (paused)
                    {
                        // freeze: render nothing new; the sender keeps re-sending the last frame (keep-alive)
                        RenderStatus = "Paused";
                        Thread.Sleep(100);
                        continue;
                    }
                    if (c.Mode == Mode.Off)
                    {
                        if (!blank) { Publish(BlackJpeg(), 0); blank = true; }
                        RenderStatus = "Off";
                        Thread.Sleep(100);
                        continue;
                    }
                    if (GameIdle(c))
                    {
                        // game running: render nothing; the sender stops too, so the cooler shows its idle screen
                        RenderStatus = "Game running (" + Game + "): cooler shows its idle screen";
                        Thread.Sleep(250);
                        continue;
                    }
                    blank = false;
                    bool slow = Game != null && c.GameAction == GameAction.LowerFps;

                    long t0 = sw.ElapsedMilliseconds;
                    int delay; string note;
                    byte[] direct;
                    using (var frame = Produce(c, out delay, out note, out direct))
                    {
                        bool overlay = c.Mode != Mode.Clock && (c.OverlayTime || c.OverlayCpuTemp || c.OverlayWaterTemp);
                        if (direct != null && !overlay && c.Rotation % 360 == 0)
                            Publish(direct, 0); // pre-encoded video frame, nothing to change
                        else
                        {
                            if (overlay) Render.DrawOverlay(frame, c, CpuTemp.C(), Coolant);
                            Render.Rotate(frame, c.Rotation);
                            Publish(Render.Jpeg(frame, c.Quality), c.Rotation);
                        }
                    }
                    if (slow) delay = Math.Max(delay, 1000 / Math.Max(1, c.GameFps)); // game running: lower frame rate
                    RenderStatus = note ?? (Label(c.Mode) + (delay < 1000 ? string.Format(" · {0:F0} fps", Fps) : ""));
                    if (slow) RenderStatus += " · game mode (" + Game + ")";

                    long wait = t0 + delay - sw.ElapsedMilliseconds;
                    while (wait > 0 && !quit && cfg == c)
                    {
                        Thread.Sleep((int)Math.Min(wait, 50));
                        wait = t0 + delay - sw.ElapsedMilliseconds;
                    }
                }
                catch (Exception e)
                {
                    RenderStatus = "Error: " + e.Message;
                    Thread.Sleep(500);
                }
            }
            if (screen != null) screen.Dispose();
        }

        public static string Label(Mode m)
        {
            switch (m)
            {
                case Mode.VirtualDisplay: return "Virtual display";
                case Mode.Monitor: return "Screen mirror";
                case Mode.Image: return "Image";
                case Mode.Gif: return "GIF";
                case Mode.Video: return "Video";
                case Mode.Color: return "Colour";
                case Mode.Clock: return "Clock";
                case Mode.Aura: return "Aura Sync";
                default: return "Off";
            }
        }

        static Color Rgb(int v) { return Color.FromArgb(v >> 16 & 255, v >> 8 & 255, v & 255); }

        static byte[] BlackJpeg()
        {
            using (var b = new Bitmap(Lcd.Width, Lcd.Height, PixelFormat.Format24bppRgb))
            {
                using (var g = Graphics.FromImage(b)) g.Clear(Color.Black);
                return Render.Jpeg(b, 50);
            }
        }

        Bitmap Message(string text)
        {
            var b = Render.Compose(null, Lcd.Width, Lcd.Height, false, Color.Black);
            using (var g = Graphics.FromImage(b))
            using (var f = new Font("Segoe UI", 24, GraphicsUnit.Pixel))
            using (var sf = new StringFormat { Alignment = StringAlignment.Center, LineAlignment = StringAlignment.Center })
                g.DrawString(text, f, Brushes.White, new RectangleF(40, 0, Lcd.Width - 80, Lcd.Height), sf);
            return b;
        }

        Bitmap Produce(Config c, out int delay, out string note, out byte[] direct)
        {
            note = null; direct = null;
            delay = 1000;
            int streamDelay = 1000 / Math.Max(1, Math.Min(30, c.Fps)); // screen capture and Aura
            switch (c.Mode)
            {
                case Mode.VirtualDisplay:
                {
                    int mon = ScreenSource.FindBySize(Lcd.Width, Lcd.Height);
                    if (mon < 0) { note = "Virtual display not installed"; return Message("Virtual display not installed.\nInstall it on the Device page."); }
                    delay = streamDelay;
                    return Capture(ScreenSource.MonitorRect(mon, null), c);
                }
                case Mode.Monitor:
                {
                    Rectangle? sub = null;
                    if (c.UseRegion) sub = new Rectangle(c.RegionX, c.RegionY, Math.Max(1, c.RegionW), Math.Max(1, c.RegionH));
                    delay = streamDelay;
                    return Capture(ScreenSource.MonitorRect(Math.Min(c.MonitorIndex, System.Windows.Forms.Screen.AllScreens.Length - 1), sub), c);
                }
                case Mode.Image:
                {
                    var i = LoadImage(c.ImagePath);
                    if (i == null) { note = "Choose an image"; return Message("No image selected"); }
                    return Render.Compose(i, Lcd.Width, Lcd.Height, c.FitContain, Rgb(c.ColorRgb));
                }
                case Mode.Gif:
                {
                    var frames = LoadGif(c);
                    if (frames == null || frames.Count == 0) { note = "Choose a GIF"; return Message("No GIF selected"); }
                    if (gifIdx >= frames.Count) gifIdx = 0;
                    var f = frames[gifIdx++];
                    delay = f.Value;
                    return new Bitmap(f.Key);
                }
                case Mode.Video:
                {
                    var clip = LoadVideo(c);
                    if (clip == null)
                    {
                        if (string.IsNullOrEmpty(c.VideoPath)) { note = "Choose a video"; return Message("No video selected"); }
                        double p = VideoProgress;
                        note = p >= 0 ? string.Format("Preparing video {0:P0}", p) : "Video could not be loaded";
                        delay = 200;
                        return Message(p >= 0 ? string.Format("Preparing video\n{0:P0}", p) : "Video could not be loaded:\n" + Path.GetFileName(c.VideoPath));
                    }
                    if (videoIdx >= clip.Frames.Count) videoIdx = 0;
                    direct = clip.Frames[videoIdx++];
                    delay = clip.FrameMs;
                    return (Bitmap)Image.FromStream(new MemoryStream(direct));
                }
                case Mode.Aura:
                {
                    delay = streamDelay;
                    string auraStatus;
                    var frame = AuraRender.Render(AuraState.Read(), out auraStatus);
                    note = auraStatus + string.Format(" · {0:F0} fps", Fps);
                    return frame;
                }
                case Mode.Clock:
                {
                    delay = 500;
                    var b = Render.Compose(null, Lcd.Width, Lcd.Height, false, Rgb(c.ColorRgb));
                    Render.DrawClock(b, c, Coolant, CpuTemp.C(), Cpu.Percent());
                    return b;
                }
                default: // Color
                    return Render.Compose(null, Lcd.Width, Lcd.Height, false, Rgb(c.ColorRgb));
            }
        }

        public static VideoOptions VideoOptionsOf(Config c)
        {
            return new VideoOptions
            {
                Path = c.VideoPath, Start = c.VideoStart, Length = c.VideoLength, Fps = c.VideoFps,
                Zoom = c.VideoZoom, PanX = c.VideoPanX, PanY = c.VideoPanY, Quality = c.Quality
            };
        }

        // Returns the clip when ready; otherwise starts preparing it in the background and returns null.
        VideoClip LoadVideo(Config c)
        {
            if (string.IsNullOrEmpty(c.VideoPath) || !File.Exists(c.VideoPath)) return null;
            var o = VideoOptionsOf(c);
            string key = o.Key();
            if (key == videoKey) return video;
            if (key == failedVideoKey) return null;
            if (videoLoader != null && videoLoader.IsAlive) return null; // previous load still running; it re-checks on finish
            videoKey = null; video = null;
            VideoProgress = 0;
            videoLoader = new Thread(() =>
            {
                VideoClip clip = null;
                try { clip = Video.Load(o, p => { VideoProgress = p; return !quit && VideoOptionsOf(cfg).Key() == key; }); }
                catch (Exception) { }
                if (clip != null && clip.Frames.Count > 0) { video = clip; videoIdx = 0; videoKey = key; VideoProgress = -1; }
                else if (VideoOptionsOf(cfg).Key() == key) { failedVideoKey = key; VideoProgress = -1; }
                else VideoProgress = 0; // superseded by new settings; next frame starts the new load
            }) { IsBackground = true, Name = "lcd-video" };
            videoLoader.Start();
            return null;
        }

        Bitmap Capture(Rectangle r, Config c)
        {
            if (screen == null || r != screenRect)
            {
                if (screen != null) screen.Dispose();
                screen = new ScreenSource(r);
                screenRect = r;
            }
            var shot = screen.Grab();
            if (r.Width == Lcd.Width && r.Height == Lcd.Height) return new Bitmap(shot); // no resampling needed
            return Render.Compose(shot, Lcd.Width, Lcd.Height, c.FitContain, Color.Black);
        }

        Image LoadImage(string path)
        {
            if (string.IsNullOrEmpty(path) || !File.Exists(path)) return null;
            string key = path + "|" + File.GetLastWriteTimeUtc(path).Ticks;
            if (key != imgKey)
            {
                if (img != null) img.Dispose();
                img = Image.FromStream(new MemoryStream(File.ReadAllBytes(path))); // don't lock the file
                imgKey = key;
            }
            return img;
        }

        List<KeyValuePair<Bitmap, int>> LoadGif(Config c)
        {
            string path = c.GifPath;
            if (string.IsNullOrEmpty(path) || !File.Exists(path)) return null;
            string key = path + "|" + File.GetLastWriteTimeUtc(path).Ticks + "|" + c.FitContain + "|" + c.ColorRgb;
            if (key == gifKey) return gif;
            if (gif != null) foreach (var f in gif) f.Key.Dispose();
            gif = new List<KeyValuePair<Bitmap, int>>();
            using (var g = Image.FromStream(new MemoryStream(File.ReadAllBytes(path))))
            {
                var dim = new FrameDimension(g.FrameDimensionsList[0]);
                int n = g.GetFrameCount(dim);
                byte[] delays = null;
                try { delays = g.GetPropertyItem(0x5100).Value; } catch (ArgumentException) { }
                for (int i = 0; i < n; i++)
                {
                    g.SelectActiveFrame(dim, i);
                    int d = delays != null && delays.Length >= (i + 1) * 4 ? BitConverter.ToInt32(delays, i * 4) * 10 : 100;
                    if (d < 20) d = 100;
                    gif.Add(new KeyValuePair<Bitmap, int>(Render.Compose(g, Lcd.Width, Lcd.Height, c.FitContain, Rgb(c.ColorRgb)), d));
                }
            }
            gifKey = key; gifIdx = 0;
            return gif;
        }

        // ================= sender =================
        volatile Lcd current;

        void SendLoop()
        {
            Lcd lcd = null;
            long sentSeq = -1;
            var sw = Stopwatch.StartNew();
            long fpsT = 0, lastSent = 0; int fpsN = 0;
            while (!quit)
            {
                if (lcd == null)
                {
                    try
                    {
                        lcd = Lcd.Open();
                        DeviceStatus = "Connecting to cooler...";
                        StartControl(lcd);
                        sentSeq = -1;
                    }
                    catch (Exception e)
                    {
                        Connected = false;
                        DeviceStatus = "Cooler not found: " + e.Message;
                        for (int i = 0; i < 20 && !quit; i++) Thread.Sleep(100);
                        continue;
                    }
                }
                newFrame.WaitOne(250);
                var f = latest;
                // Repeat the current frame as a keep-alive: the LX800 firmware falls back to its built-in
                // temperature screen when no frame arrives for a few seconds (static images change rarely).
                bool keepAlive = f != null && f.Seq == sentSeq && sw.ElapsedMilliseconds - lastSent >= 500;
                if (f == null || (f.Seq == sentSeq && !keepAlive) || holdFrames || suspended || GameIdle(cfg)) { if (GameIdle(cfg)) Fps = 0; continue; }
                try
                {
                    lcd.ShowJpeg(f.Jpeg); // first transfer after opening stalls ~5 s (device wake-up)
                    sentSeq = f.Seq;
                    lastSent = sw.ElapsedMilliseconds;
                    Connected = true;
                    fpsN++;
                    long now = sw.ElapsedMilliseconds;
                    if (now - fpsT >= 1000) { Fps = fpsN * 1000.0 / (now - fpsT); fpsN = 0; fpsT = now; }
                }
                catch (Exception e)
                {
                    current = null; Connected = false; Fps = 0;
                    DeviceStatus = "Connection lost: " + e.Message;
                    lcd.Dispose(); lcd = null;
                }
            }
            if (lcd != null)
            {
                try { if (cfg.BlankOnExit) lcd.ShowJpeg(BlackJpeg()); } catch (Exception) { }
                current = null;
                lcd.Dispose();
            }
        }

        // ---- device jobs (uploads to the cooler's flash), run on the control thread ----
        public class DeviceJob
        {
            public string Name;
            public Action<Lcd, Action<int>> Work;
            public Action<int> Progress;      // percent, called on the control thread
            public Action<Exception> Done;    // null on success, called on the control thread
        }

        readonly Queue<DeviceJob> jobs = new Queue<DeviceJob>();
        volatile bool holdFrames;

        // Runs once the cooler is connected; live frames pause while it runs.
        public void Enqueue(DeviceJob job) { lock (jobs) jobs.Enqueue(job); }

        public bool JobsPending { get { lock (jobs) return jobs.Count > 0 || holdFrames; } }

        // Control commands (brightness, serial, uploads) on their own thread: the cooler stalls the first control
        // transfer after opening for ~10 s, which must not block frame streaming on the LCD interface.
        void StartControl(Lcd lcd)
        {
            current = lcd;
            HwRotation = -1; // re-read from the cooler on every connect
            var t = new Thread(() =>
            {
                int sent = -1, sleepSent = -1;
                bool gotSerial = false;
                long lastTemp = 0;
                var tsw = Stopwatch.StartNew();
                while (!quit && current == lcd)
                {
                    try
                    {
                        int want = cfg.Brightness;
                        if (want != sent) { lcd.SetBrightness(want); sent = want; }
                        int wantSleep = cfg.DeviceSleep ? 1 : 0;
                        if (wantSleep != sleepSent) { lcd.SetSleep(cfg.DeviceSleep); sleepSent = wantSleep; }
                        // queries time out while the cooler is still waking up; keep trying instead of giving up
                        if (!gotSerial) try { Serial = lcd.Serial(); gotSerial = true; } catch (Exception) { }
                        // the cooler's stored rotation (turns its own idle/boot screens, not streamed frames)
                        if (HwRotation < 0) try { HwRotation = lcd.Rotation(); } catch (Exception) { }
                        int hw = HwFor(cfg);
                        if (hw >= 0 && HwRotation >= 0 && hw != HwRotation) { lcd.SetRotation(hw); HwRotation = hw; } // flash write: only on change
                        // water temperature every 2 s, or at the game-mode interval while a game runs
                        if (tsw.ElapsedMilliseconds - lastTemp >= (Gaming ? Math.Max(2000, cfg.GameSensorMs) : 2000))
                        {
                            try
                            {
                                CoolantHundredths = (int)Math.Round(lcd.CoolantC() * 100);
                                long now = NowMs;
                                if (CoolantAtMs >= 0) CoolantPeriodMs = (int)(now - CoolantAtMs);
                                CoolantAtMs = now;
                            }
                            catch (Exception) { }
                            lastTemp = tsw.ElapsedMilliseconds;
                        }
                    }
                    catch (Exception) { return; } // session closed; sender reconnects
                    DeviceJob job = null;
                    lock (jobs) if (jobs.Count > 0) job = jobs.Dequeue();
                    if (job != null) RunJob(lcd, job);
                    Thread.Sleep(100);
                }
            }) { IsBackground = true, Name = "lcd-control" };
            t.Start();
        }

        void RunJob(Lcd lcd, DeviceJob job)
        {
            Exception err = null;
            holdFrames = true;
            try
            {
                Thread.Sleep(200); // let an in-flight frame finish
                job.Work(lcd, p => { if (job.Progress != null) job.Progress(p); });
            }
            catch (Exception e) { err = e; }
            finally { holdFrames = false; newFrame.Set(); }
            if (job.Done != null) job.Done(err);
        }
    }

    // Total CPU load from GetSystemTimes (locale-independent, unlike performance counters).
    static class Cpu
    {
        [DllImport("kernel32.dll")] static extern bool GetSystemTimes(out long idle, out long kernel, out long user);
        static long lastIdle, lastTotal; static int last;
        static DateTime lastAt = DateTime.MinValue;

        public static int Percent()
        {
            if ((DateTime.UtcNow - lastAt).TotalMilliseconds < 900) return last;
            long idle, kernel, user;
            if (!GetSystemTimes(out idle, out kernel, out user)) return last;
            long total = kernel + user; // kernel includes idle
            long dt = total - lastTotal, di = idle - lastIdle;
            if (lastTotal != 0 && dt > 0) last = (int)Math.Round(100.0 * (dt - di) / dt);
            lastIdle = idle; lastTotal = total; lastAt = DateTime.UtcNow;
            return last;
        }
    }

    // CPU temperature from the `ttlcd hwmon` helper (vendor HWiNFO-based DLL, elevated). -1 when the helper
    // isn't running. Falls back to the ACPI thermal zone where that exists (it doesn't on most AMD boards).
    static class CpuTemp
    {
        static int last = -1; static DateTime at = DateTime.MinValue;
        public static int C()
        {
            if ((DateTime.UtcNow - at).TotalMilliseconds < 1500) return last;
            at = DateTime.UtcNow;
            int t = Sensors.CpuTempC();
            if (t >= 0) { last = t; return last; }
            try
            {
                using (var s = new System.Management.ManagementObjectSearcher(@"root\WMI",
                    "SELECT CurrentTemperature FROM MSAcpi_ThermalZoneTemperature"))
                    foreach (System.Management.ManagementObject o in s.Get())
                    {
                        int c = (int)Math.Round(Convert.ToDouble(o["CurrentTemperature"]) / 10.0 - 273.15);
                        if (c > 0 && c < 150) { last = c; return last; }
                    }
            }
            catch (Exception) { }
            last = -1;
            return last;
        }
    }

    // Physical RAM load (percent) via GlobalMemoryStatusEx.
    static class Mem
    {
        [StructLayout(LayoutKind.Sequential)]
        struct MEMORYSTATUSEX { public uint dwLength, dwMemoryLoad; public ulong a, b, c, d, e, f, g; }
        [DllImport("kernel32.dll")] static extern bool GlobalMemoryStatusEx(ref MEMORYSTATUSEX m);
        public static int PercentUsed()
        {
            var m = new MEMORYSTATUSEX { dwLength = (uint)Marshal.SizeOf(typeof(MEMORYSTATUSEX)) };
            return GlobalMemoryStatusEx(ref m) ? (int)m.dwMemoryLoad : 0;
        }
    }
}
