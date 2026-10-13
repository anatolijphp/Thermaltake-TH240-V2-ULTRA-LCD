//! PatchPanel: builds an LX800 firmware image whose ST7701S panel init (FUN_0010bd00) is replaced by the sequence
//! from Thermaltake's TT021 2.1" firmware. Everything else in the image stays byte-identical.
//! usage: PatchPanel <make_fix.bin> <out.bin>
//! Port of rescue/PatchPanel.cs; firmware/make_fix.bin -> firmware/lx800_tt021panel.bin (see the test below).

const BASE: u32 = 0xFFFE0; // file offset 0x20 is loaded at 0x100000
const CMD: u32 = 0x10bad8;
const DAT: u32 = 0x10bbec;
const DLY: u32 = 0x10b934;
const NOP: u32 = 0xE1A00000; // mov r0,r0

/// TT021-00-0_FW_V1.0.5.4 FUN_000097e8, preceded by the LX800's sleep-out + 120 ms
const SEQ: &str = "C11 W78 CFF D77 D01 D00 D00 D10 CC0 D3B D00 CC1 D0B D02 CC2 D07 D02 CCC D10 CCD D08 CB0 D00 D11 D16 D0E D11 D06 D05 D09 D08 D21 D06 D13 D10 D29 D31 D18 CB1 D00 D11 D16 D0E D11 D07 D05 D09 D09 D21 D05 D13 D11 D2A D31 D18 CFF D77 D01 D00 D00 D11 CB0 D6D CB1 D37 CB2 D81 CB3 D80 CB5 D43 CB7 D85 CB8 D20 CC1 D78 CC2 D78 CD0 D88 CE0 D00 D00 D02 CE1 D03 DA0 D00 D00 D04 DA0 D00 D00 D00 D20 D20 CE2 D00 D00 D00 D00 D00 D00 D00 D00 D00 D00 D00 D00 D00 CE3 D00 D00 D11 D00 CE4 D22 D00 CE5 D05 DEC DA0 DA0 D07 DEE DA0 DA0 D00 D00 D00 D00 D00 D00 D00 D00 CE6 D00 D00 D11 D00 CE7 D22 D00 CE8 D06 DED DA0 DA0 D08 DEF DA0 DA0 D00 D00 D00 D00 D00 D00 D00 D00 CEB D00 D00 D40 D40 D00 D00 D00 CED DFF DFF DFF DBA D0A DBF D45 DFF DFF D54 DFB DA0 DAB DFF DFF DFF CEF D10 D0D D04 D08 D3F D1F CFF D77 D01 D00 D00 D13 CEF D08 CFF D77 D01 D00 D00 D00 C11 W78 C29 W32 C36 D00 C3A D60";

struct Img(Vec<u8>);

impl Img {
    fn idx(a: u32) -> Result<usize, String> {
        a.checked_sub(BASE).map(|i| i as usize).ok_or_else(|| format!("address 0x{a:X} below image base"))
    }
    fn r(&self, a: u32) -> Result<u32, String> {
        let i = Self::idx(a)?;
        let s = self.0.get(i..i + 4).ok_or_else(|| format!("address 0x{a:X} past end of image"))?;
        Ok(u32::from_le_bytes([s[0], s[1], s[2], s[3]]))
    }
    fn w(&mut self, a: u32, v: u32) -> Result<(), String> {
        let i = Self::idx(a)?;
        let s = self.0.get_mut(i..i + 4).ok_or_else(|| format!("address 0x{a:X} past end of image"))?;
        s.copy_from_slice(&v.to_le_bytes());
        Ok(())
    }
}

fn mov(imm: u32) -> u32 {
    assert!(imm <= 0xFF, "imm");
    0xE3A00000 | imm
}

fn bl(at: u32, target: u32) -> u32 {
    0xEB000000 | (((target as i32).wrapping_sub(at as i32 + 8) >> 2) as u32 & 0xFFFFFF)
}

/// Patches the image in place. Progress lines go to `out`.
fn patch(data: Vec<u8>, out: &mut dyn FnMut(String)) -> Result<Vec<u8>, String> {
    let orig = data.clone();
    let mut b = Img(data);
    // the encoder must reproduce the original instructions exactly
    let ok = b.r(0x10bd28)? == mov(0x11)
        && b.r(0x10bd2c)? == bl(0x10bd2c, CMD)
        && b.r(0x10bd34)? == bl(0x10bd34, DLY)
        && b.r(0x10bd44)? == bl(0x10bd44, DAT)
        && b.r(0x10c0f4)? == bl(0x10c0f4, DAT)
        && b.r(0x10c40c)? == bl(0x10c40c, CMD)
        && b.r(0x10c0dc)? == 0xEA000004
        && b.r(0x10c410)? == 0xE8BD8010;
    out(format!("encoder/layout check: {}", if ok { "True" } else { "False" }));
    if !ok {
        return Err("not the expected LX800 image".into());
    }

    let mut slots: Vec<u32> = (0x10bd28..0x10c0d8).step_by(8).collect(); // before the branch over the string
    slots.extend((0x10c0f4..0x10c40c).step_by(8)); // after the string, up to the return
    let ops: Vec<&str> = SEQ.split(' ').collect();
    out(format!("ops {}, slots {}", ops.len(), slots.len()));
    if ops.len() > slots.len() {
        return Err("sequence does not fit".into());
    }
    for (i, &p) in slots.iter().enumerate() {
        if let Some(op) = ops.get(i) {
            let v = u32::from_str_radix(&op[1..], 16).map_err(|e| format!("bad op {op}: {e}"))?;
            let t = match op.as_bytes()[0] {
                b'C' => CMD,
                b'D' => DAT,
                _ => DLY,
            };
            b.w(p, mov(v))?;
            b.w(p + 4, bl(p + 4, t))?;
        } else {
            b.w(p, NOP)?;
            b.w(p + 4, NOP)?;
        }
    }
    // single leftover words (branch at 0x10c0dc and return at 0x10c410 kept)
    b.w(0x10c0d8, NOP)?;
    b.w(0x10c40c, NOP)?;
    if b.r(0x10c0dc)? != 0xEA000004 || b.r(0x10c410)? != 0xE8BD8010 {
        return Err("layout broken".into());
    }

    let changed: Vec<usize> = (0..b.0.len()).filter(|&i| b.0[i] != orig[i]).collect();
    if let (Some(&lo), Some(&hi)) = (changed.first(), changed.last()) {
        out(format!("changed {} bytes, addr 0x{:X}-0x{:X}", changed.len(), lo as u32 + BASE, hi as u32 + BASE));
    }
    Ok(b.0)
}

fn main() {
    let args: Vec<String> = std::env::args().skip(1).collect();
    if args.len() != 2 {
        eprintln!("usage: PatchPanel <make_fix.bin> <out.bin>");
        std::process::exit(2);
    }
    let run = || -> Result<(), String> {
        let data = std::fs::read(&args[0]).map_err(|e| format!("{}: {e}", args[0]))?;
        let b = patch(data, &mut |s| println!("{s}"))?;
        std::fs::write(&args[1], b).map_err(|e| format!("{}: {e}", args[1]))
    };
    if let Err(e) = run() {
        println!("{e}");
        std::process::exit(1);
    }
}

#[cfg(test)]
mod tests {
    /// The shipped firmware must be exactly what PatchPanel makes from the LX800 base image.
    #[test]
    fn rebuilds_shipped_firmware() {
        let fw = std::path::Path::new(env!("CARGO_MANIFEST_DIR")).join("../../firmware");
        let base = std::fs::read(fw.join("make_fix.bin")).expect("firmware/make_fix.bin");
        let want = std::fs::read(fw.join("lx800_tt021panel.bin")).expect("firmware/lx800_tt021panel.bin");
        let got = super::patch(base, &mut |_| {}).unwrap();
        assert!(got == want, "output differs from firmware/lx800_tt021panel.bin");
    }
}
