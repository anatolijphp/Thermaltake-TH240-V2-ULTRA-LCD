// CPU (and GPU) temperature read exactly as the vendor app (TOWEI) does: through its bundled HWiNFO-based
// XYDataCenter.dll (MXNSDK_* API), which needs admin rights for the sensor driver. So a small elevated helper
// (`ttlcd hwmon`) loads that DLL, reads cpu_temperature (struct offset 0x188), and publishes it to shared memory
// that TT LCD Studio (normal user) reads. No sensor access happens without that helper running.
using System;
using System.IO;
using System.IO.MemoryMappedFiles;
using System.Runtime.InteropServices;
using System.Threading;

namespace TtLcd
{
    public static class Sensors
    {
        const string MapName = @"Global\TTLcdSensors", MapNameLocal = @"Local\TTLcdSensors";
        const int Size = 32; // magic(4) cpuTempC(4) gpuTempC(4) cpuLoad(4) updatedTicks(8) intervalMs(4) ...
        const int IntervalOffset = 24; // written by Studio (game mode), read by the helper each cycle
        const uint Magic = 0x4E455354; // "TSEN"

        // ---- reader (Studio / normal user) ----
        static MemoryMappedViewAccessor view;
        static DateTime nextTry;
        static MemoryMappedViewAccessor View()
        {
            if (view != null) return view;
            if (DateTime.UtcNow < nextTry) return null;
            nextTry = DateTime.UtcNow.AddSeconds(2);
            foreach (var n in new[] { MapName, MapNameLocal })
                try { view = MemoryMappedFile.OpenExisting(n, MemoryMappedFileRights.Read).CreateViewAccessor(0, Size, MemoryMappedFileAccess.Read); return view; }
                catch (Exception) { }
            return null;
        }

        // CPU temperature in C, or -1 when the helper isn't running / data is stale.
        public static int CpuTempC()
        {
            try
            {
                var v = View();
                if (v == null || v.ReadUInt32(0) != Magic) return -1;
                long ticks = v.ReadInt64(16);
                int iv = v.ReadInt32(IntervalOffset);
                double maxAge = Math.Max(10, 3 * (iv > 0 ? iv : 1500) / 1000.0);
                if ((DateTime.UtcNow - new DateTime(ticks, DateTimeKind.Utc)).TotalSeconds > maxAge) return -1; // stale
                float t = v.ReadSingle(4);
                return t > 0 && t < 150 ? (int)Math.Round(t) : -1;
            }
            catch (Exception) { return -1; }
        }

        public static bool HelperRunning { get { return CpuTempC() >= 0; } }

        // Time of the helper's last update (UTC ticks), 0 when unknown.
        public static long HelperUpdatedTicks()
        {
            try
            {
                var v = View();
                return v != null && v.ReadUInt32(0) == Magic ? v.ReadInt64(16) : 0;
            }
            catch (Exception) { return 0; }
        }

        // Ask the helper to poll every `ms` (e.g. slower while a game runs). The map grants Everyone read/write.
        public static void RequestInterval(int ms)
        {
            foreach (var n in new[] { MapName, MapNameLocal })
                try
                {
                    using (var m = MemoryMappedFile.OpenExisting(n, MemoryMappedFileRights.ReadWrite))
                    using (var a = m.CreateViewAccessor(0, Size, MemoryMappedFileAccess.ReadWrite))
                        a.Write(IntervalOffset, ms);
                    return;
                }
                catch (Exception) { }
        }

        // ---- helper (elevated): `ttlcd hwmon` ----
        const string Dll = "XYDataCenter.dll";
        static readonly string[] DllDirs =
        {
            @"C:\Program Files\T'OWEI\plugin\x64",
            @"C:\Program Files (x86)\T'OWEI\plugin\x64",
        };

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode)] static extern bool SetDllDirectory(string path);
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode)] static extern IntPtr LoadLibrary(string path);
        [DllImport("kernel32.dll")] static extern IntPtr GetProcAddress(IntPtr h, string name);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate int InitDel();
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate int ReadyDel();
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate int GetDel(IntPtr buf);

        public static int RunHelper() { return RunHelper(() => false); }

        // Runs until stop() returns true (the service stops it; the console helper runs forever).
        public static int RunHelper(Func<bool> stop)
        {
            string dir = null;
            foreach (var d in DllDirs) if (File.Exists(Path.Combine(d, Dll))) { dir = d; break; }
            if (dir == null) { Console.Error.WriteLine("XYDataCenter.dll not found (TOWEI not installed)"); return 1; }
            SetDllDirectory(dir);
            var h = LoadLibrary(Path.Combine(dir, Dll));
            if (h == IntPtr.Zero) { Console.Error.WriteLine("failed to load " + Dll); return 1; }
            var init = Marshal.GetDelegateForFunctionPointer<InitDel>(GetProcAddress(h, "MXNSDK_Init"));
            var ready = Marshal.GetDelegateForFunctionPointer<ReadyDel>(GetProcAddress(h, "MXNSDK_IsReady"));
            var get = Marshal.GetDelegateForFunctionPointer<GetDel>(GetProcAddress(h, "MXNSDK_GetHWInfo"));
            init();
            for (int i = 0; i < 60 && ready() == 0; i++) Thread.Sleep(500);

            var sec = new MemoryMappedFileSecurity();
            sec.AddAccessRule(new System.Security.AccessControl.AccessRule<MemoryMappedFileRights>(
                new System.Security.Principal.SecurityIdentifier(System.Security.Principal.WellKnownSidType.WorldSid, null),
                MemoryMappedFileRights.ReadWrite, System.Security.AccessControl.AccessControlType.Allow));
            MemoryMappedFile map;
            try { map = MemoryMappedFile.CreateOrOpen(MapName, Size, MemoryMappedFileAccess.ReadWrite, MemoryMappedFileOptions.None, sec, HandleInheritability.None); }
            catch (Exception) { map = MemoryMappedFile.CreateOrOpen(MapNameLocal, Size); }
            var acc = map.CreateViewAccessor(0, Size);
            acc.Write(0, Magic);
            var buf = Marshal.AllocHGlobal(65536);
            Console.WriteLine("ttlcd hwmon: publishing CPU temperature to " + MapName);
            while (!stop())
            {
                try
                {
                    if (get(buf) != 0)
                    {
                        float cpu = BitConverter.ToSingle(BitConverter.GetBytes(Marshal.ReadInt32(buf, 0x188)), 0); // cpu_temperature
                        acc.Write(4, cpu);
                        acc.Write(16, DateTime.UtcNow.Ticks);
                    }
                }
                catch (Exception) { }
                int iv = acc.ReadInt32(IntervalOffset); // Studio may ask for slower polling while a game runs
                int wait = iv >= 500 && iv <= 120000 ? iv : 1500;
                for (int t = 0; t < wait && !stop(); t += 250) Thread.Sleep(Math.Min(250, wait - t));
            }
            return 0;
        }
    }

    // "TT LCD sensors" Windows service: runs the CPU-temperature helper at boot (as LocalSystem, so no UAC prompt and
    // no "Enable CPU temperature" click after every restart). Installed once with `ttlcd service install` (admin).
    public class SensorService : System.ServiceProcess.ServiceBase
    {
        public const string Name = "TTLcdSensors";
        volatile bool stopping;
        Thread worker;

        public SensorService() { ServiceName = Name; CanStop = true; }

        protected override void OnStart(string[] args)
        {
            stopping = false;
            worker = new Thread(() => { try { Sensors.RunHelper(() => stopping); } catch (Exception) { } }) { IsBackground = true, Name = "sensors" };
            worker.Start();
        }

        protected override void OnStop() { stopping = true; if (worker != null) worker.Join(3000); }

        public static bool Installed
        {
            get { try { using (var sc = new System.ServiceProcess.ServiceController(Name)) { var s = sc.Status; return true; } } catch (Exception) { return false; } }
        }

        public static bool Running
        {
            get { try { using (var sc = new System.ServiceProcess.ServiceController(Name)) return sc.Status == System.ServiceProcess.ServiceControllerStatus.Running; } catch (Exception) { return false; } }
        }

        // ttlcd service install|uninstall|status (install/uninstall elevate themselves)
        public static int Run(string action)
        {
            switch ((action ?? "").ToLowerInvariant())
            {
                case "status": Console.WriteLine("service installed: {0}, running: {1}", Installed, Running); return 0;
                case "install": return Elevated("install") ? Install() : 0;
                case "uninstall": return Elevated("uninstall") ? Uninstall() : 0;
                default: Console.WriteLine("ttlcd service install|uninstall|status"); return 1;
            }
        }

        static bool Elevated(string action)
        {
            if (new System.Security.Principal.WindowsPrincipal(System.Security.Principal.WindowsIdentity.GetCurrent())
                    .IsInRole(System.Security.Principal.WindowsBuiltInRole.Administrator)) return true;
            string log = Path.Combine(Path.GetTempPath(), "ttlcd-service.log");
            var psi = new System.Diagnostics.ProcessStartInfo("cmd.exe", "/c \"\"" + System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName + "\" service " + action + " > \"" + log + "\" 2>&1\"")
            { Verb = "runas", UseShellExecute = true, WindowStyle = System.Diagnostics.ProcessWindowStyle.Hidden };
            try { System.Diagnostics.Process.Start(psi).WaitForExit(); }
            catch (System.ComponentModel.Win32Exception) { Console.WriteLine("error: administrator rights were refused"); return false; }
            if (File.Exists(log)) Console.Write(File.ReadAllText(log));
            return false;
        }

        static int Sc(string args)
        {
            var psi = new System.Diagnostics.ProcessStartInfo("sc.exe", args) { UseShellExecute = false, CreateNoWindow = true, RedirectStandardOutput = true };
            using (var p = System.Diagnostics.Process.Start(psi)) { Console.Write(p.StandardOutput.ReadToEnd()); p.WaitForExit(); return p.ExitCode; }
        }

        static int Install()
        {
            string exe = System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName;
            if (Installed) Uninstall();
            // a console helper started earlier would publish twice; stop it (other ttlcd.exe processes)
            int self = System.Diagnostics.Process.GetCurrentProcess().Id;
            foreach (var p in System.Diagnostics.Process.GetProcessesByName("ttlcd"))
                using (p) if (p.Id != self) try { p.Kill(); } catch (Exception) { }
            int rc = Sc("create " + Name + " binPath= \"\\\"" + exe + "\\\" sensor-service\" start= auto DisplayName= \"TT LCD sensors\"");
            if (rc != 0) { Console.WriteLine("error: could not create the service (" + rc + ")"); return 2; }
            Sc("description " + Name + " \"Publishes the CPU temperature for TT LCD Studio (uses TOWEI's sensor driver).\"");
            Sc("failure " + Name + " reset= 86400 actions= restart/5000");
            Sc("start " + Name);
            Console.WriteLine("installed and started the TT LCD sensors service");
            return 0;
        }

        static int Uninstall()
        {
            Sc("stop " + Name);
            for (int i = 0; i < 20 && Running; i++) Thread.Sleep(250);
            int rc = Sc("delete " + Name);
            Console.WriteLine(rc == 0 ? "removed the TT LCD sensors service" : "error: could not remove the service (" + rc + ")");
            return rc == 0 ? 0 : 2;
        }
    }
}
