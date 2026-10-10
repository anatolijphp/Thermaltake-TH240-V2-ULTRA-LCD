// Dev aid: TTLcdStudio --record <out.gif> <scene> plays a scripted scene in the real window and saves it as an animated
// GIF for the README (docs/media). The window is shown off-screen so layout, storyboards and the live preview run as on a
// desktop, but nothing touches the user's screen or mouse; frames come from RenderTargetBitmap and the cursor is drawn in.
// Scenes: dashboard (navigation tour), modes, style. They change the mode and styles like a user would; restore config.xml afterwards.
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Threading;

namespace TtLcd
{
    partial class MainUi
    {
        class Step
        {
            public int At, Dur;
            public Func<Point> To;          // cursor glide target
            public Action Do;               // one-off action (a click if Click is set)
            public Action<double> Tween;    // called with 0..1 over Dur
            public bool Click, Started, Done;
            public Point From, Target;
        }

        public void Record(string file, string scene, double scale, Action finished)
        {
            var root = (FrameworkElement)w.Content;
            var steps = new List<Step>();
            Point cursor = new Point(760, 700);
            int length;
            Action setup;
            Func<Step, Step> add = s => { steps.Add(s); return s; };
            Func<string, Func<Point>> el = name => () => CentreOf(F<FrameworkElement>(name), root);

            switch (scene)
            {
                case "modes":
                    Action gifSample = () =>
                    {
                        // GIF mode needs a file; TTLCD_REC_GIF points at a sample for the recording
                        var g = Environment.GetEnvironmentVariable("TTLCD_REC_GIF");
                        if (!string.IsNullOrEmpty(g)) { Change(c => c.GifPath = g); F<TextBox>("GifPath").Text = g; }
                    };
                    setup = () => { gifSample(); F<RadioButton>("NavModes").IsChecked = true; F<RadioButton>("ModeClock").IsChecked = true; F<ScrollViewer>("PageScroll").ScrollToTop(); };
                    cursor = new Point(560, 760);
                    add(new Step { At = 500, Dur = 600, To = el("ModeVirtualDisplay") });
                    add(new Step { At = 1250, Click = true, Do = () => F<RadioButton>("ModeVirtualDisplay").IsChecked = true });
                    add(new Step { At = 3500, Dur = 600, To = el("ModeAura") });
                    add(new Step { At = 4200, Click = true, Do = () => F<RadioButton>("ModeAura").IsChecked = true });
                    add(new Step { At = 6400, Dur = 600, To = el("ModeGif") });
                    add(new Step { At = 7100, Click = true, Do = () => F<RadioButton>("ModeGif").IsChecked = true });
                    length = 9600;
                    break;

                case "dashboard":
                    // a tour through the main navigation
                    setup = () => { F<RadioButton>("ModeAura").IsChecked = true; F<RadioButton>("NavDashboard").IsChecked = true; F<ScrollViewer>("PageScroll").ScrollToTop(); };
                    cursor = new Point(1000, 690);
                    int at = 2200;
                    foreach (var nav in new[] { "NavModes", "NavDevice", "NavSettings", "NavDashboard" })
                    {
                        string n = nav;
                        add(new Step { At = at, Dur = 600, To = el(n) });
                        add(new Step { At = at + 750, Click = true, Do = () => { F<ScrollViewer>("PageScroll").ScrollToTop(); F<RadioButton>(n).IsChecked = true; } });
                        at += 2400;
                    }
                    add(new Step { At = at - 1200, Dur = 700, To = () => new Point(1000, 690) });
                    length = at;
                    break;

                case "style":
                    // Aura Sync with the overlay: switch the time on, select it, make it bigger and give it a neon glow
                    Func<ItemList> ov = () => itemLists.First(x => x.Panel == "OvItems");
                    setup = () =>
                    {
                        F<RadioButton>("NavModes").IsChecked = true; F<RadioButton>("ModeAura").IsChecked = true;
                        w.Dispatcher.BeginInvoke(new Action(() => ScrollToCentre(F<FrameworkElement>("OvItems"), () => { })), DispatcherPriority.Loaded);
                    };
                    cursor = new Point(700, 300);
                    add(new Step { At = 400, Dur = 700, To = () => { var p = CentreOf(Chip("OvItems", "Time"), root); var c = Chip("OvItems", "Time"); return new Point(p.X - c.ActualWidth / 2 - 13, p.Y); } });
                    add(new Step { At = 1300, Click = true, Do = () => { Change(c => c.OverlayTime = true); SetPanels(); BuildItems(ov()); } });
                    add(new Step { At = 2300, Dur = 500, To = () => CentreOf(Chip("OvItems", "Time"), root) });
                    add(new Step { At = 3000, Click = true, Do = () =>
                    {
                        selList = ov(); selId = 0;
                        BuildItems(ov()); ShowStyleEditor();
                        w.Dispatcher.BeginInvoke(new Action(() => ScrollToCentre(F<FrameworkElement>("OptText"), () => { })), DispatcherPriority.Loaded);
                    } });
                    double v0 = 0, v1 = 0;
                    add(new Step { At = 4300, Dur = 600, To = () => ThumbOf(F<Slider>("StSize"), root) });
                    add(new Step { At = 5000, Do = () => { v0 = F<Slider>("StSize").Value; v1 = Math.Min(F<Slider>("StSize").Maximum, Math.Round(v0 * 2.2)); } });
                    add(new Step { At = 5100, Dur = 1300, Tween = k => { F<Slider>("StSize").Value = Math.Round(v0 + (v1 - v0) * Ease(k)); } });
                    add(new Step { At = 7000, Dur = 600, To = el("FxGlow") });
                    add(new Step { At = 7700, Click = true, Do = () => F<RadioButton>("FxGlow").IsChecked = true });
                    length = 10200;
                    break;
                case "game":
                    // Settings → Game mode: turn detection on, pick "lower frame rate", force game mode on and back off
                    setup = () =>
                    {
                        F<RadioButton>("ModeAura").IsChecked = true; F<RadioButton>("NavSettings").IsChecked = true;
                        w.Dispatcher.BeginInvoke(new Action(() => ScrollToCentre(F<FrameworkElement>("GmOptions"), () => { })), DispatcherPriority.Loaded);
                    };
                    cursor = new Point(700, 640);
                    add(new Step { At = 400, Dur = 600, To = el("GmOn") });
                    add(new Step { At = 1100, Click = true, Do = () => F<CheckBox>("GmOn").IsChecked = true });
                    add(new Step { At = 2200, Dur = 600, To = el("GmActLowerFps") });
                    add(new Step { At = 2900, Click = true, Do = () => F<RadioButton>("GmActLowerFps").IsChecked = true });
                    add(new Step { At = 3900, Dur = 600, To = el("GmForce") });
                    add(new Step { At = 4600, Click = true, Do = () => F<CheckBox>("GmForce").IsChecked = true });
                    add(new Step { At = 5400, Dur = 900, To = el("GmStatus") });
                    add(new Step { At = 8600, Dur = 700, To = el("GmForce") });
                    add(new Step { At = 9400, Click = true, Do = () => F<CheckBox>("GmForce").IsChecked = false });
                    length = 11400;
                    break;

                default: throw new ArgumentException("unknown scene: " + scene);
            }

            // off-screen but shown, so everything renders and animates as usual
            w.ShowInTaskbar = false; w.ShowActivated = false;
            w.WindowStartupLocation = WindowStartupLocation.Manual; w.Left = -12000; w.Top = 0;
            w.Show();
            setup();

            int width = (int)Math.Round(w.ActualWidth * scale), height = (int)Math.Round(w.ActualHeight * scale);
            var gif = new GifWriter(width, height);
            var arrow = Geometry.Parse("M0,0 L0,18 L4.6,13.8 L7.6,20.6 L10.6,19.4 L7.6,12.6 L13.4,12.6 Z");
            var clicks = new List<KeyValuePair<long, Point>>();
            const int fps = 12;
            Stopwatch clock = null;
            long last = 0;
            var timer = new DispatcherTimer(DispatcherPriority.Render) { Interval = TimeSpan.FromMilliseconds(1000.0 / fps) };
            // let the setup (page, mode, scroll) settle before the clock starts
            var settle = new DispatcherTimer { Interval = TimeSpan.FromSeconds(1.5) };
            settle.Tick += (s, e) => { settle.Stop(); clock = Stopwatch.StartNew(); timer.Start(); };
            settle.Start();

            timer.Tick += (s, e) =>
            {
                long t = clock.ElapsedMilliseconds;
                foreach (var st in steps)
                {
                    if (st.Done || t < st.At) continue;
                    if (st.To != null)
                    {
                        if (!st.Started) { st.Started = true; st.From = cursor; st.Target = st.To(); }
                        double k = Math.Min(1, (t - st.At) / (double)Math.Max(1, st.Dur));
                        double q = Ease(k);
                        cursor = new Point(st.From.X + (st.Target.X - st.From.X) * q, st.From.Y + (st.Target.Y - st.From.Y) * q);
                        if (k >= 1) st.Done = true;
                    }
                    else if (st.Tween != null)
                    {
                        double k = Math.Min(1, (t - st.At) / (double)Math.Max(1, st.Dur));
                        st.Tween(k);
                        var s2 = F<Slider>("StSize");
                        if (s2.IsVisible) cursor = ThumbOf(s2, root);
                        if (k >= 1) st.Done = true;
                    }
                    else
                    {
                        st.Done = true;
                        if (st.Click) clicks.Add(new KeyValuePair<long, Point>(t, cursor));
                        st.Do();
                    }
                }
                root.UpdateLayout();

                var dv = new DrawingVisual();
                using (var dc = dv.RenderOpen())
                {
                    dc.PushTransform(new ScaleTransform(scale, scale));
                    dc.DrawRectangle(w.Background, null, new Rect(0, 0, w.ActualWidth, w.ActualHeight));
                    dc.DrawRectangle(new VisualBrush(root), null, new Rect(0, 0, root.ActualWidth, root.ActualHeight));
                    foreach (var c in clicks)
                    {
                        double age = (t - c.Key) / 400.0;
                        if (age > 1) continue;
                        var pen = new Pen(new SolidColorBrush(Color.FromArgb((byte)(220 * (1 - age)), 255, 255, 255)), 2);
                        dc.DrawEllipse(null, pen, c.Value, 6 + 16 * age, 6 + 16 * age);
                    }
                    dc.PushTransform(new TranslateTransform(cursor.X, cursor.Y));
                    dc.DrawGeometry(Brushes.White, new Pen(Brushes.Black, 1.2), arrow);
                    dc.Pop();
                    dc.Pop();
                }
                var rtb = new RenderTargetBitmap(width, height, 96, 96, PixelFormats.Pbgra32);
                rtb.Render(dv);
                var px = new byte[width * height * 4];
                rtb.CopyPixels(px, width * 4, 0);
                long now = clock.ElapsedMilliseconds;
                gif.Add(px, (int)Math.Max(2, (now - last) / 10));
                last = now;

                if (t >= length)
                {
                    timer.Stop();
                    gif.HoldLast(80);
                    new Thread(() => { gif.Save(file); w.Dispatcher.BeginInvoke(finished); }) { IsBackground = true }.Start();
                }
            };
        }

        static double Ease(double k) { return k < 0.5 ? 4 * k * k * k : 1 - Math.Pow(-2 * k + 2, 3) / 2; }

        static Point CentreOf(FrameworkElement e, FrameworkElement root)
        {
            return e.TranslatePoint(new Point(e.ActualWidth / 2, e.ActualHeight / 2), root);
        }

        static Point ThumbOf(Slider s, FrameworkElement root)
        {
            double k = (s.Value - s.Minimum) / Math.Max(1e-9, s.Maximum - s.Minimum);
            return s.TranslatePoint(new Point(8 + k * (s.ActualWidth - 16), s.ActualHeight / 2), root);
        }

        static Point LinkCentre(Hyperlink link, FrameworkElement root)
        {
            var owner = (FrameworkElement)link.Parent;
            while (!(owner is TextBlock) && owner != null) owner = owner.Parent as FrameworkElement;
            Rect a = link.ContentStart.GetCharacterRect(LogicalDirection.Forward), b = link.ContentEnd.GetCharacterRect(LogicalDirection.Backward);
            return owner.TranslatePoint(new Point((a.Left + b.Right) / 2, (a.Top + a.Bottom) / 2), root);
        }

        // the chip (reading) in an item list whose label is `text`
        FrameworkElement Chip(string panel, string text)
        {
            var stack = new Stack<DependencyObject>();
            stack.Push(F<FrameworkElement>(panel));
            while (stack.Count > 0)
            {
                var d = stack.Pop();
                var tb = d as TextBlock;
                if (tb != null && tb.Text == text) return tb;
                for (int i = 0; i < VisualTreeHelper.GetChildrenCount(d); i++) stack.Push(VisualTreeHelper.GetChild(d, i));
            }
            return F<FrameworkElement>(panel);
        }
    }
}
