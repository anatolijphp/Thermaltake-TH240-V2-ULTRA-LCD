// Aura Sync (Armoury Crate) support:
//  - AuraState: reads what the TTLcdAura.dll HAL plugin (running inside ASUS LightingService) publishes
//  - AuraRender: turns the effect + LED colours into a 480x480 frame
//  - AuraInstall: registers/unregisters the plugin and restarts LightingService (admin)
using System;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.IO;
using System.IO.MemoryMappedFiles;
using System.Runtime.InteropServices;
using System.Security.Principal;
using Microsoft.Win32;

namespace TtLcd
{
    public class AuraSnapshot
    {
        public int Seq, Effect, LedCount, Speed, Direction;
        public long SyncMs;
        public DateTime UpdatedUtc;
        public uint[] Colors = new uint[64];
    }

    // Layout must match aura\AuraHal.cs (Shared).
    public static class AuraState
    {
        const int Size = 40 + 64 * 4;
        static MemoryMappedFile map;
        static MemoryMappedViewAccessor view;
        static DateTime nextTry;

        // Linux: armory-crated (github: Armory-crate, docs/virtual-devices.md) publishes the same TTAS block as a
        // file. Its writer uses a seqlock: seq is odd while a frame is being written, even when it is complete.
        public static string LinuxPath()
        {
            string run = Environment.GetEnvironmentVariable("XDG_RUNTIME_DIR");
            return string.IsNullOrEmpty(run) ? null : Path.Combine(run, "armory-crate/virtual/ttlcd.ttas");
        }

        static byte[] ReadBlock(string path)
        {
            // A fresh unbuffered open each time: re-reading through one FileStream could be served from its buffer.
            using (var fs = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.ReadWrite | FileShare.Delete, 1))
            {
                var buf = new byte[Size];
                int got = 0, n;
                while (got < Size && (n = fs.Read(buf, got, Size - got)) > 0) got += n;
                return got == Size ? buf : null;
            }
        }

        static AuraSnapshot ReadLinux()
        {
            string path = LinuxPath();
            if (path == null || !File.Exists(path)) return null;
            for (int attempt = 0; attempt < 3; attempt++)
            {
                byte[] a, b;
                try { a = ReadBlock(path); b = ReadBlock(path); }
                catch (IOException) { return null; }
                catch (UnauthorizedAccessException) { return null; }
                if (a == null || b == null) return null;
                int seq = BitConverter.ToInt32(a, 4);
                if ((seq & 1) != 0 || seq != BitConverter.ToInt32(b, 4)) continue;   // writer was mid-update
                if (BitConverter.ToInt32(a, 0) != 0x53415454) return null;           // not running / closed
                var s = new AuraSnapshot();
                s.Seq = seq;
                s.Effect = BitConverter.ToInt32(a, 8);
                s.LedCount = BitConverter.ToInt32(a, 12);
                s.Speed = BitConverter.ToInt32(a, 16);
                s.Direction = BitConverter.ToInt32(a, 20);
                s.SyncMs = BitConverter.ToInt64(a, 24);
                long ticks = BitConverter.ToInt64(a, 32);
                s.UpdatedUtc = ticks > 0 ? new DateTime(ticks, DateTimeKind.Utc) : DateTime.MinValue;
                Buffer.BlockCopy(a, 40, s.Colors, 0, 64 * 4);
                return s;
            }
            return null;
        }

        public static AuraSnapshot Read()
        {
            if (Os.Linux) return ReadLinux();
            if (view == null)
            {
                if (DateTime.UtcNow < nextTry) return null;
                nextTry = DateTime.UtcNow.AddSeconds(2);
                foreach (var name in new[] { @"Global\TTLcdAuraSync", @"Local\TTLcdAuraSync" })
                {
                    try
                    {
                        map = MemoryMappedFile.OpenExisting(name, MemoryMappedFileRights.Read);
                        view = map.CreateViewAccessor(0, Size, MemoryMappedFileAccess.Read);
                        break;
                    }
                    catch (Exception) { }
                }
                if (view == null) return null;
            }
            var s = new AuraSnapshot();
            for (int attempt = 0; attempt < 3; attempt++)
            {
                int seq = view.ReadInt32(4);
                s.Effect = view.ReadInt32(8);
                s.LedCount = view.ReadInt32(12);
                s.Speed = view.ReadInt32(16);
                s.Direction = view.ReadInt32(20);
                s.SyncMs = view.ReadInt64(24);
                long ticks = view.ReadInt64(32);
                view.ReadArray(40, s.Colors, 0, 64);
                if (view.ReadInt32(4) == seq) // no writer interleaved
                {
                    s.Seq = seq;
                    s.UpdatedUtc = ticks > 0 ? new DateTime(ticks, DateTimeKind.Utc) : DateTime.MinValue;
                    return view.ReadInt32(0) == 0x53415454 ? s : null;
                }
            }
            return null;
        }
    }

    public static class AuraRender
    {
        static long syncBaseMs = -1; static long syncLocalMs; static readonly Stopwatch clock = Stopwatch.StartNew();

        static Color FromAura(uint c) { return Color.FromArgb((int)(c & 0xFF), (int)(c >> 8 & 0xFF), (int)(c >> 16 & 0xFF)); }

        static Color Hsv(double h, double v)
        {
            h = (h % 1 + 1) % 1 * 6; int i = (int)h; double f = h - i;
            double q = 1 - f, t = f; double r, g, b;
            switch (i) { case 0: r = 1; g = t; b = 0; break; case 1: r = q; g = 1; b = 0; break; case 2: r = 0; g = 1; b = t; break;
                         case 3: r = 0; g = q; b = 1; break; case 4: r = t; g = 0; b = 1; break; default: r = 1; g = 0; b = q; break; }
            return Color.FromArgb((int)(r * v * 255), (int)(g * v * 255), (int)(b * v * 255));
        }

        static Color Scale(Color c, double k) { k = Math.Max(0, Math.Min(1, k)); return Color.FromArgb((int)(c.R * k), (int)(c.G * k), (int)(c.B * k)); }

        // Aura time in ms: follows Synchronize() so several devices animate in phase.
        static double Now(AuraSnapshot s)
        {
            long local = clock.ElapsedMilliseconds;
            if (s.SyncMs != syncBaseMs) { syncBaseMs = s.SyncMs; syncLocalMs = local; }
            return syncBaseMs > 0 ? syncBaseMs + (local - syncLocalMs) : local;
        }

        public static Bitmap Render(AuraSnapshot s, out string status)
        {
            var bmp = new Bitmap(Lcd.Width, Lcd.Height, PixelFormat.Format32bppRgb);
            if (s == null)
            {
                status = Os.Linux ? "Waiting for armory-crated (Aura Sync service not running)"
                                  : "Waiting for Armoury Crate (Aura plugin not active)";
                using (var g = Graphics.FromImage(bmp))
                using (var f = new Font("Segoe UI", 24, GraphicsUnit.Pixel))
                using (var sf = new StringFormat { Alignment = StringAlignment.Center, LineAlignment = StringAlignment.Center })
                {
                    g.Clear(Color.Black);
                    g.DrawString(Os.Linux ? "Waiting for\narmory-crated / Aura Sync" : "Waiting for\nArmoury Crate / Aura Sync", f, Brushes.White, new RectangleF(0, 0, Lcd.Width, Lcd.Height), sf);
                }
                return bmp;
            }
            int n = Math.Max(1, Math.Min(64, s.LedCount));
            var leds = new Color[n];
            for (int i = 0; i < n; i++) leds[i] = FromAura(s.Colors[i]);
            double t = Now(s) / 1000.0;
            double period = s.Speed <= 0 ? 4 : s.Speed == 1 ? 3 : s.Speed == 2 ? 2 : 1.2; // speed level -> seconds per cycle
            string name;
            switch (s.Effect)
            {
                case 2: // breathing
                    name = "Breathing";
                    double k = 0.5 - 0.5 * Math.Cos(2 * Math.PI * t / period);
                    for (int i = 0; i < n; i++) leds[i] = Scale(leds[i], k);
                    break;
                case 3: // strobing
                    name = "Strobing";
                    bool on = (t / period * 4) % 1 < 0.5;
                    for (int i = 0; i < n; i++) leds[i] = on ? leds[i] : Color.Black;
                    break;
                case 4: // colour cycle: whole screen one hue
                    name = "Color cycle";
                    var cc = Hsv(t / (period * 3), 1);
                    for (int i = 0; i < n; i++) leds[i] = cc;
                    break;
                case 5: case 0xFF: // rainbow around the ring
                    name = s.Effect == 5 ? "Rainbow" : "Default";
                    for (int i = 0; i < n; i++) leds[i] = Hsv(t / (period * 2) + (double)i / n, 1);
                    break;
                case 1: name = "Static"; break;
                default: name = "Manual"; break;
            }
            PaintVisual(bmp, leds, t);
            bool stale = (DateTime.UtcNow - s.UpdatedUtc).TotalSeconds > 30 && s.Effect <= 1;
            status = "Aura Sync · " + name + (stale ? " (no recent updates)" : "");
            return bmp;
        }

        // MilkDrop-style visualiser. Every frame the previous image is warped (zoom, swirl, wavy displacement) and
        // faded, then a fresh waveform is drawn on top in the Aura colours; the feedback smears each waveform into
        // flowing abstract trails. Motion parameters drift on unrelated periods so the picture keeps morphing, and
        // the waveform shape changes every few seconds. Rendered at low resolution and upscaled (soft glow).
        const int N = 200;
        static float[] fb = new float[N * N * 3], fbNext = new float[N * N * 3];
        static readonly float[] rowWarp = new float[N], colWarp = new float[N];
        static readonly byte[] tone = new byte[4096];
        static Bitmap low;
        static double lastT = double.NaN;

        static unsafe void PaintVisual(Bitmap bmp, Color[] leds, double t)
        {
            if (low == null)
            {
                low = new Bitmap(N, N, PixelFormat.Format32bppRgb);
                for (int i = 0; i < tone.Length; i++) tone[i] = (byte)(255 * (1 - Math.Exp(-i / 160.0))); // soft clip
            }
            double dt = double.IsNaN(lastT) || t < lastT ? 1 / 30.0 : Math.Min(0.2, t - lastT);
            lastT = t;
            double step = dt * 30; // per-frame amounts below are tuned for 30 fps

            Warp(t, step);
            DrawWave(leds, t);

            var bd = low.LockBits(new Rectangle(0, 0, N, N), ImageLockMode.WriteOnly, PixelFormat.Format32bppRgb);
            try
            {
                for (int y = 0; y < N; y++)
                {
                    int* row = (int*)((byte*)bd.Scan0 + y * bd.Stride);
                    for (int x = 0, i = y * N * 3; x < N; x++, i += 3)
                        row[x] = (0xFF << 24) | (tone[Math.Min(4095, (int)fb[i])] << 16) | (tone[Math.Min(4095, (int)fb[i + 1])] << 8) | tone[Math.Min(4095, (int)fb[i + 2])];
                }
            }
            finally { low.UnlockBits(bd); }
            using (var g = Graphics.FromImage(bmp))
            {
                g.InterpolationMode = InterpolationMode.HighQualityBilinear;
                g.DrawImage(low, new Rectangle(-4, -4, bmp.Width + 8, bmp.Height + 8));
            }
        }

        // fbNext = previous frame sampled through a zoom/swirl/wave field, faded; then swapped into fb.
        static void Warp(double t, double step)
        {
            double zoom = 1 + (0.018 + 0.014 * Math.Sin(t * 0.21)) * step;          // >1 flows outward
            double zoomExp = 0.35 * Math.Sin(t * 0.17 + 1);                          // centre vs edge speed
            double rot = (0.006 + 0.012 * Math.Sin(t * 0.11)) * step;
            double swirl = 0.03 * Math.Sin(t * 0.07 + 2) * step;                     // extra twist near the centre
            double wa = (0.006 + 0.005 * Math.Sin(t * 0.29)) * step, wf = 5 + 3 * Math.Sin(t * 0.05);
            for (int k = 0; k < N; k++)
            {
                double c = (k + 0.5) / N * 2 - 1;
                rowWarp[k] = (float)(wa * Math.Sin(c * wf + t * 1.3));
                colWarp[k] = (float)(wa * Math.Cos(c * wf - t * 1.1));
            }
            // slow, slightly different decay per channel tints the trails as they fade
            double d = Math.Pow(0.935, step);
            float dr = (float)(d * (1 + 0.012 * Math.Sin(t * 0.23))), dg = (float)(d * (1 + 0.012 * Math.Sin(t * 0.31 + 2))), db = (float)(d * (1 + 0.012 * Math.Sin(t * 0.19 + 4)));
            float[] src = fb, dst = fbNext;
            for (int y = 0; y < N; y++)
            {
                double v = (y + 0.5) / N * 2 - 1;
                for (int x = 0; x < N; x++)
                {
                    double u = (x + 0.5) / N * 2 - 1;
                    double r = Math.Sqrt(u * u + v * v);
                    double a = rot + swirl * Math.Max(0, 1 - r);
                    double ca = Math.Cos(a), sa = Math.Sin(a);
                    double z = zoom * (1 + (zoom - 1) * zoomExp * (r - 0.5) * 8);
                    double su = (u * ca - v * sa) / z + rowWarp[y];
                    double sv = (u * sa + v * ca) / z + colWarp[x];
                    double fx = (su + 1) * 0.5 * N - 0.5, fy = (sv + 1) * 0.5 * N - 0.5;
                    int x0 = (int)Math.Floor(fx), y0 = (int)Math.Floor(fy);
                    int o = (y * N + x) * 3;
                    if (x0 < 0 || y0 < 0 || x0 >= N - 1 || y0 >= N - 1) { dst[o] = dst[o + 1] = dst[o + 2] = 0; continue; }
                    float ax = (float)(fx - x0), ay = (float)(fy - y0);
                    float w00 = (1 - ax) * (1 - ay), w10 = ax * (1 - ay), w01 = (1 - ax) * ay, w11 = ax * ay;
                    int i00 = (y0 * N + x0) * 3, i10 = i00 + 3, i01 = i00 + N * 3, i11 = i01 + 3;
                    dst[o] = (src[i00] * w00 + src[i10] * w10 + src[i01] * w01 + src[i11] * w11) * dr;
                    dst[o + 1] = (src[i00 + 1] * w00 + src[i10 + 1] * w10 + src[i01 + 1] * w01 + src[i11 + 1] * w11) * dg;
                    dst[o + 2] = (src[i00 + 2] * w00 + src[i10 + 2] * w10 + src[i01 + 2] * w01 + src[i11 + 2] * w11) * db;
                }
            }
            fb = dst; fbNext = src;
        }

        // The "waveform": a pseudo-audio signal (sum of drifting sines) drawn as one of several shapes,
        // coloured along its length by the Aura LEDs.
        static void DrawWave(Color[] leds, double t)
        {
            const int K = 900;
            int shape = (int)(t / 14) % 4;
            double energy = 0.55 + 0.45 * Math.Pow(Math.Sin(t * 1.7), 2);
            double spin = t * 0.35;
            for (int k = 0; k < K; k++)
            {
                double s = (double)k / K, th = s * 2 * Math.PI;
                double wave = 0.55 * Math.Sin(3 * th + t * 2.1) + 0.3 * Math.Sin(7 * th - t * 3.3) + 0.15 * Math.Sin(13 * th + t * 5.2);
                double x, y;
                switch (shape)
                {
                    case 0: // pulsing ring
                    {
                        double r = 0.32 + 0.14 * energy * wave;
                        x = r * Math.Cos(th + spin); y = r * Math.Sin(th + spin);
                        break;
                    }
                    case 1: // Lissajous knot
                    {
                        double fa = 3 + Math.Sin(t * 0.05), fbq = 2 + Math.Cos(t * 0.04);
                        x = 0.5 * Math.Sin(fa * th + t * 0.7) + 0.04 * wave; y = 0.5 * Math.Sin(fbq * th + t * 0.5);
                        break;
                    }
                    case 2: // spinning oscilloscope line
                    {
                        double along = (s * 2 - 1) * 0.8, off = 0.18 * energy * wave;
                        double ca = Math.Cos(spin), sa = Math.Sin(spin);
                        x = along * ca - off * sa; y = along * sa + off * ca;
                        break;
                    }
                    default: // three spiral arms
                    {
                        int arm = k % 3; double p = (double)(k / 3) / (K / 3);
                        double a = p * 5 + spin * 2 + arm * 2 * Math.PI / 3, r = p * 0.7 * (0.85 + 0.15 * wave);
                        x = r * Math.Cos(a); y = r * Math.Sin(a);
                        break;
                    }
                }
                Color c = LedAt(leds, s);
                Splat((x + 1) * 0.5 * N - 0.5, (y + 1) * 0.5 * N - 0.5, c, 0.9f);
            }
            // a soft beat at the centre
            Color core = LedAt(leds, (t * 0.05) % 1);
            float beat = (float)Math.Pow(Math.Max(0, Math.Sin(t * 3.4)), 8) * 6;
            for (int k = 0; k < 24; k++)
            {
                double a = k * Math.PI / 12 + spin;
                Splat(N / 2 - 0.5 + 3 * Math.Cos(a), N / 2 - 0.5 + 3 * Math.Sin(a), core, beat);
            }
        }

        static Color LedAt(Color[] leds, double f)
        {
            int n = leds.Length;
            double p = (f % 1 + 1) % 1 * n;
            int i0 = (int)p % n, i1 = (i0 + 1) % n;
            double k = p - Math.Floor(p);
            Color a = leds[i0], b = leds[i1];
            return Color.FromArgb((int)(a.R + (b.R - a.R) * k), (int)(a.G + (b.G - a.G) * k), (int)(a.B + (b.B - a.B) * k));
        }

        // Additive bilinear dot into the feedback buffer.
        static void Splat(double fx, double fy, Color c, float amount)
        {
            int x0 = (int)Math.Floor(fx), y0 = (int)Math.Floor(fy);
            if (x0 < 0 || y0 < 0 || x0 >= N - 1 || y0 >= N - 1) return;
            float ax = (float)(fx - x0), ay = (float)(fy - y0);
            float[] w = { (1 - ax) * (1 - ay), ax * (1 - ay), (1 - ax) * ay, ax * ay };
            int[] idx = { (y0 * N + x0) * 3, (y0 * N + x0 + 1) * 3, ((y0 + 1) * N + x0) * 3, ((y0 + 1) * N + x0 + 1) * 3 };
            for (int j = 0; j < 4; j++)
            {
                float k = w[j] * amount;
                fb[idx[j]] += c.R * k; fb[idx[j] + 1] += c.G * k; fb[idx[j] + 2] += c.B * k;
            }
        }
    }

    // Registration of the HAL plugin (both registry views; LightingService is 32-bit).
    public static class AuraInstall
    {
        const string Clsid = "{C0C96C60-30E3-4021-9904-966EC4FA0F0F}";
        const string Category = @"CLSID\{109DC3E4-B9FF-4AF3-9008-AB13705D4E5F}\Instance\{E9BBD754-6CF4-492E-BA89-782177A2771B}\Instance\" + Clsid;
        static readonly string Dir = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), "ttlcd", "aura");

        public static bool Installed
        {
            get
            {
                using (var k = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, RegistryView.Registry32).OpenSubKey(@"SOFTWARE\Classes\" + Category))
                    return k != null;
            }
        }

        public static int Run(string action)
        {
            switch ((action ?? "").ToLowerInvariant())
            {
                case "status":
                    Console.WriteLine("plugin registered: {0}", Installed);
                    var s = AuraState.Read();
                    Console.WriteLine(s == null ? "no data from Armoury Crate yet" : string.Format("effect {0}, {1} LEDs, seq {2}, updated {3:HH:mm:ss}", s.Effect, s.LedCount, s.Seq, s.UpdatedUtc.ToLocalTime()));
                    return 0;
                case "install": return Elevated("install") ? Install() : 0;
                case "uninstall": return Elevated("uninstall") ? Uninstall() : 0;
                default: Console.WriteLine("ttlcd aura install|uninstall|status"); return 1;
            }
        }

        static bool Elevated(string action)
        {
            if (new WindowsPrincipal(WindowsIdentity.GetCurrent()).IsInRole(WindowsBuiltInRole.Administrator)) return true;
            string log = Path.Combine(Path.GetTempPath(), "ttlcd-aura.log");
            var psi = new ProcessStartInfo("cmd.exe", "/c \"\"" + Process.GetCurrentProcess().MainModule.FileName + "\" aura " + action + " > \"" + log + "\" 2>&1\"")
            { Verb = "runas", UseShellExecute = true, WindowStyle = ProcessWindowStyle.Hidden };
            Process.Start(psi).WaitForExit();
            if (File.Exists(log)) Console.Write(File.ReadAllText(log));
            return false;
        }

        static int Install()
        {
            string src = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "TTLcdAura.dll");
            if (!File.Exists(src)) throw new FileNotFoundException("TTLcdAura.dll missing next to ttlcd.exe", src);
            Directory.CreateDirectory(Dir);
            string dll = Path.Combine(Dir, "TTLcdAura.dll");
            RestartLighting(false);
            File.Copy(src, dll, true);
            foreach (var view in new[] { RegistryView.Registry32, RegistryView.Registry64 })
                using (var hklm = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, view))
                {
                    using (var k = hklm.CreateSubKey(@"SOFTWARE\Classes\CLSID\" + Clsid))
                    {
                        k.SetValue("", "TT LCD Aura HAL");
                        using (var ip = k.CreateSubKey("InprocServer32"))
                        {
                            ip.SetValue("", "mscoree.dll");
                            ip.SetValue("ThreadingModel", "Both");
                            ip.SetValue("Class", "TtLcd.Aura.TtLcdAuraHal");
                            ip.SetValue("Assembly", "TTLcdAura, Version=0.0.0.0, Culture=neutral, PublicKeyToken=null");
                            ip.SetValue("RuntimeVersion", "v4.0.30319");
                            ip.SetValue("CodeBase", new Uri(dll).AbsoluteUri);
                        }
                        using (var p = k.CreateSubKey("ProgID")) p.SetValue("", "TTLcd.AuraHal.1");
                    }
                    using (var c = hklm.CreateSubKey(@"SOFTWARE\Classes\" + Category))
                    {
                        c.SetValue("Description", "Thermaltake CL-W383 LCD (TT LCD Studio)");
                        c.SetValue("Name", "TT LCD Studio");
                        c.SetValue("Manufacturer", "TT LCD Studio");
                        c.SetValue("DeviceModel", "CL-W383 LCD");
                        c.SetValue("DeviceType", "External");
                        c.SetValue("Version", "1.0.0.0");
                        c.SetValue("SpecVersion", "1.00.00");
                        c.SetValue("Pluging", 0, RegistryValueKind.DWord);
                    }
                }
            RestartLighting(true);
            Console.WriteLine("Aura plugin installed; ASUS LightingService restarted.");
            Console.WriteLine("Open Armoury Crate > Aura Sync: the cooler appears as \"TT LCD CL-W383\". Select the Aura Sync mode in TT LCD Studio.");
            return 0;
        }

        static int Uninstall()
        {
            RestartLighting(false);
            foreach (var view in new[] { RegistryView.Registry32, RegistryView.Registry64 })
                using (var hklm = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, view))
                {
                    hklm.DeleteSubKeyTree(@"SOFTWARE\Classes\" + Category, false);
                    hklm.DeleteSubKeyTree(@"SOFTWARE\Classes\CLSID\" + Clsid, false);
                }
            try { if (Directory.Exists(Dir)) Directory.Delete(Dir, true); } catch (IOException e) { Console.WriteLine("note: " + e.Message); }
            RestartLighting(true);
            Console.WriteLine("Aura plugin removed; ASUS LightingService restarted. Registered: " + Installed);
            return 0;
        }

        // stop (start=false) or start LightingService; a stop releases the plugin DLL
        static void RestartLighting(bool start)
        {
            try
            {
                using (var sc = new System.ServiceProcess.ServiceController("LightingService"))
                {
                    if (start) { if (sc.Status != System.ServiceProcess.ServiceControllerStatus.Running) sc.Start(); sc.WaitForStatus(System.ServiceProcess.ServiceControllerStatus.Running, TimeSpan.FromSeconds(30)); }
                    else if (sc.Status != System.ServiceProcess.ServiceControllerStatus.Stopped) { sc.Stop(); sc.WaitForStatus(System.ServiceProcess.ServiceControllerStatus.Stopped, TimeSpan.FromSeconds(30)); }
                }
            }
            catch (InvalidOperationException e) { Console.WriteLine("LightingService: " + e.Message); }
        }
    }
}
