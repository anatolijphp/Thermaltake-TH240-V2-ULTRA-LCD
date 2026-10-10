// Video decoding with Windows Media Foundation (built into Windows, no codecs to install).
// Converts a clip (max 30 s) into 480x480 JPEG frames with crop/zoom/pan, cached on disk.
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;

namespace TtLcd
{
    public class VideoClip
    {
        public List<byte[]> Frames = new List<byte[]>(); // upright 480x480 JPEGs
        public int FrameMs;
        public double SourceSeconds;                     // full length of the source file
    }

    public class VideoOptions
    {
        public string Path;
        public double Start;      // seconds
        public double Length;     // seconds, clamped to 30
        public int Fps;           // output frame rate, clamped 1..30
        public double Zoom;       // 1 = cover, >1 zooms in
        public double PanX, PanY; // -1..1, which part of the cropped area to keep
        public int Quality;

        public string Key()
        {
            string s = string.Format(System.Globalization.CultureInfo.InvariantCulture, "{0}|{1}|{2:F2}|{3:F2}|{4}|{5:F2}|{6:F2}|{7:F2}|{8}",
                Path, File.GetLastWriteTimeUtc(Path).Ticks, Start, Length, Fps, Zoom, PanX, PanY, Quality);
            using (var sha = SHA1.Create())
                return BitConverter.ToString(sha.ComputeHash(Encoding.UTF8.GetBytes(s))).Replace("-", "").Substring(0, 16);
        }
    }

    public static class Video
    {
        public const double MaxSeconds = 30;
        static readonly string CacheDir = System.IO.Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "ttlcd", "video");

        // progress: 0..1; return false from progress to cancel
        public static VideoClip Load(VideoOptions o, Func<double, bool> progress)
        {
            o.Length = Math.Max(0.5, Math.Min(MaxSeconds, o.Length));
            o.Fps = Math.Max(1, Math.Min(30, o.Fps));
            string cache = System.IO.Path.Combine(CacheDir, o.Key() + ".bin");
            var clip = ReadCache(cache);
            if (clip != null) return clip;
            clip = Decode(o, progress);
            if (clip != null && clip.Frames.Count > 0) WriteCache(cache, clip);
            return clip;
        }

        // Single upright frame at time t (for the editor preview).
        public static Bitmap Thumbnail(VideoOptions o, double t, out double sourceSeconds)
        {
            sourceSeconds = 0;
            using (var r = new Reader(o.Path))
            {
                sourceSeconds = r.Duration;
                Bitmap result = null;
                r.Seek(t);
                r.ReadFrames(ts =>
                {
                    return ts >= t - 0.05;
                }, (bmp, ts) => { result = CropTo480(bmp, o); return false; });
                return result;
            }
        }

        public static double Duration(string path)
        {
            using (var r = new Reader(path)) return r.Duration;
        }

        static VideoClip Decode(VideoOptions o, Func<double, bool> progress)
        {
            var clip = new VideoClip { FrameMs = 1000 / o.Fps };
            double step = 1.0 / o.Fps, next = o.Start, end = o.Start + o.Length;
            bool cancelled = false;
            using (var r = new Reader(o.Path))
            {
                clip.SourceSeconds = r.Duration;
                if (o.Start > 0) r.Seek(o.Start);
                r.ReadFrames(ts => ts >= next - step / 2 || ts >= end, (bmp, ts) =>
                {
                    if (ts >= end) return false;
                    // the same source frame may cover several output slots (low-fps source)
                    using (var f = CropTo480(bmp, o))
                    {
                        byte[] jpg = Render.Jpeg(f, o.Quality);
                        while (next <= ts + step / 2 && next < end) { clip.Frames.Add(jpg); next += step; }
                    }
                    if (progress != null && !progress(Math.Min(1, (ts - o.Start) / o.Length))) { cancelled = true; return false; }
                    return ts < end;
                });
            }
            return cancelled ? null : clip;
        }

        static Bitmap CropTo480(Bitmap src, VideoOptions o)
        {
            // cover-crop to square, then zoom in and pan within the remaining area
            double side = Math.Min(src.Width, src.Height) / Math.Max(1.0, o.Zoom);
            double maxX = src.Width - side, maxY = src.Height - side;
            double x = maxX / 2 * (1 + Math.Max(-1, Math.Min(1, o.PanX)));
            double y = maxY / 2 * (1 + Math.Max(-1, Math.Min(1, o.PanY)));
            var dst = new Bitmap(Lcd.Width, Lcd.Height, PixelFormat.Format24bppRgb);
            using (var g = Graphics.FromImage(dst))
            {
                g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBilinear;
                g.PixelOffsetMode = System.Drawing.Drawing2D.PixelOffsetMode.HighQuality;
                g.DrawImage(src, new Rectangle(0, 0, Lcd.Width, Lcd.Height), new RectangleF((float)x, (float)y, (float)side, (float)side), GraphicsUnit.Pixel);
            }
            return dst;
        }

        public static void ClearCache()
        {
            try { if (Directory.Exists(CacheDir)) Directory.Delete(CacheDir, true); } catch (Exception) { }
        }

        public static long CacheBytes()
        {
            long n = 0;
            if (Directory.Exists(CacheDir)) foreach (var f in new DirectoryInfo(CacheDir).GetFiles()) n += f.Length;
            return n;
        }

        static VideoClip ReadCache(string file)
        {
            try
            {
                if (!File.Exists(file)) return null;
                using (var r = new BinaryReader(File.OpenRead(file)))
                {
                    if (r.ReadInt32() != 0x4C434456) return null; // "VDCL"
                    var c = new VideoClip { FrameMs = r.ReadInt32(), SourceSeconds = r.ReadDouble() };
                    int n = r.ReadInt32();
                    for (int i = 0; i < n; i++) c.Frames.Add(r.ReadBytes(r.ReadInt32()));
                    return c;
                }
            }
            catch (Exception) { return null; }
        }

        static void WriteCache(string file, VideoClip c)
        {
            try
            {
                Directory.CreateDirectory(CacheDir);
                // keep the cache small: only the most recent few clips
                var old = new DirectoryInfo(CacheDir).GetFiles("*.bin");
                Array.Sort(old, (a, b) => b.LastWriteTimeUtc.CompareTo(a.LastWriteTimeUtc));
                for (int i = 4; i < old.Length; i++) old[i].Delete();
                using (var w = new BinaryWriter(File.Create(file)))
                {
                    w.Write(0x4C434456); w.Write(c.FrameMs); w.Write(c.SourceSeconds); w.Write(c.Frames.Count);
                    foreach (var f in c.Frames) { w.Write(f.Length); w.Write(f); }
                }
            }
            catch (Exception) { } // cache is optional
        }

        // ---------- Media Foundation source reader ----------
        class Reader : IDisposable
        {
            const int FirstVideo = unchecked((int)0xFFFFFFFC), AllStreams = unchecked((int)0xFFFFFFFE), MediaSource = unchecked((int)0xFFFFFFFF);
            const int EndOfStream = 0x2, TypeChanged = 0x20;
            IMFSourceReader reader;
            int width, height, stride;
            public double Duration;

            public Reader(string path)
            {
                Startup();
                IMFAttributes attr;
                Check(MFCreateAttributes(out attr, 2));
                attr.SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, 1);
                Check(MFCreateSourceReaderFromURL(path, attr, out reader));
                Marshal.ReleaseComObject(attr);
                reader.SetStreamSelection(AllStreams, false);
                reader.SetStreamSelection(FirstVideo, true);
                IMFMediaType t;
                Check(MFCreateMediaType(out t));
                t.SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
                t.SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
                reader.SetCurrentMediaType(FirstVideo, IntPtr.Zero, t);
                Marshal.ReleaseComObject(t);
                ReadFormat();
                var pv = new PROPVARIANT();
                if (reader.GetPresentationAttribute(MediaSource, MF_PD_DURATION, ref pv) == 0) Duration = pv.longVal / 1e7;
            }

            void ReadFormat()
            {
                IMFMediaType t;
                reader.GetCurrentMediaType(FirstVideo, out t);
                long size; t.GetUINT64(MF_MT_FRAME_SIZE, out size);
                width = (int)(size >> 32); height = (int)(size & 0xFFFFFFFF);
                int s;
                stride = t.GetUINT32(MF_MT_DEFAULT_STRIDE, out s) == 0 ? s : width * 4;
                // decoders pad to 16-pixel blocks (e.g. 600 -> 608); the aperture is the real picture
                visX = 0; visY = 0; visW = width; visH = height;
                var area = new byte[16]; int n;
                if (t.GetBlob(MF_MT_MINIMUM_DISPLAY_APERTURE, area, 16, out n) == 0 && n == 16)
                {
                    // MFVideoArea: MFOffset x {WORD fract; short value}, MFOffset y, SIZE {int cx, cy}
                    int ox = BitConverter.ToInt16(area, 2), oy = BitConverter.ToInt16(area, 6);
                    int cx = BitConverter.ToInt32(area, 8), cy = BitConverter.ToInt32(area, 12);
                    if (cx > 0 && cy > 0 && ox >= 0 && oy >= 0 && ox + cx <= width && oy + cy <= height) { visX = ox; visY = oy; visW = cx; visH = cy; }
                }
                Marshal.ReleaseComObject(t);
                if (Environment.GetEnvironmentVariable("TTLCD_DEBUG") != null)
                    Console.WriteLine("video {0}x{1} stride {2} aperture {3},{4} {5}x{6}", width, height, stride, visX, visY, visW, visH);
            }
            int visX, visY, visW, visH;

            public void Seek(double seconds)
            {
                var pv = new PROPVARIANT { vt = 20 /*VT_I8*/, longVal = (long)(seconds * 1e7) };
                reader.SetCurrentPosition(Guid.Empty, ref pv);
            }

            // want(ts): decode this frame to a bitmap?  got(bmp, ts): return false to stop.
            public void ReadFrames(Func<double, bool> want, Func<Bitmap, double, bool> got)
            {
                using (var bmp = new Bitmap(Math.Max(1, visW), Math.Max(1, visH), PixelFormat.Format32bppRgb))
                {
                    var frame = bmp;
                    for (; ; )
                    {
                        int actual, flags; long ts; IMFSample sample;
                        reader.ReadSample(FirstVideo, 0, out actual, out flags, out ts, out sample);
                        if ((flags & TypeChanged) != 0) ReadFormat();
                        if (sample == null) { if ((flags & EndOfStream) != 0) return; continue; }
                        try
                        {
                            double sec = ts / 1e7;
                            if (!want(sec)) { if (sec > 0 && (flags & EndOfStream) != 0) return; continue; }
                            if (frame.Width != visW || frame.Height != visH)
                            {
                                if (frame != bmp) frame.Dispose();
                                frame = new Bitmap(visW, visH, PixelFormat.Format32bppRgb);
                            }
                            Copy(sample, frame);
                            if (!got(frame, sec)) return;
                        }
                        finally { Marshal.ReleaseComObject(sample); }
                        if ((flags & EndOfStream) != 0) return;
                    }
                }
            }

            void Copy(IMFSample sample, Bitmap dst)
            {
                IMFMediaBuffer buf;
                sample.ConvertToContiguousBuffer(out buf);
                try
                {
                    IntPtr p; int max, cur;
                    buf.Lock(out p, out max, out cur);
                    try
                    {
                        var bd = dst.LockBits(new Rectangle(0, 0, visW, visH), ImageLockMode.WriteOnly, PixelFormat.Format32bppRgb);
                        try
                        {
                            int abs = Math.Abs(stride), row = visW * 4;
                            var line = new byte[row];
                            for (int y = 0; y < visH; y++)
                            {
                                // negative stride = bottom-up image
                                int srcRow = stride < 0 ? height - 1 - (visY + y) : visY + y;
                                long off = (long)srcRow * abs + visX * 4;
                                if (off + row > cur) break;
                                Marshal.Copy(new IntPtr(p.ToInt64() + off), line, 0, row);
                                Marshal.Copy(line, 0, new IntPtr(bd.Scan0.ToInt64() + (long)y * bd.Stride), row);
                            }
                        }
                        finally { dst.UnlockBits(bd); }
                    }
                    finally { buf.Unlock(); }
                }
                finally { Marshal.ReleaseComObject(buf); }
            }

            public void Dispose()
            {
                if (reader != null) { Marshal.ReleaseComObject(reader); reader = null; }
            }
        }

        static bool started;
        static void Startup()
        {
            if (started) return;
            Check(MFStartup(0x20070, 0));
            started = true;
        }

        static void Check(int hr) { if (hr < 0) Marshal.ThrowExceptionForHR(hr); }

        static readonly Guid MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING = new Guid("fb394f3d-ccf1-42ee-bbb3-f9b845d5681d");
        static readonly Guid MF_MT_MAJOR_TYPE = new Guid("48eba18e-f8c9-4687-bf11-0a74c9f96a8f");
        static readonly Guid MF_MT_SUBTYPE = new Guid("f7e34c9a-42e8-4714-b74b-cb29d72c35e5");
        static readonly Guid MF_MT_FRAME_SIZE = new Guid("1652c33d-d6b2-4012-b834-72030849a37d");
        static readonly Guid MF_MT_DEFAULT_STRIDE = new Guid("644b4e48-1e02-4516-b0eb-c01ca9d49ac6");
        static readonly Guid MF_MT_MINIMUM_DISPLAY_APERTURE = new Guid("d7388766-18fe-48c6-a177-ee894867c8c4");
        static readonly Guid MF_PD_DURATION = new Guid("6c990d33-bb8e-477a-8598-0d5d96fcd88a");
        static readonly Guid MFMediaType_Video = new Guid("73646976-0000-0010-8000-00AA00389B71");
        static readonly Guid MFVideoFormat_RGB32 = new Guid("00000016-0000-0010-8000-00AA00389B71");

        [DllImport("mfplat.dll")] static extern int MFStartup(int version, int flags);
        [DllImport("mfplat.dll")] static extern int MFCreateAttributes(out IMFAttributes attr, int size);
        [DllImport("mfplat.dll")] static extern int MFCreateMediaType(out IMFMediaType t);
        [DllImport("mfreadwrite.dll", CharSet = CharSet.Unicode)] static extern int MFCreateSourceReaderFromURL(string url, IMFAttributes attr, out IMFSourceReader reader);

        [StructLayout(LayoutKind.Explicit, Size = 24)]
        struct PROPVARIANT { [FieldOffset(0)] public short vt; [FieldOffset(8)] public long longVal; }

        // IMFAttributes vtable (30 methods); unused slots are placeholders to keep the order.
        [ComImport, Guid("2cd2d921-c447-44a7-a13c-4adabfc247e3"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        interface IMFAttributes
        {
            void GetItem(); void GetItemType(); void CompareItem(); void Compare();
            [PreserveSig] int GetUINT32([In, MarshalAs(UnmanagedType.LPStruct)] Guid key, out int value);
            [PreserveSig] int GetUINT64([In, MarshalAs(UnmanagedType.LPStruct)] Guid key, out long value);
            void GetDouble(); void GetGUID(); void GetStringLength(); void GetString(); void GetAllocatedString();
            void GetBlobSize(); void GetBlob(); void GetAllocatedBlob(); void GetUnknown(); void SetItem(); void DeleteItem(); void DeleteAllItems();
            void SetUINT32([In, MarshalAs(UnmanagedType.LPStruct)] Guid key, int value);
            void SetUINT64(); void SetDouble();
            void SetGUID([In, MarshalAs(UnmanagedType.LPStruct)] Guid key, [In, MarshalAs(UnmanagedType.LPStruct)] Guid value);
            void SetString(); void SetBlob(); void SetUnknown(); void LockStore(); void UnlockStore(); void GetCount(); void GetItemByIndex(); void CopyAllItems();
        }

        [ComImport, Guid("44ae0fa8-ea31-4109-8d2e-4cae4997c555"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        interface IMFMediaType
        {
            void GetItem(); void GetItemType(); void CompareItem(); void Compare();
            [PreserveSig] int GetUINT32([In, MarshalAs(UnmanagedType.LPStruct)] Guid key, out int value);
            [PreserveSig] int GetUINT64([In, MarshalAs(UnmanagedType.LPStruct)] Guid key, out long value);
            void GetDouble(); void GetGUID(); void GetStringLength(); void GetString(); void GetAllocatedString();
            void GetBlobSize();
            [PreserveSig] int GetBlob([In, MarshalAs(UnmanagedType.LPStruct)] Guid key, [Out, MarshalAs(UnmanagedType.LPArray)] byte[] buf, int size, out int written);
            void GetAllocatedBlob(); void GetUnknown(); void SetItem(); void DeleteItem(); void DeleteAllItems();
            void SetUINT32([In, MarshalAs(UnmanagedType.LPStruct)] Guid key, int value);
            void SetUINT64(); void SetDouble();
            void SetGUID([In, MarshalAs(UnmanagedType.LPStruct)] Guid key, [In, MarshalAs(UnmanagedType.LPStruct)] Guid value);
            void SetString(); void SetBlob(); void SetUnknown(); void LockStore(); void UnlockStore(); void GetCount(); void GetItemByIndex(); void CopyAllItems();
            void GetMajorType(); void IsCompressedFormat(); void IsEqual(); void GetRepresentation(); void FreeRepresentation();
        }

        [ComImport, Guid("c40a00f2-b93a-4d80-ae8c-5a1c634f58e4"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        interface IMFSample
        {
            void GetItem(); void GetItemType(); void CompareItem(); void Compare(); void GetUINT32(); void GetUINT64();
            void GetDouble(); void GetGUID(); void GetStringLength(); void GetString(); void GetAllocatedString();
            void GetBlobSize(); void GetBlob(); void GetAllocatedBlob(); void GetUnknown(); void SetItem(); void DeleteItem(); void DeleteAllItems();
            void SetUINT32(); void SetUINT64(); void SetDouble(); void SetGUID(); void SetString(); void SetBlob(); void SetUnknown();
            void LockStore(); void UnlockStore(); void GetCount(); void GetItemByIndex(); void CopyAllItems();
            void GetSampleFlags(); void SetSampleFlags(); void GetSampleTime(); void SetSampleTime(); void GetSampleDuration(); void SetSampleDuration();
            void GetBufferCount(); void GetBufferByIndex();
            void ConvertToContiguousBuffer(out IMFMediaBuffer buffer);
        }

        [ComImport, Guid("045FA593-8799-42b8-BC8D-8968C6453507"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        interface IMFMediaBuffer
        {
            void Lock(out IntPtr buffer, out int maxLength, out int currentLength);
            void Unlock();
        }

        [ComImport, Guid("70ae66f2-c809-4e4f-8915-bdcb406b7993"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        interface IMFSourceReader
        {
            void GetStreamSelection(int stream, [MarshalAs(UnmanagedType.Bool)] out bool selected);
            void SetStreamSelection(int stream, [MarshalAs(UnmanagedType.Bool)] bool selected);
            void GetNativeMediaType(int stream, int index, out IMFMediaType type);
            void GetCurrentMediaType(int stream, out IMFMediaType type);
            void SetCurrentMediaType(int stream, IntPtr reserved, IMFMediaType type);
            void SetCurrentPosition([In, MarshalAs(UnmanagedType.LPStruct)] Guid format, [In] ref PROPVARIANT position);
            void ReadSample(int stream, int control, out int actualStream, out int flags, out long timestamp, out IMFSample sample);
            void Flush(int stream);
            void GetServiceForStream();
            [PreserveSig] int GetPresentationAttribute(int stream, [In, MarshalAs(UnmanagedType.LPStruct)] Guid attr, ref PROPVARIANT value);
        }
    }
}
