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
  ttlcd temp                          water temperature from the cooler's sensor
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
  ttlcd hwmon                              CPU temperature helper in this console (admin)";

        static int Main(string[] args)
        {
            string cmd = args.Length > 0 ? args[0].ToLowerInvariant() : "help";
            Console.CancelKeyPress += (s, e) => { e.Cancel = true; stop = true; };
            try
            {
                switch (cmd)
                {
                    case "probe": return Probe();
                    case "info": using (var l = Lcd.Open()) { Console.WriteLine("serial={0} type={1} rotation={2}", l.Serial(), l.DeviceType(), l.Rotation()); } return 0;
                    case "temp":
                        using (var l = Lcd.Open())
                            for (int i = 0; ; i++)   // the first control reply after opening can take ~10 s
                                try { Console.WriteLine("water={0:F2} C", l.CoolantC()); return 0; }
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
            if (devs.Count == 0) { Console.WriteLine("No 264A:233C device found."); return 1; }
            foreach (var d in devs) Console.WriteLine(d);
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
