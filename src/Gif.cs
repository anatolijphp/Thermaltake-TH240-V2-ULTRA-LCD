// Animated GIF writer (dev aid for the README media; used by TTLcdStudio --record on Windows and Linux).
// Each frame stores only the rectangle that changed since the previous one, with its own palette, nearest-colour
// mapping without dithering (so unchanged areas compress well), and the standard GIF LZW coder.
// No WPF here, so Mono builds it too: the palette uses a median cut, or Quantiser when one is set (WPF's on Windows).
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace TtLcd
{
    class GifWriter
    {
        readonly List<byte[]> frames = new List<byte[]>();
        readonly List<int> delays = new List<int>(); // 1/100 s
        readonly int w, h;

        public GifWriter(int width, int height) { w = width; h = height; }

        // (Bgr32 pixels, pixel count, max colours) -> up to max colours as 0xRRGGBB; null = the median cut below
        public static Func<byte[], int, int, IEnumerable<int>> Quantiser = null;

        // px: Bgra32/Pbgra32 (opaque), w*h*4 bytes
        public void Add(byte[] px, int delayCs)
        {
            if (frames.Count > 0 && Same(px, frames[frames.Count - 1])) { delays[delays.Count - 1] += delayCs; return; }
            frames.Add(px);
            delays.Add(delayCs);
        }

        public void HoldLast(int extraCs) { if (delays.Count > 0) delays[delays.Count - 1] += extraCs; }

        public void Save(string path)
        {
            using (var o = new FileStream(path, FileMode.Create))
            {
                o.Write(new byte[] { (byte)'G', (byte)'I', (byte)'F', (byte)'8', (byte)'9', (byte)'a' }, 0, 6);
                U16(o, w); U16(o, h); o.WriteByte(0); o.WriteByte(0); o.WriteByte(0);   // no global colour table
                o.Write(new byte[] { 0x21, 0xFF, 11 }, 0, 3);                            // loop forever
                foreach (char c in "NETSCAPE2.0") o.WriteByte((byte)c);
                o.Write(new byte[] { 3, 1, 0, 0, 0 }, 0, 5);
                var shown = new byte[w * h * 4];   // what a viewer sees so far (quantised colours)
                for (int i = 0; i < frames.Count; i++)
                {
                    int x0 = 0, y0 = 0, x1 = w - 1, y1 = h - 1;
                    if (i > 0) Bounds(shown, frames[i], out x0, out y0, out x1, out y1);
                    WriteFrame(o, frames[i], shown, i > 0, x0, y0, x1 - x0 + 1, y1 - y0 + 1, Math.Max(2, delays[i]));
                }
                o.WriteByte(0x3B);
            }
        }

        static bool Same(byte[] a, byte[] b)
        {
            for (int i = 0; i < a.Length; i++) if (a[i] != b[i]) return false;
            return true;
        }

        void Bounds(byte[] a, byte[] b, out int x0, out int y0, out int x1, out int y1)
        {
            x0 = w; y0 = h; x1 = 0; y1 = 0;
            for (int y = 0; y < h; y++)
                for (int x = 0; x < w; x++)
                {
                    int k = (y * w + x) * 4;
                    if (Differs(a, b, k))
                    {
                        if (x < x0) x0 = x; if (x > x1) x1 = x;
                        if (y < y0) y0 = y; if (y > y1) y1 = y;
                    }
                }
            if (x0 > x1) { x0 = y0 = x1 = y1 = 0; }
        }

        // Pixels equal to the previous frame are written as the transparent index 0, so only real changes are redrawn:
        // no seams from re-quantised unchanged areas, and much better compression.
        // a shown pixel counts as right when every channel is within Tol of the source
        const int Tol = 0;
        static bool Differs(byte[] a, byte[] b, int k)
        {
            return Math.Abs(a[k] - b[k]) > Tol || Math.Abs(a[k + 1] - b[k + 1]) > Tol || Math.Abs(a[k + 2] - b[k + 2]) > Tol;
        }

        void WriteFrame(Stream o, byte[] px, byte[] shown, bool partial, int fx, int fy, int fw, int fh, int delay)
        {
            var keep = new bool[fw * fh];
            var changed = new List<byte>();
            for (int y = 0; y < fh; y++)
                for (int x = 0; x < fw; x++)
                {
                    int k = ((fy + y) * w + fx + x) * 4;
                    if (partial && !Differs(shown, px, k)) { keep[y * fw + x] = true; continue; }
                    changed.Add(px[k]); changed.Add(px[k + 1]); changed.Add(px[k + 2]); changed.Add(255);
                }
            var sample = changed.ToArray();
            int first = partial ? 1 : 0;                                            // index 0 = transparent
            var pal = Palette(sample, Math.Max(1, sample.Length / 4), 256 - first);
            int n = pal.Count + first, bits = 1;
            while ((1 << bits) < n) bits++;

            // nearest palette colour, cached per 15-bit colour
            var cache = new short[32768];
            for (int i = 0; i < cache.Length; i++) cache[i] = -1;
            var ix = new byte[fw * fh];
            for (int y = 0; y < fh; y++)
                for (int x = 0; x < fw; x++)
                {
                    int i = y * fw + x;
                    if (keep[i]) { ix[i] = 0; continue; }
                    int k = ((fy + y) * w + fx + x) * 4;
                    int b = px[k], g = px[k + 1], r = px[k + 2];
                    int key = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
                    if (cache[key] < 0)
                    {
                        int best = 0, bd = int.MaxValue;
                        for (int j = 0; j < pal.Count; j++)
                        {
                            int c = pal[j];
                            int dr = (c >> 16 & 255) - r, dg = (c >> 8 & 255) - g, db = (c & 255) - b, d = dr * dr * 3 + dg * dg * 4 + db * db * 2;
                            if (d < bd) { bd = d; best = j; }
                        }
                        cache[key] = (short)(best + first);
                    }
                    ix[i] = (byte)cache[key];
                    int sc = pal[cache[key] - first];
                    shown[k] = (byte)sc; shown[k + 1] = (byte)(sc >> 8); shown[k + 2] = (byte)(sc >> 16);
                }

            o.Write(new byte[] { 0x21, 0xF9, 4, (byte)(0x04 | first) /* keep previous; transparent index */ }, 0, 4);
            U16(o, delay); o.WriteByte(0); o.WriteByte(0);
            o.WriteByte(0x2C); U16(o, fx); U16(o, fy); U16(o, fw); U16(o, fh);
            o.WriteByte((byte)(0x80 | (bits - 1)));                                    // local colour table
            for (int i = 0; i < (1 << bits); i++)
            {
                int c = i >= first && i - first < pal.Count ? pal[i - first] : 0;
                o.WriteByte((byte)(c >> 16)); o.WriteByte((byte)(c >> 8)); o.WriteByte((byte)c);
            }
            Lzw(o, ix, Math.Max(2, bits));
        }
        // The most frequent exact colours first (flat UI surfaces then match the frames around them exactly, so a partial
        // frame leaves no visible seams), the rest from the quantiser for gradients and pictures. Colours are 0xRRGGBB.
        static List<int> Palette(byte[] sub, int count, int max)
        {
            var freq = new Dictionary<int, int>();
            for (int i = 0; i < sub.Length; i += 4)
            {
                int c = sub[i] | (sub[i + 1] << 8) | (sub[i + 2] << 16), k;
                freq.TryGetValue(c, out k);
                freq[c] = k + 1;
            }
            var colors = new List<int>();
            var seen = new HashSet<int>();
            foreach (var kv in freq.OrderByDescending(x => x.Value).Take(freq.Count <= max ? max : max / 2))
            {
                colors.Add(kv.Key);
                seen.Add(kv.Key);
            }
            if (freq.Count > max)
                foreach (var c in (Quantiser ?? MedianCut)(sub, count, 256))
                {
                    if (colors.Count >= max) break;
                    if (seen.Add(c)) colors.Add(c);
                }
            return colors;
        }

        // Median cut: split the box with the widest channel range at its median until there are max boxes, then take
        // each box's average. Samples at most 64K pixels.
        static IEnumerable<int> MedianCut(byte[] sub, int count, int max)
        {
            int step = Math.Max(1, count / 65536);
            var px = new List<int>();
            for (int i = 0; i < count; i += step) px.Add(sub[i * 4] | (sub[i * 4 + 1] << 8) | (sub[i * 4 + 2] << 16));
            var boxes = new List<List<int>> { px };
            Func<List<int>, int, int> range = (b, sh) => { int lo = 255, hi = 0; foreach (int c in b) { int v = c >> sh & 255; if (v < lo) lo = v; if (v > hi) hi = v; } return hi - lo; };
            while (boxes.Count < max)
            {
                List<int> big = null; int bestR = 0, bestSh = 0;
                foreach (var b in boxes)
                {
                    if (b.Count < 2) continue;
                    foreach (int sh in new[] { 16, 8, 0 })
                    {
                        int r = range(b, sh);
                        if (r > bestR) { bestR = r; big = b; bestSh = sh; }
                    }
                }
                if (big == null) break;
                int shift = bestSh;
                big.Sort((x, y) => (x >> shift & 255).CompareTo(y >> shift & 255));
                int mid = big.Count / 2;
                boxes.Remove(big);
                boxes.Add(big.GetRange(0, mid)); boxes.Add(big.GetRange(mid, big.Count - mid));
            }
            foreach (var b in boxes)
            {
                if (b.Count == 0) continue;
                long r = 0, g = 0, bl = 0;
                foreach (int c in b) { r += c >> 16 & 255; g += c >> 8 & 255; bl += c & 255; }
                yield return (int)(r / b.Count) << 16 | (int)(g / b.Count) << 8 | (int)(bl / b.Count);
            }
        }

        // GIF LZW (variable code width, clear at 4096 codes), same width rules as the classic compress/GIF encoders
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
}
