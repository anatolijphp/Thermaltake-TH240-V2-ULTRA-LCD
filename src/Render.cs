// Image preparation for the LCD: fit/crop to panel size, JPEG encode, GIF frame extraction.
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;

namespace TtLcd
{
    public class GifFrame { public byte[] Jpeg; public int DelayMs; }

    public static class Render
    {
        static readonly ImageCodecInfo JpegCodec = ImageCodecInfo.GetImageEncoders().First(c => c.FormatID == ImageFormat.Jpeg.Guid);

        // Scale to cover the target size, center-crop, and optionally rotate (0/90/180/270).
        public static Bitmap Fit(Image src, int w, int h, int rotate)
        {
            var bmp = new Bitmap(w, h, PixelFormat.Format24bppRgb);
            using (var g = Graphics.FromImage(bmp))
            {
                g.Clear(Color.Black);
                g.InterpolationMode = InterpolationMode.HighQualityBicubic;
                g.PixelOffsetMode = PixelOffsetMode.HighQuality;
                float scale = Math.Max((float)w / src.Width, (float)h / src.Height);
                float sw = src.Width * scale, sh = src.Height * scale;
                g.DrawImage(src, (w - sw) / 2f, (h - sh) / 2f, sw, sh);
            }
            switch (rotate)
            {
                case 90: bmp.RotateFlip(RotateFlipType.Rotate90FlipNone); break;
                case 180: bmp.RotateFlip(RotateFlipType.Rotate180FlipNone); break;
                case 270: bmp.RotateFlip(RotateFlipType.Rotate270FlipNone); break;
            }
            return bmp;
        }

        // Upright 480x480 frame: fill (cover+crop) or contain (letterbox on bg colour).
        public static Bitmap Compose(Image src, int w, int h, bool contain, Color bg)
        {
            var bmp = new Bitmap(w, h, PixelFormat.Format24bppRgb);
            using (var g = Graphics.FromImage(bmp))
            {
                g.Clear(bg);
                if (src != null)
                {
                    g.InterpolationMode = InterpolationMode.HighQualityBilinear;
                    g.PixelOffsetMode = PixelOffsetMode.HighQuality;
                    float scale = contain ? Math.Min((float)w / src.Width, (float)h / src.Height)
                                          : Math.Max((float)w / src.Width, (float)h / src.Height);
                    float sw = src.Width * scale, sh = src.Height * scale;
                    g.DrawImage(src, (w - sw) / 2f, (h - sh) / 2f, sw, sh);
                }
            }
            return bmp;
        }

        public static void Rotate(Bitmap bmp, int degrees)
        {
            switch (((degrees % 360) + 360) % 360)
            {
                case 90: bmp.RotateFlip(RotateFlipType.Rotate90FlipNone); break;
                case 180: bmp.RotateFlip(RotateFlipType.Rotate180FlipNone); break;
                case 270: bmp.RotateFlip(RotateFlipType.Rotate270FlipNone); break;
            }
        }

        static void Text(Graphics g, string s, Font f, Color c, RectangleF r)
        {
            using (var sf = new StringFormat { Alignment = StringAlignment.Center, LineAlignment = StringAlignment.Center })
            using (var shadow = new SolidBrush(Color.FromArgb(160, 0, 0, 0)))
            using (var fg = new SolidBrush(c))
            {
                var r2 = r; r2.Offset(2, 2);
                g.DrawString(s, f, shadow, r2, sf);
                g.DrawString(s, f, fg, r, sf);
            }
        }

        // ---- LCD text (clock face and overlay) ----
        // The panel is round: every line is placed inside the circle (radius FitR around the centre), and the
        // whole block shrinks until each line fits the chord at its height.
        const float Cx = 240, Cy = 240, FitR = 226;

        public static bool WaterValid(double c) { return c >= -5 && c <= 99; }
        public static string Deg(double c, bool fahrenheit) { return fahrenheit ? Math.Round(c * 9 / 5 + 32) + "°F" : Math.Round(c) + "°C"; }

        // cool -> hot over lo..hi: cyan, green, yellow, red
        public static Color Heat(double v, double lo, double hi)
        {
            double k = Math.Max(0, Math.Min(1, (v - lo) / (hi - lo)));
            double h = 3 * (1 - k); int i = (int)h; double f = h - i;
            double r, g, b;
            switch (i) { case 0: r = 1; g = f; b = 0; break; case 1: r = 1 - f; g = 1; b = 0; break; case 2: r = 0; g = 1; b = f; break; default: r = 0; g = 1; b = 1; break; }
            return Color.FromArgb((int)(r * 255), (int)(g * 255), (int)(b * 255));
        }

        static Color Rgb(int v) { return Color.FromArgb(v >> 16 & 255, v >> 8 & 255, v & 255); }

        static readonly Dictionary<string, FontFamily> families = new Dictionary<string, FontFamily>();
        static FontFamily Family(string name)
        {
            if (string.IsNullOrEmpty(name)) name = "Segoe UI";
            lock (families)
            {
                FontFamily f;
                if (!families.TryGetValue(name, out f))
                {
                    try { f = new FontFamily(name); } catch (ArgumentException) { f = new FontFamily("Segoe UI"); }
                    families[name] = f;
                }
                return f;
            }
        }

        static FontStyle StyleFor(FontFamily fam, bool bold, bool italic)
        {
            var s = (bold ? FontStyle.Bold : FontStyle.Regular) | (italic ? FontStyle.Italic : FontStyle.Regular);
            if (fam.IsStyleAvailable(s)) return s;
            foreach (var alt in new[] { FontStyle.Regular, FontStyle.Bold, FontStyle.Italic, FontStyle.Bold | FontStyle.Italic })
                if (fam.IsStyleAvailable(alt)) return alt;
            return s;
        }

        // One reading (time, "Water: 31°C", ...) with its own text style.
        class Seg
        {
            public string Text; public Color Col; public float Size; public int Id; public ElementStyle St;
            public string Above;  // label drawn small above the value ("Coolant" over "31°C"), or null
            public Seg(int id, string t, ElementStyle st, Color c) { Id = id; Text = t; St = st; Size = Math.Max(8, st.Size); Col = c; }
            public float LabelSize { get { return Size * 0.42f; } }
            public float Height { get { return Size * 1.2f + (Above != null ? LabelSize * 1.15f : 0); } }   // unscaled
        }
        class Row
        {
            public readonly List<Seg> Segs = new List<Seg>();
            public float Blank;   // height (as a font size) of an intentionally empty line
            public float Size { get { float m = Blank; foreach (var s in Segs) m = Math.Max(m, s.Size); return m; } }
            public float Height { get { float m = Blank * 1.2f; foreach (var s in Segs) m = Math.Max(m, s.Height); return m; } }
        }
        // A laid-out reading: its outline, colour and effect.
        class Placed { public GraphicsPath Path; public Color Col; public TextEffect Fx; public float Size; }

        // The readings' lines as arranged by drag and drop: "0,2|3|" = line 1: time + water, line 2: CPU.
        // Saved strings always contain '|'. Older configs (plain order + layout setting) are converted: Stacked = one
        // reading per line, Custom = its "new line after" choices, One line / Auto = all on one line.
        public static List<List<int>> Lines(string order, TextLayout legacy, int breaks, int[] allIds)
        {
            var lines = new List<List<int>>();
            var seen = new HashSet<int>();
            if (order != null && order.Contains("|"))
            {
                var parts = order.Split('|');
                // every line ends with '|', so the last part is always empty; empty lines in between are kept
                for (int k = 0; k < parts.Length - 1; k++)
                {
                    var line = new List<int>();
                    foreach (var p in parts[k].Split(','))
                    {
                        int id;
                        if (int.TryParse(p.Trim(), out id) && Array.IndexOf(allIds, id) >= 0 && seen.Add(id)) line.Add(id);
                    }
                    lines.Add(line);
                }
                TrimEmptyEnds(lines);
                foreach (int id in allIds) if (seen.Add(id)) lines.Add(new List<int> { id }); // never lose a reading
                return lines;
            }
            var ids = ParseOrder(order, allIds);
            var cur = new List<int>();
            for (int i = 0; i < ids.Count; i++)
            {
                cur.Add(ids[i]);
                bool brk = legacy == TextLayout.Stack || (legacy == TextLayout.Custom && (breaks >> ids[i] & 1) != 0);
                if (brk || i == ids.Count - 1) { lines.Add(cur); cur = new List<int>(); }
            }
            return lines;
        }

        // "0,2|" + "|" (empty line) + "3|"  ->  "0,2||3|"
        public static string LinesToString(List<List<int>> lines)
        {
            var sb = new System.Text.StringBuilder();
            foreach (var l in lines) sb.Append(string.Join(",", l)).Append('|');
            return sb.ToString();
        }

        // Empty lines only make sense between readings; at the top or bottom they would just shift the block.
        public static void TrimEmptyEnds(List<List<int>> lines)
        {
            while (lines.Count > 0 && lines[0].Count == 0) lines.RemoveAt(0);
            while (lines.Count > 0 && lines[lines.Count - 1].Count == 0) lines.RemoveAt(lines.Count - 1);
        }

        // Rows to draw: the enabled readings of each line. A line left empty on purpose becomes a blank row
        // (as tall as an average reading); a line whose readings are all switched off is skipped.
        static List<Row> RowsFrom(List<Seg> items, List<List<int>> lines)
        {
            var rows = new List<Row>();
            float blank = items.Count > 0 ? items.Average(i => i.Size) : 40;
            foreach (var line in lines)
            {
                var r = new Row();
                foreach (int id in line) { var s = items.Find(x => x.Id == id); if (s != null) r.Segs.Add(s); }
                if (line.Count == 0) r.Blank = blank;
                if (r.Segs.Count > 0 || line.Count == 0) rows.Add(r);
            }
            while (rows.Count > 0 && rows[0].Segs.Count == 0) rows.RemoveAt(0);               // nothing above it
            while (rows.Count > 0 && rows[rows.Count - 1].Segs.Count == 0) rows.RemoveAt(rows.Count - 1);
            return rows;
        }

        // Draws the rows; if a line doesn't fit inside the circle, everything is shrunk until it does.
        // vAnchor: -1 = block top at anchorY, 0 = centred on it, 1 = block bottom at it.
        static void DrawReadings(Graphics g, Config cfg, List<Row> rows, float gap, float anchorY, int vAnchor, bool bar)
        {
            if (rows.Count == 0) return;
            g.SmoothingMode = SmoothingMode.AntiAlias;
            for (float s = 1; ; s *= 0.94f)
            {
                List<Placed> placed; RectangleF ink;
                bool fits = Place(cfg, rows, s, gap, anchorY, vAnchor, out placed, out ink);
                bool done = fits || s < 0.25f;
                if (done)
                {
                    if (bar && !ink.IsEmpty)
                        using (var bb = new SolidBrush(Color.FromArgb(150, 0, 0, 0)))
                            g.FillRectangle(bb, 0, ink.Top - 12, Lcd.Width, ink.Height + 24);
                    foreach (var pl in placed) DrawPath(g, pl.Fx, pl.Path, pl.Col, pl.Size);
                }
                foreach (var pl in placed) pl.Path.Dispose();
                if (done) return;
            }
        }

        static float EffectPad(TextEffect fx) { return fx == TextEffect.Glow ? 0.16f : fx == TextEffect.Outline ? 0.06f : 0.04f; }

        // Lays rows out at scale s; returns whether every line lies inside the circle.
        static bool Place(Config cfg, List<Row> rows, float s, float gap, float anchorY, int vAnchor,
                          out List<Placed> placed, out RectangleF ink)
        {
            placed = new List<Placed>();
            ink = RectangleF.Empty;
            bool fits = true;
            float total = gap * s * (rows.Count - 1);
            foreach (var r in rows) total += r.Height * s;
            float y = vAnchor < 0 ? anchorY : vAnchor > 0 ? anchorY - total : anchorY - total / 2;
            var lines = new List<KeyValuePair<List<Placed>, RectangleF>>();
            var halves = new List<float>();
            foreach (var r in rows)
            {
                float rowH = r.Height * s, pad = 0;
                if (r.Segs.Count == 0) { y += rowH + gap * s; continue; } // blank line: space only
                // readings side by side, each in its own font, vertically centred in the row;
                // a label "above" sits small over its value, both centred on the wider of the two
                var segs = new List<Placed>();
                float x = 0;
                foreach (var sg in r.Segs)
                {
                    float size = sg.Size * s, top = y + (rowH - sg.Height * s) / 2;
                    var fam = Family(sg.St.Font);
                    int fs = (int)StyleFor(fam, sg.St.Bold, sg.St.Italic);
                    GraphicsPath lp = null;
                    if (sg.Above != null)
                    {
                        lp = new GraphicsPath();
                        lp.AddString(sg.Above, fam, fs, sg.LabelSize * s, new PointF(x, top), StringFormat.GenericTypographic);
                        top += sg.LabelSize * s * 1.15f;
                    }
                    var p = new GraphicsPath();
                    p.AddString(sg.Text, fam, fs, size, new PointF(x, top), StringFormat.GenericTypographic);
                    var pb = p.GetBounds();
                    float right = pb.Right;
                    if (lp != null)
                    {
                        var lb = lp.GetBounds();
                        float w = Math.Max(pb.Width, lb.Width);
                        using (var m = new Matrix()) { m.Translate(pb.Left + (w - lb.Width) / 2 - lb.Left, 0); lp.Transform(m); }
                        using (var m = new Matrix()) { m.Translate((w - pb.Width) / 2, 0); p.Transform(m); }
                        right = pb.Left + w;
                        segs.Add(new Placed { Path = lp, Col = sg.Col, Fx = sg.St.Effect, Size = sg.LabelSize * s });
                    }
                    if (pb.Width > 0) x = right + Math.Max(size, r.Size * s * 0.5f) * 0.6f;
                    pad = Math.Max(pad, size * EffectPad(sg.St.Effect));
                    segs.Add(new Placed { Path = p, Col = sg.Col, Fx = sg.St.Effect, Size = size });
                }
                RectangleF b = RectangleF.Empty;
                foreach (var sp in segs) { var pb = sp.Path.GetBounds(); if (pb.Width > 0) b = b.IsEmpty ? pb : RectangleF.Union(b, pb); }
                float dy = Math.Max(Math.Abs(b.Top - pad - Cy), Math.Abs(b.Bottom + pad - Cy));
                float half = dy < FitR ? (float)Math.Sqrt(FitR * FitR - dy * dy) - pad : 0;
                lines.Add(new KeyValuePair<List<Placed>, RectangleF>(segs, b));
                halves.Add(half);
                placed.AddRange(segs);
                y += rowH + gap * s;
            }
            // left/right alignment share one edge: the innermost chord end over all lines
            float edgeL = float.MinValue, edgeR = float.MaxValue;
            foreach (var h in halves) { edgeL = Math.Max(edgeL, Cx - h); edgeR = Math.Min(edgeR, Cx + h); }
            for (int i = 0; i < lines.Count; i++)
            {
                var b = lines[i].Value; float half = halves[i];
                float left = cfg.TextAlign == 0 ? edgeL : cfg.TextAlign == 2 ? edgeR - b.Width : Cx - b.Width / 2;
                if (b.Width > 2 * half || left < Cx - half - 0.5f || left + b.Width > Cx + half + 0.5f) fits = false;
                using (var m = new Matrix()) { m.Translate(left - b.Left, 0); foreach (var sp in lines[i].Key) sp.Path.Transform(m); }
                b.Offset(left - b.Left, 0);
                ink = ink.IsEmpty ? b : RectangleF.Union(ink, b);
            }
            return fits;
        }

        // size = font size in px (effect strengths scale with it)
        static void DrawPath(Graphics g, TextEffect fx, GraphicsPath path, Color c, float size)
        {
            switch (fx)
            {
                case TextEffect.Shadow:
                    using (var sh = (GraphicsPath)path.Clone())
                    using (var m = new Matrix())
                    using (var b = new SolidBrush(Color.FromArgb(170, 0, 0, 0)))
                    {
                        float d = Math.Max(2, size * 0.03f);
                        m.Translate(d, d); sh.Transform(m); g.FillPath(b, sh);
                    }
                    break;
                case TextEffect.Outline:
                    using (var p = new Pen(Color.Black, Math.Max(3, size * 0.08f)) { LineJoin = LineJoin.Round })
                        g.DrawPath(p, path);
                    break;
                case TextEffect.Glow:
                    for (int i = 6; i >= 1; i--)
                        using (var p = new Pen(Color.FromArgb(16 + (6 - i) * 9, c), Math.Max(2, size * 0.045f * i)) { LineJoin = LineJoin.Round })
                            g.DrawPath(p, path);
                    break;
            }
            using (var f = new SolidBrush(c)) g.FillPath(f, path);
        }

        // Ids of the readings, shared by the clock face and the overlay (used in ClockOrder / OverlayOrder).
        public const int IdTime = 0, IdDate = 1, IdWater = 2, IdCpuTemp = 3, IdLoad = 4;

        // "2,0,3" -> [2,0,3] followed by any of allIds missing from it (so a saved order never loses a reading)
        public static List<int> ParseOrder(string order, int[] allIds)
        {
            var r = new List<int>();
            foreach (var p in (order ?? "").Split(','))
            {
                int id;
                if (int.TryParse(p.Trim(), out id) && Array.IndexOf(allIds, id) >= 0 && !r.Contains(id)) r.Add(id);
            }
            foreach (int id in allIds) if (!r.Contains(id)) r.Add(id);
            return r;
        }

        public static readonly int[] ClockIds = { IdTime, IdDate, IdWater, IdCpuTemp, IdLoad };
        public static readonly int[] OverlayIds = { IdTime, IdWater, IdCpuTemp };

        // The reading's text and colour in its style ("Water: 31°C" or "31°C", °C/°F, colour by value).
        static Seg Reading(Config cfg, bool overlay, int id, double water, int cpuTemp, int cpuLoad)
        {
            var st = cfg.Style(overlay, id);
            var now = DateTime.Now;
            var c = Rgb(st.Rgb);
            switch (id)
            {
                case IdTime: return new Seg(id, now.ToString("HH:mm"), st, c);
                case IdDate: return new Seg(id, now.ToString("ddd d MMM"), st, c);
                case IdWater: return Labelled(id, "Coolant", Deg(water, st.Fahrenheit), st, st.ByTemp ? Heat(water, 25, 45) : c);
                case IdCpuTemp: return Labelled(id, "CPU", Deg(cpuTemp, st.Fahrenheit), st, st.ByTemp ? Heat(cpuTemp, 35, 90) : c);
                default: return Labelled(id, "Load", cpuLoad + "%", st, st.ByTemp ? Heat(cpuLoad, 0, 100) : c);
            }
        }

        // Label beside ("Water: 31°C"), above (small "Water" over "31°C") or none.
        static Seg Labelled(int id, string label, string value, ElementStyle st, Color c)
        {
            if (!st.Label) return new Seg(id, value, st, c);
            if (st.LabelAbove) return new Seg(id, value, st, c) { Above = label };
            return new Seg(id, label + ": " + value, st, c);
        }

        // Overlay readings at the top, centre or bottom of the round panel.
        public static void DrawOverlay(Bitmap bmp, Config cfg, int cpuTempC, double waterC)
        {
            var items = new List<Seg>();
            if (cfg.OverlayTime) items.Add(Reading(cfg, true, IdTime, waterC, cpuTempC, 0));
            if (cfg.OverlayWaterTemp && WaterValid(waterC)) items.Add(Reading(cfg, true, IdWater, waterC, cpuTempC, 0));
            if (cfg.OverlayCpuTemp && cpuTempC > 0) items.Add(Reading(cfg, true, IdCpuTemp, waterC, cpuTempC, 0));
            float gap = items.Count > 0 ? items.Max(i => i.Size) * 0.1f : 0;
            using (var g = Graphics.FromImage(bmp))
                DrawReadings(g, cfg, RowsFrom(items, Lines(cfg.OverlayOrder, cfg.OverlayLayout, cfg.OverlayBreaks, OverlayIds)), gap,
                             cfg.OverlayPos == 1 ? 62 : cfg.OverlayPos == 2 ? Cy : Lcd.Height - 62,
                             cfg.OverlayPos == 1 ? -1 : cfg.OverlayPos == 2 ? 0 : 1, false);
        }

        // Configurable clock / readings face, around the centre (plus the vertical offset).
        // waterC is out of range when the cooler sensor is open; cpuTemp < 0 when unknown.
        public static void DrawClock(Bitmap bmp, Config cfg, double waterC, int cpuTemp, int cpuLoad)
        {
            var items = new List<Seg>();
            if (cfg.ShowTime) items.Add(Reading(cfg, false, IdTime, waterC, cpuTemp, cpuLoad));
            if (cfg.ShowDate) items.Add(Reading(cfg, false, IdDate, waterC, cpuTemp, cpuLoad));
            if (cfg.ShowWaterTemp && WaterValid(waterC)) items.Add(Reading(cfg, false, IdWater, waterC, cpuTemp, cpuLoad));
            if (cfg.ShowCpuTemp && cpuTemp > 0) items.Add(Reading(cfg, false, IdCpuTemp, waterC, cpuTemp, cpuLoad));
            if (cfg.ShowCpuLoad) items.Add(Reading(cfg, false, IdLoad, waterC, cpuTemp, cpuLoad));
            using (var g = Graphics.FromImage(bmp))
                DrawReadings(g, cfg, RowsFrom(items, Lines(cfg.ClockOrder, cfg.ClockLayout, cfg.ClockBreaks, ClockIds)), cfg.LineSpacing, Cy + cfg.TextOffsetY, 0, false);
        }

        // Dashboard of arc gauges for a round panel. Each enabled sensor gets one gauge; up to four are laid out
        // in a 2x2 grid, a single gauge is centred. Water temp is from the cooler; the rest from Windows.
        public static void DrawDashboard(Bitmap bmp, Config cfg, double waterC, int cpuTemp, int cpuLoad, int ramPct)
        {
            var items = new List<Gauge>();
            if (cfg.DashWater && waterC >= -5 && waterC <= 99) items.Add(new Gauge("COOLANT", Math.Round(waterC) + "°", (waterC - 20) / 40.0, Color.FromArgb(80, 180, 255)));
            if (cfg.DashCpuLoad) items.Add(new Gauge("CPU", cpuLoad + "%", cpuLoad / 100.0, Color.FromArgb(255, 90, 90)));
            if (cfg.DashCpuTemp && cpuTemp > 0) items.Add(new Gauge("CPU °C", cpuTemp.ToString(), (cpuTemp - 20) / 70.0, Color.FromArgb(255, 170, 60)));
            if (cfg.DashRam) items.Add(new Gauge("RAM", ramPct + "%", ramPct / 100.0, Color.FromArgb(120, 220, 120)));

            using (var g = Graphics.FromImage(bmp))
            {
                g.SmoothingMode = SmoothingMode.AntiAlias;
                g.TextRenderingHint = System.Drawing.Text.TextRenderingHint.AntiAliasGridFit;
                int n = items.Count;
                if (n == 0) { if (cfg.DashClock) DrawClock(bmp, cfg, waterC, cpuTemp, cpuLoad); return; }
                if (n == 1) DrawGauge(g, 240, 240, 150, items[0]);
                else
                {
                    float[,] pos = { { 142, 150 }, { 338, 150 }, { 142, 346 }, { 338, 346 } };
                    for (int i = 0; i < n && i < 4; i++) DrawGauge(g, pos[i, 0], pos[i, 1], 86, items[i]);
                }
                if (cfg.DashClock)
                    using (var f = new Font(string.IsNullOrEmpty(cfg.FontName) ? "Segoe UI" : cfg.FontName, n == 1 ? 34 : 30, FontStyle.Bold, GraphicsUnit.Pixel))
                        Text(g, DateTime.Now.ToString("HH:mm"), f, Color.White, new RectangleF(0, n == 1 ? 408 : 232, bmp.Width, 44));
            }
        }

        class Gauge
        {
            public string Label, Value; public double Frac; public Color Col;
            public Gauge(string l, string v, double f, Color c) { Label = l; Value = v; Frac = Math.Max(0, Math.Min(1, f)); Col = c; }
        }

        static void DrawGauge(Graphics g, float cx, float cy, float r, Gauge ga)
        {
            const float start = 135, sweep = 270;
            var rect = new RectangleF(cx - r, cy - r, r * 2, r * 2);
            using (var bg = new Pen(Color.FromArgb(60, 70, 85), r * 0.14f) { StartCap = LineCap.Round, EndCap = LineCap.Round })
            using (var fg = new Pen(ga.Col, r * 0.14f) { StartCap = LineCap.Round, EndCap = LineCap.Round })
            {
                g.DrawArc(bg, rect, start, sweep);
                if (ga.Frac > 0) g.DrawArc(fg, rect, start, (float)(sweep * ga.Frac));
            }
            using (var vf = new Font("Segoe UI", r * 0.5f, FontStyle.Bold, GraphicsUnit.Pixel))
                Text(g, ga.Value, vf, Color.White, new RectangleF(cx - r, cy - r * 0.55f, r * 2, r * 0.8f));
            using (var lf = new Font("Segoe UI", r * 0.22f, GraphicsUnit.Pixel))
                Text(g, ga.Label, lf, Color.FromArgb(170, 180, 195), new RectangleF(cx - r, cy + r * 0.28f, r * 2, r * 0.35f));
        }

        public static byte[] Jpeg(Image img, long quality)
        {
            using (var ms = new MemoryStream())
            using (var ep = new EncoderParameters(1))
            {
                ep.Param[0] = new EncoderParameter(System.Drawing.Imaging.Encoder.Quality, quality);
                img.Save(ms, JpegCodec, ep);
                return ms.ToArray();
            }
        }

        public static byte[] FileToJpeg(string path, int w, int h, int rotate, long quality)
        {
            using (var src = Image.FromFile(path))
            using (var fit = Fit(src, w, h, rotate))
                return Jpeg(fit, quality);
        }

        public static List<GifFrame> GifFrames(string path, int w, int h, int rotate, long quality)
        {
            var frames = new List<GifFrame>();
            using (var gif = Image.FromFile(path))
            {
                var dim = new FrameDimension(gif.FrameDimensionsList[0]);
                int n = gif.GetFrameCount(dim);
                byte[] delays = null;
                try { delays = gif.GetPropertyItem(0x5100).Value; } catch (ArgumentException) { }
                for (int i = 0; i < n; i++)
                {
                    gif.SelectActiveFrame(dim, i);
                    int delay = delays != null && delays.Length >= (i + 1) * 4 ? BitConverter.ToInt32(delays, i * 4) * 10 : 100;
                    if (delay < 20) delay = 100; // browsers treat 0/1 as 100ms
                    using (var fit = Fit(gif, w, h, rotate))
                        frames.Add(new GifFrame { Jpeg = Jpeg(fit, quality), DelayMs = delay });
                }
            }
            return frames;
        }
    }
}
