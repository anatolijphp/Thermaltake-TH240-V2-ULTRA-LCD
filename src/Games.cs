// Game detection, using only what Windows provides:
//  - exclusive full-screen Direct3D apps: SHQueryUserNotificationState == QUNS_RUNNING_D3D_FULL_SCREEN
//    (the same signal Windows uses to hold back notifications during games)
//  - games the Xbox Game Bar knows: HKCU\System\GameConfigStore\Children\*\MatchedExeFullPath; any of them running
//    counts (covers borderless/windowed games too)
//  - optionally any foreground window that covers its whole monitor (also catches full-screen video)
//  - extra exe names from the settings
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using Microsoft.Win32;

namespace TtLcd
{
    public static class GameDetect
    {
        [DllImport("shell32.dll")] static extern int SHQueryUserNotificationState(out int state);
        const int QunsRunningD3DFullScreen = 3;

        [DllImport("user32.dll")] static extern IntPtr GetForegroundWindow();
        [DllImport("user32.dll")] static extern uint GetWindowThreadProcessId(IntPtr hwnd, out uint pid);
        [DllImport("user32.dll")] static extern bool GetWindowRect(IntPtr hwnd, out RECT r);
        [DllImport("user32.dll", CharSet = CharSet.Unicode)] static extern int GetClassName(IntPtr hwnd, StringBuilder s, int max);
        [DllImport("user32.dll")] static extern IntPtr MonitorFromWindow(IntPtr hwnd, uint flags);
        [DllImport("user32.dll")] static extern bool GetMonitorInfo(IntPtr mon, ref MONITORINFO mi);
        [DllImport("kernel32.dll")] static extern IntPtr OpenProcess(uint access, bool inherit, uint pid);
        [DllImport("kernel32.dll")] static extern bool CloseHandle(IntPtr h);
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode)] static extern bool QueryFullProcessImageName(IntPtr h, int flags, StringBuilder name, ref int size);
        const uint ProcessQueryLimited = 0x1000;

        [StructLayout(LayoutKind.Sequential)] struct RECT { public int L, T, R, B; }
        [StructLayout(LayoutKind.Sequential)] struct MONITORINFO { public int Size; public RECT Monitor, Work; public uint Flags; }

        // Name of the detected game (exe without extension), or null.
        public static string Detect(Config c)
        {
            var ignore = ParseNames(c.GameIgnore);
            int st;
            if (c.GameFullscreen && SHQueryUserNotificationState(out st) == 0 && st == QunsRunningD3DFullScreen)
            {
                var fg = ForegroundName();
                if (fg == null || !ignore.Contains(fg.ToLowerInvariant())) return fg ?? "full-screen game";
            }
            if (c.GameBorderless)
            {
                var fs = FullScreenForeground();
                if (fs != null && !ignore.Contains(fs.ToLowerInvariant())) return fs;
            }
            var extra = ParseNames(c.GameExtra);
            var known = c.GameKnownList ? KnownGames() : null;
            if (extra.Count == 0 && (known == null || known.Count == 0)) return null;
            int self = Process.GetCurrentProcess().Id;
            foreach (var p in Process.GetProcesses())
                using (p)
                {
                    if (p.Id == self || p.Id == 0) continue;
                    string name = p.ProcessName.ToLowerInvariant();
                    if (ignore.Contains(name)) continue;
                    if (extra.Contains(name)) return p.ProcessName;
                    List<string> paths;
                    if (known != null && known.TryGetValue(name, out paths))
                    {
                        string path = ImagePath(p.Id);
                        if (path == null || paths.Exists(x => string.Equals(x, path, StringComparison.OrdinalIgnoreCase))) return p.ProcessName;
                    }
                }
            return null;
        }

        // "eldenring.exe, Cyberpunk2077" -> {"eldenring", "cyberpunk2077"}
        public static HashSet<string> ParseNames(string list)
        {
            var r = new HashSet<string>();
            foreach (var part in (list ?? "").Split(new[] { ',', ';', '\n', '\r' }, StringSplitOptions.RemoveEmptyEntries))
            {
                string n = part.Trim().ToLowerInvariant();
                if (n.EndsWith(".exe")) n = n.Substring(0, n.Length - 4);
                if (n.Length > 0) r.Add(n);
            }
            return r;
        }

        // exe name (lower, no extension) -> full paths Windows' Game Bar has recorded as games; cached 5 minutes
        static Dictionary<string, List<string>> known;
        static DateTime knownAt;
        static Dictionary<string, List<string>> KnownGames()
        {
            if (known != null && (DateTime.UtcNow - knownAt).TotalMinutes < 5) return known;
            var d = new Dictionary<string, List<string>>();
            try
            {
                using (var k = Registry.CurrentUser.OpenSubKey(@"System\GameConfigStore\Children"))
                    if (k != null)
                        foreach (var sub in k.GetSubKeyNames())
                            using (var s = k.OpenSubKey(sub))
                            {
                                var path = s == null ? null : s.GetValue("MatchedExeFullPath") as string;
                                if (string.IsNullOrEmpty(path)) continue;
                                string name = Path.GetFileNameWithoutExtension(path).ToLowerInvariant();
                                List<string> l;
                                if (!d.TryGetValue(name, out l)) d[name] = l = new List<string>();
                                l.Add(path);
                            }
            }
            catch (Exception) { }
            known = d; knownAt = DateTime.UtcNow;
            return d;
        }

        public static int KnownGameCount { get { return KnownGames().Count; } }

        static string ImagePath(int pid)
        {
            var h = OpenProcess(ProcessQueryLimited, false, (uint)pid);
            if (h == IntPtr.Zero) return null;
            try
            {
                var sb = new StringBuilder(1024); int size = sb.Capacity;
                return QueryFullProcessImageName(h, 0, sb, ref size) ? sb.ToString() : null;
            }
            finally { CloseHandle(h); }
        }

        static string ForegroundName()
        {
            uint pid;
            GetWindowThreadProcessId(GetForegroundWindow(), out pid);
            try { using (var p = Process.GetProcessById((int)pid)) return p.ProcessName; }
            catch (Exception) { return null; }
        }

        // foreground window that exactly covers its monitor (borderless full-screen), excluding the desktop/taskbar
        static string FullScreenForeground()
        {
            var hwnd = GetForegroundWindow();
            if (hwnd == IntPtr.Zero) return null;
            var cls = new StringBuilder(64);
            GetClassName(hwnd, cls, cls.Capacity);
            string c = cls.ToString();
            if (c == "Progman" || c == "WorkerW" || c == "Shell_TrayWnd" || c == "Shell_SecondaryTrayWnd") return null;
            uint pid;
            GetWindowThreadProcessId(hwnd, out pid);
            if (pid == (uint)Process.GetCurrentProcess().Id) return null;
            RECT r;
            if (!GetWindowRect(hwnd, out r)) return null;
            var mi = new MONITORINFO { Size = Marshal.SizeOf(typeof(MONITORINFO)) };
            if (!GetMonitorInfo(MonitorFromWindow(hwnd, 2), ref mi)) return null;
            bool covers = r.L <= mi.Monitor.L && r.T <= mi.Monitor.T && r.R >= mi.Monitor.R && r.B >= mi.Monitor.B;
            return covers ? ForegroundName() : null;
        }
    }
}
