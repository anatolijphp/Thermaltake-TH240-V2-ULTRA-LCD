// TTLcd.exe: WPF window + tray icon. Built from the same sources as ttlcd.exe with /main:TtLcd.TrayProgram.
using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Threading;
using WinForms = System.Windows.Forms;

namespace TtLcd
{
    static class TrayProgram
    {
        [STAThread]
        static void Main(string[] args)
        {
            ScreenSource.InitDpi();
            bool first;
            using (var mutex = new Mutex(true, "TTLcd.Tray.SingleInstance", out first))
            {
                if (!first)
                {
                    // ask the running instance to show its window
                    using (var e = OpenShowEvent()) if (e != null) e.Set();
                    return;
                }
                var app = new Application { ShutdownMode = ShutdownMode.OnExplicitShutdown };
                // a UI bug should not kill the app (and the cooler stream): log it and carry on
                app.DispatcherUnhandledException += (s, e) =>
                {
                    try { File.AppendAllText(Path.Combine(Config.Folder, "crash.log"), DateTime.Now.ToString("s") + " " + e.Exception + Environment.NewLine + Environment.NewLine); }
                    catch (Exception) { }
                    e.Handled = true;
                };
                var shell = new Shell(args.Contains("--tray") || args.Contains("--screenshot") || args.Contains("--record"));
                int si = Array.IndexOf(args, "--screenshot");
                if (si >= 0 && si + 1 < args.Length)
                {
                    // dev aid: --screenshot <file.png> [modes|device|settings]
                    string file = args[si + 1], page = si + 2 < args.Length ? args[si + 2] : "modes";
                    shell.EnsureUi(); // build the (hidden) window now so async loads finish before the capture
                    int delay;
                    if (!int.TryParse(Environment.GetEnvironmentVariable("TTLCD_SHOT_DELAY"), out delay)) delay = 4;
                    var t = new DispatcherTimer { Interval = TimeSpan.FromSeconds(delay) };
                    t.Tick += (s, e) => { t.Stop(); shell.Screenshot(file, page); shell.Exit(); };
                    t.Start();
                }
                int ri = Array.IndexOf(args, "--record");
                if (ri >= 0 && ri + 2 < args.Length)
                {
                    // dev aid: --record <file.gif> <dashboard|modes|style> [scale] (README media, see Record.cs)
                    string file = args[ri + 1], scene = args[ri + 2];
                    double scale;
                    if (ri + 3 >= args.Length || !double.TryParse(args[ri + 3], System.Globalization.NumberStyles.Float, System.Globalization.CultureInfo.InvariantCulture, out scale)) scale = 0.75;
                    shell.EnsureUi();
                    var t = new DispatcherTimer { Interval = TimeSpan.FromSeconds(4) };
                    t.Tick += (s, e) => { t.Stop(); shell.Record(file, scene, scale, shell.Exit); };
                    t.Start();
                }
                app.Run();
                GC.KeepAlive(shell);
            }
        }

        const string ShowEventName = "TTLcd.Tray.Show";
        public static EventWaitHandle CreateShowEvent() { return new EventWaitHandle(false, EventResetMode.AutoReset, ShowEventName); }
        static EventWaitHandle OpenShowEvent()
        {
            EventWaitHandle e;
            return EventWaitHandle.TryOpenExisting(ShowEventName, out e) ? e : null;
        }
    }

    // Windows front end: the tray icon and (lazily) the WPF window on top of the shared StudioCore.
    class Shell
    {
        public readonly StudioCore Core;
        public Engine Engine { get { return Core.Engine; } }
        readonly WinForms.NotifyIcon icon;
        MainUi ui;

        public Shell(bool startHidden)
        {
            Core = new StudioCore(new WindowsPlatform());

            var menu = new WinForms.ContextMenuStrip();
            var modes = new WinForms.ToolStripMenuItem("Mode");
            foreach (Mode m in Enum.GetValues(typeof(Mode)))
            {
                var mm = m;
                modes.DropDownItems.Add(new WinForms.ToolStripMenuItem(Engine.Label(m), null, (s, e) => Change(c => c.Mode = mm)) { Tag = m });
            }
            var pause = new WinForms.ToolStripMenuItem("Pause", null, (s, e) => Engine.Paused = !Engine.Paused);
            var game = new WinForms.ToolStripMenuItem("Game mode", null, (s, e) => Engine.ForceGame = !Engine.ForceGame);
            menu.Items.Add(new WinForms.ToolStripMenuItem("Open TT LCD Studio", null, (s, e) => ShowWindow()) { Font = new System.Drawing.Font(menu.Font, System.Drawing.FontStyle.Bold) });
            menu.Items.Add(new WinForms.ToolStripSeparator());
            menu.Items.Add(modes);
            menu.Items.Add(pause);
            menu.Items.Add(game);
            menu.Items.Add(new WinForms.ToolStripSeparator());
            menu.Items.Add(new WinForms.ToolStripMenuItem("Exit", null, (s, e) => Exit()));
            menu.Opening += (s, e) =>
            {
                foreach (WinForms.ToolStripMenuItem i in modes.DropDownItems) i.Checked = (Mode)i.Tag == Engine.Config.Mode;
                pause.Checked = Engine.Paused;
                game.Checked = Engine.ForceGame;
            };

            icon = new WinForms.NotifyIcon { Icon = MakeIcon(), Text = "TT LCD Studio", ContextMenuStrip = menu, Visible = true };
            icon.MouseClick += (s, e) => { if (e.Button == WinForms.MouseButtons.Left) ShowWindow(); };

            var tip = new DispatcherTimer { Interval = TimeSpan.FromSeconds(1) };
            tip.Tick += (s, e) =>
            {
                string t = "TT LCD - " + Engine.Status;
                icon.Text = t.Length > 63 ? t.Substring(0, 63) : t;
            };
            tip.Start();

            // second launches signal this event to bring the window up
            var show = TrayProgram.CreateShowEvent();
            var waiter = new Thread(() => { while (show.WaitOne()) Application.Current.Dispatcher.BeginInvoke(new Action(ShowWindow)); }) { IsBackground = true };
            waiter.Start();

            if (!startHidden) ShowWindow();
        }

        public void Change(Action<Config> edit) { Core.Change(edit); }

        public void ShowWindow()
        {
            if (ui == null || !ui.IsAlive) ui = new MainUi(this);
            ui.Show();
        }

        public void EnsureUi() { if (ui == null || !ui.IsAlive) ui = new MainUi(this); }

        public void Screenshot(string file, string page)
        {
            EnsureUi();
            ui.Snapshot(file, page);
        }

        public void Record(string file, string scene, double scale, Action done)
        {
            EnsureUi();
            ui.Record(file, scene, scale, done);
        }

        public void Exit()
        {
            icon.Visible = false;
            icon.Dispose();
            Core.Shutdown();
            Application.Current.Shutdown();
        }

        // App icon (src\app.ico, embedded) at the tray's small-icon size; falls back to a drawn one.
        static System.Drawing.Icon MakeIcon()
        {
            using (var s = Assembly.GetExecutingAssembly().GetManifestResourceStream("TtLcd.app.ico"))
                if (s != null) return new System.Drawing.Icon(s, WinForms.SystemInformation.SmallIconSize);
            return DrawIcon();
        }

        static System.Drawing.Icon DrawIcon()
        {
            using (var b = new System.Drawing.Bitmap(32, 32))
            {
                using (var g = System.Drawing.Graphics.FromImage(b))
                {
                    g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
                    g.Clear(System.Drawing.Color.Transparent);
                    var pts = new[] { new System.Drawing.Point(2, 2), new System.Drawing.Point(26, 2), new System.Drawing.Point(30, 6), new System.Drawing.Point(30, 30), new System.Drawing.Point(6, 30), new System.Drawing.Point(2, 26) };
                    using (var fill = new System.Drawing.SolidBrush(System.Drawing.Color.FromArgb(19, 19, 20))) g.FillPolygon(fill, pts);
                    using (var pen = new System.Drawing.Pen(System.Drawing.Color.FromArgb(255, 43, 69), 2.5f)) g.DrawPolygon(pen, pts);
                    using (var f = new System.Drawing.Font("Segoe UI", 11, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Pixel))
                    using (var sf = new System.Drawing.StringFormat { Alignment = System.Drawing.StringAlignment.Center, LineAlignment = System.Drawing.StringAlignment.Center })
                        g.DrawString("TT", f, System.Drawing.Brushes.White, new System.Drawing.RectangleF(0, 1, 32, 32), sf);
                }
                return System.Drawing.Icon.FromHandle(b.GetHicon());
            }
        }
    }

    partial class MainUi
    {
        readonly Shell shell;
        readonly StudioModel model;
        readonly IdleEditor idle;
        readonly Window w;
        readonly DispatcherTimer previewTimer, statusTimer, videoApply, thumbTimer;
        bool loading;
        long shownSeq = -1;
        bool closed;
        double videoDuration;

        public bool IsAlive { get { return !closed; } }

        T F<T>(string name) where T : class { return (T)w.FindName(name); }

        public MainUi(Shell shell)
        {
            this.shell = shell;
            model = new StudioModel(shell.Core);
            idle = new IdleEditor(shell.Core);
            using (var s = Assembly.GetExecutingAssembly().GetManifestResourceStream("TtLcd.MainWindow.xaml"))
                w = (Window)XamlReader.Load(s);
            WrapSegGroups(w, (Style)w.FindResource("Seg"));
            using (var ico = Assembly.GetExecutingAssembly().GetManifestResourceStream("TtLcd.app.ico"))
                if (ico != null)
                {
                    // keep the data alive: WPF picks the best icon size per use (taskbar, Alt+Tab) from the decoder later
                    var mem = new MemoryStream(); ico.CopyTo(mem); mem.Position = 0;
                    w.Icon = BitmapFrame.Create(mem);
                }
            w.IsVisibleChanged += (s, e) => UpdateSuspend();
            w.Closing += (s, e) =>
            {
                if (shell.Engine.Config.CloseToTray) { e.Cancel = true; w.Hide(); }
            };
            w.Closed += (s, e) =>
            {
                closed = true;
                previewTimer.Stop(); statusTimer.Stop();
                if (!shell.Engine.Config.CloseToTray) shell.Exit();
            };
            F<Button>("MinBtn").Click += (s, e) => w.WindowState = WindowState.Minimized;
            // full screen: the window has no system frame, so maximizing covers the whole monitor (taskbar too)
            F<Button>("FullBtn").Click += (s, e) => ToggleFullScreen();
            w.PreviewKeyDown += (s, e) =>
            {
                if (e.Key == System.Windows.Input.Key.F11) { ToggleFullScreen(); e.Handled = true; }
                else if (e.Key == System.Windows.Input.Key.Escape && w.WindowState == WindowState.Maximized) { ToggleFullScreen(); e.Handled = true; }
            };
            w.StateChanged += (s, e) => UpdateFullScreenButton();
            w.Loaded += (s, e) => WireOverlayScroll(F<ScrollViewer>("PageScroll"));
            F<Button>("CloseBtn").Click += (s, e) => w.Close();
            // pause/play over the live preview: fades in while hovered (and stays while paused); click toggles
            var hover = F<FrameworkElement>("PvHover");
            hover.MouseEnter += (s, e) => UpdatePauseOverlay();
            hover.MouseLeave += (s, e) => UpdatePauseOverlay();
            hover.MouseLeftButtonUp += (s, e) => { shell.Engine.Paused = !shell.Engine.Paused; UpdateStatus(); };
            F<Button>("RotateBtn").Click += (s, e) => RotateClick();

            WireNav();
            WireModes();
            WireDevice();
            WireSettings();
            WireIdle();

            videoApply = new DispatcherTimer { Interval = TimeSpan.FromMilliseconds(500) };
            videoApply.Tick += (s, e) => { videoApply.Stop(); ApplyVideoSettings(); };
            thumbTimer = new DispatcherTimer { Interval = TimeSpan.FromMilliseconds(150) };
            thumbTimer.Tick += (s, e) => { thumbTimer.Stop(); UpdateThumb(); };

            previewTimer = new DispatcherTimer(DispatcherPriority.Render) { Interval = TimeSpan.FromMilliseconds(40) };
            previewTimer.Tick += (s, e) => UpdatePreview();
            statusTimer = new DispatcherTimer { Interval = TimeSpan.FromMilliseconds(500) };
            statusTimer.Tick += (s, e) => UpdateStatus();

            // first run: the current live rotation is taken as "upright on this mount" for the preview
            model.InitMount();
            Load(shell.Engine.Config);
            UpdateDashboard();
            previewTimer.Start(); statusTimer.Start();
        }

        public void Show()
        {
            w.Show();
            if (w.WindowState == WindowState.Minimized) w.WindowState = WindowState.Normal;
            w.Activate();
        }

        void Change(Action<Config> edit) { if (!loading) shell.Change(edit); }

        // Every horizontal row of Seg radio buttons becomes one segmented toggle: the row is wrapped in a rounded
        // track (keeping its grid/dock position), the options inside are pills and the selected one is red.
        void WrapSegGroups(DependencyObject node, Style seg)
        {
            foreach (var child in LogicalTreeHelper.GetChildren(node).OfType<DependencyObject>().ToList()) WrapSegGroups(child, seg);
            var sp = node as StackPanel;
            if (sp == null || sp.Orientation != Orientation.Horizontal || sp.Children.Count < 2) return;
            if (sp.Children.Cast<UIElement>().Any(c => !(c is RadioButton) || ((RadioButton)c).Style != seg)) return;
            var parent = LogicalTreeHelper.GetParent(sp) as Panel;
            if (parent == null) return;
            int idx = parent.Children.IndexOf(sp);
            parent.Children.RemoveAt(idx);
            var track = new Border
            {
                CornerRadius = new CornerRadius(16), Padding = new Thickness(2), BorderThickness = new Thickness(1),
                Background = (Brush)w.FindResource("CardHi"), BorderBrush = (Brush)w.FindResource("Line"),
                HorizontalAlignment = HorizontalAlignment.Left, VerticalAlignment = sp.VerticalAlignment, Margin = sp.Margin,
            };
            Grid.SetRow(track, Grid.GetRow(sp)); Grid.SetColumn(track, Grid.GetColumn(sp));
            Grid.SetRowSpan(track, Grid.GetRowSpan(sp)); Grid.SetColumnSpan(track, Grid.GetColumnSpan(sp));
            DockPanel.SetDock(track, DockPanel.GetDock(sp));
            sp.Margin = new Thickness(0);
            // one red pill behind the options, sliding to the selected one (same 0.2 s ease-out as the switches)
            var pill = new Border
            {
                CornerRadius = new CornerRadius(13), Background = (Brush)w.FindResource("Accent"), HorizontalAlignment = HorizontalAlignment.Left,
                Effect = (System.Windows.Media.Effects.Effect)w.FindResource("GlowSoft"), Opacity = 0, RenderTransform = new TranslateTransform(),
            };
            var grid = new Grid();
            grid.Children.Add(pill); grid.Children.Add(sp);
            track.Child = grid;
            parent.Children.Insert(idx, track);

            Action<bool> move = animate =>
            {
                var rb = sp.Children.OfType<RadioButton>().FirstOrDefault(r => r.IsChecked == true);
                if (rb == null || rb.ActualWidth <= 0) { pill.Opacity = 0; return; }
                double x = rb.TranslatePoint(new Point(0, 0), sp).X, width = rb.ActualWidth;
                var tt = (TranslateTransform)pill.RenderTransform;
                if (!animate || pill.Opacity == 0)
                {
                    tt.BeginAnimation(TranslateTransform.XProperty, null); tt.X = x;
                    pill.BeginAnimation(FrameworkElement.WidthProperty, null); pill.Width = width;
                    pill.Opacity = 1;
                    return;
                }
                var ease = new System.Windows.Media.Animation.CubicEase { EasingMode = System.Windows.Media.Animation.EasingMode.EaseOut };
                var dur = TimeSpan.FromMilliseconds(200);
                tt.BeginAnimation(TranslateTransform.XProperty, new System.Windows.Media.Animation.DoubleAnimation(x, dur) { EasingFunction = ease });
                pill.BeginAnimation(FrameworkElement.WidthProperty, new System.Windows.Media.Animation.DoubleAnimation(width, dur) { EasingFunction = ease });
            };
            foreach (var rb in sp.Children.OfType<RadioButton>()) rb.Checked += (s, e) => move(w.IsVisible && sp.IsVisible);
            sp.SizeChanged += (s, e) => move(false);   // first layout, page shown, text changes
        }

        double pauseShown = -1;
        void UpdatePauseOverlay()
        {
            bool paused = shell.Engine.Paused;
            F<TextBlock>("PauseIcon").Text = paused ? "" : ""; // play : pause
            F<FrameworkElement>("PvHover").ToolTip = paused ? "Resume" : "Pause";
            double target = F<FrameworkElement>("PvHover").IsMouseOver || paused ? 1 : 0;
            if (target == pauseShown) return;
            pauseShown = target;
            var btn = F<FrameworkElement>("PauseBtn");
            if (!w.IsVisible) { btn.BeginAnimation(UIElement.OpacityProperty, null); btn.Opacity = target; return; } // no fade off-screen
            btn.BeginAnimation(UIElement.OpacityProperty,
                new System.Windows.Media.Animation.DoubleAnimation(target, TimeSpan.FromMilliseconds(target > 0 ? 150 : 300)));
        }

        // Overlay scrollbar: invisible until you scroll or hover it; fades out 1.75 s after scrolling stops or the
        // mouse leaves (stays while hovered or dragged).
        void WireOverlayScroll(ScrollViewer sv)
        {
            sv.ApplyTemplate();
            var sb = sv.Template.FindName("PART_VerticalScrollBar", sv) as ScrollBar;
            if (sb == null) return;
            Action show = () => sb.BeginAnimation(UIElement.OpacityProperty,
                new System.Windows.Media.Animation.DoubleAnimation(1, TimeSpan.FromMilliseconds(120)));
            Action hideLater = () =>
            {
                if (sb.IsMouseOver || sb.IsMouseCaptureWithin) return;
                var a = new System.Windows.Media.Animation.DoubleAnimationUsingKeyFrames();
                a.KeyFrames.Add(new System.Windows.Media.Animation.DiscreteDoubleKeyFrame(1, TimeSpan.Zero));
                a.KeyFrames.Add(new System.Windows.Media.Animation.LinearDoubleKeyFrame(1, TimeSpan.FromSeconds(1.75)));
                a.KeyFrames.Add(new System.Windows.Media.Animation.LinearDoubleKeyFrame(0, TimeSpan.FromSeconds(2.15)));
                sb.BeginAnimation(UIElement.OpacityProperty, a);
            };
            sb.BeginAnimation(UIElement.OpacityProperty, new System.Windows.Media.Animation.DoubleAnimation(0, TimeSpan.Zero));
            sv.ScrollChanged += (s, e) => { if (e.VerticalChange != 0) { show(); hideLater(); } };
            sb.MouseEnter += (s, e) => show();
            sb.MouseLeave += (s, e) => hideLater();
            sb.LostMouseCapture += (s, e) => hideLater();
        }

        void ToggleFullScreen()
        {
            w.WindowState = w.WindowState == WindowState.Maximized ? WindowState.Normal : WindowState.Maximized;
        }

        [System.Runtime.InteropServices.DllImport("user32.dll")] static extern bool GetWindowRect(IntPtr h, out RECT r);
        [System.Runtime.InteropServices.DllImport("user32.dll")] static extern IntPtr MonitorFromWindow(IntPtr h, uint flags);
        [System.Runtime.InteropServices.DllImport("user32.dll")] static extern bool GetMonitorInfo(IntPtr m, ref MONITORINFO mi);
        struct RECT { public int L, T, R, B; }
        struct MONITORINFO { public int Size; public RECT Monitor, Work; public uint Flags; }

        // A maximized frameless window overhangs the monitor by its (invisible) resize frame; pad the content by
        // exactly that much so nothing is cut off.
        Thickness MaximizedOverhang()
        {
            var hwnd = new System.Windows.Interop.WindowInteropHelper(w).Handle;
            RECT r; var mi = new MONITORINFO { Size = System.Runtime.InteropServices.Marshal.SizeOf(typeof(MONITORINFO)) };
            if (hwnd == IntPtr.Zero || !GetWindowRect(hwnd, out r) || !GetMonitorInfo(MonitorFromWindow(hwnd, 2), ref mi)) return new Thickness(7);
            var src = PresentationSource.FromVisual(w);
            double k = src != null && src.CompositionTarget != null ? src.CompositionTarget.TransformFromDevice.M11 : 1;
            return new Thickness(Math.Max(0, mi.Monitor.L - r.L) * k, Math.Max(0, mi.Monitor.T - r.T) * k,
                                 Math.Max(0, r.R - mi.Monitor.R) * k, Math.Max(0, r.B - mi.Monitor.B) * k);
        }

        void UpdateFullScreenButton()
        {
            bool full = w.WindowState == WindowState.Maximized;
            ((FrameworkElement)w.Content).Margin = full ? MaximizedOverhang() : new Thickness(0);
            var b = F<Button>("FullBtn");
            b.Content = full ? "" : "";
            b.ToolTip = full ? "Exit full screen (F11 / Esc)" : "Full screen (F11)";
        }

        // Renders the window off-screen to a PNG (used to check the layout without a visible desktop).
        // page "modes:Clock" shows that mode's options without saving the choice; TTLCD_SHOT_HEIGHT renders taller.
        public void Snapshot(string file, string page)
        {
            var orig = shell.Engine.Config;
            string mode = page.Contains(":") ? page.Substring(page.IndexOf(':') + 1) : null;
            if (mode != null) { page = page.Substring(0, page.IndexOf(':')); F<RadioButton>("Mode" + mode).IsChecked = true; }
            if (Environment.GetEnvironmentVariable("TTLCD_SHOT_PAUSED") == "1") { shell.Engine.Paused = true; UpdatePauseOverlay(); }
            var pick = Environment.GetEnvironmentVariable("TTLCD_SHOT_SELECT"); // dev aid: "ClkItems:2" selects water on the clock list
            if (!string.IsNullOrEmpty(pick) && pick.Contains(":"))
            {
                selList = itemLists.FirstOrDefault(x => x.Panel == pick.Split(':')[0]);
                selId = int.Parse(pick.Split(':')[1]);
                RebuildItems(); ShowStyleEditor();
            }
            if (page == "idle")
            {
                idle.TriedRead = true; F<CheckBox>("PvIdle").IsChecked = true; page = "dashboard";
                var img = Environment.GetEnvironmentVariable("TTLCD_SHOT_IDLE"); // dev aid: show a picture being placed
                if (!string.IsNullOrEmpty(img)) { var cfg0 = shell.Engine.Config; LoadIdle(img); F<Slider>("IdleZoom").Value = 1.6; idle.Pan(60, 0); EncodeIdle(); shell.Engine.Config = cfg0; orig = cfg0; }
            }
            string nav = page == "device" ? "NavDevice" : page == "settings" ? "NavSettings" : page == "dashboard" ? "NavDashboard" : "NavModes";
            F<RadioButton>(nav).IsChecked = true;
            UpdatePreview(true); UpdateStatus(true); UpdateDashboard();
            var root = (FrameworkElement)w.Content;
            int width = (int)w.Width, height = (int)w.Height, tall;
            if (int.TryParse(Environment.GetEnvironmentVariable("TTLCD_SHOT_HEIGHT"), out tall)) height = tall;
            shell.Engine.Config = orig;
            root.Measure(new Size(width, height));
            root.Arrange(new Rect(0, 0, width, height));
            root.UpdateLayout();
            var dv = new DrawingVisual();
            using (var dc = dv.RenderOpen())
            {
                dc.DrawRectangle(w.Background, null, new Rect(0, 0, width, height));
                dc.DrawRectangle(new VisualBrush(root), null, new Rect(0, 0, width, height));
            }
            var rtb = new RenderTargetBitmap(width, height, 96, 96, PixelFormats.Pbgra32);
            rtb.Render(dv);
            var enc = new PngBitmapEncoder();
            enc.Frames.Add(BitmapFrame.Create(rtb));
            using (var f = File.Create(file)) enc.Save(f);
        }

        // ---------------- navigation ----------------
        DispatcherTimer scrollAnim;

        // Smoothly scrolls the page so the element sits in the middle of the viewport (as far as the page allows).
        void ScrollToCentre(FrameworkElement el, Action done)
        {
            var sv = F<ScrollViewer>("PageScroll");
            sv.UpdateLayout();
            double y = el.TransformToVisual(sv).Transform(new Point(0, 0)).Y;
            double from = sv.VerticalOffset;
            double to = Math.Max(0, Math.Min(sv.ScrollableHeight, from + y + el.ActualHeight / 2 - sv.ViewportHeight / 2));
            if (scrollAnim != null) scrollAnim.Stop();
            if (Math.Abs(to - from) < 1) { done(); return; }
            var start = DateTime.UtcNow;
            scrollAnim = new DispatcherTimer(DispatcherPriority.Render) { Interval = TimeSpan.FromMilliseconds(15) };
            scrollAnim.Tick += (s, e) =>
            {
                double t = Math.Min(1, (DateTime.UtcNow - start).TotalMilliseconds / 350);
                double k = 1 - Math.Pow(1 - t, 3); // cubic ease-out
                sv.ScrollToVerticalOffset(from + (to - from) * k);
                if (t >= 1) { scrollAnim.Stop(); done(); }
            };
            scrollAnim.Start();
        }

        // Blinks a CardBox's accent outline three times.
        void FlashCard(ContentControl card)
        {
            card.ApplyTemplate();
            var flash = card.Template.FindName("flash", card) as UIElement;
            if (flash == null) return;
            var a = new System.Windows.Media.Animation.DoubleAnimation(0, 1, TimeSpan.FromMilliseconds(400))
            {
                AutoReverse = true,
                RepeatBehavior = new System.Windows.Media.Animation.RepeatBehavior(3),
                EasingFunction = new System.Windows.Media.Animation.SineEase()
            };
            flash.BeginAnimation(UIElement.OpacityProperty, a);
        }

        void WireNav()
        {
            var pages = new[] { "PageModes", "PageDashboard", "PageDevice", "PageSettings" };
            var navs = new[] { "NavModes", "NavDashboard", "NavDevice", "NavSettings" };
            for (int i = 0; i < navs.Length; i++)
            {
                string page = pages[i];
                F<RadioButton>(navs[i]).Checked += (s, e) =>
                {
                    foreach (var p in pages) F<FrameworkElement>(p).Visibility = p == page ? Visibility.Visible : Visibility.Collapsed;
                    dashOpen = page == "PageDashboard";
                    if (dashOpen) UpdateDashboard();
                    if (page == "PageDevice") UpdateVd();
                    if (page == "PageSettings") { UpdateCacheInfo(); UpdateService(); }
                };
            }
            F<Button>("VdGoDevice").Click += (s, e) => F<RadioButton>("NavDevice").IsChecked = true;

            F<UniformGrid>("DbTiles").SizeChanged += (s, e) => { if (e.WidthChanged) LayoutDashboard(); };

            // collapsible navigation: full width with names, or an icon rail (names as tooltips)
            foreach (var n in navs) { var rb = F<RadioButton>(n); rb.ToolTip = rb.Content; }
            F<Button>("NavToggle").Click += (s, e) => { Change(c => c.NavCollapsed = !c.NavCollapsed); ApplyNav(true); };
            ApplyNav();
            dashTimer = new DispatcherTimer { Interval = TimeSpan.FromMilliseconds(1000) };
            dashTimer.Tick += (s, e) => { if (dashOpen && w.IsVisible) UpdateDashboard(); };
            dashTimer.Start();
            // the CPU temperature service is managed in Settings → Sensors; the dashboard hint jumps there
            F<System.Windows.Documents.Hyperlink>("CpuTempLink").Click += (s, e) =>
            {
                F<RadioButton>("NavSettings").IsChecked = true;
                w.Dispatcher.BeginInvoke(new Action(() =>
                {
                    var card = F<ContentControl>("SensorsCard");
                    ScrollToCentre(card, () => FlashCard(card));
                }), DispatcherPriority.Loaded);
            };
            foreach (var pair in new[] {
                new { N = "DbShowWater", A = (Action<Config, bool>)((c, v) => c.DashWater = v) },
                new { N = "DbShowCpuLoad", A = (Action<Config, bool>)((c, v) => c.DashCpuLoad = v) },
                new { N = "DbShowCpuTemp", A = (Action<Config, bool>)((c, v) => c.DashCpuTemp = v) },
                new { N = "DbShowRam", A = (Action<Config, bool>)((c, v) => c.DashRam = v) },
            })
            {
                var cb = F<CheckBox>(pair.N); var act = pair.A;
                cb.Click += (s, e) => { if (loading) return; Change(c => act(c, cb.IsChecked == true)); UpdateDashboard(); };
            }
        }

        // Sidebar: full width with labels, or an icon rail. Animated (width slides, labels fade) when the user
        // toggles it; applied instantly at start-up.
        // Gauge grid: as many columns as fit (~200 px each), then balanced so every row is full, e.g. 4 gauges ->
        // 2x2 or 4x1, never 3+1. Only the gauges that are switched on count.
        void LayoutDashboard()
        {
            var grid = F<UniformGrid>("DbTiles");
            int n = grid.Children.Cast<UIElement>().Count(c => c.Visibility != Visibility.Collapsed);
            if (n == 0) return;
            int cols = StudioModel.GaugeColumns(n, grid.ActualWidth);
            if (grid.Columns != cols) grid.Columns = cols;
        }

        void ApplyNav() { ApplyNav(false); }
        void ApplyNav(bool animate)
        {
            bool col = shell.Engine.Config.NavCollapsed;
            var panel = F<FrameworkElement>("NavPanel");
            var labels = new System.Collections.Generic.List<FrameworkElement> { F<FrameworkElement>("NavFooter") };
            foreach (var n in new[] { "NavDashboard", "NavModes", "NavDevice", "NavSettings" })
            {
                var rb = F<RadioButton>(n);
                rb.ApplyTemplate();
                var tx = rb.Template.FindName("tx", rb) as FrameworkElement;
                if (tx != null) labels.Add(tx);
            }
            F<Button>("NavToggle").ToolTip = col ? "Expand menu" : "Collapse menu";
            double width = col ? 66 : 214;
            if (!animate)
            {
                panel.BeginAnimation(FrameworkElement.WidthProperty, null);
                panel.Width = width;
                foreach (var l in labels) { l.BeginAnimation(UIElement.OpacityProperty, null); l.Opacity = 1; l.Visibility = col ? Visibility.Collapsed : Visibility.Visible; }
                return;
            }
            var ease = new System.Windows.Media.Animation.CubicEase { EasingMode = System.Windows.Media.Animation.EasingMode.EaseInOut };
            var slide = new System.Windows.Media.Animation.DoubleAnimation(width, TimeSpan.FromMilliseconds(260)) { EasingFunction = ease };
            if (col)
            {
                // fade the labels out quickly, then hide them once the rail is narrow
                foreach (var l in labels)
                    l.BeginAnimation(UIElement.OpacityProperty, new System.Windows.Media.Animation.DoubleAnimation(0, TimeSpan.FromMilliseconds(140)));
                slide.Completed += (s, e) => { if (shell.Engine.Config.NavCollapsed) foreach (var l in labels) l.Visibility = Visibility.Collapsed; };
            }
            else
            {
                // show the labels again and fade them in while the panel widens
                foreach (var l in labels)
                {
                    l.Visibility = Visibility.Visible;
                    l.BeginAnimation(UIElement.OpacityProperty, new System.Windows.Media.Animation.DoubleAnimation(0, 1, TimeSpan.FromMilliseconds(300)) { EasingFunction = ease });
                }
            }
            panel.BeginAnimation(FrameworkElement.WidthProperty, slide);
        }

        // Round 270° gauges: track + value arc (open at the bottom, where the label sits), value text in the middle.
        const double GaugeSize = 156, GaugeThick = 11;
        void SetGauge(string name, string text, double frac)
        {
            F<TextBlock>(name).Text = text;
            var track = F<System.Windows.Shapes.Path>(name + "Track");
            if (track.Data == null) track.Data = GaugeArc(1);
            F<System.Windows.Shapes.Path>(name + "Arc").Data = GaugeArc(Math.Max(0, Math.Min(1, frac)));
        }

        static Geometry GaugeArc(double frac)
        {
            if (frac < 0.004) return Geometry.Empty;
            double c = GaugeSize / 2, r = c - GaugeThick / 2 - 6;   // room for the glow
            double a0 = 135 * Math.PI / 180, a1 = (135 + 270 * Math.Min(frac, 0.9999)) * Math.PI / 180;
            var g = new StreamGeometry();
            using (var ctx = g.Open())
            {
                ctx.BeginFigure(new Point(c + r * Math.Cos(a0), c + r * Math.Sin(a0)), false, false);
                ctx.ArcTo(new Point(c + r * Math.Cos(a1), c + r * Math.Sin(a1)), new Size(r, r), 0, 270 * frac > 180, SweepDirection.Clockwise, true, false);
            }
            g.Freeze();
            return g;
        }

        DispatcherTimer dashTimer;
        bool dashOpen = true;   // the dashboard is the start page
        void UpdateDashboard()
        {
            var cfg = shell.Engine.Config;
            F<FrameworkElement>("DbTileWater").Visibility = cfg.DashWater ? Visibility.Visible : Visibility.Collapsed;
            F<FrameworkElement>("DbTileCpuLoad").Visibility = cfg.DashCpuLoad ? Visibility.Visible : Visibility.Collapsed;
            F<FrameworkElement>("DbTileCpuTemp").Visibility = cfg.DashCpuTemp ? Visibility.Visible : Visibility.Collapsed;
            F<FrameworkElement>("DbTileRam").Visibility = cfg.DashRam ? Visibility.Visible : Visibility.Collapsed;
            LayoutDashboard();
            var d = model.Readings();
            SetGauge("DbWater", d.Coolant.Text, d.Coolant.Frac);
            SetGauge("DbCpuTemp", d.CpuTemp.Text, d.CpuTemp.Frac);
            SetGauge("DbCpuLoad", d.CpuLoad.Text, d.CpuLoad.Frac);
            SetGauge("DbRam", d.Ram.Text, d.Ram.Frac);
            F<FrameworkElement>("CpuTempRow").Visibility = d.ShowCpuTempHint ? Visibility.Visible : Visibility.Collapsed;
            F<TextBlock>("DbNote").Text = d.Note;
        }

        // ---------------- modes page ----------------

        void WireModes()
        {
            foreach (Mode m in Enum.GetValues(typeof(Mode)))
            {
                var mm = m;
                F<RadioButton>("Mode" + m).Checked += (s, e) => { Change(c => c.Mode = mm); ShowOptions(mm); };
            }

            var mon = F<ComboBox>("MonitorBox");
            mon.SelectionChanged += (s, e) => { int i = mon.SelectedIndex; if (i >= 0) Change(c => c.MonitorIndex = i); };
            var reg = F<CheckBox>("RegionSwitch");
            reg.Checked += (s, e) => { Change(c => c.UseRegion = true); F<FrameworkElement>("RegionRow").IsEnabled = true; };
            reg.Unchecked += (s, e) => { Change(c => c.UseRegion = false); F<FrameworkElement>("RegionRow").IsEnabled = false; };
            foreach (var n in new[] { "RegX", "RegY", "RegW", "RegH" })
                F<TextBox>(n).LostFocus += (s, e) => ApplyRegion();

            F<Button>("ImageBrowse").Click += (s, e) => Browse("Images|*.jpg;*.jpeg;*.png;*.bmp", "ImagePath", p => Change(c => c.ImagePath = p));
            F<Button>("GifBrowse").Click += (s, e) => Browse("GIF animations|*.gif", "GifPath", p => Change(c => c.GifPath = p));
            F<Button>("VideoBrowse").Click += (s, e) => Browse("Videos|*.mp4;*.mov;*.avi;*.wmv;*.mkv;*.webm;*.m4v|All files|*.*", "VideoPath", p =>
            {
                Change(c => { c.VideoPath = p; c.VideoStart = 0; c.VideoZoom = 1; c.VideoPanX = 0; c.VideoPanY = 0; });
                loading = true;
                F<Slider>("VStart").Value = 0; F<Slider>("VZoom").Value = 1; F<Slider>("VPanX").Value = 0; F<Slider>("VPanY").Value = 0;
                loading = false;
                LoadVideoInfo(p);
            });

            foreach (var n in new[] { "VStart", "VLength", "VZoom", "VPanX", "VPanY", "VFps" })
                F<Slider>(n).ValueChanged += (s, e) =>
                {
                    UpdateVideoLabels();
                    if (loading) return;
                    videoApply.Stop(); videoApply.Start();
                    thumbTimer.Stop(); thumbTimer.Start();
                };

            F<RadioButton>("ScaleFill").Checked += (s, e) => Change(c => c.FitContain = false);
            F<RadioButton>("ScaleFit").Checked += (s, e) => Change(c => c.FitContain = true);
            BindRadio("OvTop", c => c.OverlayPos == 1, c => c.OverlayPos = 1);
            BindRadio("OvCentre", c => c.OverlayPos == 2, c => c.OverlayPos = 2);
            BindRadio("OvBottom", c => c.OverlayPos == 0, c => c.OverlayPos = 0);

            // reading lists, arranged into lines by drag and drop
            foreach (var rl in ReadingList.All) itemLists.Add(new ItemList { List = rl });
            loaders.Add(c => RebuildItems());
            // alignment of the whole block (clock face and overlay share it)
            foreach (var pre in new[] { "", "Ov" })
            {
                BindRadio(pre + "AlignL", c => c.TextAlign == 0, c => c.TextAlign = 0);
                BindRadio(pre + "AlignC", c => c.TextAlign == 1, c => c.TextAlign = 1);
                BindRadio(pre + "AlignR", c => c.TextAlign == 2, c => c.TextAlign = 2);
            }
            WireStyleEditor();
            // mode tiles: as many columns as fit, balanced so rows are evenly filled
            var tiles = F<UniformGrid>("ModeTiles");
            tiles.SizeChanged += (s, e) =>
            {
                int n = tiles.Children.Count;
                int cols = Math.Max(3, Math.Min(n, (int)(e.NewSize.Width / 128)));
                int rows = (n + cols - 1) / cols;
                tiles.Columns = (n + rows - 1) / rows;
            };

            FillSwatches(F<WrapPanel>("BgSwatches"), v => Change(c => c.ColorRgb = v), () => shell.Engine.Config.ColorRgb);

            // Clock-mode elements
            BindSlider("ClkOffset", "ClkOffsetT", " px", c => c.TextOffsetY, (c, v) => c.TextOffsetY = v);
            BindSlider("ClkSpacing", "ClkSpacingT", " px", c => c.LineSpacing, (c, v) => c.LineSpacing = v);
            // Sleep (Device page). Sleep applies to the cooler at once and is saved immediately.
            F<CheckBox>("SleepSwitch").Click += (s, e) => { if (loading) return; bool v = F<CheckBox>("SleepSwitch").IsChecked == true; Change(c => c.DeviceSleep = v); try { shell.Engine.Config.Save(); } catch (Exception) { } };
        }

        // Wire a Switch/CheckBox to a bool config field (ignores programmatic changes during Load).
        void Toggle(string name, Action<Config, bool> set)
        {
            var cb = F<CheckBox>(name);
            cb.Click += (s, e) => { if (!loading) Change(c => set(c, cb.IsChecked == true)); };
        }

        // A reading list (clock face or overlay) shown as the screen's lines: each row holds the chips of one line.
        // Click a chip to turn it on or off; drag it onto a line to place it beside the others there, or between /
        // above / below the lines to give it a line of its own. Empty lines disappear. Saved as "0,2|3|".
        // The WPF side of a ReadingList: its rows of chips and the mouse-drag state (the line logic is in LineDrag).
        class ItemList
        {
            public ReadingList List;
            public string Panel { get { return List.Panel; } }
            public StackPanel Rows; public Canvas Overlay;
            public FrameworkElement DragChip; public int DragId; public string DragName; public Point DragStart; public bool Dragging;
            public Border Ghost;
            public LineDrag Drag;
        }
        readonly System.Collections.Generic.List<ItemList> itemLists = new System.Collections.Generic.List<ItemList>();

        void RebuildItems() { foreach (var l in itemLists) BuildItems(l); }

        // Selected reading (its style is edited in the Text style panel); null = none selected.
        ItemList selList; int selId;
        bool IsOverlayList(ItemList l) { return l.List.Overlay; }

        void BuildItems(ItemList l)
        {
            var cfg = shell.Engine.Config;
            var host = F<Grid>(l.Panel);
            if (l.Rows == null) WireLineEditor(l, host);
            l.Rows.Children.Clear();
            var dim = (Brush)w.FindResource("Dim");
            var accent = (Brush)w.FindResource("Accent");
            var lines = l.Dragging && l.Drag != null ? l.Drag.Display : l.List.GetLines(cfg);
            for (int i = 0; i < lines.Count; i++)
            {
                var row = new DockPanel { MinHeight = 36, Tag = i, Background = Brushes.Transparent }; // compact line height
                var num = new TextBlock { Text = (i + 1).ToString(), Width = 22, Foreground = dim, FontSize = 11, VerticalAlignment = VerticalAlignment.Center };
                DockPanel.SetDock(num, Dock.Left);
                row.Children.Add(num);
                var chips = new WrapPanel { VerticalAlignment = VerticalAlignment.Center };
                row.Children.Add(chips);
                foreach (int id in lines[i])
                {
                    var chip = MakeChip(l, l.List.Def(id), cfg, dim, accent);
                    if (l.Dragging && id == l.DragId) { chip.Opacity = 0.4; chip.IsHitTestVisible = false; } // drop preview
                    chips.Children.Add(chip);
                }
                if (lines[i].Count == 0)
                {
                    var b = MakeBlankLine(l, i, dim);
                    if (l.Dragging && l.Drag != null && l.DragId == EmptyLineId && i == l.Drag.NewRowIdx) b.Opacity = 0.4;
                    chips.Children.Add(b);
                }
                l.Rows.Children.Add(row);
            }
            // palette: drag "Empty line" between two lines to leave a blank row there
            var pal = new DockPanel { MinHeight = 36, Tag = "palette" };
            pal.Children.Add(new TextBlock { Width = 22 });
            var blank = new Border
            {
                Tag = EmptyLineId, CornerRadius = new CornerRadius(15), Padding = new Thickness(12, 5, 14, 5), Margin = new Thickness(0, 3, 8, 3), VerticalAlignment = VerticalAlignment.Center,
                BorderThickness = new Thickness(1), BorderBrush = dim, Background = Brushes.Transparent, HorizontalAlignment = HorizontalAlignment.Left,
                Cursor = System.Windows.Input.Cursors.SizeAll, ToolTip = "Drag between two lines to leave an empty line there",
                Child = new TextBlock { Text = "+  Empty line", Foreground = dim, FontSize = 12 },
            };
            blank.PreviewMouseLeftButtonDown += (s, e) => { l.DragChip = blank; l.DragId = EmptyLineId; l.DragName = "Empty line"; l.DragStart = e.GetPosition(F<Grid>(l.Panel)); };
            pal.Children.Add(blank);
            l.Rows.Children.Add(pal);
        }

        const int EmptyLineId = ReadingList.EmptyLineId;

        // Placeholder shown in a blank line; × removes the line.
        FrameworkElement MakeBlankLine(ItemList l, int index, Brush dim)
        {
            var x = new TextBlock { Text = "", FontFamily = (FontFamily)w.FindResource("Icons"), FontSize = 10, Foreground = dim, Margin = new Thickness(10, 0, 0, 0),
                                    VerticalAlignment = VerticalAlignment.Center, Cursor = System.Windows.Input.Cursors.Hand, ToolTip = "Remove this empty line" };
            var sp = new StackPanel { Orientation = Orientation.Horizontal };
            sp.Children.Add(new TextBlock { Text = "empty line", Foreground = dim, FontSize = 12, FontStyle = FontStyles.Italic });
            sp.Children.Add(x);
            x.MouseLeftButtonUp += (s, e) =>
            {
                e.Handled = true;
                l.List.RemoveBlankLine(shell.Core, index);
                BuildItems(l);
            };
            return new Border
            {
                Child = sp, CornerRadius = new CornerRadius(15), Padding = new Thickness(12, 5, 10, 5), Margin = new Thickness(0, 3, 8, 3),
                BorderThickness = new Thickness(1), BorderBrush = (Brush)w.FindResource("Line"), Background = Brushes.Transparent,
            };
        }

        // Rounded pill: dot = shown on the screen (click it to turn the reading on/off), label = click to select and
        // style it, press and drag anywhere on it to move it. The selected pill has a glowing red outline.
        FrameworkElement MakeChip(ItemList l, ItemDef def, Config cfg, Brush dim, Brush accent)
        {
            bool on = def.Get(cfg), sel = selList == l && selId == def.Id;
            var dot = new System.Windows.Shapes.Ellipse { Width = 10, Height = 10, StrokeThickness = 1.5, Stroke = on ? accent : dim, Fill = on ? accent : Brushes.Transparent };
            var dotHit = new Border { Background = Brushes.Transparent, Padding = new Thickness(4), Margin = new Thickness(0, 0, 4, 0), Child = dot, Cursor = System.Windows.Input.Cursors.Hand,
                                      ToolTip = on ? "Shown on the screen: click to hide" : "Hidden: click to show" };
            var text = new TextBlock { Text = def.Name, VerticalAlignment = VerticalAlignment.Center, Foreground = on ? (Brush)w.FindResource("Text") : dim };
            var content = new StackPanel { Orientation = Orientation.Horizontal };
            content.Children.Add(dotHit); content.Children.Add(text);
            var pill = new Border
            {
                Tag = def.Id, Child = content, CornerRadius = new CornerRadius(15), Padding = new Thickness(6, 4, 14, 4), Margin = new Thickness(0, 3, 8, 3),
                BorderThickness = new Thickness(sel ? 2 : 1), BorderBrush = sel ? accent : (Brush)w.FindResource("Line"),
                Background = sel ? new SolidColorBrush(Color.FromArgb(0x2E, 0xFF, 0x1F, 0x4B)) : (Brush)w.FindResource("CardHi"),
                Effect = sel ? (System.Windows.Media.Effects.Effect)w.FindResource("Glow") : null,
                Cursor = System.Windows.Input.Cursors.Hand, ToolTip = "Click to style it, drag to move it",
            };
            if (!sel)
            {
                pill.MouseEnter += (s, e) => pill.BorderBrush = (Brush)w.FindResource("LineHi");
                pill.MouseLeave += (s, e) => pill.BorderBrush = (Brush)w.FindResource("Line");
            }
            dotHit.MouseLeftButtonUp += (s, e) =>
            {
                if (l.Dragging) return;
                e.Handled = true;
                l.List.Toggle(shell.Core, def.Id);
                SetPanels(); BuildItems(l);
            };
            pill.PreviewMouseLeftButtonDown += (s, e) => { l.DragChip = pill; l.DragId = def.Id; l.DragName = def.Name; l.DragStart = e.GetPosition(F<Grid>(l.Panel)); };
            pill.MouseLeftButtonUp += (s, e) =>
            {
                if (l.Dragging) return;
                var prev = selList;
                selList = l; selId = def.Id;
                if (prev != null && prev != l) BuildItems(prev);
                BuildItems(l);
                ShowStyleEditor();
            };
            return pill;
        }

        void ClearSelection()
        {
            if (selList == null) return;
            var l = selList; selList = null;
            BuildItems(l);
            SetPanels();
        }

        bool SelectedVisible()
        {
            if (selList == null) return false;
            return selList.List.ShownIn(shell.Engine.Config.Mode);
        }

        // ---- Text style panel: edits the selected reading's style ----
        bool styleLoading;

        ElementStyle CurStyle() { return selList.List.Style(shell.Engine.Config, selId); }

        void EditStyle(Action<ElementStyle> edit)
        {
            if (styleLoading || selList == null || loading) return;
            selList.List.EditStyle(shell.Core, selId, edit);
        }

        void WireStyleEditor()
        {
            var fontBox = F<ComboBox>("TxtFont");
            foreach (var fam in System.Drawing.FontFamily.Families)
                fontBox.Items.Add(new ComboBoxItem { Content = fam.Name, FontFamily = new FontFamily(fam.Name) });
            fontBox.SelectionChanged += (s, e) => { var it = fontBox.SelectedItem as ComboBoxItem; if (it != null) { string n = (string)it.Content; EditStyle(st => st.Font = n); } };
            F<CheckBox>("TxtBold").Click += (s, e) => { bool v = F<CheckBox>("TxtBold").IsChecked == true; EditStyle(st => st.Bold = v); };
            F<CheckBox>("TxtItalic").Click += (s, e) => { bool v = F<CheckBox>("TxtItalic").IsChecked == true; EditStyle(st => st.Italic = v); };
            var size = F<Slider>("StSize");
            size.ValueChanged += (s, e) => { int v = (int)Math.Round(size.Value); F<TextBlock>("StSizeT").Text = v + " px"; EditStyle(st => st.Size = v); };
            foreach (TextEffect fx in Enum.GetValues(typeof(TextEffect)))
            {
                var ff = fx;
                F<RadioButton>("Fx" + fx).Checked += (s, e) => EditStyle(st => st.Effect = ff);
            }
            FillSwatches(F<WrapPanel>("TextSwatches"), v => EditStyle(st => st.Rgb = v), () => selList == null ? 0xFFFFFF : CurStyle().Rgb);
            F<RadioButton>("UnitC").Checked += (s, e) => EditStyle(st => st.Fahrenheit = false);
            F<RadioButton>("UnitF").Checked += (s, e) => EditStyle(st => st.Fahrenheit = true);
            F<RadioButton>("LblNone").Checked += (s, e) => EditStyle(st => st.Label = false);
            F<RadioButton>("LblBeside").Checked += (s, e) => EditStyle(st => { st.Label = true; st.LabelAbove = false; });
            F<RadioButton>("LblAbove").Checked += (s, e) => EditStyle(st => { st.Label = true; st.LabelAbove = true; });
            F<CheckBox>("TxtByTemp").Click += (s, e) => { bool v = F<CheckBox>("TxtByTemp").IsChecked == true; EditStyle(st => st.ByTemp = v); };
        }

        // Fill the panel from the selected reading and show it.
        void ShowStyleEditor()
        {
            if (selList == null) { SetPanels(); return; }
            var st = CurStyle();
            var def = selList.List.Def(selId);
            styleLoading = true;
            F<TextBlock>("StTitle").Text = "TEXT STYLE · " + def.Name.ToUpperInvariant() + (IsOverlayList(selList) ? " (OVERLAY)" : "");
            var fontBox = F<ComboBox>("TxtFont");
            fontBox.SelectedItem = fontBox.Items.Cast<ComboBoxItem>().FirstOrDefault(i => (string)i.Content == st.Font);
            F<CheckBox>("TxtBold").IsChecked = st.Bold;
            F<CheckBox>("TxtItalic").IsChecked = st.Italic;
            F<Slider>("StSize").Value = st.Size; F<TextBlock>("StSizeT").Text = st.Size + " px";
            F<RadioButton>("Fx" + st.Effect).IsChecked = true;
            F<RadioButton>(st.Fahrenheit ? "UnitF" : "UnitC").IsChecked = true;
            F<RadioButton>(!st.Label ? "LblNone" : st.LabelAbove ? "LblAbove" : "LblBeside").IsChecked = true;
            F<CheckBox>("TxtByTemp").IsChecked = st.ByTemp;
            bool temp = ReadingList.IsTemperature(selId), reading = ReadingList.IsReading(selId);
            foreach (var n in new[] { "StUnitsLabel", "StUnits" }) F<FrameworkElement>(n).Visibility = temp ? Visibility.Visible : Visibility.Collapsed;
            foreach (var n in new[] { "StReadingLabel", "StReading" }) F<FrameworkElement>(n).Visibility = reading ? Visibility.Visible : Visibility.Collapsed;
            styleLoading = false;
            SetPanels();
        }
        void WireLineEditor(ItemList l, Grid host)
        {
            l.Rows = new StackPanel();
            l.Overlay = new Canvas { IsHitTestVisible = false, ClipToBounds = false };
            host.Children.Add(l.Rows);
            host.Children.Add(l.Overlay);

            host.PreviewMouseMove += (s, e) =>
            {
                if (l.DragChip == null || e.LeftButton != System.Windows.Input.MouseButtonState.Pressed) return;
                var p = e.GetPosition(host);
                if (!l.Dragging)
                {
                    if (Math.Abs(p.X - l.DragStart.X) < 5 && Math.Abs(p.Y - l.DragStart.Y) < 5) return;
                    BeginLineDrag(l, host);
                }
                if (l.Ghost == null) return;
                Canvas.SetLeft(l.Ghost, p.X - 20); Canvas.SetTop(l.Ghost, p.Y - 16);
                UpdateDropTarget(l, host, p);
                e.Handled = true;
            };
            host.PreviewMouseLeftButtonUp += (s, e) =>
            {
                if (!l.Dragging) { l.DragChip = null; return; }
                e.Handled = true;
                EndLineDrag(l, host, true);
            };
            host.LostMouseCapture += (s, e) => { if (l.Dragging) EndLineDrag(l, host, false); };
        }

        // A real drag starts: the selection goes away, the lines without the dragged reading become the base, and
        // the preview shows the reading (at 40 %) where it would land, starting at its own place.
        void BeginLineDrag(ItemList l, Grid host)
        {
            if (selList != null) { var sl = selList; selList = null; if (sl != l) BuildItems(sl); SetPanels(); }
            l.Dragging = true;
            l.Drag = LineDrag.Begin(l.List, shell.Engine.Config, l.DragId);
            BuildItems(l);
            // the ghost must exist before CaptureMouse: capturing raises a nested mouse-move right away
            l.Ghost = new Border
            {
                CornerRadius = new CornerRadius(15), Padding = new Thickness(12, 6, 14, 6), BorderThickness = new Thickness(1),
                Background = new SolidColorBrush(Color.FromArgb(0xE0, 0x2A, 0x0A, 0x12)), BorderBrush = (Brush)w.FindResource("Accent"),
                Effect = (System.Windows.Media.Effects.Effect)w.FindResource("GlowSoft"),
                Child = new TextBlock { Text = l.DragName, Foreground = Brushes.White },
            };
            l.Overlay.Children.Add(l.Ghost);
            host.Cursor = System.Windows.Input.Cursors.SizeAll;
            host.CaptureMouse(); // take the mouse from the chip so it isn't clicked on release
        }

        // Works out the target from the preview on screen (rows map back to base lines around the preview row).
        void UpdateDropTarget(ItemList l, Grid host, Point p)
        {
            var rows = l.Rows.Children.Cast<FrameworkElement>().Where(r => r.Tag is int).ToList();
            var d = l.Drag;
            Func<int, int> toBase = d.ToBase;
            bool tNew = true; int tLine = d.Base.Count, tIndex = 0;
            for (int r = 0; r < rows.Count; r++)
            {
                double top = rows[r].TranslatePoint(new Point(0, 0), host).Y, h = rows[r].ActualHeight, mid = top + h / 2;
                if (Math.Abs(p.Y - mid) <= h * 0.32)
                {
                    if (r == d.NewRowIdx) { tNew = true; tLine = d.NewRowIdx; }                    // over its own preview row
                    else if (l.DragId == EmptyLineId) { tNew = true; tLine = toBase(r) + (p.Y >= mid ? 1 : 0); }
                    else
                    {
                        tNew = false; tLine = toBase(r);
                        var chips = ((WrapPanel)((DockPanel)rows[r]).Children[1]).Children.Cast<FrameworkElement>()
                                    .Where(c => c.Tag is int && (int)c.Tag >= 0 && (int)c.Tag != l.DragId).ToList();
                        tIndex = chips.Count(c => c.TranslatePoint(new Point(c.ActualWidth / 2, 0), host).X < p.X);
                    }
                    break;
                }
                if (p.Y < mid) { tNew = true; tLine = r == d.NewRowIdx + 1 && d.NewRowIdx >= 0 ? d.NewRowIdx : toBase(r); break; }
            }
            if (d.Target(tNew, tLine, tIndex)) BuildItems(l);
        }

        void EndLineDrag(ItemList l, Grid host, bool drop)
        {
            l.Dragging = false;
            host.Cursor = null;
            if (l.Ghost != null) { l.Overlay.Children.Remove(l.Ghost); l.Ghost = null; }
            if (host.IsMouseCaptured) host.ReleaseMouseCapture();
            l.DragChip = null;
            if (drop && l.Drag != null) l.List.Commit(shell.Core, l.Drag.Display);
            l.Drag = null;
            BuildItems(l);
        }

        // Controls registered here are also filled from the config by Load.
        readonly System.Collections.Generic.List<Action<Config>> loaders = new System.Collections.Generic.List<Action<Config>>();

        void BindCheck(string name, Func<Config, bool> get, Action<Config, bool> set, Action after)
        {
            var cb = F<CheckBox>(name);
            cb.Click += (s, e) => { if (loading) return; bool v = cb.IsChecked == true; Change(c => set(c, v)); if (after != null) after(); };
            loaders.Add(c => cb.IsChecked = get(c));
        }

        void BindRadio(string name, Func<Config, bool> isOn, Action<Config> set)
        {
            var rb = F<RadioButton>(name);
            rb.Checked += (s, e) => { if (!loading) Change(set); };
            loaders.Add(c => rb.IsChecked = isOn(c));
        }

        void BindSlider(string name, string label, string unit, Func<Config, int> get, Action<Config, int> set)
        {
            var sl = F<Slider>(name); var lb = F<TextBlock>(label);
            sl.ValueChanged += (s, e) => { int v = (int)Math.Round(sl.Value); lb.Text = v + unit; if (!loading) Change(c => set(c, v)); };
            loaders.Add(c => { sl.Value = get(c); lb.Text = get(c) + unit; });
        }

        // ---------------- idle image (stored in the cooler's flash) ----------------
        // The preview's Idle view shows the stored picture (cached in idle-current.jpg after an upload or a read
        // from the cooler). Choosing a new picture lets it be positioned (drag) and zoomed (wheel/slider); what is
        // shown is the exact JPEG that would be uploaded: 480x480, upright, quality lowered until
        // it fits the firmware's limit.
        // The picture being placed, its JPEG and the store/read jobs live in IdleEditor (shared with Linux).
        DispatcherTimer idleEncode;

        void WireIdle()
        {
            var pvIdle = F<CheckBox>("PvIdle");
            pvIdle.Checked += (s, e) => SetPreviewMode(true);
            pvIdle.Unchecked += (s, e) => SetPreviewMode(false);
            F<Button>("IdleBrowse").Click += (s, e) => ChooseIdle();
            F<Button>("IdleRead").Click += (s, e) => ReadIdle();
            F<Button>("IdleStore").Click += (s, e) => StoreIdle();
            F<Button>("IdleCancel").Click += (s, e) => { idle.Drop(); ShowStoredIdle(); };
            var zoom = F<Slider>("IdleZoom");
            zoom.ValueChanged += (s, e) => { idle.SetZoom(zoom.Value); F<TextBlock>("IdleZoomT").Text = zoom.Value.ToString("0.0") + "×"; QueueIdleEncode(); };
            var hit = F<FrameworkElement>("IdleHit");
            var last = new Point(); bool dragging = false;
            hit.MouseLeftButtonDown += (s, e) => { if (!idle.Editing) return; dragging = true; last = e.GetPosition(hit); hit.CaptureMouse(); };
            hit.MouseMove += (s, e) =>
            {
                if (!dragging) return;
                var p = e.GetPosition(hit);
                double k = Lcd.Width / Math.Max(1, hit.ActualWidth);
                idle.Pan((p.X - last.X) * k, (p.Y - last.Y) * k); last = p;
                QueueIdleEncode();
            };
            hit.MouseLeftButtonUp += (s, e) => { dragging = false; hit.ReleaseMouseCapture(); };
            hit.MouseWheel += (s, e) =>
            {
                if (!idle.Editing) return;
                zoom.Value = Math.Max(zoom.Minimum, Math.Min(zoom.Maximum, zoom.Value * (e.Delta > 0 ? 1.1 : 1 / 1.1)));
            };
            idleEncode = new DispatcherTimer { Interval = TimeSpan.FromMilliseconds(50) };
            idleEncode.Tick += (s, e) => { idleEncode.Stop(); EncodeIdle(); };
        }

        // While the Idle view is open (and the window visible) no frames go to the cooler, so its own idle
        // screen shows on the LCD. Hiding the window or switching back to Live resumes streaming.
        void UpdateSuspend()
        {
            bool on = F<CheckBox>("PvIdle").IsChecked == true && w.IsVisible;
            shell.Engine.Suspended = on;
            if (on && !idle.Busy) F<TextBlock>("IdleStatus").Text = "Streaming paused: the cooler shows its idle screen now (after a few seconds).";
        }

        void SetPreviewMode(bool idleView)
        {
            UpdateSuspend();
            Action<string, bool> vis = (n, on) => F<FrameworkElement>(n).Visibility = on ? Visibility.Visible : Visibility.Collapsed;
            vis("Preview", !idleView); vis("LivePanel", !idleView); vis("PvHover", !idleView);
            vis("IdlePreview", idleView); vis("IdlePanel", idleView); vis("IdleHit", idleView);
            F<TextBlock>("PvTitle").Text = idleView ? "IDLE IMAGE" : "LIVE PREVIEW";
            UpdateIdleTemp();
            if (!idleView) { F<FrameworkElement>("IdleEmpty").Visibility = Visibility.Collapsed; return; }
            if (idle.Editing) { EncodeIdle(); return; }
            ShowStoredIdle();
            if (!File.Exists(IdleEditor.Cache) && !idle.TriedRead) ReadIdle(); // nothing known yet: ask the cooler once
        }

        // Coolant temperature ("30 C") over the idle picture, while the Idle view is on and the sensor reads.
        void UpdateIdleTemp()
        {
            var panel = F<FrameworkElement>("IdleTemp");
            double water = shell.Engine.Coolant;
            bool show = F<CheckBox>("PvIdle").IsChecked == true && Render.WaterValid(water);
            panel.Visibility = show ? Visibility.Visible : Visibility.Collapsed;
            if (!show) return;
            var num = F<TextBlock>("IdleTempNum");
            num.Text = Math.Round(water).ToString();
            // "34°C": the half-size degree sign is moved down so the top of its ink meets the top of the digits
            var deg = F<TextBlock>("IdleTempDeg");
            if (deg.Tag == null)
            {
                deg.Margin = new Thickness(1, InkTop(num, "0") - InkTop(deg, "°"), 1, 0);
                deg.Tag = true;
            }
        }

        // Distance from a TextBlock's top to the top of the ink of `text` in its font.
        static double InkTop(TextBlock tb, string text)
        {
            var ft = new FormattedText(text, System.Globalization.CultureInfo.CurrentCulture, FlowDirection.LeftToRight,
                new Typeface(tb.FontFamily, tb.FontStyle, tb.FontWeight, tb.FontStretch), tb.FontSize, Brushes.White, 1.0);
            return ft.BuildGeometry(new Point(0, 0)).Bounds.Top;
        }

        void ShowIdleImage(byte[] jpg)
        {
            var img = F<Image>("IdlePreview");
            img.Source = jpg == null || jpg.Length == 0 ? null : ToImage(jpg);
            // stored picture: turned as the cooler shows it; a picture being placed stays upright for dragging
            img.RenderTransform = idle.Editing ? null : new RotateTransform(StudioModel.IdlePreviewAngle(shell.Engine.Config));
            F<FrameworkElement>("IdleEmpty").Visibility = img.Source == null ? Visibility.Visible : Visibility.Collapsed;
        }

        // Idle view without a pending picture: the stored image, and the choose / read buttons.
        void ShowStoredIdle()
        {
            idle.Jpeg = null;
            byte[] cur = IdleEditor.Stored();
            ShowIdleImage(cur);
            F<TextBlock>("IdleTitle").Text = cur != null ? "Current idle image" : "Idle image";
            F<FrameworkElement>("IdleEdit").Visibility = Visibility.Collapsed;
            F<FrameworkElement>("IdleStore").Visibility = Visibility.Collapsed;
            F<FrameworkElement>("IdleCancel").Visibility = Visibility.Collapsed;
            F<FrameworkElement>("IdleBrowse").Visibility = Visibility.Visible;
            F<FrameworkElement>("IdleRead").Visibility = Visibility.Visible;
            F<FrameworkElement>("IdleHit").Cursor = null;
        }

        void ChooseIdle()
        {
            var d = new Microsoft.Win32.OpenFileDialog { Filter = "Images, GIF or video|*.jpg;*.jpeg;*.png;*.bmp;*.gif;*.mp4;*.mov;*.avi;*.mkv;*.webm;*.wmv;*.m4v" };
            string cur = shell.Engine.Config.IdlePath;
            if (File.Exists(cur)) { d.InitialDirectory = Path.GetDirectoryName(cur); d.FileName = Path.GetFileName(cur); }
            if (d.ShowDialog(w) == true) LoadIdle(d.FileName);
        }

        void LoadIdle(string file)
        {
            try { idle.Load(file); }
            catch (Exception ex) { F<TextBlock>("IdleStatus").Text = "Could not open the file: " + ex.Message; return; }
            F<Slider>("IdleZoom").Value = 1;
            F<TextBlock>("IdleTitle").Text = "New idle image";
            F<TextBlock>("IdleStatus").Text = "Drag the picture to position it, scroll to zoom.";
            F<FrameworkElement>("IdleEdit").Visibility = Visibility.Visible;
            F<FrameworkElement>("IdleStore").Visibility = Visibility.Visible;
            F<FrameworkElement>("IdleCancel").Visibility = Visibility.Visible;
            F<FrameworkElement>("IdleRead").Visibility = Visibility.Collapsed;
            F<FrameworkElement>("IdleHit").Cursor = System.Windows.Input.Cursors.SizeAll;
            ShowEncodedIdle();
        }

        void QueueIdleEncode() { if (idle.Editing && !idleEncode.IsEnabled) idleEncode.Start(); }

        void EncodeIdle() { idle.Encode(); ShowEncodedIdle(); }

        void ShowEncodedIdle()
        {
            if (!idle.Editing) return;
            F<TextBlock>("IdleInfo").Text = idle.Info;
            F<Button>("IdleStore").IsEnabled = idle.Fits && !idle.Busy;
            ShowIdleImage(idle.Jpeg);
        }

        void SetIdleBusy(bool busy, string status)
        {
            var bar = F<ProgressBar>("IdleProgress");
            bar.Visibility = busy ? Visibility.Visible : Visibility.Collapsed; bar.Value = 0;
            foreach (var n in new[] { "IdleBrowse", "IdleRead", "IdleCancel" }) F<Button>(n).IsEnabled = !busy;
            F<Button>("IdleStore").IsEnabled = !busy && idle.Fits;
            if (status != null) F<TextBlock>("IdleStatus").Text = status;
            if (!busy) F<System.Windows.Shapes.Path>("IdleRing").Data = null;
        }

        // Progress of a read/upload: bar, percentage text and a ring around the preview (from the top, clockwise).
        void IdleProgress(int pct, string what)
        {
            pct = Math.Max(0, Math.Min(100, pct));
            F<ProgressBar>("IdleProgress").Value = pct;
            F<TextBlock>("IdleStatus").Text = what + "… " + pct + "%";
            const double c = 172, r = 168;
            Geometry g;
            if (pct >= 100) g = new EllipseGeometry(new Point(c, c), r, r);
            else if (pct <= 0) g = null;
            else
            {
                double a = (-90 + 360 * pct / 100.0) * Math.PI / 180;
                var sg = new StreamGeometry();
                using (var ctx = sg.Open())
                {
                    ctx.BeginFigure(new Point(c, c - r), false, false);
                    ctx.ArcTo(new Point(c + r * Math.Cos(a), c + r * Math.Sin(a)), new Size(r, r), 0, pct > 50, SweepDirection.Clockwise, true, false);
                }
                g = sg;
            }
            F<System.Windows.Shapes.Path>("IdleRing").Data = g;
        }

        // Read the stored picture back from the cooler (read-only commands 0x0C/0x0D, LX800 firmware).
        void ReadIdle()
        {
            if (idle.Busy) return;
            SetIdleBusy(true, shell.Engine.Connected ? "Reading the idle image from the cooler…" : "Waiting for the cooler…");
            idle.Read(p => Ui(() => IdleProgress(p, "Reading from the cooler")),
                status => Ui(() => { SetIdleBusy(false, status); if (!idle.Editing) ShowStoredIdle(); }));
        }

        // Write the pending picture into the cooler's flash (LX800 only; verified by read-back). Streaming pauses.
        void StoreIdle()
        {
            if (idle.Jpeg == null || idle.Busy) return;
            SetIdleBusy(true, shell.Engine.Connected ? "Writing to the cooler… do not unplug." : "Waiting for the cooler…");
            idle.Store(p => Ui(() => IdleProgress(p, IdleEditor.StoreProgressText(p))),
                (status, ok) => Ui(() =>
                {
                    if (ok) { idle.Drop(); ShowStoredIdle(); }
                    SetIdleBusy(false, status);
                }));
        }

        void Ui(Action a) { w.Dispatcher.BeginInvoke(a); }

        void ShowOptions(Mode m)
        {
            SetPanels();
            F<TextBlock>("OptTitle").Text = StudioModel.OptionsTitle(m);
            F<TextBlock>("OptDesc").Text = StudioModel.ModeDescription(m);
            if (m == Mode.VirtualDisplay) UpdateVd();
            if (m == Mode.Video) UpdateThumb();
            if (m == Mode.Aura) UpdateAura();
        }

        // Which option panels the current mode (and overlay choice) needs.
        void SetPanels()
        {
            foreach (var x in StudioModel.Panels(shell.Engine.Config, SelectedVisible()))
                F<FrameworkElement>(x.Key).Visibility = x.Value ? Visibility.Visible : Visibility.Collapsed;
        }

        void Browse(string filter, string box, Action<string> set)
        {
            var d = new Microsoft.Win32.OpenFileDialog { Filter = filter };
            string cur = F<TextBox>(box).Text;
            if (File.Exists(cur)) { d.InitialDirectory = Path.GetDirectoryName(cur); d.FileName = Path.GetFileName(cur); }
            if (d.ShowDialog(w) == true) { F<TextBox>(box).Text = d.FileName; set(d.FileName); }
        }

        void ApplyRegion()
        {
            int x, y, ww, h;
            if (int.TryParse(F<TextBox>("RegX").Text, out x) && int.TryParse(F<TextBox>("RegY").Text, out y) &&
                int.TryParse(F<TextBox>("RegW").Text, out ww) && int.TryParse(F<TextBox>("RegH").Text, out h) && ww > 0 && h > 0)
                Change(c => { c.RegionX = x; c.RegionY = y; c.RegionW = ww; c.RegionH = h; });
        }

        void FillSwatches(WrapPanel panel, Action<int> set, Func<int> get)
        {
            var style = (Style)w.FindResource("Swatch");
            foreach (int rgb in StudioModel.Palette)
            {
                int v = rgb;
                var b = new Button { Style = style, Background = new SolidColorBrush(ToColor(v)), ToolTip = "#" + v.ToString("X6") };
                b.Click += (s, e) => set(v);
                panel.Children.Add(b);
            }
            var custom = new Button { Style = style, Background = (Brush)w.FindResource("Rgb"), ToolTip = "Custom colour…" };
            custom.Click += (s, e) =>
            {
                using (var d = new WinForms.ColorDialog { FullOpen = true, Color = System.Drawing.Color.FromArgb(get() | unchecked((int)0xFF000000)) })
                    if (d.ShowDialog() == WinForms.DialogResult.OK) set(d.Color.R << 16 | d.Color.G << 8 | d.Color.B);
            };
            panel.Children.Add(custom);
        }

        static Color ToColor(int v) { return Color.FromRgb((byte)(v >> 16), (byte)(v >> 8), (byte)v); }

        // ---------------- video editor ----------------
        void LoadVideoInfo(string path)
        {
            videoDuration = 0;
            F<TextBlock>("VInfo").Text = "Reading video...";
            ThreadPool.QueueUserWorkItem(_ =>
            {
                double d = 0; string err = null;
                try { d = Video.Duration(path); } catch (Exception e) { err = e.Message; }
                w.Dispatcher.BeginInvoke(new Action(() =>
                {
                    if (err != null) { F<TextBlock>("VInfo").Text = "Cannot open: " + err; return; }
                    videoDuration = d;
                    loading = true;
                    F<Slider>("VStart").Maximum = Math.Max(0, d - 1);
                    F<Slider>("VLength").Maximum = Math.Max(1, Math.Min(Video.MaxSeconds, d));
                    if (F<Slider>("VLength").Value > F<Slider>("VLength").Maximum || shell.Engine.Config.VideoLength <= 0) F<Slider>("VLength").Value = F<Slider>("VLength").Maximum;
                    loading = false;
                    UpdateVideoLabels();
                    UpdateThumb();
                }));
            });
        }

        void UpdateVideoLabels()
        {
            double start = F<Slider>("VStart").Value, len = F<Slider>("VLength").Value;
            F<TextBlock>("VStartT").Text = Time(start);
            F<TextBlock>("VLengthT").Text = len.ToString("0.0") + " s";
            F<TextBlock>("VZoomT").Text = F<Slider>("VZoom").Value.ToString("0.0") + "×";
            F<TextBlock>("VFpsT").Text = ((int)F<Slider>("VFps").Value).ToString();
            if (videoDuration > 0)
                F<TextBlock>("VInfo").Text = string.Format("Source {0} · clip {1}–{2}", Time(videoDuration), Time(start), Time(Math.Min(videoDuration, start + len)));
        }

        static string Time(double s) { return string.Format("{0}:{1:00}.{2}", (int)s / 60, (int)s % 60, (int)(s * 10) % 10); }

        void ApplyVideoSettings()
        {
            double start = F<Slider>("VStart").Value, len = F<Slider>("VLength").Value;
            if (videoDuration > 0 && start + len > videoDuration) len = Math.Max(0.5, videoDuration - start);
            double zoom = F<Slider>("VZoom").Value, px = F<Slider>("VPanX").Value, py = F<Slider>("VPanY").Value;
            int fps = (int)F<Slider>("VFps").Value;
            Change(c => { c.VideoStart = Math.Round(start, 1); c.VideoLength = Math.Round(len, 1); c.VideoZoom = Math.Round(zoom, 2); c.VideoPanX = Math.Round(px, 2); c.VideoPanY = Math.Round(py, 2); c.VideoFps = fps; });
        }

        int thumbGen;
        void UpdateThumb()
        {
            var c = shell.Engine.Config;
            if (string.IsNullOrEmpty(c.VideoPath) || !File.Exists(c.VideoPath)) { F<Image>("VThumb").Source = null; return; }
            var o = new VideoOptions
            {
                Path = c.VideoPath, Zoom = F<Slider>("VZoom").Value, PanX = F<Slider>("VPanX").Value, PanY = F<Slider>("VPanY").Value, Quality = 80
            };
            double t = F<Slider>("VStart").Value;
            int gen = ++thumbGen;
            ThreadPool.QueueUserWorkItem(_ =>
            {
                byte[] jpg = null;
                try
                {
                    double dur;
                    using (var b = Video.Thumbnail(o, t, out dur)) if (b != null) jpg = Render.Jpeg(b, 80);
                }
                catch (Exception) { }
                w.Dispatcher.BeginInvoke(new Action(() => { if (gen == thumbGen && jpg != null) F<Image>("VThumb").Source = ToImage(jpg); }));
            });
        }

        // ---------------- device page ----------------
        void WireDevice()
        {
            var br = F<Slider>("Bright");
            br.ValueChanged += (s, e) => { int v = (int)br.Value; F<TextBlock>("BrightT").Text = v + "%"; Change(c => c.Brightness = v); };
            foreach (int r in new[] { 0, 90, 180, 270 })
            {
                int rr = r;
                F<RadioButton>("Rot" + r).Checked += (s, e) => Change(c => c.Rotation = StudioModel.RotationFor(c, rr)); // shown relative to upright
            }
            F<Button>("VdInstall").Click += (s, e) => RunDisplay("install");
            F<Button>("VdUninstall").Click += (s, e) => RunDisplay("uninstall");
            F<Button>("AuraInstall").Click += (s, e) => RunCli("aura install", "AuraStatus", "Installing… (confirm the UAC prompt)", UpdateAura);
            F<Button>("AuraUninstall").Click += (s, e) => RunCli("aura uninstall", "AuraStatus", "Removing…", UpdateAura);
        }

        void UpdateAura()
        {
            bool installed = AuraInstall.Installed;
            var st = AuraState.Read();
            F<TextBlock>("AuraStatus").Text = !installed ? "○ Plugin not installed"
                : st == null ? "● Installed · waiting for Armoury Crate" : "● Installed · receiving Aura data";
            F<Button>("AuraInstall").IsEnabled = !installed;
            F<Button>("AuraUninstall").IsEnabled = installed;
        }

        // Runs "ttlcd.exe <args>" (which elevates itself when needed), then refreshes.
        void RunCli(string args, string statusLabel, string busyText, Action done)
        {
            F<TextBlock>(statusLabel).Text = busyText;
            shell.Core.Platform.RunCli(args, output => Ui(() =>
            {
                done();
                if (output.Contains("error")) MessageBox.Show(w, output.Trim(), "TT LCD Studio");
            }));
        }

        void UpdateVd()
        {
            int mon = ScreenSource.FindBySize(Lcd.Width, Lcd.Height);
            string t = mon >= 0 ? "● Installed (monitor " + (mon + 1) + ")" : "○ Not installed";
            F<TextBlock>("VdStatus1").Text = t;
            F<TextBlock>("VdStatus2").Text = t;
            F<Button>("VdInstall").IsEnabled = mon < 0;
            F<Button>("VdUninstall").IsEnabled = mon >= 0;
            RefreshMonitors();
        }

        void RefreshMonitors()
        {
            var box = F<ComboBox>("MonitorBox");
            bool was = loading; loading = true;
            box.Items.Clear();
            var screens = WinForms.Screen.AllScreens;
            for (int i = 0; i < screens.Length; i++)
                box.Items.Add(string.Format("Monitor {0} — {1}×{2}{3}", i + 1, screens[i].Bounds.Width, screens[i].Bounds.Height, screens[i].Primary ? " (primary)" : ""));
            box.SelectedIndex = Math.Min(shell.Engine.Config.MonitorIndex, screens.Length - 1);
            loading = was;
        }

        void RunDisplay(string action)
        {
            F<TextBlock>("VdStatus2").Text = action == "install" ? "Installing… (confirm the UAC prompt)" : "Removing…";
            F<Button>("VdInstall").IsEnabled = F<Button>("VdUninstall").IsEnabled = false;
            shell.Core.Platform.RunCli("display " + action, output =>
            {
                Thread.Sleep(1500); // display topology settles
                w.Dispatcher.BeginInvoke(new Action(() =>
                {
                    UpdateVd();
                    if (output.Contains("error")) MessageBox.Show(w, output.Trim(), "Virtual display");
                }));
            });
        }

        // ---------------- settings page ----------------
        void WireSettings()
        {
            var fps = F<Slider>("Fps");
            fps.ValueChanged += (s, e) => { int v = (int)fps.Value; F<TextBlock>("FpsT").Text = v.ToString(); Change(c => c.Fps = v); };
            var q = F<Slider>("Quality");
            q.ValueChanged += (s, e) => { int v = (int)q.Value; F<TextBlock>("QualityT").Text = v.ToString(); Change(c => c.Quality = v); };
            var st = F<CheckBox>("StartupSwitch");
            st.Checked += (s, e) => { if (!loading) shell.Core.Platform.Autostart = true; };
            st.Unchecked += (s, e) => { if (!loading) shell.Core.Platform.Autostart = false; };
            var bl = F<CheckBox>("BlankSwitch");
            bl.Checked += (s, e) => Change(c => c.BlankOnExit = true);
            bl.Unchecked += (s, e) => Change(c => c.BlankOnExit = false);
            var tc = F<CheckBox>("TrayCloseSwitch");
            tc.Checked += (s, e) => Change(c => c.CloseToTray = true);
            tc.Unchecked += (s, e) => Change(c => c.CloseToTray = false);
            // game mode: manual force (title-bar gamepad button + switch here; not saved)
            foreach (var tb in new ToggleButton[] { F<ToggleButton>("GameBtn"), F<CheckBox>("GmForce") })
            {
                var t = tb;
                RoutedEventHandler h = (s, e) =>
                {
                    bool v = t.IsChecked == true;
                    if (v != shell.Engine.ForceGame) { shell.Engine.ForceGame = v; UpdateStatus(); }
                };
                t.Checked += h; t.Unchecked += h;
            }
            BindCheck("GmOn", c => c.GameDetect, (c, v) => c.GameDetect = v, UpdateGamePanel);
            BindCheck("GmFullscreen", c => c.GameFullscreen, (c, v) => c.GameFullscreen = v, null);
            BindCheck("GmKnown", c => c.GameKnownList, (c, v) => c.GameKnownList = v, null);
            BindCheck("GmBorderless", c => c.GameBorderless, (c, v) => c.GameBorderless = v, null);
            foreach (GameAction a in Enum.GetValues(typeof(GameAction)))
            {
                var aa = a;
                BindRadio("GmAct" + a, c => c.GameAction == aa, c => c.GameAction = aa);
                F<RadioButton>("GmAct" + a).Checked += (s, e) => UpdateGamePanel();
            }
            BindSlider("GmFps", "GmFpsT", " fps", c => c.GameFps, (c, v) => c.GameFps = v);
            BindSlider("GmSensor", "GmSensorT", " s", c => c.GameSensorMs / 1000, (c, v) => c.GameSensorMs = v * 1000);
            var extra = F<TextBox>("GmExtra");
            extra.LostFocus += (s, e) => { string v = extra.Text.Trim(); Change(c => c.GameExtra = v); };
            var ignore = F<TextBox>("GmIgnore");
            ignore.LostFocus += (s, e) => { string v = ignore.Text.Trim(); Change(c => c.GameIgnore = v); };
            loaders.Add(c => { extra.Text = c.GameExtra; ignore.Text = c.GameIgnore; UpdateGamePanel(); });

            F<Button>("SvcInstall").Click += (s, e) => RunCli("service install", "SvcStatus", "Installing… (confirm the UAC prompt)", UpdateService);
            F<Button>("SvcUninstall").Click += (s, e) => RunCli("service uninstall", "SvcStatus", "Removing… (confirm the UAC prompt)", UpdateService);
            F<Button>("ClearCache").Click += (s, e) => { Video.ClearCache(); UpdateCacheInfo(); };
            F<Button>("OpenConfig").Click += (s, e) => shell.Core.Platform.OpenFolder(Config.Folder);
        }

        void UpdateGamePanel()
        {
            var c = shell.Engine.Config;
            F<FrameworkElement>("GmOptions").IsEnabled = c.GameDetect;
            F<FrameworkElement>("GmOptions").Opacity = c.GameDetect ? 1 : 0.45;
            var fpsVis = c.GameAction == GameAction.LowerFps ? Visibility.Visible : Visibility.Collapsed;
            foreach (var n in new[] { "GmFpsLabel", "GmFps", "GmFpsT" }) F<FrameworkElement>(n).Visibility = fpsVis;
        }

        void UpdateService()
        {
            bool inst = shell.Core.Platform.SensorInstalled;
            F<TextBlock>("SvcStatus").Text = shell.Core.Platform.SensorStatus;
            F<Button>("SvcInstall").IsEnabled = !inst;
            F<Button>("SvcUninstall").IsEnabled = inst;
        }

        void UpdateCacheInfo()
        {
            F<TextBlock>("CacheInfo").Text = StudioModel.CacheInfo();
        }

        // ---------------- load config into controls ----------------
        void Load(Config c)
        {
            loading = true;
            F<RadioButton>("Mode" + c.Mode).IsChecked = true;
            ShowOptions(c.Mode);
            RefreshMonitors();
            F<CheckBox>("RegionSwitch").IsChecked = c.UseRegion;
            F<FrameworkElement>("RegionRow").IsEnabled = c.UseRegion;
            F<TextBox>("RegX").Text = c.RegionX.ToString(); F<TextBox>("RegY").Text = c.RegionY.ToString();
            F<TextBox>("RegW").Text = c.RegionW.ToString(); F<TextBox>("RegH").Text = c.RegionH.ToString();
            F<TextBox>("ImagePath").Text = c.ImagePath;
            F<TextBox>("GifPath").Text = c.GifPath;
            F<TextBox>("VideoPath").Text = c.VideoPath;
            F<Slider>("VStart").Maximum = Math.Max(30, c.VideoStart);
            F<Slider>("VStart").Value = c.VideoStart;
            F<Slider>("VLength").Value = c.VideoLength;
            F<Slider>("VZoom").Value = c.VideoZoom;
            F<Slider>("VPanX").Value = c.VideoPanX;
            F<Slider>("VPanY").Value = c.VideoPanY;
            F<Slider>("VFps").Value = c.VideoFps;
            F<RadioButton>(c.FitContain ? "ScaleFit" : "ScaleFill").IsChecked = true;
            foreach (var l in loaders) l(c);
            F<Slider>("Bright").Value = c.Brightness;
            F<TextBlock>("BrightT").Text = c.Brightness + "%";
            F<RadioButton>("Rot" + StudioModel.UserRotation(c)).IsChecked = true;
            F<Slider>("Fps").Value = c.Fps; F<TextBlock>("FpsT").Text = c.Fps.ToString();
            F<Slider>("Quality").Value = c.Quality; F<TextBlock>("QualityT").Text = c.Quality.ToString();
            F<CheckBox>("StartupSwitch").IsChecked = shell.Core.Platform.Autostart;
            F<CheckBox>("BlankSwitch").IsChecked = c.BlankOnExit;
            F<CheckBox>("TrayCloseSwitch").IsChecked = c.CloseToTray;
            F<CheckBox>("DbShowWater").IsChecked = c.DashWater;
            F<CheckBox>("DbShowCpuLoad").IsChecked = c.DashCpuLoad;
            F<CheckBox>("DbShowCpuTemp").IsChecked = c.DashCpuTemp;
            F<CheckBox>("DbShowRam").IsChecked = c.DashRam;
            F<CheckBox>("SleepSwitch").IsChecked = c.DeviceSleep;
            loading = false;
            UpdateVideoLabels();
            if (File.Exists(c.VideoPath)) LoadVideoInfo(c.VideoPath);
        }

        // ---------------- live preview & status ----------------
        void UpdatePreview() { UpdatePreview(false); }
        void UpdatePreview(bool force)
        {
            if (!w.IsVisible && !force) return;
            var f = shell.Engine.Latest;
            if (f == null || f.Seq == shownSeq) return;
            shownSeq = f.Seq;
            var img = F<Image>("Preview");
            img.Source = ToImage(f.Jpeg);
            img.RenderTransform = new RotateTransform(-StudioModel.Mount(shell.Engine.Config)); // the LCD as you see it on this mount
        }

        // Rotate arrow on the preview: the one Rotation setting, +90° clockwise per click (saved). The engine also
        // sets the cooler's own rotation to match, so its idle screen turns with the live picture.
        void RotateClick()
        {
            int r = StudioModel.NextUserRotation(shell.Engine.Config);
            F<RadioButton>("Rot" + r).IsChecked = true; // saves it (Device page handler) and keeps that page in sync
        }

        // Cooler rotation calibration on first connect (StudioModel); keeps the idle preview turned like the LCD.
        void UpdateHwRotation()
        {
            if (model.CheckHwRotation() && !idle.Editing)
                F<Image>("IdlePreview").RenderTransform = new RotateTransform(StudioModel.IdlePreviewAngle(shell.Engine.Config));
        }

        void UpdateStatus() { UpdateStatus(false); }
        void UpdateStatus(bool force)
        {
            if (!w.IsVisible && !force) return;
            var e = shell.Engine;
            UpdateIdleTemp();
            UpdateHwRotation();
            var dot = F<System.Windows.Shapes.Ellipse>("ConnDot");
            var dotColor = e.Connected ? Color.FromRgb(0x2B, 0xE3, 0x6B) : Color.FromRgb(0xFF, 0x1F, 0x4B);
            dot.Fill = new SolidColorBrush(dotColor);
            var glow = dot.Effect as System.Windows.Media.Effects.DropShadowEffect;
            if (glow != null && !glow.IsFrozen) glow.Color = dotColor;
            F<TextBlock>("ConnText").Text = model.ConnText;
            F<TextBlock>("FpsBig").Text = model.FpsText;
            F<TextBlock>("ModeText").Text = model.ModeText;
            F<TextBlock>("TitleStatus").Text = model.TitleStatus;
            F<TextBlock>("GmStatus").Text = model.GameStatus();
            F<ToggleButton>("GameBtn").IsChecked = e.ForceGame;
            F<CheckBox>("GmForce").IsChecked = e.ForceGame;
            UpdatePauseOverlay();
            F<TextBlock>("DevStatus").Text = model.DeviceStatus;
            F<TextBlock>("DevSerial").Text = model.SerialText;
            F<TextBlock>("NavSerial").Text = model.NavSerial;
            F<TextBlock>("DevFps").Text = model.DevFps;
            if (e.Config.Mode == Mode.Aura) UpdateAura();
            var pb = F<ProgressBar>("VProgress");
            double p = e.VideoProgress;
            pb.Visibility = p >= 0 && e.Config.Mode == Mode.Video ? Visibility.Visible : Visibility.Collapsed;
            if (p >= 0) pb.Value = p;
        }

        static BitmapImage ToImage(byte[] jpg)
        {
            var bi = new BitmapImage();
            bi.BeginInit();
            bi.CacheOption = BitmapCacheOption.OnLoad;
            bi.StreamSource = new MemoryStream(jpg);
            bi.EndInit();
            bi.Freeze();
            return bi;
        }
    }
}
