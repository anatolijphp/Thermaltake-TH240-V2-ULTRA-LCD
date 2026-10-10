// GifRec: records a window (with the mouse cursor) to an animated GIF, for the README media in docs/media.
// Built with the C# compiler that ships with Windows, no packages:
//   %WINDIR%\Microsoft.NET\Framework64\v4.0.30319\csc.exe /nologo /out:GifRec.exe /r:System.Drawing.dll
//     /r:%WINDIR%\Microsoft.NET\Framework64\v4.0.30319\WPF\PresentationCore.dll
//     /r:%WINDIR%\Microsoft.NET\Framework64\v4.0.30319\WPF\WindowsBase.dll tools-src\GifRec.cs
// usage: GifRec <out.gif> <window title> <seconds> [fps=12] [scale=0.7]
// Frames are captured scaled, then each frame stores only the rectangle that changed since the previous one, with its
// own 256-colour palette (WPF's quantiser) and the standard GIF LZW coder.
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using D = System.Drawing;

static class GifRec
{
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] static extern IntPtr FindWindow(string cls, string title);
    [DllImport("dwmapi.dll")] static extern int DwmGetWindowAttribute(IntPtr h, int attr, out RECT r, int size);
    [DllImport("user32.dll")] static extern bool GetCursorInfo(ref CURSORINFO ci);
    [DllImport("user32.dll")] static extern bool DrawIconEx(IntPtr hdc, int x, int y, IntPtr icon, int w, int h, int step, IntPtr brush, int flags);
    [DllImport("user32.dll")] static extern bool GetIconInfo(IntPtr icon, out ICONINFO info);
    [DllImport("gdi32.dll")] static extern bool DeleteObject(IntPtr o);
    struct RECT { public int L, T, R, B; }
    struct POINT { public int X, Y; }
    struct CURSORINFO { public int Size, Flags; public IntPtr Cursor; public POINT Pos; }
    struct ICONINFO { public bool IsIcon; public int HotX, HotY; public IntPtr Mask, Color; }

    class Frame { public byte[] Px; public long Ms; }

    [STAThread]
    static int Main(string[] a)
    {
        if (a.Length < 3) { Console.WriteLine("usage: GifRec <out.gif> <window title> <seconds> [fps=12] [scale=0.7]"); return 1; }
        double secs = double.Parse(a[2], System.Globalization.CultureInfo.InvariantCulture);
        int fps = a.Length > 3 ? int.Parse(a[3]) : 12;
        double scale = a.Length > 4 ? double.Parse(a[4], System.Globalization.CultureInfo.InvariantCulture) : 0.7;
        IntPtr h = FindWindow(null, a[1]);
        if (h == IntPtr.Zero) { Console.WriteLine("window not found: " + a[1]); return 2; }
        RECT r;
        DwmGetWindowAttribute(h, 9 /* DWMWA_EXTENDED_FRAME_BOUNDS */, out r, 16);
        int sw = r.R - r.L, sh = r.B - r.T, w = (int)(sw * scale), hh = (int)(sh * scale);

        var frames = new List<Frame>();
        var clock = Stopwatch.StartNew();
        using (var raw = new D.Bitmap(sw, sh, D.Imaging.PixelFormat.Format32bppRgb))
        using (var small = new D.Bitmap(w, hh, D.Imaging.PixelFormat.Format32bppRgb))
        using (var gr = D.Graphics.FromImage(raw))
        using (var gs = D.Graphics.FromImage(small))
        {
            gs.InterpolationMode = D.Drawing2D.InterpolationMode.HighQualityBilinear;
            gs.PixelOffsetMode = D.Drawing2D.PixelOffsetMode.HighQuality;
            for (int i = 0; clock.ElapsedMilliseconds < secs * 1000; i++)
            {
                long t = clock.ElapsedMilliseconds;
                gr.CopyFromScreen(r.L, r.T, 0, 0, new D.Size(sw, sh));
                DrawCursor(gr, r.L, r.T);
                gs.DrawImage(raw, 0, 0, w, hh);
                var bd = small.LockBits(new D.Rectangle(0, 0, w, hh), D.Imaging.ImageLockMode.ReadOnly, D.Imaging.PixelFormat.Format32bppRgb);
                var px = new byte[w * hh * 4];
                Marshal.Copy(bd.Scan0, px, 0, px.Length);
                small.UnlockBits(bd);
                frames.Add(new Frame { Px = px, Ms = t });
                long wait = (i + 1) * 1000L / fps - clock.ElapsedMilliseconds;
                if (wait > 0) Thread.Sleep((int)wait);
            }
        }
        long end = clock.ElapsedMilliseconds;
        Write(a[0], frames, w, hh, end);
        Console.WriteLine("{0}: {1} frames, {2}x{3}, {4:0} KB", a[0], frames.Count, w, hh, new FileInfo(a[0]).Length / 1024.0);
        return 0;
    }

    static void DrawCursor(D.Graphics g, int ox, int oy)
    {
        var ci = new CURSORINFO { Size = Marshal.SizeOf(typeof(CURSORINFO)) };
        if (!GetCursorInfo(ref ci) || (ci.Flags & 1) == 0) return;
        ICONINFO ii;
        int hx = 0, hy = 0;
        if (GetIconInfo(ci.Cursor, out ii))
        {
            hx = ii.HotX; hy = ii.HotY;
            if (ii.Mask != IntPtr.Zero) DeleteObject(ii.Mask);
            if (ii.Color != IntPtr.Zero) DeleteObject(ii.Color);
        }
        IntPtr hdc = g.GetHdc();
        try { DrawIconEx(hdc, ci.Pos.X - ox - hx, ci.Pos.Y - oy - hy, ci.Cursor, 0, 0, 0, IntPtr.Zero, 3 /* DI_NORMAL */); }
        finally { g.ReleaseHdc(hdc); }
    }

    // ---------------- GIF writer ----------------

    static void Write(string path, List<Frame> frames, int w, int h, long endMs)
    {
        using (var o = new FileStream(path, FileMode.Create))
        {
            o.Write(new byte[] { (byte)'G', (byte)'I', (byte)'F', (byte)'8', (byte)'9', (byte)'a' }, 0, 6);
            U16(o, w); U16(o, h); o.WriteByte(0); o.WriteByte(0); o.WriteByte(0);   // no global colour table
            o.Write(new byte[] { 0x21, 0xFF, 11 }, 0, 3);                            // loop forever
            foreach (char c in "NETSCAPE2.0") o.WriteByte((byte)c);
            o.Write(new byte[] { 3, 1, 0, 0, 0 }, 0, 5);

            byte[] prev = null;
            int i = 0;
            while (i < frames.Count)
            {
                // merge identical frames into one longer delay
                int j = i + 1;
                while (j < frames.Count && Same(frames[j].Px, frames[i].Px)) j++;
                long until = j < frames.Count ? frames[j].Ms : endMs;
                int delay = (int)Math.Max(2, (until - frames[i].Ms + 5) / 10);
                int x0 = 0, y0 = 0, x1 = w - 1, y1 = h - 1;
                if (prev != null && !Bounds(prev, frames[i].Px, w, h, out x0, out y0, out x1, out y1)) { x0 = y0 = 0; x1 = y1 = 0; }
                WriteFrame(o, frames[i].Px, w, x0, y0, x1 - x0 + 1, y1 - y0 + 1, delay);
                prev = frames[i].Px;
                i = j;
            }
            o.WriteByte(0x3B);
        }
    }

    static bool Same(byte[] a, byte[] b)
    {
        for (int i = 0; i < a.Length; i++) if (a[i] != b[i]) return false;
        return true;
    }

    // bounding box of the pixels that differ (false if none)
    static bool Bounds(byte[] a, byte[] b, int w, int h, out int x0, out int y0, out int x1, out int y1)
    {
        x0 = w; y0 = h; x1 = -1; y1 = -1;
        for (int y = 0; y < h; y++)
            for (int x = 0; x < w; x++)
            {
                int k = (y * w + x) * 4;
                if (a[k] != b[k] || a[k + 1] != b[k + 1] || a[k + 2] != b[k + 2])
                {
                    if (x < x0) x0 = x; if (x > x1) x1 = x;
                    if (y < y0) y0 = y; if (y > y1) y1 = y;
                }
            }
        return x1 >= 0;
    }

    static void WriteFrame(Stream o, byte[] px, int stride, int fx, int fy, int fw, int fh, int delay)
    {
        var sub = new byte[fw * fh * 4];
        for (int y = 0; y < fh; y++) Buffer.BlockCopy(px, ((fy + y) * stride + fx) * 4, sub, y * fw * 4, fw * 4);
        var src = BitmapSource.Create(fw, fh, 96, 96, PixelFormats.Bgr32, null, sub, fw * 4);
        var pal = new BitmapPalette(src, 256);
        var idx = new FormatConvertedBitmap(src, PixelFormats.Indexed8, pal, 0);
        var ix = new byte[fw * fh];
        idx.CopyPixels(ix, fw, 0);
        int n = pal.Colors.Count, bits = 1;
        while ((1 << bits) < n) bits++;

        o.Write(new byte[] { 0x21, 0xF9, 4, 0x04 /* disposal: keep */ }, 0, 4);
        U16(o, delay); o.WriteByte(0); o.WriteByte(0);
        o.WriteByte(0x2C); U16(o, fx); U16(o, fy); U16(o, fw); U16(o, fh);
        o.WriteByte((byte)(0x80 | (bits - 1)));                                    // local colour table
        for (int i = 0; i < (1 << bits); i++)
        {
            Color c = i < n ? pal.Colors[i] : Colors.Black;
            o.WriteByte(c.R); o.WriteByte(c.G); o.WriteByte(c.B);
        }
        Lzw(o, ix, Math.Max(2, bits));
    }

    // GIF LZW (variable code size, clear at 4096 codes), same code-width rules as the classic compress/gif encoders
    static void Lzw(Stream o, byte[] px, int minBits)
    {
        o.WriteByte((byte)minBits);
        var data = new List<byte>();
        int clear = 1 << minBits, eoi = clear + 1, next = eoi + 1, bits = minBits + 1;
        var dict = new Dictionary<int, int>();
        int acc = 0, cnt = 0;
        Action<int> emit = code =>
        {
            acc |= code << cnt; cnt += bits;
            while (cnt >= 8) { data.Add((byte)acc); acc >>= 8; cnt -= 8; }
            if (next > (1 << bits) - 1 && bits < 12) bits++;
        };
        emit(clear);
        int prefix = px[0];
        for (int i = 1; i < px.Length; i++)
        {
            int k = px[i], key = (prefix << 8) | k, c;
            if (dict.TryGetValue(key, out c)) { prefix = c; continue; }
            emit(prefix);
            if (next < 4096) dict[key] = next++;
            else { emit(clear); dict.Clear(); next = eoi + 1; bits = minBits + 1; }
            prefix = k;
        }
        emit(prefix);
        emit(eoi);
        if (cnt > 0) data.Add((byte)acc);
        for (int i = 0; i < data.Count; i += 255)
        {
            int len = Math.Min(255, data.Count - i);
            o.WriteByte((byte)len);
            for (int j = 0; j < len; j++) o.WriteByte(data[i + j]);
        }
        o.WriteByte(0);
    }

    static void U16(Stream o, int v) { o.WriteByte((byte)v); o.WriteByte((byte)(v >> 8)); }
}
