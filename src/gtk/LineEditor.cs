// Linux front end: the reading-lines editor (clock face / overlay). Same behaviour as the WPF one: each row is a line on
// the screen; the dot turns a reading on or off, a click selects it for styling, dragging moves it beside others or onto
// a line of its own (preview at 40 % where it would land), "Empty line" can be dragged in, × removes a blank line.
// The line logic is the shared ReadingList / LineDrag; this class only draws and handles the mouse.
using System;
using System.Collections.Generic;
using System.Linq;
using Cairo;
using Gtk;

namespace TtLcd.GtkUi
{
    class LineEditor : DrawingArea
    {
        readonly StudioCore core;
        public readonly ReadingList List;
        public int Selected = int.MinValue;       // selected reading id, MinValue = none
        public event System.Action SelectionChanged, Edited;

        const double RowH = 36, NumW = 22, ChipH = 28, ChipGap = 8;
        class Hit { public int Id, Line; public double X, Y, W; public bool Blank, Palette; public double XBtn; }
        readonly List<Hit> hits = new List<Hit>();

        // drag state
        LineDrag drag; int pressId = int.MinValue; double pressX, pressY, curX, curY; bool dragging;
        string dragName;

        public LineEditor(StudioCore core, ReadingList list)
        {
            this.core = core; List = list;
            Hexpand = true;
            Events |= Gdk.EventMask.ButtonPressMask | Gdk.EventMask.ButtonReleaseMask | Gdk.EventMask.PointerMotionMask;
            ButtonPressEvent += (o, a) => Press(a.Event.X, a.Event.Y);
            MotionNotifyEvent += (o, a) => Motion(a.Event.X, a.Event.Y);
            ButtonReleaseEvent += (o, a) => Release(a.Event.X, a.Event.Y);
            Relayout();
        }

        List<List<int>> Lines { get { return dragging && drag != null ? drag.Display : List.GetLines(core.Config); } }

        public void Relayout()
        {
            int rows = Lines.Count + 1;   // + palette row
            HeightRequest = (int)(rows * RowH);
            QueueDraw();
        }

        double ChipWidth(string text, bool dot)
        {
            int w, h;
            using (var l = CreatePangoLayout(text)) { l.FontDescription = Pango.FontDescription.FromString(Theme.FontName + " 9.5"); l.GetPixelSize(out w, out h); }
            return w + (dot ? 34 : 28);
        }

        protected override bool OnDrawn(Context cr)
        {
            hits.Clear();
            var cfg = core.Config;
            var lines = Lines;
            for (int i = 0; i < lines.Count; i++)
            {
                double y = i * RowH;
                Theme.DrawTextLeft(cr, (i + 1).ToString(), Theme.FontName + " 8", Theme.Dim, 0, y + RowH / 2);
                double x = NumW;
                foreach (int id in lines[i])
                {
                    var def = List.Def(id);
                    double w = ChipWidth(def.Name, true);
                    bool ghost = dragging && id == drag.Id;
                    DrawChip(cr, x, y + (RowH - ChipH) / 2, w, def.Name, def.Get(cfg), id == Selected && !dragging, ghost ? 0.4 : 1);
                    if (!ghost) hits.Add(new Hit { Id = id, Line = i, X = x, Y = y, W = w });
                    x += w + ChipGap;
                }
                if (lines[i].Count == 0)
                {
                    double w = ChipWidth("empty line", false) + 18;
                    bool ghost = dragging && drag.Id == ReadingList.EmptyLineId && i == drag.NewRowIdx;
                    DrawBlank(cr, x, y + (RowH - ChipH) / 2, w, ghost ? 0.4 : 1);
                    hits.Add(new Hit { Id = ReadingList.EmptyLineId, Line = i, X = x, Y = y, W = w, Blank = true, XBtn = x + w - 22 });
                }
            }
            // palette: "+ Empty line"
            double py = lines.Count * RowH;
            double pw = ChipWidth("+  Empty line", false);
            Theme.RoundRect(cr, NumW + 0.5, py + (RowH - ChipH) / 2 + 0.5, pw - 1, ChipH - 1, (ChipH - 1) / 2);
            Theme.Set(cr, Theme.Dim); cr.LineWidth = 1; cr.Stroke();
            Theme.DrawTextLeft(cr, "+  Empty line", Theme.FontName + " 9", Theme.Dim, NumW + 12, py + RowH / 2);
            hits.Add(new Hit { Id = ReadingList.EmptyLineId, Line = -1, X = NumW, Y = py, W = pw, Palette = true });

            if (dragging) // ghost under the pointer
            {
                double w = ChipWidth(dragName, false);
                Theme.RoundRect(cr, curX - 20, curY - 16, w, 30, 15);
                cr.SetSourceRGBA(0.165, 0.04, 0.07, 0.88); cr.FillPreserve();
                Theme.Set(cr, Theme.Accent); cr.LineWidth = 1; cr.Stroke();
                Theme.DrawTextLeft(cr, dragName, Theme.FontName + " 9.5", Theme.Hex(0xFFFFFF), curX - 8, curY - 1);
            }
            return true;
        }

        void DrawChip(Context cr, double x, double y, double w, string text, bool on, bool sel, double alpha)
        {
            cr.PushGroup();
            Theme.RoundRect(cr, x + 0.5, y + 0.5, w - 1, ChipH - 1, (ChipH - 1) / 2);
            if (sel) { for (int i = 3; i >= 1; i--) { cr.LineWidth = 2 + i * 3; cr.SetSourceRGBA(1, 0.12, 0.29, 0.07); cr.StrokePreserve(); } }
            Theme.Set(cr, sel ? Theme.Hex(0xFF1F4B, 0.18) : Theme.CardHi); cr.FillPreserve();
            Theme.Set(cr, sel ? Theme.Accent : Theme.Line); cr.LineWidth = sel ? 2 : 1; cr.Stroke();
            cr.NewPath(); cr.Arc(x + 15, y + ChipH / 2, 5, 0, 2 * Math.PI);
            if (on) { Theme.Set(cr, Theme.Accent); cr.Fill(); }
            else { Theme.Set(cr, Theme.Dim); cr.LineWidth = 1.5; cr.Stroke(); }
            Theme.DrawTextLeft(cr, text, Theme.FontName + " 9.5", on ? Theme.Text : Theme.Dim, x + 28, y + ChipH / 2);
            cr.PopGroupToSource();
            cr.PaintWithAlpha(alpha);
        }

        void DrawBlank(Context cr, double x, double y, double w, double alpha)
        {
            cr.PushGroup();
            Theme.RoundRect(cr, x + 0.5, y + 0.5, w - 1, ChipH - 1, (ChipH - 1) / 2);
            Theme.Set(cr, Theme.Line); cr.LineWidth = 1; cr.Stroke();
            Theme.DrawTextLeft(cr, "empty line", Theme.FontName + " Italic 9", Theme.Dim, x + 12, y + ChipH / 2);
            Icons.Draw(cr, "close", x + w - 20, y + ChipH / 2 - 5, 10, Theme.Dim);
            cr.PopGroupToSource();
            cr.PaintWithAlpha(alpha);
        }

        Hit HitAt(double x, double y) { return hits.FirstOrDefault(h => x >= h.X && x < h.X + h.W && y >= h.Y && y < h.Y + RowH); }

        void Press(double x, double y)
        {
            var h = HitAt(x, y);
            pressId = int.MinValue;
            if (h == null) return;
            if (h.Blank && x >= h.XBtn) { List.RemoveBlankLine(core, h.Line); Relayout(); Changed(); return; }
            if (!h.Blank && !h.Palette && x < h.X + 26) { List.Toggle(core, h.Id); QueueDraw(); Changed(); return; }  // the dot
            if (h.Blank) return;
            pressId = h.Id; pressX = x; pressY = y;
            dragName = h.Palette ? "Empty line" : List.Def(h.Id).Name;
        }

        void Motion(double x, double y)
        {
            curX = x; curY = y;
            if (pressId == int.MinValue) return;
            if (!dragging)
            {
                if (Math.Abs(x - pressX) < 5 && Math.Abs(y - pressY) < 5) return;
                dragging = true;
                Selected = int.MinValue;
                if (SelectionChanged != null) SelectionChanged();
                drag = LineDrag.Begin(List, core.Config, pressId);
                Relayout();
            }
            UpdateTarget(x, y);
            QueueDraw();
        }

        // the target from the rows on screen, as the WPF editor does
        void UpdateTarget(double x, double y)
        {
            var d = drag;
            int rows = d.Display.Count;
            bool tNew = true; int tLine = d.Base.Count, tIndex = 0;
            for (int r = 0; r < rows; r++)
            {
                double mid = r * RowH + RowH / 2;
                if (Math.Abs(y - mid) <= RowH * 0.32)
                {
                    if (r == d.NewRowIdx) { tNew = true; tLine = d.NewRowIdx; }
                    else if (d.Id == ReadingList.EmptyLineId) { tNew = true; tLine = d.ToBase(r) + (y >= mid ? 1 : 0); }
                    else
                    {
                        tNew = false; tLine = d.ToBase(r);
                        tIndex = hits.Count(h => h.Line == r && !h.Blank && !h.Palette && h.Id != d.Id && h.X + h.W / 2 < x);
                    }
                    break;
                }
                if (y < mid) { tNew = true; tLine = r == d.NewRowIdx + 1 && d.NewRowIdx >= 0 ? d.NewRowIdx : d.ToBase(r); break; }
            }
            if (d.Target(tNew, tLine, tIndex)) Relayout();
        }

        void Release(double x, double y)
        {
            if (dragging)
            {
                dragging = false;
                List.Commit(core, drag.Display);
                drag = null;
                pressId = int.MinValue;
                Relayout();
                Changed();
                return;
            }
            if (pressId != int.MinValue && pressId != ReadingList.EmptyLineId)
            {
                Selected = pressId;
                if (SelectionChanged != null) SelectionChanged();
                QueueDraw();
            }
            pressId = int.MinValue;
        }

        void Changed() { if (Edited != null) Edited(); }
    }
}
