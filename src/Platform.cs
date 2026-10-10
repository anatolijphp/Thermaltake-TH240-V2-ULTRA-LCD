// What differs between Windows and Linux under the same Studio UI: autostart, the CPU temperature source, game
// detection, opening folders and running the command-line tool.
using System;
using System.Diagnostics;
using System.IO;
using System.Threading;

namespace TtLcd
{
    interface IPlatform
    {
        bool Autostart { get; set; }       // start with Windows / with the desktop session, in the tray
        bool GameDetection { get; }        // automatic game detection is available
        int KnownGameCount { get; }        // games Windows knows about; -1 = not applicable
        bool CpuTempNeedsSetup { get; }    // CPU temperature needs a one-time install (dashboard hint)
        bool SensorsManaged { get; }       // Settings can install/uninstall the CPU temperature source
        bool SensorInstalled { get; }
        string SensorStatus { get; }
        void OpenFolder(string path);
        // Runs the command-line tool (on Windows it elevates itself when needed); done(output) on a worker thread.
        void RunCli(string args, Action<string> done);
    }

    abstract class PlatformBase
    {
        protected static string Cli { get { return Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "ttlcd.exe"); } }

        protected static void Run(string exe, string args, Action<string> done)
        {
            ThreadPool.QueueUserWorkItem(_ =>
            {
                string output;
                try
                {
                    var psi = new ProcessStartInfo(exe, args) { UseShellExecute = false, RedirectStandardOutput = true, RedirectStandardError = true, CreateNoWindow = true };
                    using (var p = Process.Start(psi)) { output = p.StandardOutput.ReadToEnd() + p.StandardError.ReadToEnd(); p.WaitForExit(); }
                }
                catch (Exception e) { output = "error: " + e.Message; }
                done(output);
            });
        }
    }

    class WindowsPlatform : PlatformBase, IPlatform
    {
        public bool Autostart { get { return Config.StartWithWindows; } set { Config.StartWithWindows = value; } }
        public bool GameDetection { get { return true; } }
        public int KnownGameCount { get { return GameDetect.KnownGameCount; } }
        public bool CpuTempNeedsSetup { get { return !SensorService.Installed; } }
        public bool SensorsManaged { get { return true; } }
        public bool SensorInstalled { get { return SensorService.Installed; } }
        public string SensorStatus
        {
            get
            {
                bool inst = SensorService.Installed, run = inst && SensorService.Running;
                return !inst ? "○ Service not installed" : run ? "● Service running" : "● Installed · not running";
            }
        }
        public void OpenFolder(string path) { Process.Start("explorer.exe", path); }
        public void RunCli(string args, Action<string> done) { Run(Cli, args, done); }
    }

    class LinuxPlatform : PlatformBase, IPlatform
    {
        static string AutostartFile
        {
            get { return Path.Combine(Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "autostart"), "ttlcd-studio.desktop"); }
        }

        public bool Autostart
        {
            get { return File.Exists(AutostartFile); }
            set
            {
                if (!value) { if (File.Exists(AutostartFile)) File.Delete(AutostartFile); return; }
                Directory.CreateDirectory(Path.GetDirectoryName(AutostartFile));
                File.WriteAllText(AutostartFile,
                    "[Desktop Entry]\nType=Application\nName=TT LCD Studio\nComment=Cooler screen (in the tray)\n" +
                    "Exec=ttlcd-studio --tray\nIcon=ttlcd-studio\nX-GNOME-Autostart-enabled=true\n");
            }
        }
        public bool GameDetection { get { return false; } }
        public int KnownGameCount { get { return -1; } }
        public bool CpuTempNeedsSetup { get { return false; } }
        public bool SensorsManaged { get { return false; } }
        public bool SensorInstalled { get { return CpuTemp.C() > 0; } }
        public string SensorStatus
        {
            get { return CpuTemp.C() > 0 ? "● Read from the kernel's CPU sensor driver" : "○ No CPU sensor driver found (k10temp, zenpower or coretemp)"; }
        }
        public void OpenFolder(string path) { try { Process.Start("xdg-open", path); } catch (Exception) { } }
        public void RunCli(string args, Action<string> done) { Run("mono", "\"" + Cli + "\" " + args, done); }

        // The `ttlcd run` user service drives the cooler too; Studio takes over while it runs.
        public static bool ServiceActive()
        {
            try
            {
                var psi = new ProcessStartInfo("systemctl", "--user is-active ttlcd") { UseShellExecute = false, RedirectStandardOutput = true, CreateNoWindow = true };
                using (var p = Process.Start(psi)) { string o = p.StandardOutput.ReadToEnd().Trim(); p.WaitForExit(); return o == "active"; }
            }
            catch (Exception) { return false; }
        }

        public static void StopService()
        {
            try { using (var p = Process.Start(new ProcessStartInfo("systemctl", "--user stop ttlcd") { UseShellExecute = false, CreateNoWindow = true })) p.WaitForExit(5000); }
            catch (Exception) { }
        }
    }
}
