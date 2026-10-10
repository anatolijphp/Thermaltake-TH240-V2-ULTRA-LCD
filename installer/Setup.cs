// TT LCD Studio installer: one exe that carries the application files as embedded resources ("payload/<path>",
// added by build.cmd). Per-user install (no admin) to %LOCALAPPDATA%\Programs\TT LCD Studio, Start menu / desktop
// shortcuts, "start with Windows", an Apps & features entry and an uninstaller (a copy of this exe).
// Optional components that need administrator rights (they ask via UAC) are installed through ttlcd.exe:
// the CPU temperature service, the Armoury Crate Aura plugin and the virtual 480x480 display driver.
//
//   TTLcdStudio-Setup.exe                     interactive install
//   TTLcdStudio-Setup.exe /S [/D=<dir>]       silent install (shortcuts + start with Windows, no admin components)
//   Uninstall.exe /uninstall [/S] [/all]      uninstall (from the install folder; Apps & features calls it). /all also
//                                             removes the shared Aura plugin and virtual display (asked when interactive)
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;
using System.Windows.Forms;
using Microsoft.Win32;

static class Setup
{
    const string AppName = "TT LCD Studio";
    const string Publisher = "TT LCD Studio";
    const string UninstallKey = @"Software\Microsoft\Windows\CurrentVersion\Uninstall\TTLcdStudio";
    const string RunKey = @"Software\Microsoft\Windows\CurrentVersion\Run";
    const string RunName = "TTLcd";   // same value the app writes for "Start with Windows"
    static string Version { get { return Assembly.GetExecutingAssembly().GetName().Version.ToString(); } }

    static string DefaultDir
    {
        get { return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "Programs", AppName); }
    }

    [STAThread]
    static int Main(string[] args)
    {
        Application.EnableVisualStyles();
        bool silent = args.Any(a => a.Equals("/S", StringComparison.OrdinalIgnoreCase));
        try
        {
            bool all = args.Any(a => a.Equals("/all", StringComparison.OrdinalIgnoreCase)); // uninstall: also the shared Aura plugin / display driver
            if (args.Any(a => a.Equals("/uninstall", StringComparison.OrdinalIgnoreCase))) return Uninstaller.Start(silent, all);
            if (args.Any(a => a.Equals("/uninstall-run", StringComparison.OrdinalIgnoreCase)))
                return Uninstaller.Run(args.SkipWhile(a => !a.Equals("/uninstall-run", StringComparison.OrdinalIgnoreCase)).Skip(1).FirstOrDefault(), silent, all);
            if (silent)
            {
                var d = args.FirstOrDefault(a => a.StartsWith("/D=", StringComparison.OrdinalIgnoreCase));
                var o = new Options { Dir = d != null ? d.Substring(3).Trim('"') : DefaultDir, StartMenu = true, Desktop = false, Autostart = true };
                Install(o, s => Console.WriteLine(s));
                return 0;
            }
            Application.Run(new SetupForm());
            return 0;
        }
        catch (Exception e)
        {
            if (silent) Console.Error.WriteLine("error: " + e.Message);
            else MessageBox.Show(e.Message, AppName + " setup", MessageBoxButtons.OK, MessageBoxIcon.Error);
            return 1;
        }
    }

    public class Options
    {
        public string Dir;
        public bool StartMenu, Desktop, Autostart, Launch, SensorService, Aura, VirtualDisplay;
    }

    // ---------------- install ----------------
    public static void Install(Options o, Action<string> status)
    {
        string dir = Path.GetFullPath(o.Dir);
        Directory.CreateDirectory(dir);
        status("Closing a running TT LCD Studio…");
        StopApp(dir);

        var asm = Assembly.GetExecutingAssembly();
        var files = asm.GetManifestResourceNames().Where(n => n.StartsWith("payload/")).ToList();
        if (files.Count == 0) throw new IOException("this setup contains no files (broken build)");
        foreach (var res in files)
        {
            string rel = res.Substring("payload/".Length).Replace('/', Path.DirectorySeparatorChar);
            string target = Path.Combine(dir, rel);
            status("Copying " + rel);
            Directory.CreateDirectory(Path.GetDirectoryName(target));
            using (var s = asm.GetManifestResourceStream(res)) WriteFile(target, s);
        }
        // the uninstaller is a copy of this setup
        string self = asm.Location, uninst = Path.Combine(dir, "Uninstall.exe");
        if (!string.Equals(Path.GetFullPath(self), uninst, StringComparison.OrdinalIgnoreCase))
            using (var s = File.OpenRead(self)) WriteFile(uninst, s);

        string studio = Path.Combine(dir, "TTLcdStudio.exe");
        status("Creating shortcuts…");
        string startMenu = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.Programs), AppName + ".lnk");
        string desktop = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.DesktopDirectory), AppName + ".lnk");
        if (o.StartMenu) Shortcut(startMenu, studio, "", dir); else TryDelete(startMenu);
        if (o.Desktop) Shortcut(desktop, studio, "", dir); else TryDelete(desktop);

        using (var k = Registry.CurrentUser.CreateSubKey(RunKey))
        {
            if (o.Autostart) k.SetValue(RunName, "\"" + studio + "\" --tray");
            else if (OwnedByUs(k.GetValue(RunName) as string, dir)) k.DeleteValue(RunName, false);
        }

        using (var k = Registry.CurrentUser.CreateSubKey(UninstallKey))
        {
            k.SetValue("DisplayName", AppName);
            k.SetValue("DisplayVersion", Version);
            k.SetValue("Publisher", Publisher);
            k.SetValue("DisplayIcon", studio);
            k.SetValue("InstallLocation", dir);
            k.SetValue("UninstallString", "\"" + uninst + "\" /uninstall");
            k.SetValue("QuietUninstallString", "\"" + uninst + "\" /uninstall /S");
            k.SetValue("NoModify", 1, RegistryValueKind.DWord);
            k.SetValue("NoRepair", 1, RegistryValueKind.DWord);
            k.SetValue("EstimatedSize", (int)(Directory.GetFiles(dir, "*", SearchOption.AllDirectories).Sum(f => new FileInfo(f).Length) / 1024), RegistryValueKind.DWord);
        }

        // optional parts that need administrator rights: ttlcd.exe elevates itself (UAC prompt)
        string cli = Path.Combine(dir, "ttlcd.exe");
        if (o.SensorService) { status("Installing the CPU temperature service (confirm the UAC prompt)…"); RunCli(cli, "service install"); }
        if (o.Aura) { status("Installing the Aura Sync plugin (confirm the UAC prompt)…"); RunCli(cli, "aura install"); }
        if (o.VirtualDisplay) { status("Installing the virtual display (confirm the UAC prompt)…"); RunCli(cli, "display install"); }

        if (o.Launch) { status("Starting TT LCD Studio…"); Process.Start(new ProcessStartInfo(studio) { WorkingDirectory = dir }); }
        status("Installed to " + dir);
    }

    static bool OwnedByUs(string runValue, string dir)
    {
        return runValue != null && runValue.IndexOf(dir, StringComparison.OrdinalIgnoreCase) >= 0;
    }

    // Write a file; if it is in use (a running exe), move it aside first — Windows allows renaming a running exe.
    static void WriteFile(string target, Stream src)
    {
        try { using (var f = File.Create(target)) src.CopyTo(f); return; }
        catch (IOException) { }
        catch (UnauthorizedAccessException) { }
        string aside = target + "." + DateTime.Now.Ticks + ".old";
        File.Move(target, aside);
        using (var f = File.Create(target)) src.CopyTo(f);
    }

    public static void StopApp(string dir)
    {
        foreach (var p in Process.GetProcessesByName("TTLcdStudio"))
            using (p)
                try
                {
                    string path = p.MainModule.FileName;
                    if (!path.StartsWith(dir, StringComparison.OrdinalIgnoreCase)) continue;
                    if (!p.CloseMainWindow() || !p.WaitForExit(3000)) { p.Kill(); p.WaitForExit(3000); }
                }
                catch (Exception) { }
    }

    public static int RunCli(string cli, string args)
    {
        if (!File.Exists(cli)) return -1;
        try
        {
            var psi = new ProcessStartInfo(cli, args) { UseShellExecute = false, CreateNoWindow = true, RedirectStandardOutput = true, RedirectStandardError = true };
            using (var p = Process.Start(psi)) { p.StandardOutput.ReadToEnd(); p.StandardError.ReadToEnd(); p.WaitForExit(); return p.ExitCode; }
        }
        catch (Exception) { return -1; }
    }

    // .lnk through the Windows Script Host COM object (part of Windows), late-bound so no interop assembly is needed
    static void Shortcut(string lnk, string target, string args, string workDir)
    {
        var t = Type.GetTypeFromProgID("WScript.Shell");
        object shell = Activator.CreateInstance(t);
        try
        {
            object sc = t.InvokeMember("CreateShortcut", BindingFlags.InvokeMethod, null, shell, new object[] { lnk });
            var st = sc.GetType();
            st.InvokeMember("TargetPath", BindingFlags.SetProperty, null, sc, new object[] { target });
            st.InvokeMember("Arguments", BindingFlags.SetProperty, null, sc, new object[] { args });
            st.InvokeMember("WorkingDirectory", BindingFlags.SetProperty, null, sc, new object[] { workDir });
            st.InvokeMember("IconLocation", BindingFlags.SetProperty, null, sc, new object[] { target + ",0" });
            st.InvokeMember("Description", BindingFlags.SetProperty, null, sc, new object[] { "Thermaltake cooler LCD control" });
            st.InvokeMember("Save", BindingFlags.InvokeMethod, null, sc, null);
        }
        finally { System.Runtime.InteropServices.Marshal.FinalReleaseComObject(shell); }
    }

    public static void TryDelete(string f) { try { if (File.Exists(f)) File.Delete(f); } catch (Exception) { } }

    // ---------------- uninstall ----------------
    static class Uninstaller
    {
        // Started from the install folder: copy ourselves to %TEMP% and run from there, so the folder can be deleted.
        public static int Start(bool silent, bool all)
        {
            string self = Assembly.GetExecutingAssembly().Location, dir = Path.GetDirectoryName(self);
            string tmp = Path.Combine(Path.GetTempPath(), "TTLcdStudio-uninstall-" + Guid.NewGuid().ToString("N").Substring(0, 8) + ".exe");
            File.Copy(self, tmp, true);
            Process.Start(new ProcessStartInfo(tmp, "/uninstall-run \"" + dir + "\"" + (silent ? " /S" : "") + (all ? " /all" : "")) { UseShellExecute = false });
            return 0;
        }

        public static int Run(string dir, bool silent, bool removeShared)
        {
            if (string.IsNullOrEmpty(dir) || !File.Exists(Path.Combine(dir, "TTLcdStudio.exe")))
                throw new IOException("TT LCD Studio was not found in " + dir);
            if (!silent && MessageBox.Show("Remove TT LCD Studio from this PC?\n\nYour settings in %APPDATA%\\ttlcd are kept.",
                    AppName, MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes) { SelfDelete(); return 1; }

            StopApp(dir);
            // admin parts (each asks via UAC). The sensor service is removed only if it runs from this folder; the Aura
            // plugin and the virtual display are shared system-wide (another copy may use them), so only on request.
            string cli = Path.Combine(dir, "ttlcd.exe");
            if (ServiceOwnedBy(dir)) RunCli(cli, "service uninstall");
            bool aura = AuraInstalled(), display = DisplayInstalled(cli);
            if ((aura || display) && (removeShared || (!silent && MessageBox.Show(
                    "Also remove " + (aura && display ? "the Aura Sync plugin and the virtual display driver" : aura ? "the Aura Sync plugin" : "the virtual display driver") +
                    "?\n(Needs administrator rights.)", AppName, MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)))
            {
                if (aura) RunCli(cli, "aura uninstall");
                if (display) RunCli(cli, "display uninstall");
            }

            using (var k = Registry.CurrentUser.CreateSubKey(RunKey))
                if (OwnedByUs(k.GetValue(RunName) as string, dir)) k.DeleteValue(RunName, false);
            TryDelete(Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.Programs), AppName + ".lnk"));
            TryDelete(Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.DesktopDirectory), AppName + ".lnk"));
            Registry.CurrentUser.DeleteSubKeyTree(UninstallKey, false);

            for (int i = 0; i < 10 && Directory.Exists(dir); i++)
            {
                try { Directory.Delete(dir, true); }
                catch (Exception) { Thread.Sleep(500); }
            }
            if (!silent)
                MessageBox.Show(Directory.Exists(dir) ? "TT LCD Studio was removed. Some files were in use and stay in\n" + dir + "\nuntil the next restart."
                                                      : "TT LCD Studio was removed.", AppName, MessageBoxButtons.OK, MessageBoxIcon.Information);
            SelfDelete();
            return 0;
        }

        static bool ServiceOwnedBy(string dir)
        {
            using (var k = Registry.LocalMachine.OpenSubKey(@"SYSTEM\CurrentControlSet\Services\TTLcdSensors"))
            {
                var path = k == null ? null : k.GetValue("ImagePath") as string;
                return path != null && path.IndexOf(dir, StringComparison.OrdinalIgnoreCase) >= 0;
            }
        }

        static bool AuraInstalled()
        {
            using (var hk = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, RegistryView.Registry32))
            using (var k = hk.OpenSubKey(@"SOFTWARE\Classes\CLSID\{C0C96C60-30E3-4021-9904-966EC4FA0F0F}")) return k != null;
        }

        static bool DisplayInstalled(string cli)
        {
            try
            {
                var psi = new ProcessStartInfo(cli, "display status") { UseShellExecute = false, CreateNoWindow = true, RedirectStandardOutput = true };
                using (var p = Process.Start(psi)) { string o = p.StandardOutput.ReadToEnd(); p.WaitForExit(); return o.Contains("driver package:") && !o.Contains("driver package: not installed"); }
            }
            catch (Exception) { return false; }
        }

        // the temp copy deletes itself once it has exited
        static void SelfDelete()
        {
            string self = Assembly.GetExecutingAssembly().Location;
            if (!self.StartsWith(Path.GetTempPath(), StringComparison.OrdinalIgnoreCase)) return;
            Process.Start(new ProcessStartInfo("cmd.exe", "/c ping -n 3 127.0.0.1 > nul & del /f /q \"" + self + "\"")
                          { CreateNoWindow = true, UseShellExecute = false, WindowStyle = ProcessWindowStyle.Hidden });
        }
    }

    // ---------------- UI ----------------
    class SetupForm : Form
    {
        static readonly Color Bg = Color.FromArgb(13, 13, 14), Card = Color.FromArgb(27, 27, 29), Line = Color.FromArgb(41, 41, 44),
                              Fg = Color.FromArgb(237, 237, 237), Dim = Color.FromArgb(143, 143, 148), Accent = Color.FromArgb(255, 31, 75);
        readonly TextBox dir = new TextBox();
        readonly CheckBox startMenu = Box("Start menu shortcut", true), desktop = Box("Desktop shortcut", true),
                          autostart = Box("Start with Windows (in the tray)", true), launch = Box("Launch TT LCD Studio when done", true),
                          sensors = Box("CPU temperature service (needs TOWEI; administrator)", false),
                          aura = Box("Armoury Crate / Aura Sync plugin (administrator)", false),
                          vdd = Box("Virtual 480×480 display for Wallpaper Engine (driver; administrator)", false);
        readonly Button install = new Button(), browse = new Button();
        readonly Label status = new Label();
        readonly ProgressBar bar = new ProgressBar();
        bool done;

        static CheckBox Box(string text, bool on)
        {
            return new CheckBox { Text = text, Checked = on, AutoSize = true, ForeColor = Fg, FlatStyle = FlatStyle.Flat, Margin = new Padding(0, 4, 0, 4) };
        }

        public SetupForm()
        {
            Text = AppName + " setup";
            FormBorderStyle = FormBorderStyle.FixedDialog; MaximizeBox = false; StartPosition = FormStartPosition.CenterScreen;
            ClientSize = new Size(560, 470); BackColor = Bg; ForeColor = Fg; Font = new Font("Segoe UI", 9.5f);
            try { Icon = Icon.ExtractAssociatedIcon(Assembly.GetExecutingAssembly().Location); } catch (Exception) { }

            var title = new Label { Text = "TT  LCD STUDIO", Font = new Font("Segoe UI Semibold", 16f), ForeColor = Fg, AutoSize = true, Location = new Point(24, 20) };
            var rgb = new Panel { Location = new Point(24, 58), Size = new Size(512, 2) };
            rgb.Paint += (s, e) =>
            {
                using (var br = new System.Drawing.Drawing2D.LinearGradientBrush(rgb.ClientRectangle, Accent, Color.FromArgb(0, 225, 255), 0f))
                {
                    var blend = new System.Drawing.Drawing2D.ColorBlend
                    {
                        Colors = new[] { Accent, Color.FromArgb(255, 230, 0), Color.FromArgb(0, 225, 255), Color.FromArgb(122, 75, 255), Color.FromArgb(255, 43, 214) },
                        Positions = new[] { 0f, 0.25f, 0.5f, 0.75f, 1f },
                    };
                    br.InterpolationColors = blend;
                    e.Graphics.FillRectangle(br, rgb.ClientRectangle);
                }
            };
            var sub = new Label { Text = "Control the LCD of your Thermaltake TH240 V2 Ultra cooler.  Version " + Version, ForeColor = Dim, AutoSize = true, Location = new Point(24, 70) };

            var dirLabel = new Label { Text = "INSTALL TO", ForeColor = Dim, Font = new Font("Segoe UI Semibold", 8.5f), AutoSize = true, Location = new Point(24, 106) };
            dir.Text = DefaultDir; dir.Location = new Point(24, 128); dir.Size = new Size(420, 26);
            dir.BackColor = Card; dir.ForeColor = Fg; dir.BorderStyle = BorderStyle.FixedSingle;
            Style(browse, "Browse…", false); browse.Location = new Point(452, 126); browse.Size = new Size(84, 28);
            browse.Click += (s, e) => { using (var d = new FolderBrowserDialog { SelectedPath = dir.Text }) if (d.ShowDialog(this) == DialogResult.OK) dir.Text = Path.Combine(d.SelectedPath, AppName); };

            var optLabel = new Label { Text = "OPTIONS", ForeColor = Dim, Font = new Font("Segoe UI Semibold", 8.5f), AutoSize = true, Location = new Point(24, 170) };
            var opts = new FlowLayoutPanel { Location = new Point(24, 192), Size = new Size(512, 190), FlowDirection = FlowDirection.TopDown, WrapContents = false };
            opts.Controls.AddRange(new Control[] { startMenu, desktop, autostart, launch, sensors, aura, vdd });

            bar.Location = new Point(24, 392); bar.Size = new Size(512, 4); bar.Style = ProgressBarStyle.Marquee; bar.Visible = false;
            status.Location = new Point(24, 402); status.Size = new Size(400, 40); status.ForeColor = Dim;
            Style(install, "Install", true); install.Location = new Point(436, 418); install.Size = new Size(100, 34);
            install.Click += (s, e) => { if (done) Close(); else DoInstall(); };
            AcceptButton = install;

            Controls.AddRange(new Control[] { title, rgb, sub, dirLabel, dir, browse, optLabel, opts, bar, status, install });
        }

        static void Style(Button b, string text, bool accent)
        {
            b.Text = text; b.FlatStyle = FlatStyle.Flat; b.Cursor = Cursors.Hand;
            b.BackColor = accent ? Accent : Card; b.ForeColor = accent ? Color.White : Fg;
            b.FlatAppearance.BorderColor = accent ? Accent : Line;
        }

        void DoInstall()
        {
            var o = new Options
            {
                Dir = dir.Text.Trim(), StartMenu = startMenu.Checked, Desktop = desktop.Checked, Autostart = autostart.Checked, Launch = launch.Checked,
                SensorService = sensors.Checked, Aura = aura.Checked, VirtualDisplay = vdd.Checked,
            };
            if (o.Dir.Length == 0) return;
            foreach (Control c in Controls) if (c != status && c != bar) c.Enabled = false;
            bar.Visible = true;
            var t = new Thread(() =>
            {
                Exception err = null;
                try { Install(o, s => BeginInvoke(new Action(() => status.Text = s))); }
                catch (Exception e) { err = e; }
                BeginInvoke(new Action(() =>
                {
                    bar.Visible = false;
                    if (err != null)
                    {
                        status.Text = "Failed: " + err.Message; status.ForeColor = Accent;
                        foreach (Control c in Controls) c.Enabled = true;
                        return;
                    }
                    status.Text = "Done. TT LCD Studio is installed."; status.ForeColor = Color.FromArgb(43, 227, 107);
                    install.Text = "Close"; install.Enabled = true; done = true;
                }));
            }) { IsBackground = true };
            t.Start();
        }
    }
}
