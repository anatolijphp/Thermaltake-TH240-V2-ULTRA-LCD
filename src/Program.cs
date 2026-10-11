using System;
using System.Diagnostics;
using System.Drawing;
using System.Threading;

namespace TtLcd
{
    static class Program
    {
        static volatile bool stop;

        const string Usage =
@"ttlcd - Thermaltake CL-W383 LCD tool
  ttlcd probe                         list HID interfaces (read-only)
  ttlcd info                          serial, type, rotation
  ttlcd temp                          coolant temperature from the cooler's sensor
  ttlcd brightness <0-100>
  ttlcd rotate <0|90|180|270>         hardware rotation
  ttlcd color <RRGGBB>                fill screen with a colour
  ttlcd image <file> [-q 85]          show jpg/png/bmp
  ttlcd gif <file> [-q 85]            loop an animated GIF (Ctrl+C to stop)
  ttlcd idle <file> [-rot 0] [-q 85]  store the idle (standby) screen in the cooler: picture/GIF/video frame
  ttlcd boot <file> [-rot 0] [-q 75]  store the loading (boot) animation in the cooler: GIF/video/picture
  ttlcd files [folder]                list files stored in the cooler (read-only)
  ttlcd monitors                      list monitors
  ttlcd stream [-m N] [-r x,y,w,h] [-fps 30] [-q 80]
                                      mirror a monitor (default: the 480x480 one) or region
  ttlcd display install|uninstall|status   virtual 480x480 monitor for Wallpaper Engine (admin)
  ttlcd aura install|uninstall|status      Armoury Crate / Aura Sync plugin (admin)
  ttlcd service install|uninstall|status   CPU temperature service, starts with Windows (admin)
  ttlcd hwmon                              CPU temperature helper in this console (admin)
  ttlcd run                           drive the screen from the settings (Linux service; Ctrl+C to stop)
  ttlcd config                        show the settings (%APPDATA%\ttlcd, ~/.config/ttlcd on Linux)
  ttlcd set <name> <value>            change a setting, e.g. set Mode Clock, set GifPath /path/a.gif
  ttlcd render <out.jpg> [mode]       render one frame of the current mode to a file (no cooler needed)";

        static int Main(string[] args)
        {
            string cmd = args.Length > 0 ? args[0].ToLowerInvariant() : "help";
            Console.CancelKeyPress += (s, e) => { e.Cancel = true; stop = true; };
            try
            {
                switch (cmd)
                {
                    case "probe": return Probe();
                    case "run": return Run();
                    case "config": return ShowConfig();
                    case "set": return Set(Arg(args, 1), Arg(args, 2));
                    case "render": return RenderFile(Arg(args, 1), args.Length > 2 ? args[2] : null);
                    case "info": using (var l = Lcd.Open()) { Console.WriteLine("serial={0} type={1} rotation={2}", l.Serial(), l.DeviceType(), l.Rotation()); } return 0;
                    case "temp":
                        using (var l = Lcd.Open())
                            for (int i = 0; ; i++)   // the first control reply after opening can take ~10 s
                                try { Console.WriteLine("coolant={0:F2} C", l.CoolantC()); return 0; }
                                catch (TimeoutException) { if (i >= 15) throw; }
                    case "brightness": using (var l = Lcd.Open()) l.SetBrightness(int.Parse(Arg(args, 1))); return 0;
                    case "rotate": using (var l = Lcd.Open()) l.SetRotation(int.Parse(Arg(args, 1))); return 0;
                    case "color": return Color(Arg(args, 1));
                    case "image": using (var l = Lcd.Open()) l.ShowJpeg(Render.FileToJpeg(Arg(args, 1), Lcd.Width, Lcd.Height, 0, Opt(args, "-q", 85))); return 0;
                    case "gif": return Gif(Arg(args, 1), Opt(args, "-q", 85));
                    case "idle": return Idle(Arg(args, 1), Opt(args, "-rot", 0), Opt(args, "-q", 85));
                    case "boot": return BootAnim(Arg(args, 1), Opt(args, "-rot", 0), Opt(args, "-q", 75));
                    case "files":
                        using (var l = Lcd.Open())
                            foreach (var f in l.ListFiles(args.Length > 1 ? args[1] : "")) Console.WriteLine(f);
                        return 0;
                    case "monitors": ScreenSource.ListMonitors(); return 0;
                    case "stream": return Stream(args);
                    case "display": return VirtualDisplay.Run(Arg(args, 1));
                    case "aura": return AuraInstall.Run(Arg(args, 1));
                    case "hwmon": return Sensors.RunHelper();   // elevated: publish CPU temperature (as TOWEI does)
                    case "sensor-service": System.ServiceProcess.ServiceBase.Run(new SensorService()); return 0; // run by Windows
                    case "service": return SensorService.Run(Arg(args, 1));
                    default: Console.WriteLine(Usage); return 1;
                }
            }
            catch (Exception e)
            {
                Console.Error.WriteLine("error: " + e.Message);
                return 2;
            }
        }

        static string Arg(string[] a, int i)
        {
            if (a.Length <= i) throw new ArgumentException("missing argument; run 'ttlcd' for usage");
            return a[i];
        }

        static int Opt(string[] a, string name, int def)
        {
            for (int i = 0; i + 1 < a.Length; i++) if (a[i] == name) return int.Parse(a[i + 1]);
            return def;
        }

        static string OptS(string[] a, string name)
        {
            for (int i = 0; i + 1 < a.Length; i++) if (a[i] == name) return a[i + 1];
            return null;
        }

        static int Probe()
        {
            var devs = Hid.Enumerate(Lcd.Vid, Lcd.Pid);
            devs.AddRange(Hid.Enumerate(Lcd.AltVid, Lcd.AltPid)); // LX800 firmware
            if (devs.Count == 0) { Console.WriteLine("No cooler LCD found (264A:233C or 1A2C:151D)."); return 1; }
            foreach (var d in devs) Console.WriteLine(d);
            return 0;
        }

        static string ConfigFile { get { return System.IO.Path.Combine(Config.Folder, "config.xml"); } }

        // Modes Linux can't show fall back to the clock (without changing the saved setting).
        static Config Effective(Config c)
        {
            if (!Os.Supports(c.Mode))
            {
                Console.WriteLine("mode {0} needs Windows; showing the clock (ttlcd set Mode Clock|Image|Gif|Color|Off)", c.Mode);
                c.Mode = Mode.Clock;
            }
            return c;
        }

        // Headless: what Studio does, without the window. Settings are re-read when the file changes (ttlcd set).
        static int Run()
        {
            var engine = new Engine(Effective(Config.Load()));
            engine.Start();
            Console.WriteLine("driving the cooler screen ({0}); settings: {1}", Engine.Label(engine.Config.Mode), ConfigFile);
            DateTime stamp = System.IO.File.Exists(ConfigFile) ? System.IO.File.GetLastWriteTimeUtc(ConfigFile) : DateTime.MinValue;
            string shown = null;
            while (!stop)
            {
                Thread.Sleep(1000);
                var t = System.IO.File.Exists(ConfigFile) ? System.IO.File.GetLastWriteTimeUtc(ConfigFile) : DateTime.MinValue;
                if (t != stamp) { stamp = t; engine.Config = Effective(Config.Load()); Console.WriteLine("settings reloaded"); }
                string s = engine.Status;
                if (s != shown) { Console.WriteLine(s); shown = s; }
            }
            engine.Stop();
            return 0;
        }

        static System.Reflection.FieldInfo[] Settings()
        {
            return typeof(Config).GetFields(System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.Instance);
        }

        static int ShowConfig()
        {
            var c = Config.Load();
            Console.WriteLine("# " + ConfigFile);
            foreach (var f in Settings())
            {
                if (f.FieldType.IsArray) continue;
                var v = f.GetValue(c);
                Console.WriteLine("{0} = {1}", f.Name, v is int && f.Name.EndsWith("Rgb") ? string.Format("#{0:X6}", v) : v);
            }
            return 0;
        }

        static int Set(string name, string value)
        {
            var f = typeof(Config).GetField(name, System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.Instance | System.Reflection.BindingFlags.IgnoreCase);
            if (f == null || f.FieldType.IsArray) throw new ArgumentException("unknown setting '" + name + "' (see 'ttlcd config')");
            var inv = System.Globalization.CultureInfo.InvariantCulture;
            object v;
            var t = f.FieldType;
            if (t == typeof(string)) v = value;
            else if (t.IsEnum) v = Enum.Parse(t, value, true);
            else if (t == typeof(bool)) v = value == "1" || value.Equals("on", StringComparison.OrdinalIgnoreCase) || value.Equals("yes", StringComparison.OrdinalIgnoreCase) || value.Equals("true", StringComparison.OrdinalIgnoreCase);
            else if (t == typeof(int)) v = value.StartsWith("#") ? Convert.ToInt32(value.Substring(1), 16) : int.Parse(value, inv);
            else if (t == typeof(double)) v = double.Parse(value, inv);
            else if (t == typeof(float)) v = float.Parse(value, inv);
            else throw new ArgumentException(f.Name + " can't be set from the command line");
            var c = Config.Load();
            f.SetValue(c, v);
            c.Save();
            Console.WriteLine("{0} = {1}", f.Name, v);
            if (Os.Linux && v is Mode && (Mode)v == Mode.Aura && !AuraState.LinuxServiceInstalled())
                Console.WriteLine("Note: armory-crate (armory-crated) isn't installed; the screen waits for it until it is.");
            return 0;
        }

        static int RenderFile(string file, string mode)
        {
            var c = Config.Load();
            if (mode != null) c.Mode = (Mode)Enum.Parse(typeof(Mode), mode, true);
            var engine = new Engine(Effective(c));
            System.IO.File.WriteAllBytes(file, engine.RenderOnce());
            Console.WriteLine("{0}: {1} frame, {2} KB", file, Engine.Label(c.Mode), new System.IO.FileInfo(file).Length / 1024);
            return 0;
        }

        static int Color(string hex)
        {
            int rgb = Convert.ToInt32(hex.TrimStart('#'), 16);
            using (var bmp = new Bitmap(Lcd.Width, Lcd.Height))
            using (var g = Graphics.FromImage(bmp))
            using (var l = Lcd.Open())
            {
                g.Clear(System.Drawing.Color.FromArgb(rgb >> 16 & 255, rgb >> 8 & 255, rgb & 255));
                l.ShowJpeg(Render.Jpeg(bmp, 90));
            }
            return 0;
        }

        static void Percent(int p) { Console.Write("\r{0,3}%", p); }

        static int Idle(string path, int rot, int q)
        {
            var jpg = DeviceStore.StandbyJpeg(path, rot, q);
            Console.WriteLine("idle picture: {0} KB", jpg.Length / 1024);
            bool ok;
            using (var l = Lcd.Open()) ok = DeviceStore.WriteStandby(l, jpg, Percent);
            Console.WriteLine(ok ? "\rstored and verified (read-back matches)" : "\rstored but read-back did NOT match");
            return ok ? 0 : 3;
        }

        static int BootAnim(string path, int rot, int q)
        {
            var boot = DeviceStore.BuildBoot(DeviceStore.LoadFrames(path, q, null), rot, q);
            Console.WriteLine("loading animation: {0} frames, {1} ms/frame, {2} KB", boot.Frames, boot.IntervalMs, boot.Pak.Length / 1024);
            using (var l = Lcd.Open()) DeviceStore.WriteBoot(l, boot, Percent);
            Console.WriteLine("\rstored");
            return 0;
        }

        static int Gif(string path, int q)
        {
            var frames = Render.GifFrames(path, Lcd.Width, Lcd.Height, 0, q);
            Console.WriteLine("{0} frames, Ctrl+C to stop", frames.Count);
            using (var l = Lcd.Open())
            {
                var sw = Stopwatch.StartNew();
                long next = 0;
                while (!stop)
                    foreach (var f in frames)
                    {
                        if (stop) break;
                        l.ShowJpeg(f.Jpeg);
                        next += f.DelayMs;
                        long wait = next - sw.ElapsedMilliseconds;
                        if (wait > 0) Thread.Sleep((int)wait);
                        else if (wait < -500) next = sw.ElapsedMilliseconds; // fell behind, resync
                    }
            }
            return 0;
        }

        static int Stream(string[] args)
        {
            int fps = Opt(args, "-fps", 30), q = Opt(args, "-q", 80);
            string ms = OptS(args, "-m"), rs = OptS(args, "-r");
            int mon = ms != null ? int.Parse(ms) : ScreenSource.FindBySize(Lcd.Width, Lcd.Height);
            if (mon < 0) throw new ArgumentException("no 480x480 monitor found; use -m N (see 'ttlcd monitors') or 'ttlcd display install'");
            Rectangle? sub = null;
            if (rs != null) { var p = rs.Split(','); sub = new Rectangle(int.Parse(p[0]), int.Parse(p[1]), int.Parse(p[2]), int.Parse(p[3])); }
            var rect = ScreenSource.MonitorRect(mon, sub);
            Console.WriteLine("streaming monitor {0} {1} at up to {2} fps, Ctrl+C to stop", mon, rect, fps);
            int frameMs = 1000 / Math.Max(1, fps);
            using (var src = new ScreenSource(rect))
            using (var l = Lcd.Open())
            {
                var sw = Stopwatch.StartNew();
                long last = 0, frames = 0, lastReport = 0;
                while (!stop)
                {
                    var shot = src.Grab();
                    byte[] jpg;
                    if (rect.Width == Lcd.Width && rect.Height == Lcd.Height) jpg = Render.Jpeg(shot, q);
                    else using (var fit = Render.Fit(shot, Lcd.Width, Lcd.Height, 0)) jpg = Render.Jpeg(fit, q);
                    l.ShowJpeg(jpg);
                    frames++;
                    long now = sw.ElapsedMilliseconds;
                    if (now - lastReport >= 5000)
                    {
                        Console.WriteLine("{0:F1} fps, {1} KB/frame", frames * 1000.0 / (now - lastReport), jpg.Length / 1024);
                        frames = 0; lastReport = now;
                    }
                    long wait = last + frameMs - sw.ElapsedMilliseconds;
                    if (wait > 0) Thread.Sleep((int)wait);
                    last = sw.ElapsedMilliseconds;
                }
            }
            return 0;
        }
    }
}
