// Installs/uninstalls the bundled MttVDD IddCx virtual display (driver\ next to ttlcd.exe) configured as a
// single 480x480 monitor, so Wallpaper Engine can render to it and 'ttlcd stream' can mirror it to the LCD.
using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.Text;
using System.Text.RegularExpressions;
using Microsoft.Win32;

namespace TtLcd
{
    static class VirtualDisplay
    {
        const string HwId = "Root\\MttVDD";
        const string RegKey = @"SOFTWARE\MikeTheTech\VirtualDisplayDriver";
        static readonly string SettingsDir = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), "ttlcd");
        static readonly Guid DisplayClass = new Guid("4d36e968-e325-11ce-bfc1-08002be10318");

        const int DICD_GENERATE_ID = 1, DIGCF_PRESENT = 2, DIGCF_ALLCLASSES = 4;
        const int SPDRP_HARDWAREID = 1, DIF_REGISTERDEVICE = 0x19, DIF_REMOVE = 5, INSTALLFLAG_FORCE = 1;

        [StructLayout(LayoutKind.Sequential)]
        struct SP_DEVINFO_DATA { public int cbSize; public Guid ClassGuid; public int DevInst; public IntPtr Reserved; }

        [DllImport("setupapi.dll", SetLastError = true)] static extern IntPtr SetupDiCreateDeviceInfoList(ref Guid g, IntPtr hwnd);
        [DllImport("setupapi.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        static extern bool SetupDiCreateDeviceInfo(IntPtr set, string name, ref Guid g, string desc, IntPtr hwnd, int flags, ref SP_DEVINFO_DATA d);
        [DllImport("setupapi.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        static extern bool SetupDiSetDeviceRegistryProperty(IntPtr set, ref SP_DEVINFO_DATA d, int prop, byte[] buf, int size);
        [DllImport("setupapi.dll", SetLastError = true)] static extern bool SetupDiCallClassInstaller(int fn, IntPtr set, ref SP_DEVINFO_DATA d);
        [DllImport("setupapi.dll", SetLastError = true)] static extern bool SetupDiDestroyDeviceInfoList(IntPtr set);
        [DllImport("setupapi.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        static extern IntPtr SetupDiGetClassDevs(IntPtr g, string enumerator, IntPtr hwnd, int flags);
        [DllImport("setupapi.dll", SetLastError = true)] static extern bool SetupDiEnumDeviceInfo(IntPtr set, int idx, ref SP_DEVINFO_DATA d);
        [DllImport("setupapi.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        static extern bool SetupDiGetDeviceRegistryProperty(IntPtr set, ref SP_DEVINFO_DATA d, int prop, out int type, byte[] buf, int size, out int req);
        [DllImport("newdev.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        static extern bool UpdateDriverForPlugAndPlayDevices(IntPtr hwnd, string hwid, string inf, int flags, out bool reboot);

        public static int Run(string action)
        {
            switch (action.ToLowerInvariant())
            {
                case "status": return Status();
                case "install": return Elevated("install") ? Install() : 0;
                case "uninstall": return Elevated("uninstall") ? Uninstall() : 0;
                default: Console.WriteLine("ttlcd display install|uninstall|status"); return 1;
            }
        }

        // Returns true if already admin; otherwise relaunches elevated (UAC prompt), waits, and returns false.
        static bool Elevated(string action)
        {
            if (new WindowsPrincipal(WindowsIdentity.GetCurrent()).IsInRole(WindowsBuiltInRole.Administrator)) return true;
            string log = Path.Combine(Path.GetTempPath(), "ttlcd-display.log");
            var psi = new ProcessStartInfo("cmd.exe", "/c \"\"" + Process.GetCurrentProcess().MainModule.FileName + "\" display " + action + " > \"" + log + "\" 2>&1\"")
            { Verb = "runas", UseShellExecute = true, WindowStyle = ProcessWindowStyle.Hidden };
            var p = Process.Start(psi);
            p.WaitForExit();
            if (File.Exists(log)) Console.Write(File.ReadAllText(log));
            return false;
        }

        static int Status()
        {
            int n = CountDevices(false);
            Console.WriteLine("virtual display devices: {0}", n);
            Console.WriteLine("driver package: {0}", FindOemInf() ?? "not installed");
            int mon = ScreenSource.FindBySize(Lcd.Width, Lcd.Height);
            Console.WriteLine("480x480 monitor: {0}", mon >= 0 ? "#" + mon : "none");
            return 0;
        }

        static int Install()
        {
            string drv = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "driver", "MttVDD.inf");
            if (!File.Exists(drv)) throw new FileNotFoundException("bundled driver missing", drv);
            if (CountDevices(false) > 0) { Console.WriteLine("already installed"); return 0; }

            Directory.CreateDirectory(SettingsDir);
            File.WriteAllText(Path.Combine(SettingsDir, "vdd_settings.xml"), SettingsXml, new UTF8Encoding(false));
            using (var k = Registry.LocalMachine.CreateSubKey(RegKey)) k.SetValue("VDDPATH", SettingsDir);

            var g = DisplayClass;
            IntPtr set = SetupDiCreateDeviceInfoList(ref g, IntPtr.Zero);
            try
            {
                var d = new SP_DEVINFO_DATA(); d.cbSize = Marshal.SizeOf(d);
                if (!SetupDiCreateDeviceInfo(set, "Display", ref g, null, IntPtr.Zero, DICD_GENERATE_ID, ref d)) Fail("SetupDiCreateDeviceInfo");
                byte[] ids = Encoding.Unicode.GetBytes(HwId + "\0\0");
                if (!SetupDiSetDeviceRegistryProperty(set, ref d, SPDRP_HARDWAREID, ids, ids.Length)) Fail("SetupDiSetDeviceRegistryProperty");
                if (!SetupDiCallClassInstaller(DIF_REGISTERDEVICE, set, ref d)) Fail("DIF_REGISTERDEVICE");
            }
            finally { SetupDiDestroyDeviceInfoList(set); }

            bool reboot;
            if (!UpdateDriverForPlugAndPlayDevices(IntPtr.Zero, HwId, drv, INSTALLFLAG_FORCE, out reboot))
            {
                int err = Marshal.GetLastWin32Error();
                Uninstall();
                throw new Exception("driver install failed, error 0x" + err.ToString("X") + " (rolled back)");
            }
            Console.WriteLine("installed virtual 480x480 display{0}", reboot ? " (reboot required)" : "");
            Console.WriteLine("Assign a Wallpaper Engine wallpaper to it, then run: ttlcd stream");
            return 0;
        }

        static int Uninstall()
        {
            int removed = CountDevices(true);
            Console.WriteLine("removed {0} virtual display device(s)", removed);
            string oem = FindOemInf();
            if (oem != null)
            {
                Pnputil("/delete-driver " + oem + " /uninstall /force");
                Console.WriteLine("deleted driver package " + oem);
            }
            Registry.LocalMachine.DeleteSubKeyTree(RegKey, false);
            using (var k = Registry.LocalMachine.OpenSubKey(@"SOFTWARE\MikeTheTech", true))
                if (k != null && k.SubKeyCount == 0 && k.ValueCount == 0) Registry.LocalMachine.DeleteSubKey(@"SOFTWARE\MikeTheTech", false);
            if (Directory.Exists(SettingsDir)) Directory.Delete(SettingsDir, true);
            Console.WriteLine("leftovers: devices={0} package={1}", CountDevices(false), FindOemInf() ?? "none");
            return 0;
        }

        // Counts (and optionally removes) devices whose hardware ID is Root\MttVDD.
        static int CountDevices(bool remove)
        {
            IntPtr set = SetupDiGetClassDevs(IntPtr.Zero, null, IntPtr.Zero, DIGCF_ALLCLASSES);
            int n = 0;
            try
            {
                var d = new SP_DEVINFO_DATA(); d.cbSize = Marshal.SizeOf(d);
                var buf = new byte[2048];
                for (int i = 0; SetupDiEnumDeviceInfo(set, i, ref d); i++)
                {
                    int type, req;
                    if (!SetupDiGetDeviceRegistryProperty(set, ref d, SPDRP_HARDWAREID, out type, buf, buf.Length, out req)) continue;
                    string ids = Encoding.Unicode.GetString(buf, 0, req);
                    if (ids.IndexOf(HwId, StringComparison.OrdinalIgnoreCase) < 0) continue;
                    n++;
                    if (remove && !SetupDiCallClassInstaller(DIF_REMOVE, set, ref d)) Fail("DIF_REMOVE");
                }
            }
            finally { SetupDiDestroyDeviceInfoList(set); }
            return n;
        }

        static string FindOemInf()
        {
            string output = Pnputil("/enum-drivers");
            string current = null;
            foreach (var line in output.Split('\n'))
            {
                var m = Regex.Match(line, @":\s*(oem\d+\.inf)", RegexOptions.IgnoreCase);
                if (m.Success) { current = m.Groups[1].Value; continue; }
                if (current != null && line.IndexOf("mttvdd.inf", StringComparison.OrdinalIgnoreCase) >= 0) return current;
            }
            return null;
        }

        static string Pnputil(string args)
        {
            var psi = new ProcessStartInfo(Path.Combine(Environment.SystemDirectory, "pnputil.exe"), args)
            { UseShellExecute = false, RedirectStandardOutput = true, CreateNoWindow = true };
            using (var p = Process.Start(psi)) { string s = p.StandardOutput.ReadToEnd(); p.WaitForExit(); return s; }
        }

        static void Fail(string what) { throw new Exception(what + " failed, error 0x" + Marshal.GetLastWin32Error().ToString("X")); }

        const string SettingsXml =
@"<?xml version='1.0' encoding='utf-8'?>
<vdd_settings>
    <monitors><count>1</count></monitors>
    <gpu><friendlyname>default</friendlyname></gpu>
    <global><g_refresh_rate>60</g_refresh_rate></global>
    <resolutions>
        <resolution><width>480</width><height>480</height><refresh_rate>60</refresh_rate></resolution>
    </resolutions>
    <options>
        <CustomEdid>false</CustomEdid>
        <PreventSpoof>false</PreventSpoof>
        <EdidCeaOverride>false</EdidCeaOverride>
        <HardwareCursor>true</HardwareCursor>
        <SDR10bit>false</SDR10bit>
        <HDRPlus>false</HDRPlus>
        <logging>false</logging>
        <debuglogging>false</debuglogging>
    </options>
</vdd_settings>
";
    }
}
