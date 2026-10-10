// Linux front end: the tray icon (AppIndicator, as Ubuntu's panel shows it) with the same menu as on Windows, and the
// single-instance rule (a second launch asks the running one to show its window, then exits).
using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Threading;
using Gtk;

namespace TtLcd.GtkUi
{
    static class Tray
    {
        [DllImport("libayatana-appindicator3.so.1")] static extern IntPtr app_indicator_new(string id, string icon, int category);
        [DllImport("libayatana-appindicator3.so.1")] static extern void app_indicator_set_status(IntPtr ind, int status);
        [DllImport("libayatana-appindicator3.so.1")] static extern void app_indicator_set_menu(IntPtr ind, IntPtr menu);
        [DllImport("libayatana-appindicator3.so.1")] static extern void app_indicator_set_title(IntPtr ind, string title);
        const int CategoryHardware = 3, StatusActive = 1;

        static IntPtr indicator;
        static Menu menu;   // kept alive for the indicator

        // Returns false when no indicator library is installed (then closing the window quits the app).
        public static bool Create(StudioWindow win, StudioCore core)
        {
            try
            {
                indicator = app_indicator_new("ttlcd-studio", "ttlcd-studio", CategoryHardware);
                if (indicator == IntPtr.Zero) return false;
                app_indicator_set_title(indicator, "TT LCD Studio");
            }
            catch (Exception) { return false; }   // DllNotFoundException / EntryPointNotFoundException

            menu = new Menu();
            var open = new MenuItem("Open TT LCD Studio");
            open.Activated += (o, a) => win.ShowWindow();
            menu.Append(open);
            menu.Append(new SeparatorMenuItem());
            var modeItem = new MenuItem("Mode");
            var modes = new Menu();
            RadioMenuItem first = null;
            foreach (Mode m in Enum.GetValues(typeof(Mode)))
            {
                if (!Os.Supports(m)) continue;
                var mm = m;
                var item = first == null ? new RadioMenuItem(Engine.Label(m)) : new RadioMenuItem(first, Engine.Label(m));
                if (first == null) first = item;
                item.Active = core.Config.Mode == m;
                item.Toggled += (o, a) => { if (item.Active && core.Config.Mode != mm) { core.Change(c => c.Mode = mm); win.Load(); } };
                modes.Append(item);
            }
            modeItem.Submenu = modes;
            menu.Append(modeItem);
            var pause = new CheckMenuItem("Pause");
            pause.Toggled += (o, a) => core.Engine.Paused = pause.Active;
            menu.Append(pause);
            var game = new CheckMenuItem("Game mode");
            game.Toggled += (o, a) => core.Engine.ForceGame = game.Active;
            menu.Append(game);
            menu.Append(new SeparatorMenuItem());
            var exit = new MenuItem("Exit");
            exit.Activated += (o, a) => win.Quit();
            menu.Append(exit);
            menu.ShowAll();
            // keep the check marks in step with changes made in the window
            GLib.Timeout.Add(1000, () =>
            {
                if (pause.Active != core.Engine.Paused) pause.Active = core.Engine.Paused;
                if (game.Active != core.Engine.ForceGame) game.Active = core.Engine.ForceGame;
                return true;
            });
            app_indicator_set_menu(indicator, menu.Handle);
            app_indicator_set_status(indicator, StatusActive);
            return true;
        }
    }

    // One Studio per user: the first instance listens on a local port; later launches send "show" and exit.
    static class SingleInstance
    {
        const int Port = 47913;
        static TcpListener listener;

        public static bool SignalExisting()
        {
            try
            {
                using (var c = new TcpClient())
                {
                    if (!c.ConnectAsync(IPAddress.Loopback, Port).Wait(300)) return false;
                    var w = new StreamWriter(c.GetStream()); w.WriteLine("ttlcd show"); w.Flush();
                    return true;
                }
            }
            catch (Exception) { return false; }
        }

        public static void Listen(System.Action show)
        {
            try
            {
                listener = new TcpListener(IPAddress.Loopback, Port);
                listener.Start();
            }
            catch (Exception) { return; }   // port taken by something else: run without the single-instance rule
            new Thread(() =>
            {
                while (true)
                {
                    try
                    {
                        using (var c = listener.AcceptTcpClient())
                        using (var r = new StreamReader(c.GetStream()))
                            if (r.ReadLine() == "ttlcd show") Application.Invoke((o, e) => show());
                    }
                    catch (Exception) { return; }
                }
            }) { IsBackground = true, Name = "single-instance" }.Start();
        }
    }
}
