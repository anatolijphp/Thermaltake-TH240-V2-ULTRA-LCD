//! ttrescue: flash read / write+verify for a cooler LCD in Nuvoton W55FA93 ROM recovery mode (USB 0416:9396).
//! See firmware/README.md for the burn procedure. Port of rescue/Rescue.cs.
//!
//! Mirrors TurboWriter V2.30.003_N329x1 (OpenNuvoton/N32905_NonOS_Tool):
//!   recovery ROM (IBR):  06 51 connect, 06 52/53 DRAM clock skew, 06 55 load address, 06 72 data, 06 60 run
//!   writer (FA93_musb):  61 command {action, ...} with CDB[6] = media (6 = SPI), 63 data-out, 64 data-in,
//!                        65 query, 66 set
//! Only RAM is written by the IBR stage (the helper program). `write` raw-writes an image at flash offset 0
//! and reads it back.
//!
//! Transport: Windows uses IOCTL_SCSI_PASS_THROUGH_DIRECT on the disk interface (the C# tool was built x86 to copy
//! TurboWriter's 32-bit struct; a native 64-bit build uses the 64-bit SCSI_PASS_THROUGH_DIRECT, which the kernel
//! accepts just the same). Linux uses the SG_IO ioctl on /dev/sg* (needs root, since the kernel only lets
//! CAP_SYS_RAWIO send vendor-specific CDBs). The Linux path has not been tried on hardware yet.

use std::fs::OpenOptions;
use std::io::Write;
use std::path::{Path, PathBuf};
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Mutex;
use std::thread::sleep;
use std::time::Duration;

#[cfg(windows)]
mod win;
#[cfg(windows)]
use win as os;
#[cfg(target_os = "linux")]
mod linux;
#[cfg(target_os = "linux")]
use linux as os;

static LOG: Mutex<Option<std::fs::File>> = Mutex::new(None);
static VERBOSE: AtomicBool = AtomicBool::new(true);

fn log(s: &str) {
    println!("{s}");
    if let Some(f) = LOG.lock().unwrap().as_mut() {
        let _ = writeln!(f, "{} {s}", os::clock());
        let _ = f.flush();
    }
}

type Res<T> = Result<T, String>;

/// Result of one pass-through request. `ok` = the request went through (TurboWriter checks nothing else: the
/// recovery loader answers every vendor command with CHECK CONDITION / "medium not present").
pub struct Pt {
    pub ok: bool,
    pub err: String,
    pub status: u8,
    pub sense: [u8; 32],
}

/// An opened recovery-loader or writer device.
pub trait Dev {
    fn pt(&mut self, cdb: &[u8], data: &mut [u8], data_in: bool, timeout_s: u32) -> Pt;
}

fn hex(b: &[u8]) -> String {
    b.iter().map(|x| format!("{x:02X}")).collect::<Vec<_>>().join("-")
}

fn pt(h: &mut dyn Dev, cdb: &[u8], data: &mut [u8], data_in: bool, timeout_s: u32) -> Pt {
    let r = h.pt(cdb, data, data_in, timeout_s);
    if r.ok && r.status != 0 && VERBOSE.load(Ordering::Relaxed) {
        log(&format!("  ({}) scsi status {} sense {}", hex(&cdb[..2]), r.status, hex(&r.sense[..18])));
    }
    r
}

fn must(h: &mut dyn Dev, cdb: &[u8], data: &mut [u8], data_in: bool, timeout_s: u32, what: &str) -> Res<()> {
    let r = pt(h, cdb, data, data_in, timeout_s);
    if r.ok { Ok(()) } else { Err(format!("{what} failed ({})", r.err)) }
}

fn cdb(op: u8, sub: u8, b2: u8) -> [u8; 10] {
    let mut c = [0u8; 10];
    c[0] = op;
    c[1] = sub;
    c[2] = b2;
    c
}

fn cdb_u32(sub: u8, v: u32) -> [u8; 10] {
    let mut c = [0u8; 10];
    c[0] = 0x06;
    c[1] = sub;
    c[2..6].copy_from_slice(&v.to_le_bytes());
    c
}

/// Writer-stage CDB: opcode in byte 0, selector in byte 6
fn cdb6(op: u8, what: u8) -> [u8; 10] {
    let mut c = [0u8; 10];
    c[0] = op;
    c[6] = what;
    c
}

fn le32(b: &[u8], at: usize) -> u32 {
    u32::from_le_bytes([b[at], b[at + 1], b[at + 2], b[at + 3]])
}

// ---------- stage 1: recovery ROM ----------
fn load_helper(h: &mut dyn Dev, bin: &[u8]) -> Res<()> {
    if bin.len() < 16 || &bin[..4] != b"NVT " {
        return Err("bad helper header".into());
    }
    let (addr, len) = (le32(bin, 4), le32(bin, 8));
    if 16 + len as usize > bin.len() {
        return Err("helper file shorter than its header says".into());
    }
    log(&format!("helper: load 0x{addr:X} len 0x{len:X}"));
    let mut info = vec![0u8; 0xDFF];
    let mut r = pt(h, &cdb(0x06, 0x51, 1), &mut info, true, 20);
    for _ in 1..10 {
        if r.ok {
            break;
        }
        sleep(Duration::from_millis(100));
        r = pt(h, &cdb(0x06, 0x51, 1), &mut info, true, 20);
    }
    if !r.ok {
        return Err(format!("IBR connect failed ({})", r.err));
    }
    log(&format!("IBR connect ok: {}", hex(&info[..32])));
    must(h, &cdb_u32(0x52, 0x00001010), &mut vec![0u8; 0xDFF], true, 20, "DQSODS")?;
    must(h, &cdb_u32(0x53, 0x00888800), &mut vec![0u8; 0xDFF], true, 20, "CKDQSDS")?;
    must(h, &cdb_u32(0x55, addr), &mut vec![0u8; 0xDFF], true, 20, "set address")?;
    let end = 16 + len as usize;
    let mut off = 16usize;
    while off < end {
        let n = (end - off).min(0x10000);
        let mut chunk = bin[off..off + n].to_vec();
        let mut c = cdb(0x06, 0x72, n as u8);
        c[3] = (n >> 8) as u8;
        c[4] = (n >> 16) as u8;
        must(h, &c, &mut chunk, false, 0x32, &format!("write RAM @{off}"))?;
        off += n;
    }
    log("helper uploaded");
    pt(h, &cdb(0x06, 0x60, 0), &mut [], true, 5); // run; the device drops off USB, so no status
    Ok(())
}

// ---------- stage 2: writer ----------
fn query(h: &mut dyn Dev, what: u8) -> Res<i32> {
    let mut buf = vec![0u8; if what == 6 { 5 } else { 4 }];
    must(h, &cdb6(0x65, what), &mut buf, true, 20, &format!("query {what}"))?;
    Ok(le32(&buf, 0) as i32)
}

/// TurboWriter (FUN_0041bd80 / FUN_0041e4c0): 65/7 = flash size in KB; when 0, 65/8 = JEDEC ID,
/// 66/6 = tell the helper the size (from SPIFLASH ID.ini), then 65/7 again. Without it reads return FF.
fn init_spi(h: &mut dyn Dev) -> Res<i32> {
    let kb = query(h, 7)?;
    log(&format!("SPI size reported: {kb} KB"));
    if kb > 0 {
        return Ok(kb);
    }
    let id = query(h, 8)? & 0xFFFFFF;
    log(&format!("SPI JEDEC ID: {id:06X}"));
    let known = [(0xC84017, 8192), (0xC22014, 1024), (0xEF3013, 8192), (0xC84018, 16384), (0xC84016, 4096), (0xA14018, 16384)];
    let cap = id & 0xFF; // ID as in SPIFLASH ID.ini: manufacturer, type, capacity (2^n bytes)
    let kb: i32 = match known.iter().find(|k| k.0 == id) {
        Some(k) => k.1,
        None if (0x10..=0x19).contains(&cap) => (1 << cap) / 1024,
        None => 0,
    };
    if kb == 0 {
        return Err(format!("unknown flash size for ID {id:06X}"));
    }
    log(&format!("setting SPI size {kb} KB"));
    must(h, &cdb6(0x66, 6), &mut kb.to_le_bytes(), false, 100, "set SPI size")?;
    let now = query(h, 7)?;
    log(&format!("SPI size now: {now} KB"));
    Ok(if now > 0 { now } else { kb })
}

/// TurboWriter "SPI (Raw Data)" burn (FUN_00424c90): 61 {action 0, length, 0} media 6, data via 63 in 64 KB
/// pieces, then 65/3 status (first byte 0 = OK). The image goes to flash offset 0 unchanged.
fn write_spi(h: &mut dyn Dev, img: &[u8]) -> Res<()> {
    init_spi(h)?;
    let mut cmd = [0u8; 12];
    cmd[4..8].copy_from_slice(&(img.len() as u32).to_le_bytes());
    must(h, &cdb6(0x61, 6), &mut cmd, false, 100, "write command")?;
    VERBOSE.store(false, Ordering::Relaxed);
    for off in (0..img.len()).step_by(0x10000) {
        let n = (img.len() - off).min(0x10000);
        let mut chunk = img[off..off + n].to_vec();
        must(h, &cdb6(0x63, 0), &mut chunk, false, 20, &format!("write data @{off}"))?;
        log(&format!("  wrote {} / {} KB", (off + n) / 1024, img.len() / 1024));
    }
    sleep(Duration::from_millis(100));
    let mut st = [0u8; 4];
    let mut ok = false;
    for _ in 0..10 {
        ok = pt(h, &cdb6(0x65, 3), &mut st, true, 20).ok;
        if ok {
            break;
        }
    }
    log(&format!("write status: {}", hex(&st)));
    if !ok || st[0] != 0 {
        return Err("writer reported failure".into());
    }
    VERBOSE.store(true, Ordering::Relaxed);
    Ok(())
}

fn read_spi(h: &mut dyn Dev, out: &Path, blocks: usize) -> Res<()> {
    let kb = init_spi(h)?;
    let blocks = blocks.min((kb.max(0) / 64) as usize);
    log(&format!("reading {blocks} blocks"));
    let mut cmd = [0u8; 12];
    cmd[0..4].copy_from_slice(&3u32.to_le_bytes()); // action 3 = read (as TurboWriter Read/Verify)
    must(h, &cdb6(0x61, 6), &mut cmd, false, 100, "read command")?; // media 6 = SPI
    let mut f = std::fs::File::create(out).map_err(|e| format!("{}: {e}", out.display()))?;
    let mut buf = vec![0u8; 0x10000];
    VERBOSE.store(false, Ordering::Relaxed);
    for i in 0..blocks {
        buf.fill(0);
        must(h, &cdb6(0x64, 0), &mut buf, true, 20, &format!("read block {i}"))?;
        f.write_all(&buf).map_err(|e| format!("{}: {e}", out.display()))?;
        if i % 16 == 15 || i == blocks - 1 {
            log(&format!("  {} / {} KB", (i + 1) * 64, blocks * 64));
        }
    }
    VERBOSE.store(true, Ordering::Relaxed);
    Ok(())
}

/// Where the log and default dumps go: next to the exe (as the C# tool did), or the current directory when that
/// folder isn't writable (an installed copy under /usr/lib or Program Files).
fn out_dir(exe_dir: &Path) -> PathBuf {
    let probe = exe_dir.join(".ttrescue-write-test");
    if std::fs::write(&probe, b"").is_ok() {
        let _ = std::fs::remove_file(&probe);
        exe_dir.to_path_buf()
    } else {
        std::env::current_dir().unwrap_or_else(|_| exe_dir.to_path_buf())
    }
}

fn usage() -> ! {
    eprintln!("usage: ttrescue [dump.bin [blocks]]   read flash (64 KB blocks, default 256 = 16 MB)");
    eprintln!("       ttrescue write <image.bin>     write image at flash offset 0, then read back and compare");
    std::process::exit(2);
}

fn main() {
    let args: Vec<String> = std::env::args().skip(1).collect();
    if args.first().map_or(false, |a| a == "-h" || a == "--help" || a == "/?") {
        usage();
    }
    let exe_dir = std::env::current_exe()
        .ok()
        .and_then(|p| p.parent().map(Path::to_path_buf))
        .unwrap_or_else(|| PathBuf::from("."));
    let dir = out_dir(&exe_dir);
    let write = args.len() > 1 && args[0] == "write";
    if args.first().map_or(false, |a| a == "write") && !write {
        usage();
    }
    let image = if write { Some(PathBuf::from(&args[1])) } else { None };
    let out_file = if write {
        dir.join("verify_dump.bin")
    } else if let Some(a) = args.first() {
        PathBuf::from(a)
    } else {
        dir.join("flash_dump.bin")
    };
    let blocks: usize = if !write && args.len() > 1 {
        args[1].parse().unwrap_or_else(|_| usage())
    } else {
        256
    };
    // Nuvoton's flash-writer helper: next to the exe, in firmware/ (the repo copy), or in a local TurboWriter
    let cands = [
        exe_dir.join("FA93_musb.bin"),
        exe_dir.join("../firmware/FA93_musb.bin"),
        exe_dir.join("firmware/FA93_musb.bin"),
        exe_dir.join("../tools/turbowriter/FA93_musb.bin"),
    ];
    let helper = cands.iter().find(|p| p.is_file()).cloned().unwrap_or_else(|| cands[1].clone());
    *LOG.lock().unwrap() = OpenOptions::new().create(true).append(true).open(dir.join("rescue.log")).ok();

    let code = match run(write, image.as_deref(), &out_file, blocks, &helper) {
        Ok(()) => 0,
        Err(e) => {
            log(&format!("ERROR: {e}"));
            1
        }
    };
    *LOG.lock().unwrap() = None;
    std::process::exit(code);
}

fn run(write: bool, image: Option<&Path>, out_file: &Path, blocks: usize, helper: &Path) -> Res<()> {
    log(&format!("=== read {blocks} x 64 KB -> {}", out_file.display()));
    if let Some(path) = os::find_loader() {
        log(&format!("recovery loader: {path}"));
        let bin = std::fs::read(helper).map_err(|e| format!("{}: {e}", helper.display()))?;
        {
            let mut h = os::open(&path)?;
            load_helper(&mut h, &bin)?;
        }
        log("helper started, waiting for writer device...");
        sleep(Duration::from_secs(2));
    } else {
        log("no recovery loader found, looking for an already running writer");
    }
    let mut w = None;
    for _ in 0..100 {
        w = os::find_writer();
        if w.is_some() {
            break;
        }
        sleep(Duration::from_millis(300));
    }
    let Some(w) = w else {
        for d in os::devices() {
            log(&format!("  disk: {d}"));
        }
        return Err("writer device did not appear".into());
    };
    log(&format!("writer: {w}"));
    if write {
        let image = image.unwrap();
        let img = std::fs::read(image).map_err(|e| format!("{}: {e}", image.display()))?;
        log(&format!("image {}: {} bytes", image.display(), img.len()));
        write_spi(&mut os::open(&w)?, &img)?;
        let vb = img.len().div_ceil(0x10000);
        read_spi(&mut os::open(&w)?, out_file, vb)?;
        let back = std::fs::read(out_file).map_err(|e| format!("{}: {e}", out_file.display()))?;
        if back.len() < img.len() {
            return Err(format!("verify failed: read back only {} of {} bytes", back.len(), img.len()));
        }
        if let Some(i) = (0..img.len()).find(|&i| back[i] != img[i]) {
            return Err(format!("verify failed at 0x{i:X}: wrote {:02X}, read {:02X}", img[i], back[i]));
        }
        log(&format!("verify OK: {} bytes match", img.len()));
        return Ok(());
    }
    read_spi(&mut os::open(&w)?, out_file, blocks)?;
    log("done");
    Ok(())
}

/// Device-name matching shared by both OSes (Windows: disk interface path; Linux: "vendor model usb-id" string).
pub fn is_loader(s: &str) -> bool {
    let l = s.to_ascii_lowercase();
    l.contains("ven_w55fa93&prod_dev_tool") || (l.contains("w55fa93") && l.contains("dev tool"))
}

/// The writer re-enumerates as a Nuvoton/W55FA93 mass storage that is not the recovery loader.
pub fn is_writer(s: &str) -> bool {
    let l = s.to_ascii_lowercase();
    !is_loader(s) && (l.contains("nuvoton") || l.contains("w55fa9") || l.contains("ven_nuvo") || l.contains("vid_0416"))
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn matching() {
        assert!(is_loader(r"\\?\usbstor#disk&ven_w55fa93&prod_dev_tool&rev_1.00#5&1&0#{53f56307-b6bf-11d0-94f2-00a0c91efb8b}"));
        assert!(is_loader("W55FA93 DEV TOOL vid_0416&pid_9396"));
        assert!(!is_writer("W55FA93 DEV TOOL vid_0416&pid_9396"));
        assert!(is_writer("Nuvoton USB Mass Storage vid_0416&pid_5011"));
        assert!(is_writer(r"\\?\usbstor#disk&ven_nuvoton&prod_w55fa93#x"));
        assert!(!is_writer("ATA Samsung SSD vid_0000&pid_0000"));
    }

    #[test]
    fn cdbs() {
        assert_eq!(cdb_u32(0x55, 0x00900000), [6, 0x55, 0, 0, 0x90, 0, 0, 0, 0, 0]);
        assert_eq!(cdb6(0x65, 7), [0x65, 0, 0, 0, 0, 0, 7, 0, 0, 0]);
    }

    /// Fake device: checks that the IBR stage sends the helper in the same requests as the C# tool.
    struct Fake(Vec<(Vec<u8>, usize, bool)>);
    impl Dev for Fake {
        fn pt(&mut self, cdb: &[u8], data: &mut [u8], data_in: bool, _t: u32) -> Pt {
            self.0.push((cdb.to_vec(), data.len(), data_in));
            Pt { ok: true, err: String::new(), status: 0, sense: [0; 32] }
        }
    }

    #[test]
    fn helper_upload_sequence() {
        let fw = Path::new(env!("CARGO_MANIFEST_DIR")).join("../../firmware/FA93_musb.bin");
        let bin = std::fs::read(fw).unwrap();
        let len = le32(&bin, 8) as usize;
        let mut f = Fake(vec![]);
        load_helper(&mut f, &bin).unwrap();
        let c = &f.0;
        assert_eq!(&c[0].0[..3], &[6, 0x51, 1]);
        assert_eq!(&c[3].0[..6], &cdb_u32(0x55, le32(&bin, 4))[..6]);
        let data: Vec<_> = c.iter().filter(|x| x.0[1] == 0x72).collect();
        assert_eq!(data.iter().map(|x| x.1).sum::<usize>(), len);
        for d in &data {
            assert!(!d.2);
            assert_eq!(d.0[2] as usize | (d.0[3] as usize) << 8 | (d.0[4] as usize) << 16, d.1);
        }
        assert_eq!(&c.last().unwrap().0[..2], &[6, 0x60]);
    }
}
