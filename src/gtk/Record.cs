// Dev aid: TTLcdStudio.exe --record <out.gif> <scene> [scale] plays a scripted scene in the real window and saves it
// as an animated GIF for the README (docs/media), with the cursor and click rings drawn in: the Linux counterpart of
// src/Record.cs (WPF), writing through the same GifWriter (src/Gif.cs). Frames are read from the window as X shows it.
// Scenes: tour (navigation tour with mode changes). It changes the mode like a user would: record with a copy of the
// config (XDG_CONFIG_HOME pointing at a scratch folder).
using System;
using System.Collections.Generic;
using Cairo;
using Gtk;

namespace TtLcd.GtkUi
{
    partial class StudioWindow
    {
        class RecStep
        {
            public int At, Dur;
            public Func<PointD> To;     // cursor glide target
            public System.Action Do;    // one-off action (a click if Click is set)
            public bool Click, Started, Done;
            public PointD From, Target;
        }

        public void Record(string file, string scene, double scale, System.Action finished)
        {
            var steps = new List<RecStep>();
            Func<Widget, Func<PointD>> centre = w => () =>
            {
                int x, y;
                w.TranslateCoordinates(this, w.Allocation.Width / 2, w.Allocation.Height / 2, out x, out y);
                return new PointD(x, y);
            };
            System.Action<int, Widget, System.Action> click = (at, w, act) =>
            {
                steps.Add(new RecStep { At = at, Dur = 650, To = centre(w) });
                steps.Add(new RecStep { At = at + 800, Click = true, Do = act });
            };
            System.Action<Mode> mode = m => { Change(c => c.Mode = m); ShowOptions(); };
            var cursor = new PointD(1000, 690);
            int length;
            switch (scene)
            {
                case "tour":
                    ShowPage("dashboard");
                    click(1800, navs["modes"], () => ShowPage("modes"));
                    click(4000, tiles[Mode.Clock], () => mode(Mode.Clock));
                    click(6600, tiles[Mode.Color], () => mode(Mode.Color));
                    click(9200, tiles[Mode.Aura], () => mode(Mode.Aura));
                    click(11800, navs["device"], () => ShowPage("device"));
                    click(14200, navs["settings"], () => ShowPage("settings"));
                    click(16600, navs["dashboard"], () => ShowPage("dashboard"));
                    steps.Add(new RecStep { At = 18200, Dur = 900, To = () => new PointD(Allocation.Width - 40, Allocation.Height - 40) });
                    length = 20500;
                    break;
                default: throw new ArgumentException("unknown scene: " + scene);
            }

            const int fps = 12;
            int width = (int)Math.Round(Allocation.Width * scale), height = (int)Math.Round(Allocation.Height * scale);
            var gif = new GifWriter(width, height);
            var clicks = new List<KeyValuePair<long, PointD>>();
            System.Diagnostics.Stopwatch clock = null;
            long last = 0;
            // let the page, the engine, the first preview frames and sensor readings (coolant: every 2 s) settle first
            GLib.Timeout.Add(3000, () =>
            {
                clock = System.Diagnostics.Stopwatch.StartNew();
                GLib.Timeout.Add(1000 / fps, () =>
                {
                    long t = clock.ElapsedMilliseconds;
                    foreach (var st in steps)
                    {
                        if (st.Done || t < st.At) continue;
                        if (st.To != null)
                        {
                            if (!st.Started) { st.Started = true; st.From = cursor; st.Target = st.To(); }
                            double k = Math.Min(1, (t - st.At) / (double)Math.Max(1, st.Dur));
                            double q = k < 0.5 ? 4 * k * k * k : 1 - Math.Pow(-2 * k + 2, 3) / 2;
                            cursor = new PointD(st.From.X + (st.Target.X - st.From.X) * q, st.From.Y + (st.Target.Y - st.From.Y) * q);
                            if (k >= 1) st.Done = true;
                        }
                        else
                        {
                            st.Done = true;
                            if (st.Click) clicks.Add(new KeyValuePair<long, PointD>(t, cursor));
                            st.Do();
                        }
                    }
                    UpdatePreview();
                    Window.ProcessUpdates(true);
                    var px = CaptureFrame(width, height, scale, t, cursor, clicks);
                    long now = clock.ElapsedMilliseconds;
                    gif.Add(px, (int)Math.Max(2, (now - last) / 10));
                    last = now;
                    if (t < length) return true;
                    gif.HoldLast(80);
                    try { gif.Save(file); }
                    catch (Exception e) { Console.Error.WriteLine("record: saving " + file + " failed: " + e); }
                    finished();
                    return false;
                });
                return false;
            });
        }

        // The window's pixels (scaled) with the click rings and cursor drawn in, as Bgra32 for GifWriter.
        byte[] CaptureFrame(int width, int height, double scale, long t, PointD cursor, List<KeyValuePair<long, PointD>> clicks)
        {
            var a = Allocation;
            IntPtr p = gdk_pixbuf_get_from_window(Window.Handle, 0, 0, a.Width, a.Height);
            if (p == IntPtr.Zero) throw new InvalidOperationException("could not read the window's pixels");
            using (var pix = new Gdk.Pixbuf(p))
            using (var surf = new ImageSurface(Format.Rgb24, width, height))
            {
                using (var cr = new Context(surf))
                {
                    cr.Scale(scale, scale);
                    Gdk.CairoHelper.SetSourcePixbuf(cr, pix, 0, 0);
                    cr.Paint();
                    foreach (var c in clicks)
                    {
                        double age = (t - c.Key) / 400.0;
                        if (age > 1) continue;
                        cr.SetSourceRGBA(1, 1, 1, 0.86 * (1 - age)); cr.LineWidth = 2;
                        cr.NewPath(); cr.Arc(c.Value.X, c.Value.Y, 6 + 16 * age, 0, 2 * Math.PI); cr.Stroke();
                    }
                    // the arrow cursor (same shape as the WPF recorder)
                    cr.Translate(cursor.X, cursor.Y);
                    double[] pts = { 0, 0, 0, 18, 4.6, 13.8, 7.6, 20.6, 10.6, 19.4, 7.6, 12.6, 13.4, 12.6 };
                    cr.MoveTo(pts[0], pts[1]);
                    for (int i = 2; i < pts.Length; i += 2) cr.LineTo(pts[i], pts[i + 1]);
                    cr.ClosePath();
                    cr.SetSourceRGB(1, 1, 1); cr.FillPreserve();
                    cr.SetSourceRGB(0, 0, 0); cr.LineWidth = 1.2; cr.Stroke();
                }
                surf.Flush();
                // Rgb24 rows are B,G,R,x little-endian with a stride; GifWriter wants tight w*h*4
                var px = new byte[width * height * 4];
                int stride = surf.Stride;
                var data = surf.Data;
                for (int y = 0; y < height; y++) Buffer.BlockCopy(data, y * stride, px, y * width * 4, width * 4);
                return px;
            }
        }
    }
}
