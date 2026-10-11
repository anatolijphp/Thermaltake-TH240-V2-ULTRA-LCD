// Linux support: the cooler's HID interfaces through hidraw (/sys/class/hidraw + /dev/hidrawN), and which parts of the
// app exist there. Built with Mono (build.sh); the Windows build compiles it too but never calls it.
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;

namespace TtLcd
{
    static class Os
    {
        public static readonly bool Linux = Environment.OSVersion.Platform == PlatformID.Unix;

        // Modes that need Windows (virtual display, GDI screen capture, Media Foundation). Aura works on both:
        // Armoury Crate's plugin on Windows, armory-crated's TTAS file on Linux (AuraState.ReadLinux).
        public static bool Supports(Mode m)
        {
            return !Linux || m == Mode.Image || m == Mode.Gif || m == Mode.Color || m == Mode.Clock || m == Mode.Off
                || m == Mode.Aura;
        }
    }

    static class LinuxHid
    {
        // Each /sys/class/hidraw/hidrawN/device/uevent has HID_ID=0003:00001A2C:0000151D (bus:vid:pid) and
        // HID_PHYS=usb-0000:00:14.0-2/input0, whose "inputN" is the USB interface number (MI_0N on Windows).
        public static List<HidDeviceInfo> Enumerate(ushort vid, ushort pid)
        {
            var list = new List<HidDeviceInfo>();
            const string root = "/sys/class/hidraw";
            if (!Directory.Exists(root)) return list;
            foreach (var dir in Directory.GetDirectories(root))
            {
                string uevent = Path.Combine(Path.Combine(dir, "device"), "uevent");
                if (!File.Exists(uevent)) continue;
                string id = null, phys = null;
                foreach (var line in File.ReadAllLines(uevent))
                {
                    if (line.StartsWith("HID_ID=")) id = line.Substring(7);
                    else if (line.StartsWith("HID_PHYS=")) phys = line.Substring(9);
                }
                if (id == null) continue;
                var p = id.Split(':');
                if (p.Length < 3) continue;
                uint v, d;
                if (!uint.TryParse(p[1], NumberStyles.HexNumber, CultureInfo.InvariantCulture, out v) ||
                    !uint.TryParse(p[2], NumberStyles.HexNumber, CultureInfo.InvariantCulture, out d)) continue;
                if (v != vid || d != pid) continue;
                int iface = -1;
                int k = phys == null ? -1 : phys.LastIndexOf("/input");
                if (k >= 0) int.TryParse(phys.Substring(k + 6), out iface);
                list.Add(new HidDeviceInfo { Path = "/dev/" + Path.GetFileName(dir), Vid = vid, Pid = pid, Interface = iface });
            }
            return list;
        }

        // One write() per report: [report ID 0][packet]. Reads return the report without the ID byte (see Lcd.ReadCtrl).
        public static FileStream Open(string path)
        {
            try { return new FileStream(path, FileMode.Open, FileAccess.ReadWrite, FileShare.ReadWrite, 1, false); }
            catch (UnauthorizedAccessException)
            {
                throw new IOException("no permission for " + path + ": install the udev rule (the .deb does), then replug the cooler or reboot");
            }
        }
    }
}
