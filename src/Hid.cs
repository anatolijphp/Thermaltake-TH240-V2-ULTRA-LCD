// Minimal Win32 HID access (SetupAPI + hid.dll), no external dependencies.
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;

namespace TtLcd
{
    public class HidDeviceInfo
    {
        public string Path;
        public ushort Vid, Pid, Version;
        public ushort UsagePage, Usage;
        public int InputLen, OutputLen, FeatureLen;
        public int Interface = -1;
        public override string ToString()
        {
            return string.Format("{0:X4}:{1:X4} MI_{2:D2} usage={3:X4}/{4:X4} in={5} out={6} feat={7}\n  {8}",
                Vid, Pid, Interface, UsagePage, Usage, InputLen, OutputLen, FeatureLen, Path);
        }
    }

    public static class Hid
    {
        const int DIGCF_PRESENT = 0x2, DIGCF_DEVICEINTERFACE = 0x10;
        const uint GENERIC_READ = 0x80000000, GENERIC_WRITE = 0x40000000;
        const uint FILE_SHARE_READ = 1, FILE_SHARE_WRITE = 2, OPEN_EXISTING = 3, FILE_FLAG_OVERLAPPED = 0x40000000;

        [StructLayout(LayoutKind.Sequential)]
        struct SP_DEVICE_INTERFACE_DATA { public int cbSize; public Guid InterfaceClassGuid; public int Flags; public IntPtr Reserved; }

        [StructLayout(LayoutKind.Sequential)]
        struct HIDD_ATTRIBUTES { public int Size; public ushort VendorID, ProductID, VersionNumber; }

        [StructLayout(LayoutKind.Sequential)]
        struct HIDP_CAPS
        {
            public ushort Usage, UsagePage, InputReportByteLength, OutputReportByteLength, FeatureReportByteLength;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 17)] public ushort[] Reserved;
            public ushort NumberLinkCollectionNodes, NumberInputButtonCaps, NumberInputValueCaps, NumberInputDataIndices,
                NumberOutputButtonCaps, NumberOutputValueCaps, NumberOutputDataIndices,
                NumberFeatureButtonCaps, NumberFeatureValueCaps, NumberFeatureDataIndices;
        }

        [DllImport("hid.dll")] static extern void HidD_GetHidGuid(out Guid g);
        [DllImport("hid.dll")] static extern bool HidD_GetAttributes(SafeFileHandle h, ref HIDD_ATTRIBUTES a);
        [DllImport("hid.dll")] static extern bool HidD_GetPreparsedData(SafeFileHandle h, out IntPtr p);
        [DllImport("hid.dll")] static extern bool HidD_FreePreparsedData(IntPtr p);
        [DllImport("hid.dll")] static extern int HidP_GetCaps(IntPtr p, ref HIDP_CAPS c);
        [DllImport("hid.dll")] public static extern bool HidD_SetOutputReport(SafeFileHandle h, byte[] buf, int len);
        [DllImport("hid.dll")] public static extern bool HidD_SetFeature(SafeFileHandle h, byte[] buf, int len);
        [DllImport("hid.dll")] public static extern bool HidD_GetFeature(SafeFileHandle h, byte[] buf, int len);

        [DllImport("setupapi.dll", SetLastError = true)]
        static extern IntPtr SetupDiGetClassDevs(ref Guid g, IntPtr enumerator, IntPtr hwnd, int flags);
        [DllImport("setupapi.dll", SetLastError = true)]
        static extern bool SetupDiEnumDeviceInterfaces(IntPtr set, IntPtr devInfo, ref Guid g, int idx, ref SP_DEVICE_INTERFACE_DATA d);
        [DllImport("setupapi.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        static extern bool SetupDiGetDeviceInterfaceDetail(IntPtr set, ref SP_DEVICE_INTERFACE_DATA d, IntPtr detail, int size, out int req, IntPtr devInfo);
        [DllImport("setupapi.dll")] static extern bool SetupDiDestroyDeviceInfoList(IntPtr set);

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        static extern SafeFileHandle CreateFile(string name, uint access, uint share, IntPtr sa, uint disp, uint flags, IntPtr tmpl);

        // queryCaps=false skips opening each interface for report sizes: on this cooler that query blocks ~5 s on MI_00.
        public static List<HidDeviceInfo> Enumerate(ushort vid, ushort pid, bool queryCaps = true)
        {
            var list = new List<HidDeviceInfo>();
            Guid g; HidD_GetHidGuid(out g);
            IntPtr set = SetupDiGetClassDevs(ref g, IntPtr.Zero, IntPtr.Zero, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
            try
            {
                var d = new SP_DEVICE_INTERFACE_DATA(); d.cbSize = Marshal.SizeOf(d);
                for (int i = 0; SetupDiEnumDeviceInterfaces(set, IntPtr.Zero, ref g, i, ref d); i++)
                {
                    int req;
                    SetupDiGetDeviceInterfaceDetail(set, ref d, IntPtr.Zero, 0, out req, IntPtr.Zero);
                    IntPtr buf = Marshal.AllocHGlobal(req);
                    try
                    {
                        Marshal.WriteInt32(buf, 8); // cbSize of SP_DEVICE_INTERFACE_DETAIL_DATA_W on x64
                        if (!SetupDiGetDeviceInterfaceDetail(set, ref d, buf, req, out req, IntPtr.Zero)) continue;
                        string path = Marshal.PtrToStringUni(new IntPtr(buf.ToInt64() + 4));
                        string lp = path.ToLowerInvariant();
                        if (!lp.Contains(string.Format("vid_{0:x4}&pid_{1:x4}", vid, pid))) continue;
                        var info = queryCaps ? Query(path) : new HidDeviceInfo { Path = path, Vid = vid, Pid = pid, Interface = InterfaceOf(path) };
                        if (info != null) list.Add(info);
                    }
                    finally { Marshal.FreeHGlobal(buf); }
                }
            }
            finally { SetupDiDestroyDeviceInfoList(set); }
            return list;
        }

        static HidDeviceInfo Query(string path)
        {
            // access 0 = query only, works even when another app holds the device
            using (var h = CreateFile(path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, IntPtr.Zero, OPEN_EXISTING, 0, IntPtr.Zero))
            {
                if (h.IsInvalid) return null;
                var info = new HidDeviceInfo { Path = path };
                var a = new HIDD_ATTRIBUTES(); a.Size = Marshal.SizeOf(a);
                if (HidD_GetAttributes(h, ref a)) { info.Vid = a.VendorID; info.Pid = a.ProductID; info.Version = a.VersionNumber; }
                IntPtr p;
                if (HidD_GetPreparsedData(h, out p))
                {
                    var c = new HIDP_CAPS();
                    HidP_GetCaps(p, ref c);
                    HidD_FreePreparsedData(p);
                    info.UsagePage = c.UsagePage; info.Usage = c.Usage;
                    info.InputLen = c.InputReportByteLength; info.OutputLen = c.OutputReportByteLength; info.FeatureLen = c.FeatureReportByteLength;
                }
                info.Interface = InterfaceOf(path);
                return info;
            }
        }

        static int InterfaceOf(string path)
        {
            int mi = path.ToLowerInvariant().IndexOf("&mi_");
            return mi >= 0 ? Convert.ToInt32(path.Substring(mi + 4, 2), 16) : -1;
        }

        public static FileStream Open(string path)
        {
            var h = CreateFile(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, IntPtr.Zero, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, IntPtr.Zero);
            if (h.IsInvalid) throw new IOException("Cannot open HID device (in use by TT RGB Plus?): error " + Marshal.GetLastWin32Error());
            return new FileStream(h, FileAccess.ReadWrite, 1, true);
        }
    }
}
