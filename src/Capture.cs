// Screen capture of a monitor (or a region of it) for streaming to the LCD.
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace TtLcd
{
    public class ScreenSource : IDisposable
    {
        readonly Rectangle rect;
        readonly Bitmap buf;

        [DllImport("user32.dll")] static extern bool SetProcessDpiAwarenessContext(IntPtr ctx);

        static ScreenSource()
        {
            // per-monitor DPI aware v2, so Screen bounds are physical pixels
            try { SetProcessDpiAwarenessContext(new IntPtr(-4)); } catch (EntryPointNotFoundException) { }
        }

        // Call first thing in Main: DPI awareness must be set before any window exists.
        public static void InitDpi() { }

        public ScreenSource(Rectangle r)
        {
            rect = r;
            buf = new Bitmap(r.Width, r.Height, PixelFormat.Format24bppRgb);
        }

        public static void ListMonitors()
        {
            var all = Screen.AllScreens;
            for (int i = 0; i < all.Length; i++)
                Console.WriteLine("{0}: {1} {2}x{3} at {4},{5}{6}", i, all[i].DeviceName, all[i].Bounds.Width, all[i].Bounds.Height,
                    all[i].Bounds.X, all[i].Bounds.Y, all[i].Primary ? " (primary)" : "");
        }

        // Finds a monitor whose resolution matches the panel (the virtual LCD display).
        public static int FindBySize(int w, int h)
        {
            var all = Screen.AllScreens;
            for (int i = 0; i < all.Length; i++)
                if (all[i].Bounds.Width == w && all[i].Bounds.Height == h) return i;
            return -1;
        }

        public static Rectangle MonitorRect(int index, Rectangle? sub)
        {
            var all = Screen.AllScreens;
            if (index < 0 || index >= all.Length) throw new ArgumentException("no monitor " + index);
            var b = all[index].Bounds;
            if (sub == null) return b;
            var s = sub.Value;
            return new Rectangle(b.X + s.X, b.Y + s.Y, s.Width, s.Height);
        }

        public Bitmap Grab()
        {
            using (var g = Graphics.FromImage(buf))
                g.CopyFromScreen(rect.Location, Point.Empty, rect.Size, CopyPixelOperation.SourceCopy);
            return buf;
        }

        public void Dispose() { buf.Dispose(); }
    }
}
