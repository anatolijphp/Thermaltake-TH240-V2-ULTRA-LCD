// ttrescue - flash read / write+verify for a cooler LCD in Nuvoton W55FA93 ROM recovery mode (USB 0416:9396).
// See firmware/README.md for the burn procedure.
// Mirrors TurboWriter V2.30.003_N329x1 (OpenNuvoton/N32905_NonOS_Tool):
//   recovery ROM (IBR):  06 51 connect, 06 52/53 DRAM clock skew, 06 55 load address, 06 72 data, 06 60 run
//   writer (FA93_musb):  61 command {action, ...} with CDB[6] = media (6 = SPI), 64 data-in
// Only RAM is written (the helper program); flash is only read (action 3, the same request TurboWriter's
// Verify uses). Built x86 so SCSI_PASS_THROUGH_DIRECT has TurboWriter's exact 32-bit layout.
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using Microsoft.Win32.SafeHandles;

static class Rescue
{
    static StreamWriter log;
    static void Log(string s) { Console.WriteLine(s); if (log != null) { log.WriteLine(DateTime.Now.ToString("HH:mm:ss ") + s); log.Flush(); } }

    static int Main(string[] args)
    {
        string dir = AppDomain.CurrentDomain.BaseDirectory;
        // ttrescue [dump.bin [blocks]]      read flash
        // ttrescue write <image.bin>        raw-write image at flash offset 0, then read back and compare
        bool write = args.Length > 1 && args[0] == "write";
        string image = write ? args[1] : null;
        string outFile = write ? Path.Combine(dir, "verify_dump.bin") : args.Length > 0 ? args[0] : Path.Combine(dir, "flash_dump.bin");
        int blocks = !write && args.Length > 1 ? int.Parse(args[1]) : 256; // 64 KB blocks; 256 = 16 MB
        // Nuvoton's flash-writer helper: next to the exe, in firmware\ (the repo copy), or in a local TurboWriter
        string helper = new[] { Path.Combine(dir, "FA93_musb.bin"), Path.Combine(dir, @"..\firmware\FA93_musb.bin"),
                                Path.Combine(dir, @"firmware\FA93_musb.bin"), Path.Combine(dir, @"..\tools\turbowriter\FA93_musb.bin") }
                        .FirstOrDefault(File.Exists) ?? Path.Combine(dir, @"..\firmware\FA93_musb.bin");
        log = new StreamWriter(Path.Combine(dir, "rescue.log"), true);
        try
        {
            Log("=== read " + blocks + " x 64 KB -> " + outFile);
            string path = FindDisk("ven_w55fa93&prod_dev_tool", 1);
            if (path != null)
            {
                Log("recovery loader: " + path);
                using (var h = Open(path)) LoadHelper(h, File.ReadAllBytes(helper));
                Log("helper started, waiting for writer device...");
                Thread.Sleep(2000);
            }
            else Log("no recovery loader found, looking for an already running writer");
            string w = null;
            for (int i = 0; i < 100 && w == null; i++) { w = FindWriter(); if (w == null) Thread.Sleep(300); }
            if (w == null) { ListDisks(); throw new IOException("writer device did not appear"); }
            Log("writer: " + w);
            if (write)
            {
                var img = File.ReadAllBytes(image);
                Log("image " + image + ": " + img.Length + " bytes");
                using (var h = Open(w)) WriteSpi(h, img);
                int vb = (img.Length + 0xFFFF) / 0x10000;
                using (var h = Open(w)) ReadSpi(h, outFile, vb);
                var back = File.ReadAllBytes(outFile);
                for (int i = 0; i < img.Length; i++)
                    if (back[i] != img[i]) throw new IOException(string.Format("verify failed at 0x{0:X}: wrote {1:X2}, read {2:X2}", i, img[i], back[i]));
                Log("verify OK: " + img.Length + " bytes match");
                return 0;
            }
            using (var h = Open(w)) ReadSpi(h, outFile, blocks);
            Log("done");
            return 0;
        }
        catch (Exception e) { Log("ERROR: " + e.Message); return 1; }
        finally { log.Dispose(); }
    }

    // ---------- stage 1: recovery ROM ----------
    static void LoadHelper(SafeFileHandle h, byte[] bin)
    {
        if (Encoding.ASCII.GetString(bin, 0, 4) != "NVT ") throw new IOException("bad helper header");
        uint addr = BitConverter.ToUInt32(bin, 4), len = BitConverter.ToUInt32(bin, 8);
        Log(string.Format("helper: load 0x{0:X} len 0x{1:X}", addr, len));
        var info = new byte[0xDFF];
        bool ok = false;
        for (int i = 0; i < 10 && !ok; i++) { ok = Pt(h, Cdb(0x06, 0x51, 1), info, true, 20); if (!ok) Thread.Sleep(100); }
        if (!ok) throw new IOException("IBR connect failed (" + Marshal.GetLastWin32Error() + ")");
        Log("IBR connect ok: " + BitConverter.ToString(info, 0, 32));
        Must(Pt(h, CdbU32(0x51 + 1, 0x00001010), new byte[0xDFF], true, 20), "DQSODS");   // 06 52
        Must(Pt(h, CdbU32(0x51 + 2, 0x00888800), new byte[0xDFF], true, 20), "CKDQSDS");  // 06 53
        Must(Pt(h, CdbU32(0x55, addr), new byte[0xDFF], true, 20), "set address");
        for (uint off = 16; off < 16 + len; )
        {
            int n = (int)Math.Min(0x10000, 16 + len - off);
            var chunk = new byte[n];
            Buffer.BlockCopy(bin, (int)off, chunk, 0, n);
            var c = Cdb(0x06, 0x72, (byte)n); c[3] = (byte)(n >> 8); c[4] = (byte)(n >> 16);
            Must(Pt(h, c, chunk, false, 0x32), "write RAM @" + off);
            off += (uint)n;
        }
        Log("helper uploaded");
        Pt(h, Cdb(0x06, 0x60, 0), new byte[0], true, 5); // run; the device drops off USB, so no status
    }

    // ---------- stage 2: writer ----------
    // TurboWriter (FUN_0041bd80 / FUN_0041e4c0): 65/7 = flash size in KB; when 0, 65/8 = JEDEC ID,
    // 66/6 = tell the helper the size (from SPIFLASH ID.ini), then 65/7 again. Without it reads return FF.
    static int InitSpi(SafeFileHandle h)
    {
        int kb = Query(h, 7);
        Log("SPI size reported: " + kb + " KB");
        if (kb > 0) return kb;
        int id = Query(h, 8) & 0xFFFFFF;
        Log(string.Format("SPI JEDEC ID: {0:X6}", id));
        var known = new Dictionary<int, int> { { 0xC84017, 8192 }, { 0xC22014, 1024 }, { 0xEF3013, 8192 }, { 0xC84018, 16384 }, { 0xC84016, 4096 }, { 0xA14018, 16384 } };
        int cap = id & 0xFF; // ID as in SPIFLASH ID.ini: manufacturer, type, capacity (2^n bytes)
        if (!known.TryGetValue(id, out kb)) kb = cap >= 0x10 && cap <= 0x19 ? (1 << cap) / 1024 : 0;
        if (kb == 0) throw new IOException("unknown flash size for ID " + id.ToString("X6"));
        Log("setting SPI size " + kb + " KB");
        Must(Pt(h, Cdb66(6), BitConverter.GetBytes(kb), false, 100), "set SPI size");
        int now = Query(h, 7);
        Log("SPI size now: " + now + " KB");
        return now > 0 ? now : kb;
    }

    static int Query(SafeFileHandle h, byte what)
    {
        var buf = new byte[what == 6 ? 5 : 4];
        var c = new byte[10]; c[0] = 0x65; c[6] = what;
        Must(Pt(h, c, buf, true, 20), "query " + what);
        return BitConverter.ToInt32(buf, 0);
    }

    static byte[] Cdb66(byte what) { var c = new byte[10]; c[0] = 0x66; c[6] = what; return c; }

    // TurboWriter "SPI (Raw Data)" burn (FUN_00424c90): 61 {action 0, length, 0} media 6, data via 63 in 64 KB
    // pieces, then 65/3 status (first byte 0 = OK). The image goes to flash offset 0 unchanged.
    static void WriteSpi(SafeFileHandle h, byte[] img)
    {
        InitSpi(h);
        var cmd = new byte[12];
        BitConverter.GetBytes(img.Length).CopyTo(cmd, 4);
        var c = new byte[10]; c[0] = 0x61; c[6] = 6;
        Must(Pt(h, c, cmd, false, 100), "write command");
        verbose = false;
        for (int off = 0; off < img.Length; off += 0x10000)
        {
            int n = Math.Min(0x10000, img.Length - off);
            var chunk = new byte[n];
            Buffer.BlockCopy(img, off, chunk, 0, n);
            var d = new byte[10]; d[0] = 0x63;
            Must(Pt(h, d, chunk, false, 20), "write data @" + off);
            Log(string.Format("  wrote {0} / {1} KB", (off + n) / 1024, img.Length / 1024));
        }
        Thread.Sleep(100);
        var st = new byte[4];
        bool ok = false;
        for (int i = 0; i < 10 && !ok; i++) { var q = new byte[10]; q[0] = 0x65; q[6] = 3; ok = Pt(h, q, st, true, 20); }
        Log("write status: " + BitConverter.ToString(st));
        if (!ok || st[0] != 0) throw new IOException("writer reported failure");
        verbose = true;
    }

    static void ReadSpi(SafeFileHandle h, string outFile, int blocks)
    {
        int kb = InitSpi(h);
        blocks = Math.Min(blocks, kb / 64);
        Log("reading " + blocks + " blocks");
        var cmd = new byte[12];
        BitConverter.GetBytes(3).CopyTo(cmd, 0);                 // action 3 = read (as TurboWriter Read/Verify)
        var c = new byte[10]; c[0] = 0x61; c[6] = 6;             // media 6 = SPI
        Must(Pt(h, c, cmd, false, 100), "read command");
        var buf = new byte[0x10000];
        verbose = false;
        using (var f = File.Create(outFile))
            for (int i = 0; i < blocks; i++)
            {
                var r = new byte[10]; r[0] = 0x64;
                Array.Clear(buf, 0, buf.Length);
                Must(Pt(h, r, buf, true, 20), "read block " + i);
                f.Write(buf, 0, buf.Length);
                if (i % 16 == 15 || i == blocks - 1) Log(string.Format("  {0} / {1} KB", (i + 1) * 64, blocks * 64));
            }
    }

    static void Must(bool ok, string what) { if (!ok) throw new IOException(what + " failed (" + Marshal.GetLastWin32Error() + ")"); }

    static byte[] Cdb(byte op, byte sub, byte b2) { var c = new byte[10]; c[0] = op; c[1] = sub; c[2] = b2; return c; }
    static byte[] CdbU32(int sub, uint v) { var c = new byte[10]; c[0] = 0x06; c[1] = (byte)sub; BitConverter.GetBytes(v).CopyTo(c, 2); return c; }

    // ---------- SCSI pass-through (x86 layout, identical to TurboWriter) ----------
    [StructLayout(LayoutKind.Sequential)]
    struct SPTD
    {
        public ushort Length; public byte ScsiStatus, PathId, TargetId, Lun, CdbLength, SenseInfoLength, DataIn;
        public uint DataTransferLength, TimeOutValue; public IntPtr DataBuffer; public uint SenseInfoOffset;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)] public byte[] Cdb;
    }
    [StructLayout(LayoutKind.Sequential)]
    struct SPTDWB { public SPTD s; public uint pad; [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)] public byte[] Sense; }

    const uint IOCTL_SCSI_PASS_THROUGH_DIRECT = 0x4D014;
    static bool verbose = true;

    static bool Pt(SafeFileHandle h, byte[] cdb, byte[] data, bool dataIn, uint timeout)
    {
        var pin = GCHandle.Alloc(data, GCHandleType.Pinned);
        try
        {
            var c = new byte[16]; Buffer.BlockCopy(cdb, 0, c, 0, cdb.Length);
            var w = new SPTDWB { Sense = new byte[32] };
            w.s = new SPTD
            {
                Length = (ushort)Marshal.SizeOf(typeof(SPTD)), TargetId = 1, CdbLength = 10, SenseInfoLength = 0x18,
                DataIn = (byte)(dataIn ? 1 : 0), DataTransferLength = (uint)data.Length, TimeOutValue = timeout,
                DataBuffer = data.Length > 0 ? pin.AddrOfPinnedObject() : IntPtr.Zero,
                SenseInfoOffset = (uint)Marshal.OffsetOf(typeof(SPTDWB), "Sense"), Cdb = c
            };
            int size = Marshal.SizeOf(typeof(SPTDWB));
            IntPtr p = Marshal.AllocHGlobal(size);
            try
            {
                Marshal.StructureToPtr(w, p, false);
                uint ret;
                bool ok = DeviceIoControl(h, IOCTL_SCSI_PASS_THROUGH_DIRECT, p, size, p, size, out ret, IntPtr.Zero);
                var r = (SPTDWB)Marshal.PtrToStructure(p, typeof(SPTDWB));
                // TurboWriter only checks that the request went through: the recovery loader answers every
                // vendor command with CHECK CONDITION / "medium not present" (it has no medium).
                if (ok && r.s.ScsiStatus != 0 && verbose) Log("  (" + BitConverter.ToString(cdb, 0, 2) + ") scsi status " + r.s.ScsiStatus + " sense " + BitConverter.ToString(r.Sense, 0, 18));
                return ok;
            }
            finally { Marshal.FreeHGlobal(p); }
        }
        finally { pin.Free(); }
    }

    // ---------- device discovery (disk interfaces, also works without a drive letter) ----------
    static readonly Guid DiskGuid = new Guid("53f56307-b6bf-11d0-94f2-00a0c91efb8b");

    static List<string> Disks()
    {
        var list = new List<string>();
        IntPtr set = SetupDiGetClassDevs(ref DiskGuid_, IntPtr.Zero, IntPtr.Zero, 0x12); // PRESENT | DEVICEINTERFACE
        try
        {
            var di = new SP_DEVICE_INTERFACE_DATA { cbSize = Marshal.SizeOf(typeof(SP_DEVICE_INTERFACE_DATA)) };
            for (uint i = 0; SetupDiEnumDeviceInterfaces(set, IntPtr.Zero, ref DiskGuid_, i, ref di); i++)
            {
                uint need;
                SetupDiGetDeviceInterfaceDetail(set, ref di, IntPtr.Zero, 0, out need, IntPtr.Zero);
                IntPtr buf = Marshal.AllocHGlobal((int)need);
                try
                {
                    Marshal.WriteInt32(buf, IntPtr.Size == 8 ? 8 : 6);
                    if (SetupDiGetDeviceInterfaceDetail(set, ref di, buf, need, out need, IntPtr.Zero))
                        list.Add(Marshal.PtrToStringAuto(buf + 4));
                }
                finally { Marshal.FreeHGlobal(buf); }
            }
        }
        finally { SetupDiDestroyDeviceInfoList(set); }
        return list;
    }
    static Guid DiskGuid_ = DiskGuid;

    static string FindDisk(string needle, int tries)
    {
        for (int t = 0; t < tries; t++)
        {
            foreach (var p in Disks()) if (p.ToLowerInvariant().Contains(needle)) return p;
            Thread.Sleep(300);
        }
        return null;
    }

    // The writer re-enumerates as a Nuvoton/W55FA93 mass storage that is not the recovery loader.
    static string FindWriter()
    {
        foreach (var p in Disks())
        {
            var l = p.ToLowerInvariant();
            if (l.Contains("ven_w55fa93&prod_dev_tool")) continue;
            if (l.Contains("nuvoton") || l.Contains("w55fa9") || l.Contains("ven_nuvo") || l.Contains("vid_0416")) return p;
        }
        return null;
    }

    static void ListDisks() { foreach (var p in Disks()) Log("  disk: " + p); }

    static SafeFileHandle Open(string path)
    {
        var h = CreateFile(path, 0xC0000000, 3, IntPtr.Zero, 3, 0, IntPtr.Zero);
        if (h.IsInvalid) throw new IOException("open " + path + " failed (" + Marshal.GetLastWin32Error() + ")");
        return h;
    }

    [StructLayout(LayoutKind.Sequential)]
    struct SP_DEVICE_INTERFACE_DATA { public int cbSize; public Guid g; public int flags; public IntPtr reserved; }

    [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
    static extern SafeFileHandle CreateFile(string name, uint access, uint share, IntPtr sa, uint disp, uint flags, IntPtr tmpl);
    [DllImport("kernel32.dll", SetLastError = true)]
    static extern bool DeviceIoControl(SafeFileHandle h, uint code, IntPtr inBuf, int inSize, IntPtr outBuf, int outSize, out uint ret, IntPtr ov);
    [DllImport("setupapi.dll", SetLastError = true)]
    static extern IntPtr SetupDiGetClassDevs(ref Guid g, IntPtr enumerator, IntPtr hwnd, uint flags);
    [DllImport("setupapi.dll", SetLastError = true)]
    static extern bool SetupDiEnumDeviceInterfaces(IntPtr set, IntPtr dev, ref Guid g, uint index, ref SP_DEVICE_INTERFACE_DATA di);
    [DllImport("setupapi.dll", SetLastError = true, CharSet = CharSet.Auto)]
    static extern bool SetupDiGetDeviceInterfaceDetail(IntPtr set, ref SP_DEVICE_INTERFACE_DATA di, IntPtr detail, uint size, out uint need, IntPtr dev);
    [DllImport("setupapi.dll")] static extern bool SetupDiDestroyDeviceInfoList(IntPtr set);
}
