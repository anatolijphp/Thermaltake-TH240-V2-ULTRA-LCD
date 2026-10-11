// Linux front end: the main window, laid out like MainWindow.xaml (custom title bar, collapsible navigation, pages,
// preview column). Everything it shows or changes goes through StudioCore / StudioModel, shared with the WPF window.
using System;
using System.Collections.Generic;
using Cairo;
using Gtk;

namespace TtLcd.GtkUi
{
    partial class StudioWindow : Window
    {
        readonly StudioCore core;
        readonly StudioModel model;
        readonly Dictionary<string, Widget> pages = new Dictionary<string, Widget>();
        readonly Dictionary<string, NavItem> navs = new Dictionary<string, NavItem>();
        Label titleStatus, navSerial, connText, fpsText, modeText;
        DrawingArea connDot;
        RoundPreview preview;
        Label previewTitle;
        IconButton gameBtn;
        long shownSeq = -1;
        double rgbShift;
        public string Note;   // shown once in the status card (e.g. "the ttlcd service was stopped")

        public StudioWindow(StudioCore core) : base(WindowType.Toplevel)
        {
            this.core = core;
            model = new StudioModel(core);
            model.InitMount();
            Title = "TT LCD Studio";
            Decorated = false;   // own title bar, as on Windows
            AppPaintable = true;
            SetDefaultSize(1240, 792);
            SetSizeRequest(1100, 700);
            IconName = "ttlcd-studio";

            var root = new Box(Orientation.Vertical, 0);
            root.PackStart(TitleBar(), false, false, 0);
            var body = new Box(Orientation.Horizontal, 0);
            body.PackStart(Nav(), false, false, 0);
            var content = new Bloom(this);
            var cols = new Box(Orientation.Horizontal, 0);
            cols.PackStart(Pages(), true, true, 0);
            cols.PackStart(PreviewColumn(), false, false, 0);
            content.Add(cols);
            body.PackStart(content, true, true, 0);
            root.PackStart(body, true, true, 0);
            Add(root);

            ShowPage("dashboard");
            GLib.Timeout.Add(40, () => { UpdatePreview(); return true; });
            GLib.Timeout.Add(500, () => { UpdateStatus(); return true; });
            GLib.Timeout.Add(1000, () => { UpdateDashboard(); return true; });
            UpdateStatus(); UpdateDashboard();
        }

        protected override bool OnDrawn(Context cr)
        {
            Theme.Set(cr, Theme.Bg); cr.Paint();
            return base.OnDrawn(cr);
        }

        // ---------------- title bar ----------------
        Widget TitleBar()
        {
            var bar = new TitleBarArea(this);
            bar.HeightRequest = 48;
            var row = new Box(Orientation.Horizontal, 0);
            var brand = new Box(Orientation.Horizontal, 0) { MarginLeft = 22, Valign = Align.Center };
            var tt = new Label("TT"); tt.StyleContext.AddClass("brand-tt");
            var name = new Label(" LCD STUDIO") { MarginTop = 2 }; name.StyleContext.AddClass("brand");
            brand.PackStart(tt, false, false, 0); brand.PackStart(name, false, false, 2);
            row.PackStart(brand, false, false, 0);
            titleStatus = new Label("") { Halign = Align.Center }; titleStatus.StyleContext.AddClass("titlestatus");
            row.PackStart(titleStatus, true, true, 0);
            gameBtn = new IconButton("gamepad", 48, 48, "Game mode: force on (for games that aren't detected)") { IconSize = 16 };
            gameBtn.Clicked += () => { core.Engine.ForceGame = !core.Engine.ForceGame; UpdateStatus(); };
            var min = new IconButton("min", 46, 48, "Minimise") { IconSize = 12 };
            min.Clicked += () => Iconify();
            var full = new IconButton("full", 46, 48, "Full screen") { IconSize = 12 };
            full.Clicked += () => { if ((Window.State & Gdk.WindowState.Maximized) != 0) Unmaximize(); else Maximize(); };
            var close = new IconButton("close", 46, 48, "Close") { IconSize = 12 };
            close.Clicked += CloseRequested;
            foreach (var b in new Widget[] { gameBtn, min, full, close }) row.PackStart(b, false, false, 0);
            bar.Add(row);
            GLib.Timeout.Add(50, () => { rgbShift = (rgbShift + 1.0 / 120) % 1; bar.QueueDraw(); return true; }); // flowing RGB line (6 s)
            bar.Shift = () => rgbShift;
            return bar;
        }

        public bool HasTray;
        string currentPage = "dashboard";

        // The close button: to the tray (when there is one and the setting says so), otherwise exit.
        public void CloseRequested()
        {
            if (HasTray && core.Config.CloseToTray)
            {
                Hide();
                if (idleOn) core.Engine.Suspended = false;   // the Idle view only pauses streaming while it is visible
            }
            else Quit();
        }

        public void ShowWindow()
        {
            ShowAll();
            Load();
            ShowPage(currentPage);
            if (idleOn) SetPreviewMode(true);
            Present();
        }

        public void Quit()
        {
            Hide();
            Application.Quit();
        }

        // ---------------- navigation ----------------
        Widget Nav()
        {
            var panel = new NavPanel(this) { WidthRequest = 214 };
            var col = new Box(Orientation.Vertical, 0);
            var top = new Box(Orientation.Vertical, 0) { MarginTop = 10 };
            var toggle = new IconButton("menu", 40, 40, "Collapse menu") { IconSize = 16, Halign = Align.Start, MarginLeft = 13, MarginBottom = 8 };
            toggle.Clicked += () => { core.Change(c => c.NavCollapsed = !c.NavCollapsed); ApplyNav(panel, true); };
            top.PackStart(toggle, false, false, 0);
            foreach (var n in new[] { new[] { "dashboard", "Dashboard" }, new[] { "modes", "Display modes" }, new[] { "device", "Device" }, new[] { "settings", "Settings" } })
            {
                string id = n[0];
                var item = new NavItem(n[1], id);
                item.Clicked += () => ShowPage(id);
                navs[id] = item;
                top.PackStart(item, false, false, 0);
            }
            col.PackStart(top, false, false, 0);
            var footer = new Box(Orientation.Vertical, 0) { MarginLeft = 24, MarginBottom = 20, Valign = Align.End };
            var rgb = new DrawingArea { WidthRequest = 36, HeightRequest = 2, Halign = Align.Start, MarginBottom = 10 };
            rgb.Drawn += (o, a) => { var g = Theme.RainbowGradient(0, 0, 36, 0, Theme.Rainbow); a.Cr.SetSource(g); a.Cr.Rectangle(0, 0, 36, 2); a.Cr.Fill(); g.Dispose(); };
            footer.PackStart(rgb, false, false, 0);
            var model1 = new Label("TH240 V2 Ultra · CL-W383") { Halign = Align.Start }; model1.StyleContext.AddClass("dim"); model1.StyleContext.AddClass("small");
            navSerial = new Label("") { Halign = Align.Start }; navSerial.StyleContext.AddClass("dim"); navSerial.StyleContext.AddClass("small");
            footer.PackStart(model1, false, false, 0); footer.PackStart(navSerial, false, false, 0);
            col.PackEnd(footer, false, false, 0);
            panel.Add(col);
            panel.Footer = footer;
            ApplyNav(panel);
            return panel;
        }

        // animate=false for the initial layout (no slide-in on window open), true for the toggle click.
        void ApplyNav(NavPanel panel, bool animate = false)
        {
            bool c = core.Config.NavCollapsed;
            int target = c ? 66 : 214;
            if (animate) panel.AnimateWidth(target); else panel.WidthRequest = target;
            foreach (var n in navs.Values) n.SetCollapsed(c);
            if (panel.Footer != null) panel.Footer.Visible = !c;
        }

        public void ShowPage(string page)
        {
            currentPage = page;
            foreach (var p in pages) p.Value.Visible = p.Key == page;
            foreach (var n in navs) n.Value.On = n.Key == page;
            if (page == "dashboard") UpdateDashboard();
            if (page == "modes") ShowOptions();
        }

        // ---------------- pages ----------------
        Widget Pages()
        {
            var scroll = new ScrolledWindow();
            scroll.SetPolicy(PolicyType.Never, PolicyType.Automatic);
            var stack = new Box(Orientation.Vertical, 0) { MarginLeft = 30, MarginRight = 24, MarginTop = 24, MarginBottom = 10 };
            pages["dashboard"] = Dashboard();
            pages["modes"] = ModesPage();
            pages["device"] = DevicePage();
            pages["settings"] = SettingsPage();
            foreach (var p in pages.Values) { p.NoShowAll = false; stack.PackStart(p, false, false, 0); }
            scroll.Add(stack);
            return scroll;
        }

        static Box PageHead(string title, string sub)
        {
            var b = new Box(Orientation.Vertical, 0);
            var h = new Label(title) { Halign = Align.Start }; h.StyleContext.AddClass("h1");
            var s = new Label(sub) { Halign = Align.Start, MarginTop = 2, MarginBottom = 18, Wrap = true, Xalign = 0 };
            s.StyleContext.AddClass("sub");
            b.PackStart(h, false, false, 0); b.PackStart(s, false, false, 0);
            return b;
        }

        static Label Caption(string text)
        {
            var l = new Label(text) { Halign = Align.Start, MarginBottom = 12 };
            l.StyleContext.AddClass("caption");
            return l;
        }

        Widget Placeholder(string title, string sub)
        {
            var page = PageHead(title, sub);
            var card = new CardBox();
            var inner = card.Inner();
            inner.PackStart(Caption("COMING NEXT"), false, false, 0);
            var l = new Label("This page is built in the next step of the Linux version. The Dashboard and the live preview are the prototype.")
            { Halign = Align.Start, Wrap = true, Xalign = 0 };
            l.StyleContext.AddClass("sub");
            inner.PackStart(l, false, false, 0);
            page.PackStart(card, false, false, 0);
            return page;
        }

        // Dashboard: sensor chips, the gauges in a balanced grid, the update note.
        GaugeArea gCoolant, gCpuTemp, gCpuLoad, gRam;
        Grid gaugeGrid; Label dashNote; readonly List<KeyValuePair<CardBox, Func<Config, bool>>> gaugeCards = new List<KeyValuePair<CardBox, Func<Config, bool>>>();
        bool loadingChips;

        Widget Dashboard()
        {
            var page = PageHead("Dashboard", "Live readings. Coolant temperature comes from the cooler's own sensor; the rest are read from " + (Os.Linux ? "the system." : "Windows."));
            var card = new CardBox();
            var inner = card.Inner();
            inner.PackStart(Caption("SENSORS"), false, false, 0);
            var sub = new Label("Choose which gauges the dashboard shows.") { Halign = Align.Start, MarginBottom = 12 };
            sub.StyleContext.AddClass("sub");
            inner.PackStart(sub, false, false, 0);
            var chips = new Box(Orientation.Horizontal, 6);
            chips.PackStart(Chip("Coolant temperature", c => c.DashWater, (c, v) => c.DashWater = v), false, false, 0);
            chips.PackStart(Chip("CPU temperature", c => c.DashCpuTemp, (c, v) => c.DashCpuTemp = v), false, false, 0);
            chips.PackStart(Chip("CPU load", c => c.DashCpuLoad, (c, v) => c.DashCpuLoad = v), false, false, 0);
            chips.PackStart(Chip("RAM usage", c => c.DashRam, (c, v) => c.DashRam = v), false, false, 0);
            inner.PackStart(chips, false, false, 0);
            page.PackStart(card, false, false, 0);

            gCoolant = new GaugeArea("COOLANT", "thermo", 0x50B4FF);
            gCpuTemp = new GaugeArea("CPU TEMP", "thermo", 0xFFAA3C);
            gCpuLoad = new GaugeArea("CPU LOAD", "cpu", 0xFF4B6E);
            gRam = new GaugeArea("RAM", "ram", 0x5BE38A);
            gaugeGrid = new Grid { ColumnSpacing = 14, ColumnHomogeneous = true };
            foreach (var g in new[] {
                new KeyValuePair<GaugeArea, Func<Config, bool>>(gCoolant, c => c.DashWater),
                new KeyValuePair<GaugeArea, Func<Config, bool>>(gCpuTemp, c => c.DashCpuTemp),
                new KeyValuePair<GaugeArea, Func<Config, bool>>(gCpuLoad, c => c.DashCpuLoad),
                new KeyValuePair<GaugeArea, Func<Config, bool>>(gRam, c => c.DashRam) })
            {
                var cb = new CardBox();
                var inn = cb.Inner();
                inn.PackStart(g.Key, true, true, 0);
                gaugeCards.Add(new KeyValuePair<CardBox, Func<Config, bool>>(cb, g.Value));
            }
            gaugeGrid.SizeAllocated += (o, a) => LayoutGauges(a.Allocation.Width);
            page.PackStart(gaugeGrid, false, false, 0);
            dashNote = new Label("") { Halign = Align.Start, MarginLeft = 2 };
            dashNote.StyleContext.AddClass("dim"); dashNote.StyleContext.AddClass("small");
            page.PackStart(dashNote, false, false, 0);
            LayoutGauges(560);
            return page;
        }

        int gaugeCols = -1, gaugeCount = -1;
        void LayoutGauges(int width)
        {
            var cfg = core.Config;
            var shown = gaugeCards.FindAll(p => p.Value(cfg));
            int cols = StudioModel.GaugeColumns(shown.Count, width);
            if (cols == gaugeCols && shown.Count == gaugeCount) return;
            gaugeCols = cols; gaugeCount = shown.Count;
            foreach (var p in gaugeCards) if (p.Key.Parent != null) gaugeGrid.Remove(p.Key);
            for (int i = 0; i < shown.Count; i++) gaugeGrid.Attach(shown[i].Key, i % cols, i / cols, 1, 1);
            gaugeGrid.ShowAll();
        }

        Widget Chip(string text, Func<Config, bool> get, Action<Config, bool> set)
        {
            var b = new ToggleButton();
            b.StyleContext.AddClass("chip");
            var row = new Box(Orientation.Horizontal, 8);
            var icon = new DrawingArea { WidthRequest = 11, HeightRequest = 11, Valign = Align.Center };
            icon.Drawn += (o, a) => Icons.Draw(a.Cr, b.Active ? "check" : "plus", 0, 0, 11, b.Active ? Theme.Accent : Theme.Dim);
            row.PackStart(icon, false, false, 0);
            row.PackStart(new Label(text), false, false, 0);
            b.Add(row);
            loadingChips = true; b.Active = get(core.Config); loadingChips = false;
            b.Toggled += (o, a) => { if (loadingChips) return; bool v = b.Active; core.Change(c => set(c, v)); icon.QueueDraw(); gaugeCount = -1; LayoutGauges(gaugeGrid.Allocation.Width); UpdateDashboard(); };
            return b;
        }

        void UpdateDashboard()
        {
            if (!pages.ContainsKey("dashboard") || !pages["dashboard"].Visible) return;
            var d = model.Readings();
            gCoolant.Set(d.Coolant.Text, d.Coolant.Frac);
            gCpuTemp.Set(d.CpuTemp.Text, d.CpuTemp.Frac);
            gCpuLoad.Set(d.CpuLoad.Text, d.CpuLoad.Frac);
            gRam.Set(d.Ram.Text, d.Ram.Frac);
            dashNote.Text = d.Note;
        }

        // ---------------- preview column ----------------
        Widget PreviewColumn()
        {
            var col = new Box(Orientation.Vertical, 0) { WidthRequest = 400 };
            previewTitle = new Label("LIVE PREVIEW") { Halign = Align.Start, MarginLeft = 10, MarginTop = 10 };
            previewTitle.StyleContext.AddClass("caption");
            col.PackStart(previewTitle, false, false, 0);
            var inner = new Box(Orientation.Vertical, 0) { MarginLeft = 28, MarginRight = 28, MarginTop = 6 };
            var head = new Box(Orientation.Horizontal, 0);
            idleSwitch = new SwitchToggle("Idle") { TooltipText = "Show and change the picture the cooler displays by itself" };
            idleSwitch.Toggled += SetPreviewMode;
            head.PackStart(idleSwitch, false, false, 0);
            var rotate = new IconButton("rotate", 30, 30, "Rotate 90° (saved; turns the live picture and the idle screen)") { Round = true, IconSize = 14 };
            rotate.Clicked += () => { int r = StudioModel.NextUserRotation(core.Config); core.Change(c => c.Rotation = StudioModel.RotationFor(c, r)); shownSeq = -1; };
            head.PackEnd(rotate, false, false, 0);
            inner.PackStart(head, false, false, 0);
            preview = new RoundPreview { Halign = Align.Center, MarginTop = 14 };
            preview.Clicked += () => { core.Engine.Paused = !core.Engine.Paused; UpdateStatus(); };
            inner.PackStart(preview, false, false, 0);

            var card = new StatusCard { MarginTop = 24 };
            var cb = new Box(Orientation.Vertical, 0) { MarginLeft = 14, MarginRight = 14, MarginTop = 10, MarginBottom = 10 };
            var row = new Box(Orientation.Horizontal, 9);
            connDot = new DrawingArea { WidthRequest = 9, HeightRequest = 9, Valign = Align.Center };
            connDot.Drawn += (o, a) =>
            {
                var c = core.Engine.Connected ? Theme.Hex(0x2BE36B) : Theme.Accent;
                a.Cr.Arc(4.5, 4.5, 4.5, 0, 2 * Math.PI); Theme.Set(a.Cr, c); a.Cr.Fill();
            };
            connText = new Label(""); connText.StyleContext.AddClass("semibold");
            fpsText = new Label(""); fpsText.StyleContext.AddClass("status");
            row.PackStart(connDot, false, false, 0); row.PackStart(connText, false, false, 0); row.PackEnd(fpsText, false, false, 0);
            modeText = new Label("") { Halign = Align.Start, MarginTop = 6, Wrap = true, Xalign = 0, MaxWidthChars = 44 };
            modeText.StyleContext.AddClass("dim");
            cb.PackStart(row, false, false, 0); cb.PackStart(modeText, false, false, 0);
            card.Add(cb);
            inner.PackStart(card, false, false, 0);
            liveCard = card;
            inner.PackStart(BuildIdlePanel(), false, false, 0);
            col.PackStart(inner, false, false, 0);
            return new ColumnBorder(col);
        }

        void UpdatePreview()
        {
            var f = core.Engine.Latest;
            if (f == null || f.Seq == shownSeq) return;
            shownSeq = f.Seq;
            preview.SetFrame(f.Jpeg, -StudioModel.Mount(core.Config)); // the LCD as you see it on this mount
        }

        void UpdateStatus()
        {
            if (model.CheckHwRotation() && idleOn && !idle.Editing) ShowStoredIdle();
            UpdateIdleTemp();
            preview.Paused = core.Engine.Paused;
            titleStatus.Text = model.TitleStatus;
            connText.Text = model.ConnText;
            fpsText.Text = model.FpsText;
            modeText.Text = Note ?? model.ModeText;
            navSerial.Text = model.NavSerial;
            UpdatePages();
            gameBtn.Checked = core.Engine.ForceGame; gameBtn.QueueDraw();
            connDot.QueueDraw();
        }

        // ---------------- screenshot (dev aid / CI) ----------------
        public void Screenshot(string file, string page)
        {
            // "modes:Clock" shows that mode's options; TTLCD_SHOT_SELECT=ClkItems:0 selects a reading (style panel)
            int colon = page.IndexOf(':');
            if (colon > 0)
            {
                var m = (Mode)Enum.Parse(typeof(Mode), page.Substring(colon + 1), true);
                core.Change(c => c.Mode = m);
                page = page.Substring(0, colon);
                Load();
            }
            if (page == "idle") { idle.TriedRead = true; idleSwitch.SetSilently(true); SetPreviewMode(true); page = "dashboard"; }
            var pick = Environment.GetEnvironmentVariable("TTLCD_SHOT_SELECT");
            if (!string.IsNullOrEmpty(pick) && pick.Contains(":"))
            {
                var ed = pick.StartsWith("Ov") ? overlayLines : clockLines;
                ed.Selected = int.Parse(pick.Split(':')[1]);
                selEditor = ed;
                ShowStyleEditor();
            }
            ShowPage(page);
            Pump(800);   // GTK lays out on the next frame; let it (and the engine's next frames) catch up
            UpdatePreview(); UpdateStatus(); UpdateDashboard();
            Pump(300);
            // what the X server shows for the window (Xvfb in CI), as a user would see it
            var a = Allocation;
            Window.ProcessUpdates(true);
            IntPtr p = gdk_pixbuf_get_from_window(Window.Handle, 0, 0, a.Width, a.Height);
            if (p == IntPtr.Zero) throw new InvalidOperationException("could not read the window's pixels");
            using (var pix = new Gdk.Pixbuf(p)) pix.Save(file, "png");
        }

        static void Pump(int ms)
        {
            var end = DateTime.UtcNow.AddMilliseconds(ms);
            while (DateTime.UtcNow < end)
            {
                while (Application.EventsPending()) Application.RunIteration();
                System.Threading.Thread.Sleep(10);
            }
        }

        [System.Runtime.InteropServices.DllImport("libgdk-3.so.0")]
        static extern IntPtr gdk_pixbuf_get_from_window(IntPtr window, int x, int y, int width, int height);
    }

    // Title bar background: dark strip, the brand panel with the red slash, the flowing RGB line; drag to move.
    // Decorated=false (own title bar, see TitleBarArea below) means the window manager draws no resize grips
    // of its own and gives the app no edge-drag resize at all unless it asks for one itself - which nothing
    // here did. TryBegin starts one by hand (the same BeginResizeDrag the WM would use), called from every
    // widget that covers one of the window's outer edges (TitleBarArea top, NavPanel left+bottom, Bloom
    // right+bottom) before their own click handling.
    static class ResizeEdge
    {
        public const int Margin = 6; // px from the window's outer edge that starts a resize drag

        public static bool TryBegin(Window win, Widget w, Gdk.EventButton ev)
        {
            if (ev.Button != 1 || win.Window == null) return false;
            if ((win.Window.State & (Gdk.WindowState.Maximized | Gdk.WindowState.Fullscreen)) != 0) return false;
            int wx, wy;
            w.TranslateCoordinates(win, (int)ev.X, (int)ev.Y, out wx, out wy);
            int winW, winH; win.GetSize(out winW, out winH);
            bool left = wx <= Margin, right = wx >= winW - Margin;
            bool top = wy <= Margin, bottom = wy >= winH - Margin;
            if (!left && !right && !top && !bottom) return false;
            Gdk.WindowEdge edge =
                top && left ? Gdk.WindowEdge.NorthWest : top && right ? Gdk.WindowEdge.NorthEast :
                bottom && left ? Gdk.WindowEdge.SouthWest : bottom && right ? Gdk.WindowEdge.SouthEast :
                top ? Gdk.WindowEdge.North : bottom ? Gdk.WindowEdge.South :
                left ? Gdk.WindowEdge.West : Gdk.WindowEdge.East;
            win.BeginResizeDrag(edge, (int)ev.Button, (int)ev.XRoot, (int)ev.YRoot, ev.Time);
            return true;
        }
    }

    class TitleBarArea : EventBox
    {
        readonly Window win;
        public Func<double> Shift = () => 0;
        public TitleBarArea(Window w)
        {
            win = w; VisibleWindow = false; AppPaintable = true;
            ButtonPressEvent += (o, a) =>
            {
                if (ResizeEdge.TryBegin(win, this, a.Event)) return;
                if (a.Event.Button != 1) return;
                if (a.Event.Type == Gdk.EventType.TwoButtonPress) { if ((win.Window.State & Gdk.WindowState.Maximized) != 0) win.Unmaximize(); else win.Maximize(); return; }
                win.BeginMoveDrag((int)a.Event.Button, (int)a.Event.XRoot, (int)a.Event.YRoot, a.Event.Time);
            };
        }

        protected override bool OnDrawn(Context cr)
        {
            double w = Allocation.Width, h = Allocation.Height;
            Theme.Set(cr, Theme.TitleBg); cr.Rectangle(0, 0, w, h); cr.Fill();
            cr.MoveTo(0, 0); cr.LineTo(252, 0); cr.LineTo(228, 48); cr.LineTo(0, 48); cr.ClosePath(); Theme.Set(cr, Theme.Panel); cr.Fill();
            cr.MoveTo(252, 0); cr.LineTo(264, 0); cr.LineTo(240, 48); cr.LineTo(228, 48); cr.ClosePath();
            Theme.Set(cr, Theme.Accent);
            cr.FillPreserve(); cr.LineWidth = 6; cr.SetSourceRGBA(1, 0.12, 0.29, 0.25); cr.Stroke();
            // RGB line: the rainbow repeats every half width and flows to the right
            double period = w / 2, off = Shift() * period;
            var stops = new[] { 0xFF1F4B, 0xFF8A00, 0xFFE600, 0x2BE36B, 0x00E1FF, 0x7A4BFF, 0xFF1F4B };
            for (double x0 = -period + off; x0 < w; x0 += period)
            {
                var g = Theme.RainbowGradient(x0, 0, x0 + period, 0, stops);
                cr.SetSource(g); cr.Rectangle(Math.Max(0, x0), h - 2, Math.Min(period, w - Math.Max(0, x0)), 2); cr.Fill(); g.Dispose();
            }
            return base.OnDrawn(cr);
        }
    }

    // Navigation panel background with its right hairline.
    class NavPanel : EventBox
    {
        public Widget Footer;
        uint widthAnim;
        public NavPanel(Window win) { VisibleWindow = false; AppPaintable = true; ButtonPressEvent += (o, a) => ResizeEdge.TryBegin(win, this, a.Event); }

        // Slides the collapse/expand the same way SwitchToggle/SegToggle animate elsewhere in this file
        // (200ms cubic ease) - WidthRequest snapped straight to the target had no transition at all.
        public void AnimateWidth(int target)
        {
            if (widthAnim != 0) GLib.Source.Remove(widthAnim);
            int from = WidthRequest;
            if (from == target) return;
            var start = DateTime.UtcNow;
            widthAnim = GLib.Timeout.Add(15, () =>
            {
                double k = Math.Min(1, (DateTime.UtcNow - start).TotalMilliseconds / 200);
                double e = k < 0.5 ? 4 * k * k * k : 1 - Math.Pow(-2 * k + 2, 3) / 2;
                WidthRequest = (int)Math.Round(from + (target - from) * e);
                if (k >= 1) { widthAnim = 0; return false; }
                return true;
            });
        }
        protected override bool OnDrawn(Context cr)
        {
            Theme.Set(cr, Theme.Panel); cr.Rectangle(0, 0, Allocation.Width, Allocation.Height); cr.Fill();
            Theme.Set(cr, Theme.Line); cr.Rectangle(Allocation.Width - 1, 0, 1, Allocation.Height); cr.Fill();
            return base.OnDrawn(cr);
        }
    }

    // Faint red bloom behind the content, as in the WPF window.
    class Bloom : EventBox
    {
        public Bloom(Window win) { VisibleWindow = false; AppPaintable = true; ButtonPressEvent += (o, a) => ResizeEdge.TryBegin(win, this, a.Event); }
        protected override bool OnDrawn(Context cr)
        {
            double w = Allocation.Width, h = Allocation.Height;
            var g = new RadialGradient(w * 0.15, 0, 0, w * 0.15, 0, Math.Max(w * 0.7, h * 0.6));
            g.AddColorStop(0, Theme.Hex(0xFF1F4B, 0.10)); g.AddColorStop(1, Theme.Hex(0xFF1F4B, 0));
            cr.SetSource(g); cr.Rectangle(0, 0, w, h); cr.Fill(); g.Dispose();
            return base.OnDrawn(cr);
        }
    }

    // The preview column's left hairline.
    class ColumnBorder : EventBox
    {
        public ColumnBorder(Widget child) { VisibleWindow = false; AppPaintable = true; Add(child); }
        protected override bool OnDrawn(Context cr)
        {
            Theme.Set(cr, Theme.Line); cr.Rectangle(0, 0, 1, Allocation.Height); cr.Fill();
            return base.OnDrawn(cr);
        }
    }

    // Rounded status card under the preview.
    class StatusCard : EventBox
    {
        public StatusCard() { VisibleWindow = false; AppPaintable = true; }
        protected override bool OnDrawn(Context cr)
        {
            Theme.RoundRect(cr, 0.5, 0.5, Allocation.Width - 1, Allocation.Height - 1, 3);
            Theme.Set(cr, Theme.Card); cr.FillPreserve();
            Theme.Set(cr, Theme.Line); cr.LineWidth = 1; cr.Stroke();
            return base.OnDrawn(cr);
        }
    }
}
