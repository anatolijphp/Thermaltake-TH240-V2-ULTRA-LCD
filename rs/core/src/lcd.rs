// Port of src/Lcd.cs: framing, queries and live frames. The file-upload commands (0x0A/0x0B) are deliberately left
// out: on the original Thermaltake firmware they erased the controller's flash (see CLAUDE.md).
use crate::hid::{self, Dev};
use std::io;

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

pub struct Lcd {
    ctrl: Dev,
    lcd: Dev,
    /// LX800 firmware board: its panel setup shows red and blue exchanged, so frames are swapped back before sending.
    pub swap_red_blue: bool,
    read_buf: [u8; CTRL_PACKET + 1],
    #[cfg(windows)]
    _owner: hid::DeviceMutex,
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
            ctrl,
            lcd,
            swap_red_blue: swap,
            read_buf: [0; CTRL_PACKET + 1],
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

    fn control(&mut self, cmd: u8, data: &[u8]) -> io::Result<()> {
        Self::send(&mut self.ctrl, CTRL_PACKET, CTRL_PAYLOAD, cmd, data)
    }

    /// Control command with a reply: one packet on MI_00 whose first byte is the same command.
    fn request(&mut self, cmd: u8, data: &[u8], want: usize, timeout_ms: u32) -> io::Result<Vec<u8>> {
        self.control(cmd, data)?;
        for _ in 0..10 {
            // read_buf[0] stands for the report-ID byte so offsets match the C# code and docs/protocol.md
            let got = match self.ctrl.read(&mut self.read_buf[1..], timeout_ms)? {
                Some(n) => n + 1,
                None => return Err(io::Error::new(io::ErrorKind::TimedOut, format!("no reply to 0x{cmd:02X}"))),
            };
            if got < 5 || self.read_buf[1] != cmd { continue; }
            let mut r = vec![0u8; want];
            let n = want.min(got - 5);
            r[..n].copy_from_slice(&self.read_buf[5..5 + n]);
            return Ok(r);
        }
        Err(err(format!("unexpected replies to 0x{cmd:02X}")))
    }

    fn query(&mut self, cmd: u8, want: usize) -> io::Result<Vec<u8>> {
        self.request(cmd, &[], want, 1000)
    }

    pub fn serial(&mut self) -> io::Result<String> {
        let r = self.query(0x85, 16)?;
        Ok(String::from_utf8_lossy(&r).trim_end_matches('\0').to_owned())
    }

    pub fn device_type(&mut self) -> io::Result<u8> {
        Ok(self.query(0x84, 1)?[0])
    }

    /// Stored rotation in degrees (turns only the firmware's own idle/boot screens).
    pub fn rotation(&mut self) -> io::Result<u32> {
        Ok(match self.query(0x81, 1)?[0] { 1 => 270, 2 => 180, 3 => 90, _ => 0 })
    }

    /// Coolant temperature from the pump sensor, hundredths of a degree C (~129 C = sensor open).
    pub fn coolant_c(&mut self) -> io::Result<f64> {
        let r = self.query(0x82, 4)?;
        Ok(i32::from_le_bytes([r[0], r[1], r[2], r[3]]) as f64 / 100.0)
    }

    pub fn set_brightness(&mut self, pct: u8) -> io::Result<()> {
        self.control(0x12, &[pct.min(100)])
    }

    /// Live frame: a 480x480 JPEG on MI_01. Callers send `jpeg_rgb` output, which already has the panel's
    /// red/blue order baked in.
    pub fn show_jpeg(&mut self, jpeg: &[u8]) -> io::Result<()> {
        Self::send(&mut self.lcd, LCD_PACKET, LCD_PAYLOAD, 0x08, jpeg)
    }

    /// Encode a 480x480 RGB buffer for this panel (swapping red/blue on the LX800 board).
    pub fn jpeg_rgb(&self, rgb: &[u8], quality: u8) -> io::Result<Vec<u8>> {
        let mut px;
        let src = if self.swap_red_blue {
            px = rgb.to_vec();
            px.chunks_exact_mut(3).for_each(|p| p.swap(0, 2));
            &px[..]
        } else {
            rgb
        };
        let mut out = Vec::with_capacity(64 * 1024);
        jpeg_encoder::Encoder::new(&mut out, quality)
            .encode(src, WIDTH as u16, HEIGHT as u16, jpeg_encoder::ColorType::Rgb)
            .map_err(|e| err(e.to_string()))?;
        Ok(out)
    }
}

/// A 480x480 RGB frame of one colour.
pub fn solid(r: u8, g: u8, b: u8) -> Vec<u8> {
    [r, g, b].repeat(WIDTH * HEIGHT)
}
