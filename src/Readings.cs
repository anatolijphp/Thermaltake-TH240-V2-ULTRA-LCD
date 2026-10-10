// The readings that the clock face and the overlay show, and how they are arranged into lines: the definitions and the
// line operations behind the drag-and-drop editor, without any UI. Lines are saved as "0,2|3|" (Render.Lines).
using System;
using System.Collections.Generic;
using System.Linq;

namespace TtLcd
{
    class ItemDef { public int Id; public string Name; public Func<Config, bool> Get; public Action<Config, bool> Set; }

    class ReadingList
    {
        public string Panel;    // element name of the list in the UI ("ClkItems" / "OvItems")
        public bool Overlay;
        public ItemDef[] Defs;
        public Func<Config, List<List<int>>> GetLines;
        public Action<Config, string> SetOrder;

        public const int EmptyLineId = -1;   // the "Empty line" chip in the palette

        public ItemDef Def(int id) { return Defs.First(d => d.Id == id); }

        public static readonly ReadingList Clock = new ReadingList
        {
            Panel = "ClkItems",
            Defs = new[] {
                new ItemDef { Id = Render.IdTime, Name = "Time", Get = c => c.ShowTime, Set = (c, v) => c.ShowTime = v },
                new ItemDef { Id = Render.IdDate, Name = "Date", Get = c => c.ShowDate, Set = (c, v) => c.ShowDate = v },
                new ItemDef { Id = Render.IdWater, Name = "Coolant temperature", Get = c => c.ShowWaterTemp, Set = (c, v) => c.ShowWaterTemp = v },
                new ItemDef { Id = Render.IdCpuTemp, Name = "CPU temperature", Get = c => c.ShowCpuTemp, Set = (c, v) => c.ShowCpuTemp = v },
                new ItemDef { Id = Render.IdLoad, Name = "CPU load", Get = c => c.ShowCpuLoad, Set = (c, v) => c.ShowCpuLoad = v },
            },
            GetLines = c => Render.Lines(c.ClockOrder, c.ClockLayout, c.ClockBreaks, Render.ClockIds),
            SetOrder = (c, v) => c.ClockOrder = v,
        };

        public static readonly ReadingList OverlayList = new ReadingList
        {
            Panel = "OvItems",
            Overlay = true,
            Defs = new[] {
                new ItemDef { Id = Render.IdTime, Name = "Time", Get = c => c.OverlayTime, Set = (c, v) => c.OverlayTime = v },
                new ItemDef { Id = Render.IdWater, Name = "Coolant temperature", Get = c => c.OverlayWaterTemp, Set = (c, v) => c.OverlayWaterTemp = v },
                new ItemDef { Id = Render.IdCpuTemp, Name = "CPU temperature", Get = c => c.OverlayCpuTemp, Set = (c, v) => c.OverlayCpuTemp = v },
            },
            GetLines = c => Render.Lines(c.OverlayOrder, c.OverlayLayout, c.OverlayBreaks, Render.OverlayIds),
            SetOrder = (c, v) => c.OverlayOrder = v,
        };

        public static readonly ReadingList[] All = { Clock, OverlayList };

        // Whether a reading selected in this list is on screen in the current mode (its style editor is useful).
        public bool ShownIn(Mode m) { return Overlay ? m != Mode.Clock && m != Mode.Off : m == Mode.Clock; }

        // ---- edits (return nothing; they change the config through the core) ----
        public void Toggle(StudioCore core, int id)
        {
            var d = Def(id);
            bool v = !d.Get(core.Config);
            core.Change(c => d.Set(c, v));
        }

        public void RemoveBlankLine(StudioCore core, int index)
        {
            var lines = GetLines(core.Config);
            if (index < lines.Count && lines[index].Count == 0) lines.RemoveAt(index);
            string order = Render.LinesToString(lines);
            core.Change(c => SetOrder(c, order));
        }

        public void Commit(StudioCore core, List<List<int>> lines)
        {
            Render.TrimEmptyEnds(lines);
            string order = Render.LinesToString(lines);
            core.Change(c => SetOrder(c, order));
        }

        public ElementStyle Style(Config c, int id) { return c.Style(Overlay, id); }

        public void EditStyle(StudioCore core, int id, Action<ElementStyle> edit)
        {
            bool ov = Overlay;
            core.Change(c => { var st = c.Style(ov, id).Clone(); edit(st); c.SetStyle(ov, st); });
        }

        public static bool IsTemperature(int id) { return id == Render.IdWater || id == Render.IdCpuTemp; }
        public static bool IsReading(int id) { return IsTemperature(id) || id == Render.IdLoad; }
    }

    // Drag state of one list: the lines without the dragged reading (Base), where it would land (target) and the lines
    // shown meanwhile (Display, with the reading at the target at 40 % opacity).
    class LineDrag
    {
        public int Id;                     // dragged reading, or ReadingList.EmptyLineId
        public List<List<int>> Base, Display;
        public bool TNew; public int TLine, TIndex;
        public int NewRowIdx = -1;         // display row that is the dragged item's own new line, -1 = none

        // Starts at the reading's own place.
        public static LineDrag Begin(ReadingList l, Config cfg, int id)
        {
            var d = new LineDrag { Id = id, Base = l.GetLines(cfg) };
            d.TNew = true; d.TLine = d.Base.Count; d.TIndex = 0;
            if (id != ReadingList.EmptyLineId)
                for (int i = 0; i < d.Base.Count; i++)
                {
                    int k = d.Base[i].IndexOf(id);
                    if (k < 0) continue;
                    d.Base[i].RemoveAt(k);
                    if (d.Base[i].Count == 0) { d.Base.RemoveAt(i); d.TNew = true; d.TLine = i; }   // it was alone: its line goes
                    else { d.TNew = false; d.TLine = i; d.TIndex = k; }
                    break;
                }
            d.Apply();
            return d;
        }

        // Display = base lines with the dragged reading inserted at the target (or a blank line for "Empty line").
        public void Apply()
        {
            var d = Base.Select(x => new List<int>(x)).ToList();
            if (TNew)
            {
                int at = Math.Min(TLine, d.Count);
                d.Insert(at, Id == ReadingList.EmptyLineId ? new List<int>() : new List<int> { Id });
                NewRowIdx = at;
            }
            else
            {
                d[TLine].Insert(Math.Min(TIndex, d[TLine].Count), Id);
                NewRowIdx = -1;
            }
            Display = d;
        }

        // Display row -> base line (rows after the preview's own new line shift by one).
        public int ToBase(int row) { return NewRowIdx >= 0 && row > NewRowIdx ? row - 1 : row; }

        // Sets the target; returns false when it didn't change (no rebuild needed).
        public bool Target(bool tNew, int tLine, int tIndex)
        {
            if (tNew == TNew && tLine == TLine && (tNew || tIndex == TIndex)) return false;
            TNew = tNew; TLine = tLine; TIndex = tIndex;
            Apply();
            return true;
        }
    }
}
