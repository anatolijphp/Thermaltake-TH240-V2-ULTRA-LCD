// Generates src\app.ico: a round "LCD" with a rainbow ring and a red TT, at 16..256 px.
// Small sizes are stored as 32-bit DIBs (works everywhere, including System.Drawing.Icon), 256 px as PNG.
// Build and run: csc /r:System.Drawing.dll /out:MakeIcon.exe tools-src\MakeIcon.cs && MakeIcon.exe src\app.ico
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Drawing.Text;
using System.IO;

static class MakeIcon
{
    static int Main(string[] args)
    {
        string outPath = args.Length > 0 ? args[0] : "app.ico";
        int[] sizes = { 16, 20, 24, 32, 40, 48, 64, 128, 256 };
        var images = new List<byte[]>();
        foreach (int s in sizes)
            using (var bmp = Draw(s))
                images.Add(s >= 256 ? Png(bmp) : Dib(bmp));
        using (var f = File.Create(outPath))
        using (var w = new BinaryWriter(f))
        {
            w.Write((short)0); w.Write((short)1); w.Write((short)sizes.Length);
            int offset = 6 + 16 * sizes.Length;
            for (int i = 0; i < sizes.Length; i++)
            {
                w.Write((byte)(sizes[i] >= 256 ? 0 : sizes[i])); w.Write((byte)(sizes[i] >= 256 ? 0 : sizes[i]));
                w.Write((byte)0); w.Write((byte)0); w.Write((short)1); w.Write((short)32);
                w.Write(images[i].Length); w.Write(offset);
                offset += images[i].Length;
            }
            foreach (var img in images) w.Write(img);
        }
        Console.WriteLine("wrote " + outPath);
        return 0;
    }

    static Color Hue(double h)
    {
        h = ((h % 360) + 360) % 360 / 60; int i = (int)h; double f = h - i;
        double r, g, b;
        switch (i) { case 0: r = 1; g = f; b = 0; break; case 1: r = 1 - f; g = 1; b = 0; break; case 2: r = 0; g = 1; b = f; break;
                     case 3: r = 0; g = 1 - f; b = 1; break; case 4: r = f; g = 0; b = 1; break; default: r = 1; g = 0; b = 1 - f; break; }
        return Color.FromArgb((int)(r * 255), (int)(g * 255), (int)(b * 255));
    }

    static Bitmap Draw(int s)
    {
        // draw at 4x and downscale for clean edges
        int S = s * 4;
        using (var big = new Bitmap(S, S, PixelFormat.Format32bppArgb))
        {
            using (var g = Graphics.FromImage(big))
            {
                g.SmoothingMode = SmoothingMode.AntiAlias;
                g.Clear(Color.Transparent);
                float pad = S * 0.04f, ringW = S * (s <= 24 ? 0.13f : 0.09f);
                var outer = new RectangleF(pad, pad, S - 2 * pad, S - 2 * pad);
                // dark screen
                using (var b = new SolidBrush(Color.FromArgb(255, 14, 14, 15))) g.FillEllipse(b, outer);
                // rainbow ring, starting red at the top
                var ring = RectangleF.Inflate(outer, -ringW / 2, -ringW / 2);
                for (int a = 0; a < 360; a += 2)
                    using (var p = new Pen(Hue(350 + a), ringW)) g.DrawArc(p, ring, a - 90, 3);
                // red glow behind the letters
                var inner = RectangleF.Inflate(outer, -ringW * 1.6f, -ringW * 1.6f);
                using (var path = new GraphicsPath())
                {
                    path.AddEllipse(inner);
                    using (var pg = new PathGradientBrush(path) { CenterColor = Color.FromArgb(90, 255, 31, 75), SurroundColors = new[] { Color.FromArgb(0, 255, 31, 75) } })
                        g.FillEllipse(pg, inner);
                }
                // TT
                using (var fam = new FontFamily("Segoe UI Black"))
                using (var path = new GraphicsPath())
                {
                    var sf = (StringFormat)StringFormat.GenericTypographic.Clone();
                    path.AddString("TT", fam, (int)FontStyle.Regular, S * 0.42f, PointF.Empty, sf);
                    var bb = path.GetBounds();
                    using (var m = new Matrix())
                    {
                        m.Translate(S / 2f - (bb.X + bb.Width / 2), S / 2f - (bb.Y + bb.Height / 2));
                        path.Transform(m);
                    }
                    using (var b = new SolidBrush(Color.FromArgb(255, 255, 31, 75))) g.FillPath(b, path);
                }
            }
            var bmp = new Bitmap(s, s, PixelFormat.Format32bppArgb);
            using (var g = Graphics.FromImage(bmp))
            {
                g.InterpolationMode = InterpolationMode.HighQualityBicubic;
                g.PixelOffsetMode = PixelOffsetMode.HighQuality;
                g.DrawImage(big, 0, 0, s, s);
            }
            return bmp;
        }
    }

    static byte[] Png(Bitmap bmp) { using (var ms = new MemoryStream()) { bmp.Save(ms, ImageFormat.Png); return ms.ToArray(); } }

    // BITMAPINFOHEADER + bottom-up BGRA pixels + 1-bit AND mask (all zero: alpha is used)
    static byte[] Dib(Bitmap bmp)
    {
        int s = bmp.Width, maskStride = ((s + 31) / 32) * 4;
        using (var ms = new MemoryStream())
        using (var w = new BinaryWriter(ms))
        {
            w.Write(40); w.Write(s); w.Write(s * 2); w.Write((short)1); w.Write((short)32);
            w.Write(0); w.Write(s * s * 4 + maskStride * s); w.Write(0); w.Write(0); w.Write(0); w.Write(0);
            for (int y = s - 1; y >= 0; y--)
                for (int x = 0; x < s; x++)
                {
                    var c = bmp.GetPixel(x, y);
                    w.Write(c.B); w.Write(c.G); w.Write(c.R); w.Write(c.A);
                }
            w.Write(new byte[maskStride * s]);
            return ms.ToArray();
        }
    }
}
