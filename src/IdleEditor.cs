// The cooler's idle picture (stored in its flash, LX800 firmware): choosing and placing a new picture, encoding the exact
// JPEG that would be uploaded, storing it with read-back, and reading the stored one. No UI: front ends show Jpeg,
// Info and the progress, and call Pan/SetZoom while the user drags and zooms.
using System;
using System.IO;

namespace TtLcd
{
    class IdleEditor
    {
        readonly StudioCore core;
        public IdleEditor(StudioCore core) { this.core = core; }

        public const int Limit = Lcd.StandbyMaxLx;
        public static string Cache { get { return Path.Combine(Config.Folder, "idle-current.jpg"); } }

        System.Drawing.Image src;          // picture being placed; null = showing the stored one
        public double Zoom = 1, PanX, PanY; // zoom over the "cover" scale; pan in output pixels
        public byte[] Jpeg;                 // pending upload
        public string Info = "";
        public bool Busy, TriedRead;

        public bool Editing { get { return src != null; } }
        public bool Fits { get { return Jpeg != null && Jpeg.Length <= Limit; } }

        public static byte[] Stored()
        {
            try { return File.Exists(Cache) ? File.ReadAllBytes(Cache) : null; } catch (IOException) { return null; }
        }

        public static readonly string[] VideoExt = { ".mp4", ".mov", ".avi", ".mkv", ".webm", ".wmv", ".m4v" };

        // Picture, GIF (first frame) or video (first frame; Windows). Throws with a readable message.
        public void Load(string file)
        {
            System.Drawing.Image s;
            string ext = Path.GetExtension(file).ToLowerInvariant();
            if (Array.IndexOf(VideoExt, ext) >= 0)
            {
                if (Os.Linux) throw new IOException("videos need the Windows version; choose a picture or GIF");
                double dur;
                s = Video.Thumbnail(new VideoOptions { Path = file, Zoom = 1, Quality = 92 }, 0, out dur);
                if (s == null) throw new IOException("could not decode the video");
            }
            else s = System.Drawing.Image.FromStream(new MemoryStream(File.ReadAllBytes(file)));
            Drop();
            src = s;
            PanX = PanY = 0; Zoom = 1;
            core.Change(c => c.IdlePath = file);
            Encode();
        }

        public void Drop()
        {
            if (src != null) { src.Dispose(); src = null; }
            Jpeg = null;
        }

        double Cover() { return Math.Max((double)Lcd.Width / src.Width, (double)Lcd.Height / src.Height); }

        public void SetZoom(double z) { Zoom = z; ClampPan(); }
        public void Pan(double dx, double dy) { PanX += dx; PanY += dy; ClampPan(); }

        // keep the picture covering the whole screen
        void ClampPan()
        {
            if (src == null) return;
            double s = Cover() * Zoom;
            double mx = Math.Max(0, (src.Width * s - Lcd.Width) / 2), my = Math.Max(0, (src.Height * s - Lcd.Height) / 2);
            PanX = Math.Max(-mx, Math.Min(mx, PanX));
            PanY = Math.Max(-my, Math.Min(my, PanY));
        }

        // Crop -> 480x480 -> JPEG, lowering quality until it fits the limit. Not rotated: unlike streamed frames,
        // the idle screen is drawn by the firmware, which already shows it upright.
        public void Encode()
        {
            if (src == null) return;
            using (var bmp = new System.Drawing.Bitmap(Lcd.Width, Lcd.Height, System.Drawing.Imaging.PixelFormat.Format24bppRgb))
            {
                using (var g = System.Drawing.Graphics.FromImage(bmp))
                {
                    g.Clear(System.Drawing.Color.Black);
                    g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic;
                    g.PixelOffsetMode = System.Drawing.Drawing2D.PixelOffsetMode.HighQuality;
                    double s = Cover() * Zoom;
                    float sw = (float)(src.Width * s), sh = (float)(src.Height * s);
                    g.DrawImage(src, (float)((Lcd.Width - sw) / 2 + PanX), (float)((Lcd.Height - sh) / 2 + PanY), sw, sh);
                }
                int q = 92; byte[] jpg = Render.Jpeg(bmp, q);
                while (jpg.Length > Limit && q > 30) { q -= 6; jpg = Render.Jpeg(bmp, q); }
                Jpeg = jpg;
                Info = string.Format("480×480 JPEG · quality {0} · {1:F0} KB of {2} KB{3}",
                    q, jpg.Length / 1024.0, Limit / 1024, Fits ? "" : " · too large");
            }
        }

        // Read the stored picture back from the cooler (read-only commands 0x0C/0x0D, LX800 firmware).
        // progress and done run on the engine's control thread.
        public void Read(Action<int> progress, Action<string> done)
        {
            if (Busy) return;
            Busy = true; TriedRead = true;
            byte[] data = null;
            core.Engine.Enqueue(new Engine.DeviceJob
            {
                Name = "idle-read",
                Progress = progress,
                Work = (lcd, prog) =>
                {
                    if (!lcd.SwapRedBlue) throw new IOException("reading the idle image needs the LX800 firmware");
                    var b = lcd.ReadFileLx(Lcd.StandbyFile, prog);
                    data = b.Length > 0 ? lcd.ForPanel(b) : b; // undo the panel's red/blue swap
                },
                Done = err =>
                {
                    if (err == null && data != null && data.Length > 0)
                        try { File.WriteAllBytes(Cache, data); } catch (IOException) { }
                    Busy = false;
                    done(err != null ? "Could not read it: " + err.Message
                        : data == null || data.Length == 0 ? "The cooler has no idle image stored." : "Read from the cooler.");
                }
            });
        }

        // Write the pending picture into the cooler's flash (LX800 only; verified by read-back). Streaming pauses.
        // done(status, ok) runs on the engine's control thread.
        public void Store(Action<int> progress, Action<string, bool> done)
        {
            if (Jpeg == null || Busy) return;
            Busy = true;
            var jpg = Jpeg;
            core.Engine.Enqueue(new Engine.DeviceJob
            {
                Name = "idle",
                Progress = progress,
                Work = (lcd, prog) => { if (!DeviceStore.WriteStandby(lcd, jpg, prog)) throw new IOException("read-back did not match"); },
                Done = err =>
                {
                    if (err == null) try { File.WriteAllBytes(Cache, jpg); } catch (IOException) { }
                    Busy = false;
                    done(err == null ? "Stored and verified on the cooler." : "Failed: " + err.Message, err == null);
                }
            });
        }

        public static string StoreProgressText(int p) { return p < 80 ? "Writing to the cooler, do not unplug" : "Checking the written image"; }
    }
}
