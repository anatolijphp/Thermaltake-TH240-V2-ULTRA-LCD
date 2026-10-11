// Linux front end: the Display modes, Device and Settings pages (as in MainWindow.xaml). Controls are filled from the
// config by Load() (also after the tray changes something) and write back through StudioCore.Change.
using System;
using System.Collections.Generic;
using System.Linq;
using Gtk;

namespace TtLcd.GtkUi
{
    partial class StudioWindow
    {
        readonly List<System.Action> loaders = new List<System.Action>();
        bool loading;

        void Change(Action<Config> edit) { if (!loading) core.Change(edit); }

        // Refill every control from the config.
        public void Load()
        {
            loading = true;
            foreach (var l in loaders) l();
            loading = false;
            SetPanels();
        }

        // ---- small builders with a loader each ----
        SegToggle Seg(Func<Config, int> get, Action<Config, int> set, params string[] options)
        {
            var s = new SegToggle(options);
            s.Changed += i => Change(c => set(c, i));
            loaders.Add(() => s.Select(get(core.Config), false));
            return s;
        }

        Box SliderRow(string label, double min, double max, double step, string unit, Func<Config, double> get, Action<Config, double> set, int labelWidth = 100)
        {
            var s = new Slider(min, max, step);
            var v = new Label("");
            s.Changed += x => { v.Text = Math.Round(x) + unit; Change(c => set(c, x)); };
            loaders.Add(() => { s.Set(get(core.Config), false); v.Text = Math.Round(get(core.Config)) + unit; });
            return Ui.Row(label, s, v, labelWidth);
        }

        SwitchToggle Switch(string label, Func<Config, bool> get, Action<Config, bool> set)
        {
            var s = new SwitchToggle(label) { MarginBottom = 12 };
            s.Toggled += v => Change(c => set(c, v));
            loaders.Add(() => s.SetSilently(get(core.Config)));
            return s;
        }

        // ================= Display modes =================
        readonly Dictionary<Mode, Tile> tiles = new Dictionary<Mode, Tile>();
        readonly Dictionary<string, Widget> panels = new Dictionary<string, Widget>();
        Label optTitle, optDesc;
        LineEditor clockLines, overlayLines;
        LineEditor selEditor;                 // the editor whose reading is selected (style panel), or null
        Label stTitle; Box stUnits, stReading; SegToggle stFx, stUnitSeg, stLabelSeg; Slider stSize; Label stSizeT;
        ComboBoxText stFont; ToggleButton stBold, stItalic; SwitchToggle stByTemp; Swatches stColour;
        bool styleLoading, missingFont;

        static readonly Mode[] LinuxModes = { Mode.Off, Mode.Image, Mode.Gif, Mode.Color, Mode.Clock, Mode.Aura };
        static readonly Dictionary<Mode, string> TileIcon = new Dictionary<Mode, string>
        {
            { Mode.Off, "power" }, { Mode.Image, "image" }, { Mode.Gif, "gif" }, { Mode.Color, "palette" }, { Mode.Clock, "clock" },
            { Mode.Aura, "aura" },
        };

        Widget ModesPage()
        {
            var page = PageHead("Display modes", "Choose what the cooler screen shows. Changes apply instantly.");
            var grid = new Grid { ColumnSpacing = 10, RowSpacing = 10, ColumnHomogeneous = true, MarginBottom = 8 };
            var modes = LinuxModes.Where(Os.Supports).ToArray();
            for (int i = 0; i < modes.Length; i++)
            {
                var m = modes[i];
                var t = new Tile(Engine.Label(m), TileIcon[m]);
                t.Clicked += () => { Change(c => c.Mode = m); ShowOptions(); };
                tiles[m] = t;
                grid.Attach(t, i % 3, i / 3, 1, 1);
            }
            loaders.Add(() => { foreach (var t in tiles) t.Value.On = t.Key == core.Config.Mode; });
            page.PackStart(grid, false, false, 0);

            var card = new CardBox { MarginTop = 6 };
            var inner = card.Inner();
            optTitle = new Label("") { Halign = Align.Start, MarginBottom = 2 }; optTitle.StyleContext.AddClass("caption");
            optDesc = Ui.Text("", "sub"); optDesc.MarginBottom = 12;
            inner.PackStart(optTitle, false, false, 0);
            inner.PackStart(optDesc, false, false, 0);

            panels["OptImage"] = FileRow(c => c.ImagePath, (c, v) => c.ImagePath = v, "Choose a picture", "Images", "*.jpg", "*.jpeg", "*.png", "*.bmp");
            panels["OptGif"] = FileRow(c => c.GifPath, (c, v) => c.GifPath = v, "Choose a GIF", "GIF animations", "*.gif");
            panels["OptOff"] = new Box(Orientation.Vertical, 0);
            panels["OptClock"] = ClockPanel();
            panels["OptScaling"] = Ui.Row("Scaling", Seg(c => c.FitContain ? 1 : 0, (c, i) => c.FitContain = i == 1, "Fill", "Fit"));
            var bg = new Swatches { Current = () => core.Config.ColorRgb };
            bg.Picked += v => { Change(c => c.ColorRgb = v); bg.QueueDraw(); };
            panels["OptBg"] = Ui.Row("Background", bg);
            ((Widget)panels["OptScaling"]).MarginTop = 6;
            panels["OptOverlay"] = OverlayPanel();
            panels["OptText"] = StylePanel();
            foreach (var n in new[] { "OptImage", "OptGif", "OptOff", "OptClock", "OptScaling", "OptBg", "OptOverlay", "OptText" })
                inner.PackStart(panels[n], false, false, 0);
            page.PackStart(card, false, false, 0);
            return page;
        }

        Widget FileRow(Func<Config, string> get, Action<Config, string> set, string title, string filter, params string[] patterns)
        {
            var row = new Box(Orientation.Horizontal, 8) { MarginBottom = 4 };
            var entry = new Entry { IsEditable = false, Hexpand = true };
            var browse = Ui.Button("Browse…", true, () =>
            {
                string f = Ui.ChooseFile(this, title, get(core.Config), filter, patterns);
                if (f != null) { entry.Text = f; Change(c => set(c, f)); }
            });
            row.PackStart(entry, true, true, 0); row.PackStart(browse, false, false, 0);
            loaders.Add(() => entry.Text = get(core.Config) ?? "");
            return row;
        }

        Widget ClockPanel()
        {
            var b = new Box(Orientation.Vertical, 0);
            b.PackStart(Ui.Hair(), false, false, 0);
            b.PackStart(Ui.Section("PLACEMENT", false), false, false, 0);
            var sub = Ui.Text("Where the readings sit. Everything is kept inside the round screen: text that would cross the edge is shrunk automatically.", "sub");
            sub.MarginBottom = 12; b.PackStart(sub, false, false, 0);
            b.PackStart(Ui.Row("Alignment", Seg(c => c.TextAlign, (c, i) => c.TextAlign = i, "Left", "Centre", "Right")), false, false, 0);
            b.PackStart(SliderRow("Vertical offset", -160, 160, 2, " px", c => c.TextOffsetY, (c, v) => c.TextOffsetY = (int)v), false, false, 0);
            b.PackStart(SliderRow("Line spacing", -20, 60, 1, " px", c => c.LineSpacing, (c, v) => c.LineSpacing = (int)v), false, false, 0);
            b.PackStart(Ui.Hair(), false, false, 0);
            b.PackStart(Ui.Section("ELEMENTS", false), false, false, 0);
            var sub2 = Ui.Text("Each row is a line on the screen. Drag a reading onto a line to put it beside the others, or between lines to give it its own line. The dot turns a reading on or off; click a reading to style it.", "sub");
            sub2.MarginBottom = 12; b.PackStart(sub2, false, false, 0);
            clockLines = Editor(ReadingList.Clock);
            b.PackStart(clockLines, false, false, 0);
            return b;
        }

        Widget OverlayPanel()
        {
            var b = new Box(Orientation.Vertical, 0);
            b.PackStart(Ui.Hair(), false, false, 0);
            var det = new Box(Orientation.Vertical, 0);
            det.PackStart(Ui.Section("OVERLAY · PLACEMENT", false), false, false, 0);
            var sub = Ui.Text("Time and temperatures drawn on top of this mode.", "sub"); sub.MarginBottom = 12;
            det.PackStart(sub, false, false, 0);
            det.PackStart(Ui.Row("Vertical", Seg(c => c.OverlayPos == 1 ? 0 : c.OverlayPos == 2 ? 1 : 2, (c, i) => c.OverlayPos = i == 0 ? 1 : i == 1 ? 2 : 0, "Top", "Centre", "Bottom")), false, false, 0);
            det.PackStart(Ui.Row("Alignment", Seg(c => c.TextAlign, (c, i) => c.TextAlign = i, "Left", "Centre", "Right")), false, false, 0);
            panels["OvDetails"] = det;
            b.PackStart(det, false, false, 0);
            b.PackStart(Ui.Section("OVERLAY · ELEMENTS", false), false, false, 0);
            var sub2 = Ui.Text("Each row is a line. Drag readings between lines to arrange them. The dot turns a reading on or off; click a reading to style it.", "sub");
            sub2.MarginBottom = 12; b.PackStart(sub2, false, false, 0);
            overlayLines = Editor(ReadingList.OverlayList);
            b.PackStart(overlayLines, false, false, 0);
            return b;
        }

        LineEditor Editor(ReadingList list)
        {
            var e = new LineEditor(core, list);
            e.SelectionChanged += () =>
            {
                // one selection across both lists
                var other = e == clockLines ? overlayLines : clockLines;
                if (e.Selected != int.MinValue && other != null) { other.Selected = int.MinValue; other.QueueDraw(); }
                selEditor = e.Selected != int.MinValue ? e : null;
                ShowStyleEditor();
            };
            e.Edited += SetPanels;
            loaders.Add(e.Relayout);
            return e;
        }

        Widget StylePanel()
        {
            var b = new Box(Orientation.Vertical, 0);
            b.PackStart(Ui.Hair(), false, false, 0);
            stTitle = Ui.Section("TEXT STYLE", false); b.PackStart(stTitle, false, false, 0);
            var sub = Ui.Text("How the selected reading looks. Each reading has its own style.", "sub"); sub.MarginBottom = 12;
            b.PackStart(sub, false, false, 0);

            stFont = new ComboBoxText { Hexpand = true };
            foreach (var fam in System.Drawing.FontFamily.Families) stFont.AppendText(fam.Name);
            stFont.Changed += (o, a) => { string n = stFont.ActiveText; if (n != null && !n.EndsWith(" (not installed)")) EditStyle(st => st.Font = n); };
            stBold = StyleChip("Bold", v => EditStyle(st => st.Bold = v));
            stItalic = StyleChip("Italic", v => EditStyle(st => st.Italic = v));
            var fontRow = new Box(Orientation.Horizontal, 8) { Hexpand = true };
            fontRow.PackStart(stFont, true, true, 0); fontRow.PackStart(stBold, false, false, 0); fontRow.PackStart(stItalic, false, false, 0);
            b.PackStart(Ui.Row("Font", fontRow), false, false, 0);

            stSize = new Slider(12, 220, 1); stSizeT = new Label("");
            stSize.Changed += v => { stSizeT.Text = (int)v + " px"; EditStyle(st => st.Size = (int)v); };
            b.PackStart(Ui.Row("Size", stSize, stSizeT), false, false, 0);
            stFx = new SegToggle("None", "Shadow", "Outline", "Neon glow");
            stFx.Changed += i => EditStyle(st => st.Effect = (TextEffect)i);
            b.PackStart(Ui.Row("Effect", stFx), false, false, 0);
            stColour = new Swatches { Current = () => selEditor == null ? -1 : selEditor.List.Style(core.Config, selEditor.Selected).Rgb };
            stColour.Picked += v => { EditStyle(st => st.Rgb = v); stColour.QueueDraw(); };
            b.PackStart(Ui.Row("Colour", stColour), false, false, 0);
            stUnitSeg = new SegToggle("°C", "°F");
            stUnitSeg.Changed += i => EditStyle(st => st.Fahrenheit = i == 1);
            stUnits = Ui.Row("Units", stUnitSeg);
            b.PackStart(stUnits, false, false, 0);
            stLabelSeg = new SegToggle("No label", "Coolant: 31°C", "Label above");
            stLabelSeg.Changed += i => EditStyle(st => { st.Label = i > 0; st.LabelAbove = i == 2; });
            stByTemp = new SwitchToggle("Colour by value") { Valign = Align.Center, MarginLeft = 12 };
            stByTemp.Toggled += v => EditStyle(st => st.ByTemp = v);
            var readRow = new Box(Orientation.Horizontal, 0);
            readRow.PackStart(stLabelSeg, false, false, 0); readRow.PackStart(stByTemp, false, false, 0);
            stReading = Ui.Row("Reading", readRow);
            b.PackStart(stReading, false, false, 0);
            return b;
        }

        ToggleButton StyleChip(string text, Action<bool> set)
        {
            var t = new ToggleButton(text);
            t.StyleContext.AddClass("chip");
            t.Toggled += (o, a) => set(t.Active);
            return t;
        }

        void EditStyle(Action<ElementStyle> edit)
        {
            if (styleLoading || loading || selEditor == null) return;
            selEditor.List.EditStyle(core, selEditor.Selected, edit);
        }

        void ShowStyleEditor()
        {
            if (selEditor != null)
            {
                int id = selEditor.Selected;
                var st = selEditor.List.Style(core.Config, id);
                var def = selEditor.List.Def(id);
                styleLoading = true;
                stTitle.Text = "TEXT STYLE · " + def.Name.ToUpperInvariant() + (selEditor.List.Overlay ? " (OVERLAY)" : "");
                var names = System.Drawing.FontFamily.Families.Select(f => f.Name).ToList();
                if (missingFont) { stFont.Remove(0); missingFont = false; }   // the previous "not installed" entry
                int fi = names.IndexOf(st.Font);
                if (fi < 0 && !string.IsNullOrEmpty(st.Font)) { stFont.PrependText(st.Font + " (not installed)"); fi = 0; missingFont = true; }
                stFont.Active = fi;
                stBold.Active = st.Bold; stItalic.Active = st.Italic;
                stSize.Set(st.Size, false); stSizeT.Text = st.Size + " px";
                stFx.Select((int)st.Effect, false);
                stUnitSeg.Select(st.Fahrenheit ? 1 : 0, false);
                stLabelSeg.Select(!st.Label ? 0 : st.LabelAbove ? 2 : 1, false);
                stByTemp.SetSilently(st.ByTemp);
                stUnits.Visible = ReadingList.IsTemperature(id);
                stReading.Visible = ReadingList.IsReading(id);
                stColour.QueueDraw();
                styleLoading = false;
            }
            SetPanels();
        }

        void ShowOptions()
        {
            var m = core.Config.Mode;
            foreach (var t in tiles) t.Value.On = t.Key == m;
            optTitle.Text = StudioModel.OptionsTitle(m);
            optDesc.Text = StudioModel.ModeDescription(m);
            SetPanels();
        }

        void SetPanels()
        {
            if (optTitle == null) return;
            bool selVisible = selEditor != null && selEditor.List.ShownIn(core.Config.Mode);
            foreach (var p in StudioModel.Panels(core.Config, selVisible))
            {
                Widget w;
                if (!panels.TryGetValue(p.Key, out w)) continue;
                w.NoShowAll = !p.Value;
                if (p.Value) w.ShowAll(); else w.Hide();
            }
            if (selVisible) { stUnits.Visible = ReadingList.IsTemperature(selEditor.Selected); stReading.Visible = ReadingList.IsReading(selEditor.Selected); }
        }

        // ================= Device =================
        Label devStatus, devSerial, devFps;

        Widget DevicePage()
        {
            var page = PageHead("Device", "Screen settings that live on the cooler.");
            var card = new CardBox();
            var inner = card.Inner();
            inner.PackStart(Ui.Section("SCREEN", true), false, false, 0);
            var sub = Ui.Text("Brightness, orientation and sleep. These are stored in the cooler.", "sub"); sub.MarginBottom = 12;
            inner.PackStart(sub, false, false, 0);
            inner.PackStart(SliderRow("Brightness", 0, 100, 1, "%", c => c.Brightness, (c, v) => c.Brightness = (int)v), false, false, 0);
            inner.PackStart(Ui.Row("Rotation", Seg(c => StudioModel.UserRotation(c) / 90, (c, i) => c.Rotation = StudioModel.RotationFor(c, i * 90), "0°", "90°", "180°", "270°")), false, false, 0);
            var sleep = Switch("Dim the screen when idle", c => c.DeviceSleep, (c, v) => c.DeviceSleep = v);
            sleep.MarginBottom = 0;
            inner.PackStart(Ui.Row("Sleep", sleep), false, false, 0);
            var note = Ui.Text("Rotation turns everything on the cooler to match how the pump is mounted: the live picture and the cooler's own idle and boot screens. Also the arrow on the preview.", "dim", "small");
            note.MarginTop = 8;
            inner.PackStart(note, false, false, 0);
            page.PackStart(card, false, false, 0);

            var conn = new CardBox();
            var ci = conn.Inner();
            ci.PackStart(Ui.Section("CONNECTION", true), false, false, 0);
            var sub2 = Ui.Text("The cooler as the system sees it over USB.", "sub"); sub2.MarginBottom = 12;
            ci.PackStart(sub2, false, false, 0);
            devStatus = new Label("") { Halign = Align.Start }; devSerial = new Label("") { Halign = Align.Start }; devFps = new Label("") { Halign = Align.Start };
            ci.PackStart(Ui.Row("Status", devStatus), false, false, 0);
            ci.PackStart(Ui.Row("Serial", devSerial), false, false, 0);
            ci.PackStart(Ui.Row("Frame rate", devFps), false, false, 0);
            page.PackStart(conn, false, false, 0);
            return page;
        }

        // ================= Settings =================
        Label gmStatus, svcStatus, cacheInfo;
        SwitchToggle gmForce;

        Widget SettingsPage()
        {
            var page = PageHead("Settings", "Streaming quality and how the app starts.");

            var s1 = new CardBox(); var i1 = s1.Inner();
            i1.PackStart(Ui.Section("STREAMING", true), false, false, 0);
            var sub1 = Ui.Text("How often and how sharp the pictures sent to the cooler are. Lower values use less CPU.", "sub"); sub1.MarginBottom = 12;
            i1.PackStart(sub1, false, false, 0);
            i1.PackStart(SliderRow("Frame rate (FPS)", 5, 30, 1, "", c => c.Fps, (c, v) => c.Fps = (int)v, 125), false, false, 0);
            i1.PackStart(SliderRow("JPEG quality", 40, 95, 1, "", c => c.Quality, (c, v) => c.Quality = (int)v, 125), false, false, 0);
            page.PackStart(s1, false, false, 0);

            var s2 = new CardBox(); var i2 = s2.Inner();
            i2.PackStart(Ui.Section("GAME MODE", true), false, false, 0);
            var sub2 = Ui.Text("Lowers the load while you play: fewer frames to the cooler and slower sensor polling, or no streaming at all (the cooler then shows its own idle screen).", "sub"); sub2.MarginBottom = 12;
            i2.PackStart(sub2, false, false, 0);
            gmForce = new SwitchToggle("Game mode on now (also the gamepad button in the title bar)") { MarginBottom = 12 };
            gmForce.Toggled += v => { core.Engine.ForceGame = v; UpdateStatus(); };
            i2.PackStart(gmForce, false, false, 0);
            i2.PackStart(Ui.Row("While a game runs", Seg(c => (int)c.GameAction, (c, i) => c.GameAction = (GameAction)i, "Keep going", "Lower frame rate", "Show idle screen"), null, 125), false, false, 0);
            i2.PackStart(SliderRow("Game frame rate", 1, 30, 1, " fps", c => c.GameFps, (c, v) => c.GameFps = (int)v, 125), false, false, 0);
            i2.PackStart(SliderRow("Sensor updates", 2, 60, 1, " s", c => c.GameSensorMs / 1000, (c, v) => c.GameSensorMs = (int)v * 1000, 125), false, false, 0);
            gmStatus = Ui.Text("", "status"); gmStatus.MarginTop = 6;
            i2.PackStart(gmStatus, false, false, 0);
            page.PackStart(s2, false, false, 0);

            var s3 = new CardBox(); var i3 = s3.Inner();
            i3.PackStart(Ui.Section("SENSORS", true), false, false, 0);
            var sub3 = Ui.Text("CPU temperature is read from the kernel's sensor driver (k10temp, zenpower or coretemp); nothing has to be installed.", "sub"); sub3.MarginBottom = 12;
            i3.PackStart(sub3, false, false, 0);
            svcStatus = Ui.Text("", "dim");
            i3.PackStart(svcStatus, false, false, 0);
            page.PackStart(s3, false, false, 0);

            var s4 = new CardBox(); var i4 = s4.Inner();
            i4.PackStart(Ui.Section("APPLICATION", true), false, false, 0);
            var sub4 = Ui.Text("How the app starts and closes, and the files it keeps.", "sub"); sub4.MarginBottom = 12;
            i4.PackStart(sub4, false, false, 0);
            var auto = new SwitchToggle("Start with the desktop session (in the tray)") { MarginBottom = 12 };
            auto.Toggled += v => { if (!loading) core.Platform.Autostart = v; };
            loaders.Add(() => auto.SetSilently(core.Platform.Autostart));
            i4.PackStart(auto, false, false, 0);
            i4.PackStart(Switch("Blank the screen when the app exits", c => c.BlankOnExit, (c, v) => c.BlankOnExit = v), false, false, 0);
            i4.PackStart(Switch("Keep running in the tray when the window closes", c => c.CloseToTray, (c, v) => c.CloseToTray = v), false, false, 0);
            i4.PackStart(Ui.Hair(), false, false, 0);
            var btns = new Box(Orientation.Horizontal, 8) { MarginTop = 16 };
            btns.PackStart(Ui.Button("Open settings folder", false, () => core.Platform.OpenFolder(Config.Folder)), false, false, 0);
            i4.PackStart(btns, false, false, 0);
            cacheInfo = Ui.Text("", "dim", "small"); cacheInfo.MarginTop = 8;
            i4.PackStart(cacheInfo, false, false, 0);
            page.PackStart(s4, false, false, 0);
            return page;
        }

        void UpdatePages()
        {
            if (devStatus != null)
            {
                devStatus.Text = model.DeviceStatus; devSerial.Text = model.SerialText; devFps.Text = model.DevFps;
            }
            if (gmStatus != null)
            {
                gmStatus.Text = model.GameStatus();
                if (gmForce.Active != core.Engine.ForceGame) gmForce.SetSilently(core.Engine.ForceGame);
                svcStatus.Text = core.Platform.SensorStatus;
                cacheInfo.Text = "Settings: " + Config.Folder;
            }
        }
    }
}
