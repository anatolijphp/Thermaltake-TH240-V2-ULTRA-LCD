// Thermaltake CL-W383 (264A:233C) LCD protocol, see docs/protocol.md.
using System;
using System.IO;
using System.Linq;
using System.Text;

namespace TtLcd
{
    public class Lcd : IDisposable
    {
        public const ushort Vid = 0x264A, Pid = 0x233C;
        public const ushort AltVid = 0x1A2C, AltPid = 0x151D;
        public const int Width = 480, Height = 480;
        const int CtrlPacket = 440, CtrlPayload = 436;   // MI_00
        const int LcdPacket = 1024, LcdPayload = 1020;   // MI_01

        FileStream ctrl, lcd;
        System.Threading.Mutex owner;
        public bool SwapRedBlue;  // set for the LX800-firmware board (RGB/BGR order mismatch)

        // Exchange red and blue in a JPEG (decode, swap channels in place, re-encode).
        static unsafe byte[] SwapRB(byte[] jpeg)
        {
            using (var ms = new MemoryStream(jpeg))
            using (var src = new System.Drawing.Bitmap(ms))
            using (var bmp = new System.Drawing.Bitmap(src.Width, src.Height, System.Drawing.Imaging.PixelFormat.Format24bppRgb))
            {
                using (var g = System.Drawing.Graphics.FromImage(bmp)) g.DrawImage(src, 0, 0, src.Width, src.Height);
                var d = bmp.LockBits(new System.Drawing.Rectangle(0, 0, bmp.Width, bmp.Height),
                    System.Drawing.Imaging.ImageLockMode.ReadWrite, System.Drawing.Imaging.PixelFormat.Format24bppRgb);
                try
                {
                    for (int y = 0; y < d.Height; y++)
                    {
                        byte* p = (byte*)d.Scan0 + y * d.Stride;
                        for (int x = 0; x < d.Width; x++, p += 3) { byte t = p[0]; p[0] = p[2]; p[2] = t; }
                    }
                }
                finally { bmp.UnlockBits(d); }
                return Render.Jpeg(bmp, 90);
            }
        }

        // Only one process may talk to the cooler: frames from two writers interleave packet-wise and
        // can lock up the cooler's image decoder until a full power cycle. Open/Dispose on the same thread.
        public static Lcd Open()
        {
            // (Mono keeps named mutexes inside one process, so on Linux this guards only against this process.)
            System.Threading.Mutex m = null;
            try { m = new System.Threading.Mutex(false, Os.Linux ? "TTLcd.Device" : @"Global\TTLcd.Device"); }
            catch (Exception) { if (!Os.Linux) throw; }
            bool mine = true;
            if (m != null)
                try { mine = m.WaitOne(0); }
                catch (System.Threading.AbandonedMutexException) { mine = true; } // previous owner was killed
            if (!mine) { m.Dispose(); throw new IOException("Cooler is in use by another TT LCD program (Studio or ttlcd.exe)."); }
            var r = new Lcd { owner = m };
            try
            {
                var devs = Hid.Enumerate(Vid, Pid, false);
                // same HKC board running the LX800 firmware (restored that way after the flash was erased);
                // its panel setup shows red and blue exchanged, so frames are swapped back before sending
                if (devs.Count == 0) { devs = Hid.Enumerate(AltVid, AltPid, false); r.SwapRedBlue = devs.Count > 0; }
                var c = devs.FirstOrDefault(d => d.Interface == 0);
                var l = devs.FirstOrDefault(d => d.Interface == 1);
                if (c == null || l == null) throw new IOException("Cooler 264A:233C not found.");
                r.ctrl = Hid.Open(c.Path); r.lcd = Hid.Open(l.Path);
            }
            catch { r.Dispose(); throw; }
            return r;
        }

        static void Send(FileStream s, int packet, int payload, byte cmd, byte[] data)
        {
            int len = data == null ? 0 : data.Length;
            int total = Math.Max(1, (len + payload - 1) / payload);
            var buf = new byte[packet + 1]; // [0] = HID report ID 0
            int off = 0;
            for (int seq = 0; seq < total; seq++)
            {
                Array.Clear(buf, 0, buf.Length);
                uint hdr = seq == 0 ? 0x80000000u | ((uint)total << 8) | cmd : ((uint)seq << 8) | cmd;
                buf[1] = (byte)hdr; buf[2] = (byte)(hdr >> 8); buf[3] = (byte)(hdr >> 16); buf[4] = (byte)(hdr >> 24);
                int n = Math.Min(payload, len - off);
                if (n > 0) Buffer.BlockCopy(data, off, buf, 5, n);
                off += Math.Max(n, 0);
                s.Write(buf, 0, buf.Length);
            }
        }

        // Query on the control interface: send empty command, read reply packet whose first byte is cmd.
        byte[] Query(byte cmd, int want) { return Request(cmd, null, want, 1000); }

        // Control command with a reply (TT RGB Plus ProtocolWrite + ProtocolRead): the reply is one packet
        // on MI_00 whose header carries the same command byte.
        byte[] Request(byte cmd, byte[] data, int want, int timeoutMs)
        {
            Send(ctrl, CtrlPacket, CtrlPayload, cmd, data);
            for (int tries = 0; tries < 10; tries++)
            {
                int got = ReadCtrl(timeoutMs);
                if (got < 0) throw new TimeoutException("no reply to 0x" + cmd.ToString("X2"));
                if (got < 5 || readBuf[1] != cmd) continue;
                var r = new byte[want];
                Buffer.BlockCopy(readBuf, 5, r, 0, Math.Min(want, got - 5));
                return r;
            }
            throw new IOException("unexpected replies to 0x" + cmd.ToString("X2"));
        }

        // One control-interface read at a time. A read that times out stays pending and is picked up by the next
        // call: abandoning it would let it swallow the next reply, so every later query would time out too.
        readonly byte[] readBuf = new byte[CtrlPacket + 1];
        IAsyncResult pendingRead;
        // Windows returns [report ID][report]; Linux hidraw returns the report alone, so it is read in after a 0 byte.
        int ReadCtrl(int timeoutMs)
        {
            int skip = Os.Linux ? 1 : 0;
            if (pendingRead == null) pendingRead = ctrl.BeginRead(readBuf, skip, readBuf.Length - skip, null, null);
            if (!pendingRead.AsyncWaitHandle.WaitOne(timeoutMs)) return -1;
            var ar = pendingRead; pendingRead = null;
            int n = ctrl.EndRead(ar);
            if (skip > 0) { readBuf[0] = 0; n += skip; }
            return n;
        }

        // ---- on-device storage (what the cooler shows on its own: idle/standby picture, boot animation) ----

        public const int StandbyFile = 0, BootFile = 1;            // file types of cmd 0x0A
        public const int StandbyMax = 0x4E2000, BootMax = 0x9C4000; // limits TT RGB Plus enforces (5 / 10 MB)
        public const int StandbyMaxLx = 950 * 1024;                // LX800 firmware idle-image cap (950 KB)
        const int AckWait = 600000;                                // acks can take minutes (flash erase); never abandon
        const int UpChunk = 0x2800;                                // 10 KB data per 0x0B, as TT RGB Plus uses

        // u32 0 + char[260] name: the request layout of cmds 0x02 (list) and 0x03 (delete).
        static byte[] NameArg(string name, int prefix)
        {
            var b = new byte[prefix + 260];
            var s = Encoding.ASCII.GetBytes(name);
            Buffer.BlockCopy(s, 0, b, prefix, Math.Min(s.Length, 259));
            return b;
        }

        // cmd 0x02: list a device folder. Reply: i32 count, folder path, then NUL-separated names.
        public string[] ListFiles(string folder)
        {
            var r = Request(0x02, NameArg(folder + "\\", 4), CtrlPayload, 5000);
            int count = BitConverter.ToInt32(r, 0);
            var parts = Encoding.ASCII.GetString(r, 4, r.Length - 4).Split('\0');
            // parts[0] = folder path echoed back, then the entries
            return parts.Skip(1).Where(p => p.Length > 0).Take(Math.Max(0, count)).ToArray();
        }

        // cmd 0x03: delete a device file/folder ("boot" = the unpacked boot animation).
        public bool DeleteFile(string name)
        {
            return BitConverter.ToInt32(Request(0x03, NameArg(name, 4), 4, 5000), 0) >= 0;
        }

        // cmd 0x0A (begin: u32 type, u32 size) then cmd 0x0B in 10 KB chunks, each acknowledged with
        // {u32 offset, u32 length, i32 percent}. type 0 = standby.jpg, 1 = boot.pak.
        public void UploadFile(int type, byte[] data, Action<int> progress)
        {
            var begin = new byte[8];
            BitConverter.GetBytes(type).CopyTo(begin, 0);
            BitConverter.GetBytes(data.Length).CopyTo(begin, 4);
            // TT RGB Plus waits without timeout here: the cooler erases flash and may take minutes before it
            // answers. Giving up early (and sending other commands) leaves it stuck mid-transfer until replugged.
            long ok = BitConverter.ToInt64(Request(0x0A, begin, 8, AckWait), 0);
            if (ok < 0) throw new IOException("cooler refused the upload (" + ok + ")");
            var chunk = new byte[UpChunk];
            for (int off = 0; off < data.Length; off += UpChunk)
            {
                int n = Math.Min(UpChunk, data.Length - off);
                if (n != UpChunk) chunk = new byte[n];
                Buffer.BlockCopy(data, off, chunk, 0, n);
                var ack = Request(0x0B, chunk, 12, AckWait);
                int pct = BitConverter.ToInt32(ack, 8);
                if (pct < 0) throw new IOException("upload failed at byte " + off);
                if (progress != null) progress(pct);
            }
        }

        // LX800 firmware upload: cmd 0x0A {u32 type, u32 size} begins (reply word1 < 0 = refused); cmd 0x0B
        // {u32 offset, u32 length, u32 percent, data...} writes each chunk. The firmware commits the file only
        // when the percent word exceeds 99, so the final chunk must carry 100. type 0 = idle (wapper.jpg).
        public void UploadFileLx(int type, byte[] data, Action<int> progress)
        {
            var begin = new byte[8];
            BitConverter.GetBytes(type).CopyTo(begin, 0);
            BitConverter.GetBytes(data.Length).CopyTo(begin, 4);
            var br = Request(0x0A, begin, 12, AckWait);
            if (BitConverter.ToInt32(br, 4) < 0) throw new IOException("cooler refused the upload (begin)");
            for (int off = 0; off < data.Length; off += UpChunk)
            {
                int n = Math.Min(UpChunk, data.Length - off);
                int pct = (int)((off + n) * 100L / data.Length);   // 100 on the last chunk -> firmware commits
                var payload = new byte[12 + n];
                BitConverter.GetBytes(off).CopyTo(payload, 0);
                BitConverter.GetBytes(n).CopyTo(payload, 4);
                BitConverter.GetBytes(pct).CopyTo(payload, 8);
                Buffer.BlockCopy(data, off, payload, 12, n);
                var ack = Request(0x0B, payload, 12, AckWait);
                if (BitConverter.ToInt32(ack, 8) < 0) throw new IOException("upload failed at byte " + off);
                if (progress != null) progress(pct);
            }
        }

        // LX800 read-back (read-only): cmd 0x0C {u32 type} opens the file for reading and returns its size;
        // cmd 0x0D {u32 offset, u32 length} returns that slice. Used to verify a write without a power-cycle.
        public byte[] ReadFileLx(int type, Action<int> progress = null)
        {
            var sz = Request(0x0C, BitConverter.GetBytes(type), 8, 5000);
            int size = BitConverter.ToInt32(sz, 4);
            if (size <= 0) return new byte[0];
            var outBuf = new byte[size];
            const int ReadChunk = 400;   // a reply is one 436-byte control packet (minus the 12-byte header)
            var buf = readBuf;
            for (int off = 0; off < size; off += ReadChunk)
            {
                int n = Math.Min(ReadChunk, size - off);
                var req = new byte[8];
                BitConverter.GetBytes(off).CopyTo(req, 0);
                BitConverter.GetBytes(n).CopyTo(req, 4);
                // the 0x0D reply payload is {u32 offset, u32 length, u32 word2, data...}: data starts at byte 12
                int got = 0;
                for (int tries = 0; tries < 10 && got == 0; tries++)
                {
                    Send(ctrl, CtrlPacket, CtrlPayload, 0x0D, req);
                    int r = ReadCtrl(10000);
                    if (r < 0) throw new TimeoutException("no reply to 0x0D");
                    if (r < 5 + 12 + n || buf[1] != 0x0D) continue;
                    Buffer.BlockCopy(buf, 5 + 12, outBuf, off, n);
                    got = n;
                }
                if (got == 0) throw new IOException("read-back failed at byte " + off);
                if (progress != null) progress((int)((off + n) * 100L / size));
            }
            return outBuf;
        }

        // cmd 0x13: choose the boot animation file; reply i32 != 0 on success.
        public bool SetBootFile(string name)
        {
            return BitConverter.ToInt32(Request(0x13, NameArg(name, 0).Take(0x104).ToArray(), 4, 5000), 0) != 0;
        }

        // cmd 0x14 (USB_CMD_SET_BOOT_SCREEN_RATE): boot animation frame interval in ms.
        public void SetBootFrameInterval(int ms)
        {
            Send(ctrl, CtrlPacket, CtrlPayload, 0x14, BitConverter.GetBytes(Math.Max(10, ms)));
        }

        public void ShowJpeg(byte[] jpeg) { Send(lcd, LcdPacket, LcdPayload, 0x08, ForPanel(jpeg)); }

        // Bake the panel's red/blue correction into a JPEG that will be stored or shown, so colours are right.
        public byte[] ForPanel(byte[] jpeg) { return SwapRedBlue ? SwapRB(jpeg) : jpeg; }

        public void SetBrightness(int pct) { Send(ctrl, CtrlPacket, CtrlPayload, 0x12, new[] { (byte)Math.Max(0, Math.Min(100, pct)) }); }

        // cmd 0x1E sets the idle-state byte (0x111): 0x02 lets the idle screen dim after a timeout, 0x00 keeps it
        // awake. This is exactly what the vendor app (TOWEI) sends for its sleep toggle. Re-sent on each connect.
        public void SetSleep(bool enabled) { Send(ctrl, CtrlPacket, CtrlPayload, 0x1E, new[] { (byte)(enabled ? 0x02 : 0x00) }); }

        // degrees 0/90/180/270 -> device code (0=0, 1=270, 2=180, 3=90)
        public void SetRotation(int degrees)
        {
            byte code;
            switch (((degrees % 360) + 360) % 360) { case 270: code = 1; break; case 180: code = 2; break; case 90: code = 3; break; default: code = 0; break; }
            Send(ctrl, CtrlPacket, CtrlPayload, 0x11, new[] { code });
        }

        // Coolant temperature from the pump sensor (CON1F), as the firmware reports it on cmd 0x82.
        // Value is hundredths of a degree C; ~129 C means the sensor is open (board out of the pump).
        public double CoolantC() { return BitConverter.ToInt32(Query(0x82, 4), 0) / 100.0; }

        public string Serial() { return Encoding.ASCII.GetString(Query(0x85, 16)).TrimEnd('\0'); }
        public int DeviceType() { return Query(0x84, 1)[0]; }
        public int Rotation()
        {
            switch (Query(0x81, 1)[0]) { case 1: return 270; case 2: return 180; case 3: return 90; default: return 0; }
        }

        public void Dispose()
        {
            if (ctrl != null) ctrl.Dispose();
            if (lcd != null) lcd.Dispose();
            if (owner != null)
            {
                try { owner.ReleaseMutex(); } catch (ApplicationException) { } // not owned by this thread
                owner.Dispose(); owner = null;
            }
        }
    }
}
