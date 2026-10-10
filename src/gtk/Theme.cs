// Linux front end: colours, CSS and Cairo helpers that reproduce the WPF look (MainWindow.xaml resources).
using System;
using Cairo;

namespace TtLcd.GtkUi
{
    static class Theme
    {
        // the same tokens as MainWindow.xaml
        public static readonly Color Bg = Hex(0x070707), Panel = Hex(0x0D0D0E), Card = Hex(0x131314), CardHi = Hex(0x1B1B1D),
            Line = Hex(0x29292C), LineHi = Hex(0x46464B), Text = Hex(0xEDEDED), Dim = Hex(0x8F8F94), Accent = Hex(0xFF1F4B),
            Accent2 = Hex(0x00E1FF), StatusText = Hex(0xFF7A90), TitleBg = Hex(0x050505);

        public static readonly int[] Rainbow = { 0xFF1F4B, 0xFF8A00, 0xFFE600, 0x2BE36B, 0x00E1FF, 0x7A4BFF, 0xFF2BD6 };

        public static Color Hex(int rgb, double a = 1) { return new Color((rgb >> 16 & 255) / 255.0, (rgb >> 8 & 255) / 255.0, (rgb & 255) / 255.0, a); }
        public static Color A(Color c, double a) { return new Color(c.R, c.G, c.B, a); }

        public const string Fonts = "\"Segoe UI\", \"Ubuntu\", \"Cantarell\", \"Noto Sans\", \"DejaVu Sans\", sans-serif";
        public const string FontName = "Ubuntu, Cantarell, Noto Sans, DejaVu Sans";

        // GTK CSS for the stock widgets used (labels, chips, buttons, scrollbars); custom-drawn parts use Cairo below.
        public static readonly string Css = @"
* { font-family: " + Fonts + @"; }
window, .bg { background-color: #070707; color: #EDEDED; font-size: 13px; }
label { color: #EDEDED; }
.dim { color: #8F8F94; }
.h1 { font-size: 24px; font-weight: 600; }
.sub { color: #8F8F94; }
.caption { color: #8F8F94; font-size: 11px; font-weight: bold; }
.small { font-size: 11px; }
.status { color: #FF7A90; font-size: 12px; }
.semibold { font-weight: 600; }
.brand-tt { color: #FF1F4B; font-size: 19px; font-weight: 900; }
.brand { font-size: 15px; font-weight: 600; }
.titlestatus { color: #8F8F94; font-size: 12px; }
.navlabel { font-size: 14px; color: #8F8F94; }
.navlabel.on { color: #EDEDED; }
button.chip {
  background-image: none; background-color: #1B1B1D; color: #8F8F94; border: 1px solid #29292C; border-radius: 15px;
  padding: 4px 12px 4px 9px; font-size: 12px; box-shadow: none; text-shadow: none; -gtk-icon-shadow: none; min-height: 0;
}
button.chip:hover { border-color: #46464B; }
button.chip:checked { background-color: rgba(255,31,75,0.18); border-color: #FF1F4B; color: #EDEDED; box-shadow: 0 0 8px rgba(255,31,75,0.55); }
button.chip label { color: inherit; }
button.flat {
  background-image: none; background-color: transparent; border: none; box-shadow: none; padding: 0; min-height: 0; min-width: 0;
}
button.flat:hover { background-color: rgba(255,255,255,0.13); }
button.accent, button.btn {
  background-image: none; box-shadow: none; text-shadow: none; border-radius: 2px; padding: 6px 16px; min-height: 0;
}
button.accent { background-color: #FF1F4B; color: #FFFFFF; border: 1px solid #FF1F4B; }
button.accent:hover { background-color: #FF3D63; }
button.accent:disabled { background-color: #4A1020; border-color: #4A1020; color: #8F8F94; }
button.btn { background-color: #1B1B1D; color: #EDEDED; border: 1px solid #29292C; }
button.btn:hover { border-color: #46464B; }
button.btn:disabled { color: #55555A; }
button.accent label, button.btn label { color: inherit; }
entry { background-color: #0D0D0E; color: #EDEDED; border: 1px solid #29292C; border-radius: 2px; box-shadow: none; padding: 4px 8px; }
combobox button, combobox box { background-image: none; background-color: #0D0D0E; color: #EDEDED; border-color: #29292C; box-shadow: none; text-shadow: none; }
combobox * { color: #EDEDED; }
menu, .menu, menuitem { background-color: #131314; color: #EDEDED; }
menuitem:hover { background-color: #2A0A12; }
progressbar trough { background-color: #2A2A2E; border: none; min-height: 6px; }
progressbar progress { background-color: #00E1FF; border: none; min-height: 6px; }
scrollbar { background-color: transparent; border: none; }
scrollbar slider { background-color: rgba(255,255,255,0.18); border-radius: 4px; min-width: 6px; min-height: 6px; border: none; }
tooltip, tooltip * { background-color: #1B1B1D; color: #EDEDED; }
";

        // ---- Cairo helpers ----
        public static void Set(Context cr, Color c) { cr.SetSourceRGBA(c.R, c.G, c.B, c.A); }

        public static void RoundRect(Context cr, double x, double y, double w, double h, double r)
        {
            cr.NewPath();
            cr.Arc(x + w - r, y + r, r, -Math.PI / 2, 0);
            cr.Arc(x + w - r, y + h - r, r, 0, Math.PI / 2);
            cr.Arc(x + r, y + h - r, r, Math.PI / 2, Math.PI);
            cr.Arc(x + r, y + r, r, Math.PI, 3 * Math.PI / 2);
            cr.ClosePath();
        }

        // A soft glow for strokes (approximates WPF's DropShadowEffect): wider, fainter passes under the stroke.
        public static void GlowStroke(Context cr, Color c, double width, double blur, double opacity)
        {
            for (int i = 8; i >= 1; i--)
            {
                cr.LineWidth = width + blur * i / 4.0;
                cr.SetSourceRGBA(c.R, c.G, c.B, opacity * 0.045);
                cr.StrokePreserve();
            }
            cr.LineWidth = width;
            Set(cr, c);
            cr.Stroke();
        }

        public static LinearGradient RainbowGradient(double x0, double y0, double x1, double y1, int[] stops)
        {
            var g = new LinearGradient(x0, y0, x1, y1);
            for (int i = 0; i < stops.Length; i++) g.AddColorStop(i / (double)(stops.Length - 1), Hex(stops[i]));
            return g;
        }

        // Card (CardBox in WPF): gradient fill, hairline border, the top-right corner cut off, and a red tick top-left.
        public static void PaintCard(Context cr, double w, double h)
        {
            const double cut = 14;
            cr.NewPath();
            cr.MoveTo(0.5, 0.5); cr.LineTo(w - cut, 0.5); cr.LineTo(w - 0.5, cut); cr.LineTo(w - 0.5, h - 0.5); cr.LineTo(0.5, h - 0.5); cr.ClosePath();
            var fill = new LinearGradient(0, 0, 0, h);
            fill.AddColorStop(0, Hex(0x151516)); fill.AddColorStop(1, Hex(0x0F0F10));
            cr.SetSource(fill);
            cr.FillPreserve();
            fill.Dispose();
            Set(cr, Line); cr.LineWidth = 1; cr.Stroke();
            var tick = new LinearGradient(0, 0, 44, 0);
            tick.AddColorStop(0, Accent); tick.AddColorStop(1, A(Accent, 0));
            cr.SetSource(tick);
            cr.Rectangle(0, 0, 44, 2); cr.Fill();
            tick.Dispose();
        }

        // Text with Pango (proper font fallback); returns the pixel size.
        public static void DrawText(Context cr, string text, string font, Color c, double x, double y, out int w, out int h, bool measureOnly = false)
        {
            var layout = Pango.CairoHelper.CreateLayout(cr);
            layout.FontDescription = Pango.FontDescription.FromString(font);
            layout.SetText(text);
            layout.GetPixelSize(out w, out h);
            if (!measureOnly) { Set(cr, c); cr.MoveTo(x, y); Pango.CairoHelper.ShowLayout(cr, layout); }
            layout.Dispose();
        }

        // Left-aligned text, vertically centred on cy.
        public static void DrawTextLeft(Context cr, string text, string font, Color c, double x, double cy)
        {
            int w, h;
            DrawText(cr, text, font, c, 0, 0, out w, out h, true);
            DrawText(cr, text, font, c, x, cy - h / 2.0, out w, out h);
        }

        public static void TextCentred(Context cr, string text, string font, Color c, double cx, double cy)
        {
            int w, h;
            DrawText(cr, text, font, c, 0, 0, out w, out h, true);
            DrawText(cr, text, font, c, cx - w / 2.0, cy - h / 2.0, out w, out h);
        }
    }
}
