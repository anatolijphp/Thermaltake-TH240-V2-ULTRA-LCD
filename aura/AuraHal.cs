// TTLcdAura.dll - ASUS Aura Sync (Armoury Crate) HAL plugin for the Thermaltake CL-W383 LCD.
//
// ASUS LightingService (32-bit, SYSTEM) creates this COM object from the Aura HAL category
// CLSID\{109DC3E4-...}\Instance\{E9BBD754-...}\Instance\{our CLSID}. It enumerates one "device" whose
// LEDs are virtual: every effect/colour Aura sends is written to shared memory, and TT LCD Studio draws it.
// Interface layout follows the public AacHalSample (IAacLedDeviceHal / IAacLedDevice / IAacLedDeviceOpt).
using System;
using System.Runtime.InteropServices;
using System.Text;

[assembly: ComVisible(false)]

namespace TtLcd.Aura
{
    [ComVisible(true), Guid("F2C8D5B4-3854-4325-8A4F-FD7C5072E3B9"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IAacLedDeviceHal
    {
        [PreserveSig] int Enumerate(IntPtr devices, IntPtr count); // IAacLedDevice** , ULONG*
    }

    [ComVisible(true), Guid("61711778-AB59-4026-89E8-7A63422C29C2"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IAacLedDevice
    {
        [PreserveSig] int GetCapability(IntPtr capability); // BSTR*
        [PreserveSig] int SetEffect(uint effectId, IntPtr colors, uint numberOfColors);
        [PreserveSig] int Synchronize(uint effectId, ulong milliseconds);
    }

    // IAacLedDeviceOpt(Speed): same vtable as IAacLedDevice plus SetEffectOptSpeed.
    [ComVisible(true), Guid("68F0C6E1-7469-40B3-84D5-E0793F449E4D"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IAacLedDeviceOpt
    {
        [PreserveSig] int GetCapability(IntPtr capability);
        [PreserveSig] int SetEffect(uint effectId, IntPtr colors, uint numberOfColors);
        [PreserveSig] int Synchronize(uint effectId, ulong milliseconds);
        [PreserveSig] int SetEffectOptSpeed(uint effectId, IntPtr colors, uint numberOfColors, uint speed, uint direction);
    }

    [ComVisible(true), Guid(TtLcdAuraHal.ClsidString), ClassInterface(ClassInterfaceType.None), ProgId("TTLcd.AuraHal.1")]
    public class TtLcdAuraHal : IAacLedDeviceHal
    {
        public const string ClsidString = "C0C96C60-30E3-4021-9904-966EC4FA0F0F";
        static readonly TtLcdAuraDevice device = new TtLcdAuraDevice();

        public int Enumerate(IntPtr devices, IntPtr count)
        {
            try
            {
                if (count == IntPtr.Zero) return E_POINTER;
                uint capacity = (uint)Marshal.ReadInt32(count);
                Marshal.WriteInt32(count, 1);
                // same contract as ASUS samples: devices==NULL -> just report the count
                if (devices != IntPtr.Zero && (capacity == 0 || capacity >= 1))
                    Marshal.WriteIntPtr(devices, Marshal.GetComInterfaceForObject(device, typeof(IAacLedDevice))); // AddRef'd for caller
                Shared.Log("Enumerate cap=" + capacity + " devices=" + (devices != IntPtr.Zero));
                return 0;
            }
            catch (Exception e) { Shared.Log("Enumerate: " + e); return E_FAIL; }
        }

        internal const int E_FAIL = unchecked((int)0x80004005), E_POINTER = unchecked((int)0x80004003);
    }

    [ComVisible(true), ClassInterface(ClassInterfaceType.None)]
    public class TtLcdAuraDevice : IAacLedDevice, IAacLedDeviceOpt
    {
        public const int LedCount = 8;

        // id, name, customized colour, synchronizable, speed levels (0 = none)
        static readonly object[][] Effects =
        {
            new object[] { 0, "Manual", 1, 0, 0 },
            new object[] { 1, "Static", 1, 0, 0 },
            new object[] { 2, "Breathing", 1, 1, 3 },
            new object[] { 3, "Strobing", 1, 1, 3 },
            new object[] { 4, "Color cycle", 0, 1, 3 },
            new object[] { 5, "Rainbow", 0, 1, 3 },
            new object[] { 0xFF, "Default", 0, 0, 0 },
        };

        public int GetCapability(IntPtr capability)
        {
            try
            {
                if (capability == IntPtr.Zero) return TtLcdAuraHal.E_POINTER;
                var x = new StringBuilder();
                x.Append("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n<root>\n");
                x.Append("    <version>1</version>\n");
                // EXTERNAL_GENERAL_RGB_LIGHTING: generic third-party device. (WATER_COOLER 0xD1000 is accepted and
                // synced by LightingService but Armoury Crate only shows known ASUS AIO models for it.)
                x.Append("    <type>").Append(0x64000).Append("</type>\n");
                x.Append("    <device>\n        <name>TT LCD CL-W383</name>\n        <id>0</id>\n        <layout>\n");
                x.Append("            <led_count>").Append(LedCount).Append("</led_count>\n");
                x.Append("            <size>\n                <width>").Append(LedCount).Append("</width>\n                <height>1</height>\n            </size>\n");
                x.Append("            <led_name>\n");
                for (int i = 0; i < LedCount; i++) x.Append("                <led>LCD ").Append(i + 1).Append("</led>\n");
                x.Append("            </led_name>\n        </layout>\n        <supported_effect>\n");
                foreach (var e in Effects)
                {
                    int speed = (int)e[4];
                    x.Append("            <effect>\n");
                    x.Append("                <name>").Append((string)e[1]).Append("</name>\n");
                    x.Append("                <id>").Append((int)e[0]).Append("</id>\n");
                    x.Append("                <synchronizable>").Append((int)e[3]).Append("</synchronizable>\n");
                    x.Append("                <customized_color>").Append((int)e[2]).Append("</customized_color>\n");
                    if (speed > 0)
                        x.Append("                <speed_supported>1<levels>").Append(speed).Append("</levels><default>1</default></speed_supported>\n");
                    else
                        x.Append("                <speed_supported>0</speed_supported>\n");
                    x.Append("                <direction_supported>0</direction_supported>\n");
                    x.Append("            </effect>\n");
                }
                x.Append("        </supported_effect>\n        <model>Thermaltake CL-W383 LCD</model>\n    </device>\n</root>\n");
                Marshal.WriteIntPtr(capability, Marshal.StringToBSTR(x.ToString()));
                Shared.Log("GetCapability");
                return 0;
            }
            catch (Exception e) { Shared.Log("GetCapability: " + e); return TtLcdAuraHal.E_FAIL; }
        }

        public int SetEffect(uint effectId, IntPtr colors, uint numberOfColors)
        {
            return SetEffectOptSpeed(effectId, colors, numberOfColors, 1, 0);
        }

        public int SetEffectOptSpeed(uint effectId, IntPtr colors, uint numberOfColors, uint speed, uint direction)
        {
            try
            {
                var c = new uint[Shared.MaxLeds];
                int n = (int)Math.Min(numberOfColors, (uint)Shared.MaxLeds);
                for (int i = 0; i < n && colors != IntPtr.Zero; i++) c[i] = (uint)Marshal.ReadInt32(colors, i * 4);
                Shared.Write(effectId, c, n, speed, direction, -1);
                return 0;
            }
            catch (Exception e) { Shared.Log("SetEffect: " + e); return TtLcdAuraHal.E_FAIL; }
        }

        public int Synchronize(uint effectId, ulong milliseconds)
        {
            try { Shared.WriteSync(effectId, (long)milliseconds); return 0; }
            catch (Exception e) { Shared.Log("Synchronize: " + e); return TtLcdAuraHal.E_FAIL; }
        }
    }

    // Shared memory "Global\TTLcdAuraSync" (created here, inside the SYSTEM service, readable by everyone).
    // Layout (little-endian): 0 magic 'TTAS' | 4 seq | 8 effectId | 12 ledCount | 16 speed | 20 direction |
    //                         24 syncMs (int64) | 32 updatedUtcTicks (int64) | 40 colors[64] (0x00BBGGRR)
    static class Shared
    {
        public const int MaxLeds = 64, Size = 40 + MaxLeds * 4;
        const string Name = @"Global\TTLcdAuraSync";
        static IntPtr handle, view;
        static readonly object gate = new object();
        static DateTime nextTry;

        // Raw CreateFileMapping: it creates the section or opens the existing one (e.g. kept alive by TT LCD Studio
        // across a LightingService restart) in one call. The .NET MemoryMappedFile wrappers failed to reattach there,
        // and CreateOrOpen loops forever when Global\ creation is denied.
        static IntPtr View()
        {
            if (view != IntPtr.Zero) return view;
            if (DateTime.UtcNow < nextTry) throw new InvalidOperationException("shared memory unavailable");
            nextTry = DateTime.UtcNow.AddSeconds(2);
            // Everyone: read/write; SYSTEM and Administrators: full.
            handle = Map(Name, "D:(A;;GRGW;;;WD)(A;;GA;;;SY)(A;;GA;;;BA)");
            if (handle == IntPtr.Zero) handle = Map(@"Local\TTLcdAuraSync", null); // tests from a normal user
            if (handle == IntPtr.Zero) throw new InvalidOperationException("cannot create shared memory");
            view = MapViewOfFile(handle, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, (UIntPtr)Size);
            if (view == IntPtr.Zero)
            {
                int err = Marshal.GetLastWin32Error();
                CloseHandle(handle); handle = IntPtr.Zero;
                throw new InvalidOperationException("MapViewOfFile failed, error " + err);
            }
            Marshal.WriteInt32(view, 0, 0x53415454); // "TTAS"
            Log("shared memory attached");
            return view;
        }

        static IntPtr Map(string name, string sddl)
        {
            IntPtr sd = IntPtr.Zero;
            try
            {
                var sa = new SECURITY_ATTRIBUTES { nLength = Marshal.SizeOf(typeof(SECURITY_ATTRIBUTES)) };
                if (sddl != null && ConvertStringSecurityDescriptorToSecurityDescriptorW(sddl, 1, out sd, IntPtr.Zero))
                    sa.lpSecurityDescriptor = sd;
                IntPtr h = CreateFileMappingW(new IntPtr(-1), ref sa, PAGE_READWRITE, 0, Size, name);
                if (h != IntPtr.Zero) return h;
                // An existing section that only grants read/write (e.g. created by an older build) refuses the
                // full access CreateFileMapping asks for: open it with just what we need.
                int err = Marshal.GetLastWin32Error();
                h = OpenFileMappingW(FILE_MAP_WRITE | FILE_MAP_READ, false, name);
                if (h == IntPtr.Zero) Log("open " + name + " failed, errors " + err + "/" + Marshal.GetLastWin32Error());
                return h;
            }
            finally { if (sd != IntPtr.Zero) LocalFree(sd); }
        }

        public static void Write(uint effect, uint[] colors, int n, uint speed, uint direction, long syncMs)
        {
            lock (gate)
            {
                var v = View();
                Marshal.WriteInt32(v, 8, (int)effect);
                Marshal.WriteInt32(v, 12, n);
                Marshal.WriteInt32(v, 16, (int)speed);
                Marshal.WriteInt32(v, 20, (int)direction);
                if (syncMs >= 0) Marshal.WriteInt64(v, 24, syncMs);
                Marshal.WriteInt64(v, 32, DateTime.UtcNow.Ticks);
                for (int i = 0; i < MaxLeds; i++) Marshal.WriteInt32(v, 40 + i * 4, (int)colors[i]);
                Bump(v); // written last: readers use it to detect a complete update
            }
        }

        public static void WriteSync(uint effect, long syncMs)
        {
            lock (gate)
            {
                var v = View();
                Marshal.WriteInt32(v, 8, (int)effect);
                Marshal.WriteInt64(v, 24, syncMs);
                Marshal.WriteInt64(v, 32, DateTime.UtcNow.Ticks);
                Bump(v);
            }
        }

        // Continue the existing sequence so a reattached section keeps counting forward.
        static void Bump(IntPtr v) { Marshal.WriteInt32(v, 4, Marshal.ReadInt32(v, 4) + 1); }

        const uint PAGE_READWRITE = 0x04, FILE_MAP_WRITE = 0x02, FILE_MAP_READ = 0x04;

        [StructLayout(LayoutKind.Sequential)]
        struct SECURITY_ATTRIBUTES { public int nLength; public IntPtr lpSecurityDescriptor; public int bInheritHandle; }

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        static extern IntPtr CreateFileMappingW(IntPtr hFile, ref SECURITY_ATTRIBUTES sa, uint protect, uint maxHigh, uint maxLow, string name);
        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        static extern IntPtr OpenFileMappingW(uint access, bool inherit, string name);
        [DllImport("kernel32.dll", SetLastError = true)]
        static extern IntPtr MapViewOfFile(IntPtr h, uint access, uint offHigh, uint offLow, UIntPtr bytes);
        [DllImport("kernel32.dll")] static extern bool CloseHandle(IntPtr h);
        [DllImport("kernel32.dll")] static extern IntPtr LocalFree(IntPtr p);
        [DllImport("advapi32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        static extern bool ConvertStringSecurityDescriptorToSecurityDescriptorW(string sddl, uint revision, out IntPtr sd, IntPtr size);

        // Optional diagnostics: create C:\ProgramData\ttlcd\aura\debug to enable.
        public static void Log(string s)
        {
            try
            {
                string dir = System.IO.Path.GetDirectoryName(typeof(Shared).Assembly.Location);
                if (!System.IO.Directory.Exists(System.IO.Path.Combine(dir, "debug"))) return;
                System.IO.File.AppendAllText(System.IO.Path.Combine(dir, "debug", "aura.log"), DateTime.Now.ToString("HH:mm:ss.fff ") + s + Environment.NewLine);
            }
            catch (Exception) { }
        }
    }
}
