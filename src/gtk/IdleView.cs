// Linux front end: the preview's Idle view (the picture the cooler shows by itself). Choose a picture, drag/scroll to
// place it, upload with read-back, or read the stored one from the cooler. The pipeline is the shared IdleEditor.
using System;
using Gtk;

namespace TtLcd.GtkUi
{
    partial class StudioWindow
    {
        IdleEditor idle;
        SwitchToggle idleSwitch;
        Widget liveCard; Box idlePanel, idleEditBox;
        Label idleTitle, idleInfo, idleStatus, idleZoomT;
        Slider idleZoom;
        Button idleBrowse, idleRead, idleStore, idleCancel;
        ProgressBar idleBar;
        bool idleOn;

        Box BuildIdlePanel()
        {
            idle = new IdleEditor(core);
            var card = new StatusCard { MarginTop = 24 };
            var b = new Box(Orientation.Vertical, 0) { MarginLeft = 14, MarginRight = 14, MarginTop = 10, MarginBottom = 10 };
            idleTitle = new Label("Idle image") { Halign = Align.Start }; idleTitle.StyleContext.AddClass("semibold");
            b.PackStart(idleTitle, false, false, 0);
            var hint = Ui.Text("The cooler shows this by itself when the PC isn't driving the screen (boot, sleep, powered off).", "dim", "small");
            hint.MaxWidthChars = 44;
            hint.MarginTop = 4; b.PackStart(hint, false, false, 0);

            idleEditBox = new Box(Orientation.Vertical, 0) { MarginTop = 12, NoShowAll = true };
            idleZoom = new Slider(1, 5, 0); idleZoomT = new Label("1.0×");
            idleZoom.Changed += v => { idle.SetZoom(v); idleZoomT.Text = v.ToString("0.0") + "×"; QueueIdleEncode(); };
            idleEditBox.PackStart(Ui.Row("Zoom", idleZoom, idleZoomT, 48), false, false, 0);
            idleInfo = Ui.Text("", "status"); idleInfo.MaxWidthChars = 44;
            idleEditBox.PackStart(idleInfo, false, false, 0);
            b.PackStart(idleEditBox, false, false, 0);

            idleBar = new ProgressBar { MarginTop = 12, NoShowAll = true };
            b.PackStart(idleBar, false, false, 0);
            idleStatus = Ui.Text("", "dim", "small"); idleStatus.MarginTop = 8; idleStatus.MaxWidthChars = 44;
            b.PackStart(idleStatus, false, false, 0);
            card.Add(b);

            var buttons = new Box(Orientation.Horizontal, 8) { MarginTop = 12 };
            idleBrowse = Ui.Button("Choose image…", false, ChooseIdle);
            idleRead = Ui.Button("Read from cooler", false, ReadIdle);
            idleStore = Ui.Button("Upload to cooler", true, StoreIdle);
            idleCancel = Ui.Button("Cancel", false, () => { idle.Drop(); ShowStoredIdle(); });
            foreach (var x in new[] { idleBrowse, idleRead, idleStore, idleCancel }) { x.NoShowAll = true; buttons.PackStart(x, false, false, 0); }

            idlePanel = new Box(Orientation.Vertical, 0) { NoShowAll = true };
            idlePanel.PackStart(card, false, false, 0);
            idlePanel.PackStart(buttons, false, false, 0);

            preview.Panned += (dx, dy) => { idle.Pan(dx, dy); QueueIdleEncode(); };
            preview.Zoomed += d => idleZoom.Set(idleZoom.Value * (d > 0 ? 1.1 : 1 / 1.1), true);
            return idlePanel;
        }

        static void Vis(Widget w, bool on) { w.NoShowAll = !on; if (on) w.ShowAll(); else w.Hide(); }

        // While the Idle view is open no frames go to the cooler, so its own idle screen shows on the LCD.
        void SetPreviewMode(bool on)
        {
            idleOn = on;
            core.Engine.Suspended = on && Visible;
            preview.IdleMode = on;
            Vis(liveCard, !on); Vis(idlePanel, on);
            ((Label)previewTitle).Text = on ? "IDLE IMAGE" : "LIVE PREVIEW";
            UpdateIdleTemp();
            if (!on) return;
            if (!idle.Busy) idleStatus.Text = "Streaming paused: the cooler shows its idle screen now (after a few seconds).";
            if (idle.Editing) { EncodeIdle(); return; }
            ShowStoredIdle();
            if (!System.IO.File.Exists(IdleEditor.Cache) && !idle.TriedRead) ReadIdle(); // nothing known yet: ask the cooler once
        }

        void UpdateIdleTemp()
        {
            double w = core.Engine.Coolant;
            preview.TempText = idleOn && Render.WaterValid(w) ? Math.Round(w) + "°C" : null;
        }

        void ShowStoredIdle()
        {
            idle.Jpeg = null;
            var cur = IdleEditor.Stored();
            preview.Editing = false;
            preview.SetIdle(cur, StudioModel.IdlePreviewAngle(core.Config));
            idleTitle.Text = cur != null ? "Current idle image" : "Idle image";
            Vis(idleEditBox, false); Vis(idleStore, false); Vis(idleCancel, false);
            Vis(idleBrowse, true); Vis(idleRead, true);
        }

        void ChooseIdle()
        {
            string f = Ui.ChooseFile(this, "Choose the idle picture", core.Config.IdlePath, "Images and GIF", "*.jpg", "*.jpeg", "*.png", "*.bmp", "*.gif");
            if (f == null) return;
            try { idle.Load(f); }
            catch (Exception e) { idleStatus.Text = "Could not open the file: " + e.Message; return; }
            idleZoom.Set(1, false); idleZoomT.Text = "1.0×";
            idleTitle.Text = "New idle image";
            idleStatus.Text = "Drag the picture to position it, scroll to zoom.";
            Vis(idleEditBox, true); Vis(idleStore, true); Vis(idleCancel, true); Vis(idleRead, false);
            preview.Editing = true;
            ShowEncodedIdle();
        }

        uint idleEncodeTimer;
        void QueueIdleEncode()
        {
            if (!idle.Editing || idleEncodeTimer != 0) return;
            idleEncodeTimer = GLib.Timeout.Add(50, () => { idleEncodeTimer = 0; EncodeIdle(); return false; });
        }

        void EncodeIdle() { idle.Encode(); ShowEncodedIdle(); }

        void ShowEncodedIdle()
        {
            if (!idle.Editing) return;
            idleInfo.Text = idle.Info;
            idleStore.Sensitive = idle.Fits && !idle.Busy;
            preview.SetIdle(idle.Jpeg, 0);   // a picture being placed stays upright for dragging
        }

        void SetIdleBusy(bool busy, string status)
        {
            Vis(idleBar, busy); idleBar.Fraction = 0;
            idleBrowse.Sensitive = idleRead.Sensitive = idleCancel.Sensitive = !busy;
            idleStore.Sensitive = !busy && idle.Fits;
            if (status != null) idleStatus.Text = status;
            if (!busy) preview.Progress = -1;
        }

        void IdleProgress(int pct, string what)
        {
            pct = Math.Max(0, Math.Min(100, pct));
            idleBar.Fraction = pct / 100.0;
            idleStatus.Text = what + "… " + pct + "%";
            preview.Progress = pct;
            preview.QueueDraw();
        }

        // engine callbacks arrive on its control thread
        static void OnUi(System.Action a) { Application.Invoke((o, e) => a()); }

        void ReadIdle()
        {
            if (idle.Busy) return;
            SetIdleBusy(true, core.Engine.Connected ? "Reading the idle image from the cooler…" : "Waiting for the cooler…");
            idle.Read(p => OnUi(() => IdleProgress(p, "Reading from the cooler")),
                status => OnUi(() => { SetIdleBusy(false, status); if (!idle.Editing) ShowStoredIdle(); }));
        }

        void StoreIdle()
        {
            if (idle.Jpeg == null || idle.Busy) return;
            SetIdleBusy(true, core.Engine.Connected ? "Writing to the cooler… do not unplug." : "Waiting for the cooler…");
            idle.Store(p => OnUi(() => IdleProgress(p, IdleEditor.StoreProgressText(p))),
                (status, ok) => OnUi(() => { if (ok) { idle.Drop(); ShowStoredIdle(); } SetIdleBusy(false, status); }));
        }
    }
}
