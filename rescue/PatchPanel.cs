// Builds an LX800 firmware image whose ST7701S panel init (FUN_0010bd00) is replaced by the sequence from
// Thermaltake's TT021 2.1" firmware. Everything else in the image stays byte-identical.
// usage: PatchPanel <make_fix.bin> <out.bin>
using System;
using System.Collections.Generic;
using System.IO;

static class PatchPanel
{
    const uint Base = 0xFFFE0;                 // file offset 0x20 is loaded at 0x100000
    const uint Cmd = 0x10bad8, Dat = 0x10bbec, Dly = 0x10b934;
    const uint Nop = 0xE1A00000;               // mov r0,r0
    static byte[] b;

    static uint R(uint a) { return BitConverter.ToUInt32(b, (int)(a - Base)); }
    static void W(uint a, uint v) { BitConverter.GetBytes(v).CopyTo(b, (int)(a - Base)); }
    static uint Mov(uint imm) { if (imm > 0xFF) throw new ArgumentException("imm"); return 0xE3A00000u | imm; }
    static uint Bl(uint at, uint target) { return 0xEB000000u | ((uint)(((int)target - (int)(at + 8)) >> 2) & 0xFFFFFFu); }

    // TT021-00-0_FW_V1.0.5.4 FUN_000097e8, preceded by the LX800's sleep-out + 120 ms
    const string Seq = "C11 W78 CFF D77 D01 D00 D00 D10 CC0 D3B D00 CC1 D0B D02 CC2 D07 D02 CCC D10 CCD D08 CB0 D00 D11 D16 D0E D11 D06 D05 D09 D08 D21 D06 D13 D10 D29 D31 D18 CB1 D00 D11 D16 D0E D11 D07 D05 D09 D09 D21 D05 D13 D11 D2A D31 D18 CFF D77 D01 D00 D00 D11 CB0 D6D CB1 D37 CB2 D81 CB3 D80 CB5 D43 CB7 D85 CB8 D20 CC1 D78 CC2 D78 CD0 D88 CE0 D00 D00 D02 CE1 D03 DA0 D00 D00 D04 DA0 D00 D00 D00 D20 D20 CE2 D00 D00 D00 D00 D00 D00 D00 D00 D00 D00 D00 D00 D00 CE3 D00 D00 D11 D00 CE4 D22 D00 CE5 D05 DEC DA0 DA0 D07 DEE DA0 DA0 D00 D00 D00 D00 D00 D00 D00 D00 CE6 D00 D00 D11 D00 CE7 D22 D00 CE8 D06 DED DA0 DA0 D08 DEF DA0 DA0 D00 D00 D00 D00 D00 D00 D00 D00 CEB D00 D00 D40 D40 D00 D00 D00 CED DFF DFF DFF DBA D0A DBF D45 DFF DFF D54 DFB DA0 DAB DFF DFF DFF CEF D10 D0D D04 D08 D3F D1F CFF D77 D01 D00 D00 D13 CEF D08 CFF D77 D01 D00 D00 D00 C11 W78 C29 W32 C36 D00 C3A D60";

    static int Main(string[] a)
    {
        b = File.ReadAllBytes(a[0]);
        var orig = (byte[])b.Clone();
        // the encoder must reproduce the original instructions exactly
        bool ok = R(0x10bd28) == Mov(0x11) && R(0x10bd2c) == Bl(0x10bd2c, Cmd) && R(0x10bd34) == Bl(0x10bd34, Dly)
               && R(0x10bd44) == Bl(0x10bd44, Dat) && R(0x10c0f4) == Bl(0x10c0f4, Dat) && R(0x10c40c) == Bl(0x10c40c, Cmd)
               && R(0x10c0dc) == 0xEA000004 && R(0x10c410) == 0xE8BD8010;
        Console.WriteLine("encoder/layout check: " + ok);
        if (!ok) return 1;

        var slots = new List<uint>();
        for (uint p = 0x10bd28; p < 0x10c0d8; p += 8) slots.Add(p);   // before the branch over the string
        for (uint p = 0x10c0f4; p < 0x10c40c; p += 8) slots.Add(p);   // after the string, up to the return
        var ops = Seq.Split(' ');
        Console.WriteLine("ops " + ops.Length + ", slots " + slots.Count);
        if (ops.Length > slots.Count) return 1;
        for (int i = 0; i < slots.Count; i++)
        {
            uint p = slots[i];
            if (i < ops.Length)
            {
                uint v = Convert.ToUInt32(ops[i].Substring(1), 16);
                uint t = ops[i][0] == 'C' ? Cmd : ops[i][0] == 'D' ? Dat : Dly;
                W(p, Mov(v)); W(p + 4, Bl(p + 4, t));
            }
            else { W(p, Nop); W(p + 4, Nop); }
        }
        W(0x10c0d8, Nop); W(0x10c40c, Nop);   // single leftover words (branch at 0x10c0dc and return at 0x10c410 kept)
        if (R(0x10c0dc) != 0xEA000004 || R(0x10c410) != 0xE8BD8010) { Console.WriteLine("layout broken"); return 1; }

        int diff = 0, lo = -1, hi = -1;
        for (int i = 0; i < b.Length; i++) if (b[i] != orig[i]) { diff++; if (lo < 0) lo = i; hi = i; }
        Console.WriteLine(string.Format("changed {0} bytes, addr 0x{1:X}-0x{2:X}", diff, lo + Base, hi + Base));
        File.WriteAllBytes(a[1], b);
        return 0;
    }
}
