// TTLcdStudio.exe on Linux: the GTK front end over the shared StudioCore (built by build.sh with Mono and GTK# 3).
//   ttlcd-studio [--tray] [--screenshot <file.png> <dashboard|modes[:Mode]|device|settings>] [--record <file.gif> tour [scale]]
using System;
using System.IO;
using Gtk;

namespace TtLcd.GtkUi
{
    static class GtkProgram
    {
        static int Main(string[] args)
        {
            int si = Array.IndexOf(args, "--screenshot");
            bool shot = si >= 0 && si + 1 < args.Length;
            int ri = Array.IndexOf(args, "--record");
            bool rec = ri >= 0 && ri + 2 < args.Length;   // dev aid: scripted scene to a GIF (gtk/Record.cs)
            // one Studio per user: a second launch brings the running one to the front
            if (!shot && !rec && SingleInstance.SignalExisting()) return 0;

            Application.Init();
            var css = new CssProvider();
            css.LoadFromData(Theme.Css);
            StyleContext.AddProviderForScreen(Gdk.Screen.Default, css, 800);
            GLib.ExceptionManager.UnhandledException += a =>
            {
                try { File.AppendAllText(Path.Combine(Config.Folder, "crash.log"), DateTime.Now.ToString("s") + " " + a.ExceptionObject + Environment.NewLine + Environment.NewLine); }
                catch (Exception) { }
                a.ExitApplication = false;
            };

            // the `ttlcd run` user service would drive the cooler too: Studio takes over while it is open
            string note = null;
            if (!shot && !rec && LinuxPlatform.ServiceActive())
            {
                LinuxPlatform.StopService();
                note = "The ttlcd background service was stopped while Studio runs.";
            }

            var core = new StudioCore(new LinuxPlatform());
            var win = new StudioWindow(core) { Note = note };
            win.DeleteEvent += (o, a) => { win.CloseRequested(); a.RetVal = true; };
            int height;
            if (int.TryParse(Environment.GetEnvironmentVariable("TTLCD_SHOT_HEIGHT"), out height)) win.SetDefaultSize(1240, height);
            if (!shot && !rec)
            {
                win.HasTray = Tray.Create(win, core);
                SingleInstance.Listen(win.ShowWindow);
            }

            if (shot)
            {
                win.ShowWindow();
                // dev aid / CI: capture the window after the first frames arrived, then exit
                string file = args[si + 1], page = si + 2 < args.Length ? args[si + 2] : "dashboard";
                GLib.Timeout.Add(2500, () =>
                {
                    try { win.Screenshot(file, page); }
                    catch (Exception e) { Console.Error.WriteLine("screenshot failed: " + e); }
                    win.Quit();
                    return false;
                });
            }
            else if (rec)
            {
                win.ShowWindow();
                double scale;
                if (ri + 3 >= args.Length || !double.TryParse(args[ri + 3], System.Globalization.NumberStyles.Float, System.Globalization.CultureInfo.InvariantCulture, out scale)) scale = 1;
                win.Record(args[ri + 1], args[ri + 2], scale, win.Quit);
            }
            else if (Array.IndexOf(args, "--tray") >= 0 && win.HasTray) { /* start in the tray: the window opens from its menu */ }
            else win.ShowWindow();

            Application.Run();
            core.Shutdown();
            return 0;
        }
    }
}
