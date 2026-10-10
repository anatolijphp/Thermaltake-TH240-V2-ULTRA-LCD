// Pictures the cooler keeps in its own flash and shows without the PC:
//  - idle (standby.jpg): shown when no frames arrive (PC asleep / no software running)
//  - loading (boot animation, boot.pak): played while the PC boots
// Upload protocol and the boot.pak layout are from TT RGB Plus (Device_Toughliquid_CPU_HKC::WriteBootFile,
// WriteStandbyFile, write_boot and its packer), see docs/protocol.md.
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;

namespace TtLcd
{
    public static class DeviceStore
    {
        // TT RGB Plus packs the frames folder %APPDATA%\Tt\TTRGBPlus2.0\Update_Boot_GIF, so that is the
        // root name inside the archive; frames are named 000_.jpg, 001_.jpg, ...
        const string BootRoot = "Update_Boot_GIF";
        public const int MaxBootFrames = 300;

        static bool IsVideo(string p)
        {
            var e = Path.GetExtension(p).ToLowerInvariant();
            return e == ".mp4" || e == ".mov" || e == ".wmv" || e == ".avi" || e == ".mkv" || e == ".webm" || e == ".m4v";
        }

        static bool IsGif(string p) { return Path.GetExtension(p).Equals(".gif", StringComparison.OrdinalIgnoreCase); }

        // Upright 480x480 frames (JPEG) with per-frame delay, from a picture, GIF or video (first 10 s).
        public static List<GifFrame> LoadFrames(string path, int quality, Func<double, bool> progress)
        {
            if (IsGif(path)) return Render.GifFrames(path, Lcd.Width, Lcd.Height, 0, quality);
            if (IsVideo(path))
            {
                var clip = Video.Load(new VideoOptions { Path = path, Start = 0, Length = 10, Fps = 15, Zoom = 1, Quality = quality }, progress);
                if (clip == null) throw new IOException("could not decode " + Path.GetFileName(path));
                return clip.Frames.Select(f => new GifFrame { Jpeg = f, DelayMs = clip.FrameMs }).ToList();
            }
            return new List<GifFrame> { new GifFrame { Jpeg = Render.FileToJpeg(path, Lcd.Width, Lcd.Height, 0, quality), DelayMs = 1000 } };
        }

        static byte[] Rotated(byte[] jpeg, int rotation, int quality)
        {
            if (((rotation % 360) + 360) % 360 == 0) return jpeg;
            using (var ms = new MemoryStream(jpeg))
            using (var bmp = new Bitmap(ms))
            {
                Render.Rotate(bmp, rotation);
                return Render.Jpeg(bmp, quality);
            }
        }

        // ---- idle ----

        public static byte[] StandbyJpeg(string path, int rotation, int quality)
        {
            var f = LoadFrames(path, quality, null);
            return Rotated(f[0].Jpeg, rotation, quality);
        }

        // Store the idle picture. On LX800 firmware the panel's red/blue swap is baked in, the LX800 chunk
        // protocol is used, the 950 KB cap is enforced (re-encoding to fit if needed), and the write is read
        // back and byte-compared before we trust it. Returns true if the read-back matched.
        public static bool WriteStandby(Lcd lcd, byte[] jpeg, Action<int> progress)
        {
            jpeg = lcd.ForPanel(jpeg);
            if (lcd.SwapRedBlue) // LX800 firmware
            {
                jpeg = FitJpeg(jpeg, Lcd.StandbyMaxLx);
                // progress: writing 0..80 %, reading back 80..100 %
                lcd.UploadFileLx(Lcd.StandbyFile, jpeg, p => { if (progress != null) progress(p * 80 / 100); });
                var back = lcd.ReadFileLx(Lcd.StandbyFile, p => { if (progress != null) progress(80 + p / 5); });
                return back.Length == jpeg.Length && back.SequenceEqual(jpeg);
            }
            // The original Thermaltake firmware erased its whole flash during this upload once (see CLAUDE.md),
            // so the write is only allowed on the LX800 firmware, where it is verified by read-back.
            throw new IOException("storing an idle image is disabled on this firmware (an upload erased the flash once)");
        }

        // Re-encode the JPEG at lower quality until it fits maxBytes; throw if it cannot.
        static byte[] FitJpeg(byte[] jpeg, int maxBytes)
        {
            if (jpeg.Length <= maxBytes) return jpeg;
            for (int q = 85; q >= 30; q -= 10)
            {
                var r = Recompress(jpeg, q);
                if (r.Length <= maxBytes) return r;
            }
            throw new IOException("idle picture is too large for the cooler (" + (maxBytes / 1024) + " KB limit)");
        }

        // ---- loading (boot animation) ----

        public class Boot { public byte[] Pak; public int Frames, IntervalMs; }

        // Frames are rotated like live frames, thinned and re-compressed until the pack fits the device.
        public static Boot BuildBoot(List<GifFrame> frames, int rotation, int quality)
        {
            if (frames.Count == 0) throw new IOException("no frames");
            int step = Math.Max(1, (frames.Count + MaxBootFrames - 1) / MaxBootFrames);
            for (int q = quality; ; q = Math.Max(30, q - 15), step++)
            {
                var pick = new List<GifFrame>();
                for (int i = 0; i < frames.Count; i += step)
                    pick.Add(new GifFrame { Jpeg = q == quality ? frames[i].Jpeg : Recompress(frames[i].Jpeg, q), DelayMs = frames.Skip(i).Take(step).Sum(f => f.DelayMs) });
                var files = pick.Select((f, i) => new KeyValuePair<string, byte[]>(i.ToString("000") + "_.jpg", Rotated(f.Jpeg, rotation, q))).ToList();
                var pak = Pack(BootRoot, files);
                if (pak.Length <= Lcd.BootMax || (q == 30 && pick.Count == 1))
                {
                    if (pak.Length > Lcd.BootMax) throw new IOException("animation does not fit into the cooler");
                    // the device plays at one fixed rate: TT RGB Plus uses total duration / frame count
                    int interval = (int)Math.Round(pick.Sum(f => (double)f.DelayMs) / pick.Count);
                    return new Boot { Pak = pak, Frames = pick.Count, IntervalMs = Math.Max(20, interval) };
                }
            }
        }

        static byte[] Recompress(byte[] jpeg, int q)
        {
            using (var ms = new MemoryStream(jpeg))
            using (var bmp = new Bitmap(ms))
                return Render.Jpeg(bmp, q);
        }

        public static void WriteBoot(Lcd lcd, Boot boot, Action<int> progress)
        {
            lcd.UploadFile(Lcd.BootFile, boot.Pak, progress);
            if (!lcd.SetBootFile("boot.pak")) throw new IOException("cooler did not accept boot.pak");
            lcd.SetBootFrameInterval(boot.IntervalMs);
        }

        // boot.pak: one folder record followed by an all-zero terminator record.
        //  record header (16 bytes, LE): i32 check, i32 a, i32 b, u16 nameLen (incl. NUL), u16 flags
        //    check = -(a + b + nameLen + (flags & 0xFFF))
        //  folder: a = total record size, b = 16 * fileCount, flags = 0x1000 | fileCount
        //          header, name, fileCount file headers, then (name, data) per file
        //  file:   a = offset of its name from the folder header, b = size, flags = 0
        //  files are ordered by name length, then name.
        public static byte[] Pack(string root, IList<KeyValuePair<string, byte[]>> files)
        {
            var sorted = files.OrderBy(f => f.Key.Length).ThenBy(f => f.Key, StringComparer.Ordinal).ToList();
            int count = sorted.Count;
            byte[] rootName = Z(root);
            int start = 16 + rootName.Length + 16 * count;
            int body = sorted.Sum(f => Z(f.Key).Length + f.Value.Length);
            using (var ms = new MemoryStream())
            using (var w = new BinaryWriter(ms))
            {
                Header(w, start + body, 16 * count, rootName.Length, 0x1000 | count);
                w.Write(rootName);
                int off = start;
                foreach (var f in sorted)
                {
                    int nameLen = Z(f.Key).Length;
                    Header(w, off, f.Value.Length, nameLen, 0);
                    off += nameLen + f.Value.Length;
                }
                foreach (var f in sorted) { w.Write(Z(f.Key)); w.Write(f.Value); }
                w.Write(new byte[14]); w.Write((ushort)0x1000); // terminator
                w.Flush();
                return ms.ToArray();
            }
        }

        static byte[] Z(string s) { return Encoding.ASCII.GetBytes(s + "\0"); }

        static void Header(BinaryWriter w, int a, int b, int nameLen, int flags)
        {
            w.Write(-(a + b + nameLen + (flags & 0xFFF)));
            w.Write(a); w.Write(b);
            w.Write((ushort)nameLen); w.Write((ushort)flags);
        }
    }
}
