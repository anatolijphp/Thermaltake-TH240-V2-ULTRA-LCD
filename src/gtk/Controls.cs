// Linux front end: the remaining WPF-styled controls, drawn with Cairo: the segmented toggle with its sliding red pill,
// the slider with the diamond thumb, mode tiles, colour swatches and the flat buttons.
using System;
using System.Collections.Generic;
using Cairo;
using Gtk;

namespace TtLcd.GtkUi
{
    // Segmented toggle (WPF Seg rows): rounded track, the selected option on a red pill that slides (200 ms ease).
    class SegToggle : DrawingArea
    {
        readonly string[] options;
        readonly double[] xs, ws;
        int selected = -1; double pillX, pillW; uint anim;
        public event Action<int> Changed;
        const double Pad = 3, H = 30;

        public SegToggle(params string[] options)
        {
            this.options = options;
            xs = new double[options.Length]; ws = new double[options.Length];
            double x = Pad;
            for (int i = 0; i < options.Length; i++)
            {
                int w, h;
                using (var l = CreatePangoLayout(options[i])) { l.FontDescription = Pango.FontDescription.FromString(Theme.FontName + " 9.5"); l.GetPixelSize(out w, out h); }
                xs[i] = x; ws[i] = w + 28; x += ws[i];
            }
            SetSizeRequest((int)(x + Pad), (int)H);
            Halign = Align.Start; Valign = Align.Center;
            Events |= Gdk.EventMask.ButtonPressMask;
            ButtonPressEvent += (o, a) =>
            {
                for (int i = 0; i < options.Length; i++)
                    if (a.Event.X >= xs[i] && a.Event.X < xs[i] + ws[i] && i != selected)
                    {
                        Select(i, true);
                        if (Changed != null) Changed(i);
                    }
            };
        }

        public int Selected { get { return selected; } }

        public void Select(int i, bool animate)
        {
            if (i < 0 || i >= options.Length) return;
            double fx = pillX, fw = pillW;
            selected = i;
            if (anim != 0) { GLib.Source.Remove(anim); anim = 0; }
            if (!animate || fw <= 0) { pillX = xs[i]; pillW = ws[i]; QueueDraw(); return; }
            var start = DateTime.UtcNow; double tx = xs[i], tw = ws[i];
            anim = GLib.Timeout.Add(15, () =>
            {
                double k = Math.Min(1, (DateTime.UtcNow - start).TotalMilliseconds / 200);
                double e = 1 - Math.Pow(1 - k, 3);
                pillX = fx + (tx - fx) * e; pillW = fw + (tw - fw) * e;
                QueueDraw();
                if (k >= 1) { anim = 0; return false; }
                return true;
            });
        }

        protected override bool OnDrawn(Context cr)
        {
            double w = Allocation.Width;
            Theme.RoundRect(cr, 0.5, 0.5, w - 1, H - 1, (H - 1) / 2);
            Theme.Set(cr, Theme.Card); cr.FillPreserve();
            Theme.Set(cr, Theme.Line); cr.LineWidth = 1; cr.Stroke();
            if (selected >= 0 && pillW > 0)
            {
                Theme.RoundRect(cr, pillX, Pad, pillW, H - 2 * Pad, (H - 2 * Pad) / 2);
                for (int i = 3; i >= 1; i--) { cr.LineWidth = i * 3; cr.SetSourceRGBA(1, 0.12, 0.29, 0.06); cr.StrokePreserve(); }
                Theme.Set(cr, Theme.Accent); cr.Fill();
            }
            for (int i = 0; i < options.Length; i++)
            {
                double centre = xs[i] + ws[i] / 2;
                bool on = selected >= 0 && Math.Abs(centre - (pillX + pillW / 2)) < ws[i] / 2;
                Theme.TextCentred(cr, options[i], Theme.FontName + " 9.5", on ? Theme.Hex(0xFFFFFF) : Theme.Dim, centre, H / 2);
            }
            return true;
        }
    }

    // Slider (WPF Slider style): thin track, red filled part with a glow, white diamond thumb with a red edge.
    class Slider : DrawingArea
    {
        public double Min, Max, Step;
        double value;
        bool dragging;
        public event Action<double> Changed;

        public Slider(double min, double max, double step)
        {
            Min = min; Max = max; Step = step;
            HeightRequest = 30; Hexpand = true; Valign = Align.Center;
            Events |= Gdk.EventMask.ButtonPressMask | Gdk.EventMask.ButtonReleaseMask | Gdk.EventMask.PointerMotionMask | Gdk.EventMask.ScrollMask;
            ButtonPressEvent += (o, a) => { dragging = true; SetFromX(a.Event.X); };
            ButtonReleaseEvent += (o, a) => dragging = false;
            MotionNotifyEvent += (o, a) => { if (dragging) SetFromX(a.Event.X); };
            ScrollEvent += (o, a) =>
            {
                double d = Step > 0 ? Step : (Max - Min) / 100;
                Set(value + (a.Event.Direction == Gdk.ScrollDirection.Up ? d : a.Event.Direction == Gdk.ScrollDirection.Down ? -d : 0), true);
            };
        }

        public double Value { get { return value; } }

        void SetFromX(double x)
        {
            double k = (x - 8) / Math.Max(1, Allocation.Width - 16);
            Set(Min + Math.Max(0, Math.Min(1, k)) * (Max - Min), true);
        }

        public void Set(double v, bool notify)
        {
            if (Step > 0) v = Math.Round(v / Step) * Step;
            v = Math.Max(Min, Math.Min(Max, v));
            if (Math.Abs(v - value) < 1e-9 && notify) return;
            value = v;
            QueueDraw();
            if (notify && Changed != null) Changed(v);
        }

        protected override bool OnDrawn(Context cr)
        {
            double w = Allocation.Width, cy = Allocation.Height / 2.0;
            double k = (value - Min) / Math.Max(1e-9, Max - Min), x = 8 + k * (w - 16);
            cr.Rectangle(8, cy - 1.5, w - 16, 3); Theme.Set(cr, Theme.Hex(0x2A2A2E)); cr.Fill();
            var g = new LinearGradient(8, 0, Math.Max(9, x), 0);
            g.AddColorStop(0, Theme.Hex(0x8A1028)); g.AddColorStop(1, Theme.Accent);
            cr.Rectangle(8, cy - 1.5, x - 8, 3); cr.SetSource(g); cr.Fill(); g.Dispose();
            // diamond thumb
            cr.Save(); cr.Translate(x, cy); cr.Rotate(Math.PI / 4);
            cr.Rectangle(-5.5, -5.5, 11, 11);
            for (int i = 3; i >= 1; i--) { cr.LineWidth = 2 + i * 3; cr.SetSourceRGBA(1, 0.12, 0.29, 0.07); cr.StrokePreserve(); }
            cr.SetSourceRGB(1, 1, 1); cr.FillPreserve();
            Theme.Set(cr, Theme.Accent); cr.LineWidth = 2; cr.Stroke();
            cr.Restore();
            return true;
        }
    }

    // Mode tile (WPF Tile): icon over the name; the selected one is red with a glowing edge, a red corner and the
    // RGB line along the bottom.
    class Tile : EventBox
    {
        readonly string text, icon; bool on, hover;
        public event System.Action Clicked;
        public Tile(string text, string icon)
        {
            this.text = text; this.icon = icon;
            VisibleWindow = false; AppPaintable = true;
            HeightRequest = 94; Hexpand = true;
            Events |= Gdk.EventMask.EnterNotifyMask | Gdk.EventMask.LeaveNotifyMask;
            EnterNotifyEvent += (o, a) => { hover = true; QueueDraw(); };
            LeaveNotifyEvent += (o, a) => { hover = false; QueueDraw(); };
            ButtonPressEvent += (o, a) => { if (Clicked != null) Clicked(); };
        }

        public bool On { get { return on; } set { on = value; QueueDraw(); } }

        protected override bool OnDrawn(Context cr)
        {
            double w = Allocation.Width, h = Allocation.Height;
            cr.Rectangle(0.5, 0.5, w - 1, h - 1);
            if (on)
            {
                var g = new LinearGradient(0, 0, 0, h);
                g.AddColorStop(0, Theme.Hex(0x5A0E20)); g.AddColorStop(1, Theme.Hex(0x2A0710));
                cr.SetSource(g); cr.FillPreserve(); g.Dispose();
                for (int i = 3; i >= 1; i--) { cr.LineWidth = 1 + i * 3; cr.SetSourceRGBA(1, 0.12, 0.29, 0.06); cr.StrokePreserve(); }
                Theme.Set(cr, Theme.Accent); cr.LineWidth = 1; cr.Stroke();
                cr.MoveTo(0, 0); cr.LineTo(12, 0); cr.LineTo(0, 12); cr.ClosePath(); Theme.Set(cr, Theme.Accent); cr.Fill();
                var rgb = Theme.RainbowGradient(1, 0, w - 1, 0, Theme.Rainbow);
                cr.Rectangle(1, h - 3, w - 2, 2); cr.SetSource(rgb); cr.Fill(); rgb.Dispose();
            }
            else
            {
                Theme.Set(cr, hover ? Theme.CardHi : Theme.Card); cr.FillPreserve();
                Theme.Set(cr, hover ? Theme.LineHi : Theme.Line); cr.LineWidth = 1; cr.Stroke();
            }
            Icons.Draw(cr, icon, w / 2 - 14, 22, 28, on ? Theme.Accent : Theme.Dim);
            Theme.TextCentred(cr, text, Theme.FontName + " 9.5", on ? Theme.Text : Theme.Dim, w / 2, 68);
            return true;
        }
    }

    // Colour swatches (WPF Swatch buttons) plus a rainbow "custom colour" one.
    class Swatches : DrawingArea
    {
        public event Action<int> Picked;
        public Func<int> Current = () => -1;
        const int D = 22, Gap = 10;
        public Swatches()
        {
            SetSizeRequest((StudioModel.Palette.Length + 1) * (D + Gap), D + 6);
            Halign = Align.Start; Valign = Align.Center;
            Events |= Gdk.EventMask.ButtonPressMask;
            ButtonPressEvent += (o, a) =>
            {
                int i = (int)((a.Event.X - 2) / (D + Gap));
                if (i < StudioModel.Palette.Length) { if (Picked != null) Picked(StudioModel.Palette[i]); }
                else if (i == StudioModel.Palette.Length) PickCustom();
                QueueDraw();
            };
        }

        void PickCustom()
        {
            var d = new ColorSelectionDialog("Custom colour");
            int cur = Math.Max(0, Current());
            d.ColorSelection.CurrentColor = new Gdk.Color((byte)(cur >> 16), (byte)(cur >> 8), (byte)cur);
            if (d.Run() == (int)ResponseType.Ok)
            {
                var c = d.ColorSelection.CurrentColor;
                int v = (c.Red >> 8) << 16 | (c.Green >> 8) << 8 | c.Blue >> 8;
                if (Picked != null) Picked(v);
            }
            d.Destroy();
        }

        protected override bool OnDrawn(Context cr)
        {
            int cur = Current();
            for (int i = 0; i <= StudioModel.Palette.Length; i++)
            {
                double cx = 2 + i * (D + Gap) + D / 2.0, cy = Allocation.Height / 2.0;
                cr.NewPath(); cr.Arc(cx, cy, D / 2.0, 0, 2 * Math.PI);
                if (i < StudioModel.Palette.Length) Theme.Set(cr, Theme.Hex(StudioModel.Palette[i]));
                else { var g = Theme.RainbowGradient(cx - D / 2.0, 0, cx + D / 2.0, 0, Theme.Rainbow); cr.SetSource(g); g.Dispose(); }
                cr.FillPreserve();
                bool sel = i < StudioModel.Palette.Length && StudioModel.Palette[i] == cur;
                Theme.Set(cr, sel ? Theme.Accent : Theme.Line); cr.LineWidth = sel ? 2 : 1; cr.Stroke();
            }
            return true;
        }
    }

    static class Ui
    {
        public static Button Button(string text, bool accent, System.Action click)
        {
            var b = new Button(text);
            b.StyleContext.AddClass(accent ? "accent" : "btn");
            b.Clicked += (o, a) => click();
            return b;
        }

        public static Label Text(string text, params string[] classes)
        {
            var l = new Label(text) { Halign = Align.Start, Xalign = 0, Wrap = true };
            foreach (var c in classes) l.StyleContext.AddClass(c);
            return l;
        }

        // A labelled row (WPF Grid rows: 100 px label, control, value): 44 px high.
        public static Box Row(string label, Widget control, Label value = null, int labelWidth = 100)
        {
            var row = new Box(Orientation.Horizontal, 0) { HeightRequest = 44 };
            var l = new Label(label) { Halign = Align.Start, Xalign = 0, WidthRequest = labelWidth, Valign = Align.Center };
            l.StyleContext.AddClass("dim");
            row.PackStart(l, false, false, 0);
            row.PackStart(control, value == null ? false : true, value != null || control.Hexpand, 0);
            if (value != null) { value.WidthRequest = 56; value.Xalign = 1; value.StyleContext.AddClass("semibold"); row.PackStart(value, false, false, 0); }
            return row;
        }

        public static Widget Hair()
        {
            var d = new DrawingArea { HeightRequest = 1, MarginTop = 18 };
            d.Drawn += (o, a) => { Theme.Set(a.Cr, Theme.Line); a.Cr.Rectangle(0, 0, d.Allocation.Width, 1); a.Cr.Fill(); };
            return d;
        }

        public static Label Section(string text, bool first)
        {
            var l = new Label(text) { Halign = Align.Start, MarginTop = first ? 0 : 18, MarginBottom = 12 };
            l.StyleContext.AddClass("caption");
            return l;
        }

        public static string ChooseFile(Window parent, string title, string current, string filterName, params string[] patterns)
        {
            var d = new FileChooserDialog(title, parent, FileChooserAction.Open, "Cancel", ResponseType.Cancel, "Open", ResponseType.Accept);
            var f = new FileFilter { Name = filterName };
            foreach (var p in patterns) { f.AddPattern(p); f.AddPattern(p.ToUpperInvariant()); }
            d.AddFilter(f);
            if (!string.IsNullOrEmpty(current) && System.IO.File.Exists(current)) d.SetFilename(current);
            string result = d.Run() == (int)ResponseType.Accept ? d.Filename : null;
            d.Destroy();
            return result;
        }
    }
}
