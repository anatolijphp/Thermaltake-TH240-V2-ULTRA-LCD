// Linux front end: the custom-drawn controls that GTK CSS can't express (cards with the cut corner, gauges, the round
// preview with its rainbow ring, the animated switch, nav items) and the small vector icons used instead of Segoe's font.
using System;
using Cairo;
using Gtk;

namespace TtLcd.GtkUi
{
    // CardBox: a vertical box painted as the WPF card, content inset 20/18 px.
    class CardBox : Box
    {
        public CardBox() : base(Orientation.Vertical, 0)
        {
            MarginBottom = 14;
            AppPaintable = true;
        }

        public Box Inner()
        {
            var b = new Box(Orientation.Vertical, 0) { MarginLeft = 20, MarginRight = 20, MarginTop = 18, MarginBottom = 18 };
            PackStart(b, true, true, 0);
            return b;
        }

        protected override bool OnDrawn(Context cr)
        {
            Theme.PaintCard(cr, Allocation.Width, Allocation.Height);
            return base.OnDrawn(cr);
        }
    }

    // Round 270° gauge (DbWater etc.): track, value arc with glow, icon, value, label.
    class GaugeArea : DrawingArea
    {
        public string Value = "--", Label, Icon; public double Frac; public Color Col;
        public GaugeArea(string label, string icon, int rgb)
        {
            Label = label; Icon = icon; Col = Theme.Hex(rgb);
            SetSizeRequest(180, 180);
        }

        public void Set(string value, double frac) { Value = value; Frac = Math.Max(0, Math.Min(1, frac)); QueueDraw(); }

        protected override bool OnDrawn(Context cr)
        {
            double size = Math.Min(Allocation.Width, Allocation.Height), k = size / 156.0;
            cr.Translate((Allocation.Width - size) / 2, (Allocation.Height - size) / 2);
            cr.Scale(k, k);
            double c = 78, r = c - 5.5 - 6, a0 = 135 * Math.PI / 180;
            cr.LineCap = LineCap.Round;
            cr.NewPath(); cr.Arc(c, c, r, a0, a0 + 270 * Math.PI / 180);
            Theme.Set(cr, Theme.Hex(0x242427)); cr.LineWidth = 11; cr.Stroke();
            if (Frac > 0.004)
            {
                cr.NewPath(); cr.Arc(c, c, r, a0, a0 + 270 * Math.Min(Frac, 0.9999) * Math.PI / 180);
                Theme.GlowStroke(cr, Col, 11, 14, 0.85);
            }
            Icons.Draw(cr, Icon, c - 7.5, 34, 15, Col);
            Theme.TextCentred(cr, Value, Theme.FontName + " Bold 25", Theme.Text, c, c + 5);
            int w, h;
            Theme.DrawText(cr, Label, Theme.FontName + " Bold 8", Col, 0, 0, out w, out h, true);
            Theme.DrawText(cr, Label, Theme.FontName + " Bold 8", Col, c - w / 2.0, 156 - 14 - h, out w, out h);
            return true;
        }
    }

    // The round live preview: animated rainbow ring with a red glow, the latest frame clipped to the circle and
    // turned like the LCD on this mount.
    class RoundPreview : DrawingArea
    {
        Gdk.Pixbuf frame; double angle; double ringTurn;
        public RoundPreview()
        {
            SetSizeRequest(344, 344);
            GLib.Timeout.Add(50, () => { ringTurn = (ringTurn + 360.0 / 160) % 360; QueueDraw(); return true; }); // 8 s per turn
        }

        public void SetFrame(byte[] jpg, double turn)
        {
            try
            {
                var p = new Gdk.Pixbuf(jpg);
                var old = frame;
                frame = p.ScaleSimple(328, 328, Gdk.InterpType.Bilinear);
                p.Dispose();
                if (old != null) old.Dispose();
            }
            catch (Exception) { }
            angle = turn;
            QueueDraw();
        }

        protected override bool OnDrawn(Context cr)
        {
            double cx = Allocation.Width / 2.0, cy = Allocation.Height / 2.0;
            // ring: rotating gradient, glow underneath
            cr.NewPath(); cr.Arc(cx, cy, 170, 0, 2 * Math.PI);
            for (int i = 5; i >= 1; i--) { cr.LineWidth = 4 + i * 6; cr.SetSourceRGBA(1, 0.12, 0.29, 0.05); cr.StrokePreserve(); }
            double t = ringTurn * Math.PI / 180, dx = Math.Cos(t) * 172, dy = Math.Sin(t) * 172;
            var g = Theme.RainbowGradient(cx - dx, cy - dy, cx + dx, cy + dy, new[] { 0xFF1F4B, 0xFFE600, 0x00E1FF, 0x7A4BFF, 0xFF2BD6 });
            cr.SetSource(g); cr.LineWidth = 4; cr.Stroke(); g.Dispose();
            // screen
            cr.NewPath(); cr.Arc(cx, cy, 164, 0, 2 * Math.PI);
            cr.SetSourceRGB(0, 0, 0); cr.FillPreserve();
            if (frame != null)
            {
                cr.Save();
                cr.Clip();
                cr.Translate(cx, cy); cr.Rotate(angle * Math.PI / 180);
                Gdk.CairoHelper.SetSourcePixbuf(cr, frame, -164, -164);
                cr.Paint();
                cr.Restore();
            }
            cr.NewPath();
            return true;
        }
    }

    // iOS-style switch (the WPF Switch): 40x20 track, sliding knob, 200 ms ease. Optional label to the right.
    class SwitchToggle : Box
    {
        readonly DrawingArea track = new DrawingArea();
        bool active; double pos; uint anim;
        public event Action<bool> Toggled;

        public SwitchToggle(string label) : base(Orientation.Horizontal, 10)
        {
            track.SetSizeRequest(40, 20);
            track.Valign = Align.Center;
            track.Drawn += (o, a) => DrawTrack(a.Cr);
            var ev = new EventBox { VisibleWindow = false };
            var inner = new Box(Orientation.Horizontal, 10);
            inner.PackStart(track, false, false, 0);
            if (label != null) inner.PackStart(new Label(label), false, false, 0);
            ev.Add(inner);
            ev.ButtonPressEvent += (o, a) => { Active = !active; if (Toggled != null) Toggled(active); };
            PackStart(ev, false, false, 0);
        }

        public bool Active
        {
            get { return active; }
            set
            {
                if (value == active) return;
                active = value;
                if (anim != 0) GLib.Source.Remove(anim);
                var start = DateTime.UtcNow; double from = pos, to = active ? 1 : 0;
                anim = GLib.Timeout.Add(15, () =>
                {
                    double k = Math.Min(1, (DateTime.UtcNow - start).TotalMilliseconds / 200);
                    double e = k < 0.5 ? 4 * k * k * k : 1 - Math.Pow(-2 * k + 2, 3) / 2;
                    pos = from + (to - from) * e;
                    track.QueueDraw();
                    if (k >= 1) { anim = 0; return false; }
                    return true;
                });
            }
        }

        public void SetSilently(bool v) { active = v; pos = v ? 1 : 0; track.QueueDraw(); }

        void DrawTrack(Context cr)
        {
            Theme.RoundRect(cr, 0.5, 0.5, 39, 19, 9.5);
            var off = Theme.Hex(0x2A2A2E); var on = Theme.Accent;
            cr.SetSourceRGBA(off.R + (on.R - off.R) * pos, off.G + (on.G - off.G) * pos, off.B + (on.B - off.B) * pos, 1);
            cr.Fill();
            double x = 3 + pos * 20;
            cr.NewPath(); cr.Arc(x + 7, 10, 7, 0, 2 * Math.PI);
            Theme.Set(cr, pos > 0.5 ? Theme.Hex(0xFFFFFF) : Theme.Hex(0xCFCFD2)); cr.Fill();
        }
    }

    // Left navigation item: icon + label; the selected one has the red bar with glow and a red-to-dark wash.
    class NavItem : EventBox
    {
        readonly string icon; readonly Label label;
        bool on, hover;
        public event System.Action Clicked;
        public bool Collapsed;

        public NavItem(string text, string icon)
        {
            this.icon = icon;
            VisibleWindow = false;
            AppPaintable = true;
            HeightRequest = 50;
            TooltipText = text;
            label = new Label(text) { Halign = Align.Start, MarginLeft = 56 };
            label.StyleContext.AddClass("navlabel");
            Add(label);
            Events |= Gdk.EventMask.EnterNotifyMask | Gdk.EventMask.LeaveNotifyMask;
            EnterNotifyEvent += (o, a) => { hover = true; QueueDraw(); };
            LeaveNotifyEvent += (o, a) => { hover = false; QueueDraw(); };
            ButtonPressEvent += (o, a) => { if (Clicked != null) Clicked(); };
        }

        public bool On
        {
            get { return on; }
            set
            {
                on = value;
                if (on) label.StyleContext.AddClass("on"); else label.StyleContext.RemoveClass("on");
                QueueDraw();
            }
        }

        public void SetCollapsed(bool c) { Collapsed = c; label.Visible = !c; }

        protected override bool OnDrawn(Context cr)
        {
            double w = Allocation.Width, h = Allocation.Height;
            if (on)
            {
                var g = new LinearGradient(0, 0, w, 0);
                g.AddColorStop(0, Theme.Hex(0xFF1F4B, 0.22)); g.AddColorStop(1, Theme.Hex(0xFF1F4B, 0));
                cr.SetSource(g); cr.Rectangle(0, 0, w, h); cr.Fill(); g.Dispose();
                cr.Rectangle(0, 0, 3, h); cr.SetSourceRGBA(1, 0.12, 0.29, 0.25); cr.Fill();
                cr.Rectangle(0, 0, 3, h); Theme.Set(cr, Theme.Accent); cr.Fill();
            }
            else if (hover) { cr.Rectangle(0, 0, w, h); cr.SetSourceRGBA(1, 1, 1, 0.04); cr.Fill(); }
            Icons.Draw(cr, icon, 24, (h - 18) / 2, 18, on ? Theme.Accent : Theme.Dim);
            return base.OnDrawn(cr);
        }
    }

    // Small icon button (title bar, rotate): draws an icon, optional round border, hover highlight.
    class IconButton : EventBox
    {
        public string Icon; public bool Round, Checked; bool hover;
        public event System.Action Clicked;
        public double IconSize = 14;

        public IconButton(string icon, int w, int h, string tip)
        {
            Icon = icon; VisibleWindow = false; AppPaintable = true; TooltipText = tip;
            SetSizeRequest(w, h);
            Events |= Gdk.EventMask.EnterNotifyMask | Gdk.EventMask.LeaveNotifyMask;
            EnterNotifyEvent += (o, a) => { hover = true; QueueDraw(); };
            LeaveNotifyEvent += (o, a) => { hover = false; QueueDraw(); };
            ButtonPressEvent += (o, a) => { if (Clicked != null) Clicked(); a.RetVal = true; };
        }

        protected override bool OnDrawn(Context cr)
        {
            double w = Allocation.Width, h = Allocation.Height;
            var col = Checked || (Round && hover) ? Theme.Accent : Round ? Theme.Text : Theme.Dim;
            if (Round)
            {
                cr.NewPath(); cr.Arc(w / 2, h / 2, Math.Min(w, h) / 2 - 1, 0, 2 * Math.PI);
                Theme.Set(cr, Theme.Hex(0x131314, 0.9)); cr.FillPreserve();
                Theme.Set(cr, hover ? Theme.Accent : Theme.LineHi); cr.LineWidth = 1; cr.Stroke();
            }
            else if (hover) { cr.Rectangle(0, 0, w, h); cr.SetSourceRGBA(1, 1, 1, 0.13); cr.Fill(); }
            Icons.Draw(cr, Icon, (w - IconSize) / 2, (h - IconSize) / 2, IconSize, col);
            return true;
        }
    }

    // Line icons drawn with Cairo in a size x size box (the WPF version uses Segoe Fluent Icons glyphs).
    static class Icons
    {
        public static void Draw(Context cr, string name, double x, double y, double size, Color c)
        {
            cr.Save();
            cr.Translate(x, y);
            cr.Scale(size / 24.0, size / 24.0);
            Theme.Set(cr, c);
            cr.LineWidth = 1.8;   // in the 24-unit icon space: ~1.3 px at 18 px
            cr.LineCap = LineCap.Round; cr.LineJoin = LineJoin.Round;
            cr.NewPath();
            switch (name)
            {
                case "dashboard": // gauge
                    cr.Arc(12, 14, 9, Math.PI, 2 * Math.PI); cr.Stroke();
                    cr.MoveTo(12, 14); cr.LineTo(16.5, 8.5); cr.Stroke();
                    cr.Arc(12, 14, 1.5, 0, 2 * Math.PI); cr.Fill();
                    cr.MoveTo(3, 19); cr.LineTo(21, 19); cr.Stroke();
                    break;
                case "modes": // 2x2 tiles
                    foreach (var p in new[] { new[] { 3.0, 3.0 }, new[] { 13.0, 3.0 }, new[] { 3.0, 13.0 }, new[] { 13.0, 13.0 } })
                    { Theme.RoundRect(cr, p[0], p[1], 8, 8, 1.5); cr.Stroke(); }
                    break;
                case "device": // sun (brightness)
                    cr.Arc(12, 12, 4.5, 0, 2 * Math.PI); cr.Stroke();
                    for (int i = 0; i < 8; i++)
                    {
                        double a = i * Math.PI / 4;
                        cr.MoveTo(12 + Math.Cos(a) * 7.5, 12 + Math.Sin(a) * 7.5); cr.LineTo(12 + Math.Cos(a) * 10, 12 + Math.Sin(a) * 10);
                    }
                    cr.Stroke();
                    break;
                case "settings": // gear
                    for (int i = 0; i < 8; i++)
                    {
                        double a = i * Math.PI / 4;
                        cr.MoveTo(12 + Math.Cos(a) * 7, 12 + Math.Sin(a) * 7); cr.LineTo(12 + Math.Cos(a) * 10, 12 + Math.Sin(a) * 10);
                    }
                    cr.Stroke();
                    cr.Arc(12, 12, 6.5, 0, 2 * Math.PI); cr.Stroke();
                    cr.Arc(12, 12, 2.5, 0, 2 * Math.PI); cr.Stroke();
                    break;
                case "menu":
                    cr.MoveTo(4, 7); cr.LineTo(20, 7); cr.MoveTo(4, 12); cr.LineTo(20, 12); cr.MoveTo(4, 17); cr.LineTo(20, 17); cr.Stroke();
                    break;
                case "gamepad":
                    Theme.RoundRect(cr, 2.5, 7, 19, 11, 5.5); cr.Stroke();
                    cr.MoveTo(7.5, 10.5); cr.LineTo(7.5, 14.5); cr.MoveTo(5.5, 12.5); cr.LineTo(9.5, 12.5); cr.Stroke();
                    cr.Arc(15.5, 11.5, 1, 0, 2 * Math.PI); cr.Fill(); cr.Arc(17.5, 13.8, 1, 0, 2 * Math.PI); cr.Fill();
                    break;
                case "min": cr.MoveTo(6, 12); cr.LineTo(18, 12); cr.Stroke(); break;
                case "full":
                    cr.MoveTo(14, 4); cr.LineTo(20, 4); cr.LineTo(20, 10); cr.MoveTo(20, 4); cr.LineTo(13, 11);
                    cr.MoveTo(10, 20); cr.LineTo(4, 20); cr.LineTo(4, 14); cr.MoveTo(4, 20); cr.LineTo(11, 13); cr.Stroke();
                    break;
                case "close": cr.MoveTo(6, 6); cr.LineTo(18, 18); cr.MoveTo(18, 6); cr.LineTo(6, 18); cr.Stroke(); break;
                case "rotate":
                    cr.Arc(12, 12, 7, -Math.PI * 0.35, Math.PI * 1.45); cr.Stroke();
                    cr.MoveTo(16.5, 2.5); cr.LineTo(17.5, 6.8); cr.LineTo(13.2, 7.5); cr.Stroke();
                    break;
                case "thermo":
                    cr.MoveTo(10, 15); cr.LineTo(10, 5); cr.Arc(12, 5, 2, Math.PI, 2 * Math.PI); cr.LineTo(14, 15); cr.Stroke();
                    cr.Arc(12, 18, 4, 0, 2 * Math.PI); cr.Stroke();
                    cr.Arc(12, 18, 2, 0, 2 * Math.PI); cr.Fill();
                    break;
                case "cpu":
                    Theme.RoundRect(cr, 6, 6, 12, 12, 1.5); cr.Stroke();
                    cr.Rectangle(9.5, 9.5, 5, 5); cr.Stroke();
                    for (int i = 0; i < 3; i++)
                    {
                        double p = 9 + i * 3;
                        cr.MoveTo(p, 3); cr.LineTo(p, 6); cr.MoveTo(p, 18); cr.LineTo(p, 21);
                        cr.MoveTo(3, p); cr.LineTo(6, p); cr.MoveTo(18, p); cr.LineTo(21, p);
                    }
                    cr.Stroke();
                    break;
                case "ram":
                    Theme.RoundRect(cr, 2.5, 7, 19, 10, 1.5); cr.Stroke();
                    for (int i = 0; i < 4; i++) cr.Rectangle(5.5 + i * 3.8, 10, 2.2, 4);
                    cr.Fill();
                    break;
                case "check": cr.MoveTo(5, 12.5); cr.LineTo(10, 17.5); cr.LineTo(19.5, 7); cr.Stroke(); break;
                case "plus": cr.MoveTo(12, 5); cr.LineTo(12, 19); cr.MoveTo(5, 12); cr.LineTo(19, 12); cr.Stroke(); break;
            }
            cr.Restore();
        }
    }
}
