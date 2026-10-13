// Port of src/Lcd.cs: framing, queries and live frames. The file-upload commands (0x0A/0x0B) are deliberately left
// out: on the original Thermaltake firmware they erased the controller's flash (see CLAUDE.md).
use crate::hid::{self, Dev};
use std::io;
use std::sync::Mutex;

pub const VID: u16 = 0x264A;
pub const PID: u16 = 0x233C;
pub const ALT_VID: u16 = 0x1A2C; // same HKC board running the LX800 rescue firmware
pub const ALT_PID: u16 = 0x151D;
pub const WIDTH: usize = 480;
pub const HEIGHT: usize = 480;
const CTRL_PACKET: usize = 440; // MI_00
const CTRL_PAYLOAD: usize = 436;
const LCD_PACKET: usize = 1024; // MI_01
const LCD_PAYLOAD: usize = 1020;

/// The cooler. MI_00 (control) and MI_01 (frames) each have their own lock, so one thread can stream frames while
/// another sends control commands, as the engine does: the first control reply after opening can take ~10 s.
pub struct Lcd {
    ctrl: Mutex<Ctrl>,
    lcd: Mutex<Dev>,
    /// LX800 firmware board: its panel setup shows red and blue exchanged, so frames are swapped back before sending.
    pub swap_red_blue: bool,
    #[cfg(windows)]
    _owner: hid::DeviceMutex,
}

struct Ctrl {
    dev: Dev,
    // read_buf[0] stands for the report-ID byte so offsets match the C# code and docs/protocol.md
    buf: [u8; CTRL_PACKET + 1],
}

fn err(msg: impl Into<String>) -> io::Error {
    io::Error::other(msg.into())
}

impl Lcd {
    /// Only one process may talk to the cooler: frames from two writers interleave packet-wise and can lock up the
    /// cooler's image decoder until a power cycle. Open and drop on the same thread (the Windows mutex is per thread).
    pub fn open() -> io::Result<Lcd> {
        #[cfg(windows)]
        let owner = hid::DeviceMutex::acquire()?;
        let mut devs = hid::enumerate(VID, PID);
        let mut swap = false;
        if devs.is_empty() {
            devs = hid::enumerate(ALT_VID, ALT_PID);
            swap = !devs.is_empty();
        }
        let c = devs.iter().find(|d| d.interface == 0);
        let l = devs.iter().find(|d| d.interface == 1);
        let (Some(c), Some(l)) = (c, l) else { return Err(err("Cooler 264A:233C not found.")) };
        let ctrl = Dev::open(&c.path)?;
        if !ctrl.try_lock() {
            return Err(err("Cooler is in use by another TT LCD program (Studio or the ttlcd service)."));
        }
        let lcd = Dev::open(&l.path)?;
        Ok(Lcd {
            ctrl: Mutex::new(Ctrl { dev: ctrl, buf: [0; CTRL_PACKET + 1] }),
            lcd: Mutex::new(lcd),
            swap_red_blue: swap,
            #[cfg(windows)]
            _owner: owner,
        })
    }

    // header u32 LE: (first ? 0x80000000 : 0) | ((first ? total : seq) << 8) | cmd
    fn send(dev: &mut Dev, packet: usize, payload: usize, cmd: u8, data: &[u8]) -> io::Result<()> {
        let total = data.len().div_ceil(payload).max(1);
        let mut buf = vec![0u8; packet + 1]; // [0] = HID report ID 0
        for seq in 0..total {
            buf.fill(0);
            let hdr: u32 = if seq == 0 { 0x8000_0000 | ((total as u32) << 8) } else { (seq as u32) << 8 } | cmd as u32;
            buf[1..5].copy_from_slice(&hdr.to_le_bytes());
            let chunk = data.get(seq * payload..).unwrap_or(&[]);
            let n = chunk.len().min(payload);
            buf[5..5 + n].copy_from_slice(&chunk[..n]);
            dev.write(&buf)?;
        }
        Ok(())
    }

    fn control(&self, cmd: u8, data: &[u8]) -> io::Result<()> {
        Self::send(&mut self.ctrl.lock().unwrap().dev, CTRL_PACKET, CTRL_PAYLOAD, cmd, data)
    }

    /// Control command with a reply: one packet on MI_00 whose first byte is the same command.
    fn request(&self, cmd: u8, data: &[u8], want: usize, timeout_ms: u32) -> io::Result<Vec<u8>> {
        let mut c = self.ctrl.lock().unwrap();
        let c = &mut *c;
        Self::send(&mut c.dev, CTRL_PACKET, CTRL_PAYLOAD, cmd, data)?;
        for _ in 0..10 {
            let got = match c.dev.read(&mut c.buf[1..], timeout_ms)? {
                Some(n) => n + 1,
                None => return Err(io::Error::new(io::ErrorKind::TimedOut, format!("no reply to 0x{cmd:02X}"))),
            };
            if got < 5 || c.buf[1] != cmd { continue; }
            let mut r = vec![0u8; want];
            let n = want.min(got - 5);
            r[..n].copy_from_slice(&c.buf[5..5 + n]);
            return Ok(r);
        }
        Err(err(format!("unexpected replies to 0x{cmd:02X}")))
    }

    fn query(&self, cmd: u8, want: usize) -> io::Result<Vec<u8>> {
        self.request(cmd, &[], want, 1000)
    }

    pub fn serial(&self) -> io::Result<String> {
        let r = self.query(0x85, 16)?;
        Ok(String::from_utf8_lossy(&r).trim_end_matches('\0').to_owned())
    }

    pub fn device_type(&self) -> io::Result<u8> {
        Ok(self.query(0x84, 1)?[0])
    }

    /// Stored rotation in degrees (turns only the firmware's own idle/boot screens).
    pub fn rotation(&self) -> io::Result<u32> {
        Ok(match self.query(0x81, 1)?[0] { 1 => 270, 2 => 180, 3 => 90, _ => 0 })
    }

    /// Coolant temperature from the pump sensor, hundredths of a degree C (~129 C = sensor open).
    pub fn coolant_c(&self) -> io::Result<f64> {
        let r = self.query(0x82, 4)?;
        Ok(i32::from_le_bytes([r[0], r[1], r[2], r[3]]) as f64 / 100.0)
    }

    pub fn set_brightness(&self, pct: u8) -> io::Result<()> {
        self.control(0x12, &[pct.min(100)])
    }

    pub fn set_rotation(&self, degrees: i32) -> io::Result<()> {
        // degrees 0/90/180/270 -> device code (0=0, 1=270, 2=180, 3=90)
        let code = match (degrees % 360 + 360) % 360 {
            270 => 1,
            180 => 2,
            90 => 3,
            _ => 0,
        };
        self.control(0x11, &[code])
    }

    /// cmd 0x1E sets the idle-state byte (0x111): 0x02 lets the idle screen dim after a timeout, 0x00 keeps it awake.
    /// This is exactly what the vendor app (TOWEI) sends for its sleep toggle. Re-sent on each connect.
    pub fn set_sleep(&self, enabled: bool) -> io::Result<()> {
        self.control(0x1E, &[if enabled { 0x02 } else { 0x00 }])
    }

    /// Live frame: a 480x480 JPEG on MI_01 (red/blue already in the panel's order, see `jpeg_rgb` / `for_panel`).
    pub fn show_jpeg(&self, jpeg: &[u8]) -> io::Result<()> {
        Self::send(&mut self.lcd.lock().unwrap(), LCD_PACKET, LCD_PAYLOAD, 0x08, jpeg)
    }

    /// Encode a 480x480 RGB buffer for this panel (swapping red/blue on the LX800 board).
    pub fn jpeg_rgb(&self, rgb: &[u8], quality: u8) -> io::Result<Vec<u8>> {
        let mut out = Vec::with_capacity(64 * 1024);
        let ct = if self.swap_red_blue { jpeg_encoder::ColorType::Bgr } else { jpeg_encoder::ColorType::Rgb };
        jpeg_encoder::Encoder::new(&mut out, quality)
            .encode(rgb, WIDTH as u16, HEIGHT as u16, ct)
            .map_err(|e| err(e.to_string()))?;
        Ok(out)
    }

    /// Bake the panel's red/blue correction into a JPEG that will be stored or shown (and undo it on one read back).
    pub fn for_panel(&self, jpeg: &[u8]) -> Vec<u8> {
        if self.swap_red_blue { crate::render::recompress(jpeg, 90, true).unwrap_or_else(|| jpeg.to_vec()) } else { jpeg.to_vec() }
    }

    // ---- on-device storage (what the cooler shows on its own) ----

    /// cmd 0x02: list a device folder. Reply: i32 count, folder path, then NUL-separated names. Read-only.
    pub fn list_files(&self, folder: &str) -> io::Result<Vec<String>> {
        let r = self.request(0x02, &name_arg(&format!("{folder}\\"), 4), CTRL_PAYLOAD, 5000)?;
        let count = i32::from_le_bytes([r[0], r[1], r[2], r[3]]).max(0) as usize;
        let text = String::from_utf8_lossy(&r[4..]).into_owned();
        // parts[0] = folder path echoed back, then the entries
        Ok(text.split('\0').skip(1).filter(|p| !p.is_empty()).take(count).map(str::to_owned).collect())
    }

    /// LX800 firmware upload: cmd 0x0A {u32 type, u32 size} begins (reply word1 < 0 = refused); cmd 0x0B
    /// {u32 offset, u32 length, u32 percent, data...} writes each chunk. The firmware commits the file only when the
    /// percent word exceeds 99, so the final chunk must carry 100. type 0 = idle (c:\syst\wapper.jpg).
    /// Refused on the original Thermaltake firmware: its upload erased the controller's flash once (see CLAUDE.md).
    pub fn upload_file_lx(&self, file_type: i32, data: &[u8], progress: &mut dyn FnMut(i32)) -> io::Result<()> {
        if !self.swap_red_blue {
            return Err(err("uploads are disabled on the original firmware (an upload erased its flash once)"));
        }
        let mut begin = file_type.to_le_bytes().to_vec();
        begin.extend_from_slice(&(data.len() as i32).to_le_bytes());
        let br = self.request(0x0A, &begin, 12, ACK_WAIT)?;
        if i32::from_le_bytes([br[4], br[5], br[6], br[7]]) < 0 {
            return Err(err("cooler refused the upload (begin)"));
        }
        let mut off = 0;
        while off < data.len() {
            let n = UP_CHUNK.min(data.len() - off);
            let pct = ((off + n) as u64 * 100 / data.len() as u64) as i32; // 100 on the last chunk -> firmware commits
            let mut payload = Vec::with_capacity(12 + n);
            payload.extend_from_slice(&(off as i32).to_le_bytes());
            payload.extend_from_slice(&(n as i32).to_le_bytes());
            payload.extend_from_slice(&pct.to_le_bytes());
            payload.extend_from_slice(&data[off..off + n]);
            let ack = self.request(0x0B, &payload, 12, ACK_WAIT)?;
            if i32::from_le_bytes([ack[8], ack[9], ack[10], ack[11]]) < 0 {
                return Err(err(format!("upload failed at byte {off}")));
            }
            progress(pct);
            off += n;
        }
        Ok(())
    }

    /// LX800 read-back (read-only): cmd 0x0C {u32 type} opens the file and returns its size; cmd 0x0D
    /// {u32 offset, u32 length} returns that slice. Used to verify a write without a power cycle.
    pub fn read_file_lx(&self, file_type: i32, progress: &mut dyn FnMut(i32)) -> io::Result<Vec<u8>> {
        let sz = self.request(0x0C, &file_type.to_le_bytes(), 8, 5000)?;
        let size = i32::from_le_bytes([sz[4], sz[5], sz[6], sz[7]]);
        if size <= 0 {
            return Ok(Vec::new());
        }
        let size = size as usize;
        let mut out = vec![0u8; size];
        const READ_CHUNK: usize = 400; // a reply is one 436-byte control packet (minus the 12-byte header)
        let mut off = 0;
        while off < size {
            let n = READ_CHUNK.min(size - off);
            let mut req = (off as i32).to_le_bytes().to_vec();
            req.extend_from_slice(&(n as i32).to_le_bytes());
            // the 0x0D reply payload is {u32 offset, u32 length, u32 word2, data...}: data starts at byte 12
            let mut got = false;
            let mut guard = self.ctrl.lock().unwrap();
            let c = &mut *guard;
            for _ in 0..10 {
                Self::send(&mut c.dev, CTRL_PACKET, CTRL_PAYLOAD, 0x0D, &req)?;
                let r = match c.dev.read(&mut c.buf[1..], 10000)? {
                    Some(k) => k + 1,
                    None => return Err(io::Error::new(io::ErrorKind::TimedOut, "no reply to 0x0D")),
                };
                if r < 5 + 12 + n || c.buf[1] != 0x0D {
                    continue;
                }
                out[off..off + n].copy_from_slice(&c.buf[5 + 12..5 + 12 + n]);
                got = true;
                break;
            }
            drop(guard);
            if !got {
                return Err(err(format!("read-back failed at byte {off}")));
            }
            off += n;
            progress((off as u64 * 100 / size as u64) as i32);
        }
        Ok(out)
    }
}

/// u32 0 + char[260] name: the request layout of cmds 0x02 (list) and 0x03 (delete).
fn name_arg(name: &str, prefix: usize) -> Vec<u8> {
    let mut b = vec![0u8; prefix + 260];
    let s = name.as_bytes();
    let n = s.len().min(259);
    b[prefix..prefix + n].copy_from_slice(&s[..n]);
    b
}

pub const STANDBY_FILE: i32 = 0;
pub const STANDBY_MAX_LX: usize = 950 * 1024; // LX800 firmware idle-image cap (950 KB)
const ACK_WAIT: u32 = 600_000; // acks can take long (flash erase); never abandon
const UP_CHUNK: usize = 0x2800; // 10 KB data per 0x0B, as TT RGB Plus uses

/// A 480x480 RGB frame of one colour.
pub fn solid(r: u8, g: u8, b: u8) -> Vec<u8> {
    [r, g, b].repeat(WIDTH * HEIGHT)
}
