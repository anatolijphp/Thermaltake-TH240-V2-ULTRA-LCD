//! The Pump mode drawn in 3D (in progress, design stage 1: geometry, materials, lights). A small CPU renderer: the
//! impeller and chamber are triangle meshes, rasterised into a buffer of position / normal / material per pixel,
//! then each pixel is lit by the LEDs (point lights along the left-bottom-right arc) and a soft room light, with a
//! microfacet (GGX) model for metal. No GPU: it runs where the engine runs (the Linux service, CI).
//!
//! World units: the chamber's radius is 1. x right, y down (as the screen), z up out of the screen, towards the
//! camera, which looks straight down from `CAM_Z`.
use std::f32::consts::{PI, TAU};
use std::ops::{Add, Mul, Neg, Sub};
use tiny_skia::Pixmap;

const SIZE: usize = 480;
const C: f32 = SIZE as f32 / 2.0;
/// Pixels per world unit at the floor's height (the chamber's radius 1 = 238 px).
const SCALE: f32 = 238.0;
const CAM_Z: f32 = 6.0;

// geometry, world units
const BLADE_OUT: f32 = 0.68;
const HUB: f32 = 0.16;
const BLADES: usize = 7;
const BLADE_H: f32 = 0.10; // blade height above the disc
const BEVEL: f32 = 0.02; // the rounded top edge
const DISC_Z: f32 = 0.0;
const FLOOR_Z: f32 = -0.06;
/// The LEDs' height: by the cap, above the blades.
const LED_Z: f32 = 0.26;

#[derive(Clone, Copy, Debug, Default, PartialEq)]
pub struct V3 {
    pub x: f32,
    pub y: f32,
    pub z: f32,
}

const fn v3(x: f32, y: f32, z: f32) -> V3 {
    V3 { x, y, z }
}

impl Add for V3 {
    type Output = V3;
    fn add(self, o: V3) -> V3 {
        v3(self.x + o.x, self.y + o.y, self.z + o.z)
    }
}
impl Sub for V3 {
    type Output = V3;
    fn sub(self, o: V3) -> V3 {
        v3(self.x - o.x, self.y - o.y, self.z - o.z)
    }
}
impl Mul<f32> for V3 {
    type Output = V3;
    fn mul(self, k: f32) -> V3 {
        v3(self.x * k, self.y * k, self.z * k)
    }
}
impl Neg for V3 {
    type Output = V3;
    fn neg(self) -> V3 {
        v3(-self.x, -self.y, -self.z)
    }
}
impl V3 {
    fn dot(self, o: V3) -> f32 {
        self.x * o.x + self.y * o.y + self.z * o.z
    }
    fn len(self) -> f32 {
        self.dot(self).sqrt()
    }
    fn norm(self) -> V3 {
        let l = self.len();
        if l > 0.0 { self * (1.0 / l) } else { self }
    }
    fn mul(self, o: V3) -> V3 {
        v3(self.x * o.x, self.y * o.y, self.z * o.z)
    }
    /// Turned about the z axis by `a` radians (positive = clockwise on screen, y being down).
    fn turn(self, a: f32) -> V3 {
        let (s, c) = a.sin_cos();
        v3(self.x * c - self.y * s, self.x * s + self.y * c, self.z)
    }
}

#[derive(Clone, Copy, PartialEq, Debug)]
#[repr(u8)]
pub enum Mat {
    None = 0,
    /// black anodised metal: the blades, the disc
    Blade = 1,
    /// polished dark metal: the hub
    Hub = 2,
    /// the impeller's disc under the blades: darker, rougher (machined)
    Disc = 4,
    /// the blades' rounded top edges: polished, so the LEDs show on them as sharp highlights
    Edge = 5,
    /// the turbo wheel's hub: matte machined metal
    Matte = 6,
    /// bright steel: the nut / screw at the middle
    Steel = 7,
    /// the chamber floor, machined black
    Floor = 3,
}

#[derive(Clone, Copy)]
struct Vert {
    p: V3,
    n: V3,
}

#[derive(Clone, Copy)]
struct Tri {
    v: [Vert; 3],
    mat: Mat,
    /// part of the impeller (turns with it)
    turns: bool,
}

fn tri(a: Vert, b: Vert, c: Vert, mat: Mat, turns: bool) -> Tri {
    Tri { v: [a, b, c], mat, turns }
}

fn quad(out: &mut Vec<Tri>, a: Vert, b: Vert, c: Vert, d: Vert, mat: Mat, turns: bool) {
    out.push(tri(a, b, c, mat, turns));
    out.push(tri(a, c, d, mat, turns));
}

fn vert(p: V3, n: V3) -> Vert {
    Vert { p, n: n.norm() }
}

/// How the pump looks (the app's look is the default; the rest are the alternatives from the design stages).
#[derive(Clone, Copy, Debug)]
pub struct Design {
    /// which impeller
    pub wheel: Wheel,
    /// the turbo wheel: how many full blades, and whether shorter splitter blades sit between them
    pub turbo_blades: usize,
    pub splitters: bool,
    /// what sits on the nose: the bare bore, a hex nut on the shaft end, or a socket cap screw
    pub centre: Centre,
    /// the wheel made of smoked glass, and an AM5 CPU on the chamber's floor behind it (else dark metal over a
    /// dark floor)
    pub glass: bool,
    pub cpu: bool,
    /// the coolant's colour (linear 0..1): the water scatters the LEDs' light in it
    pub coolant: V3,
    /// the blades' half thickness at the hub (world units; it thins to 55 % at the tip)
    pub blade_half: f32,
    /// light points per LED colour: 1 = separate LEDs, more = a diffused strip
    pub led_spread: usize,
    /// shadow edges: higher = crisper
    pub shadow_crisp: f32,
    /// LED light scattered in the coolant
    pub water_glow: f32,
    /// how many particles are in the water, and the share of them that are bubbles
    pub particles: usize,
    pub bubbles: f32,
}

impl Default for Design {
    fn default() -> Self {
        Design { wheel: Wheel::Turbo, turbo_blades: 9, splitters: false, centre: Centre::CapScrew, glass: true, cpu: true, coolant: v3(0.0, 0.64, 1.0), blade_half: 0.045, led_spread: 3, shadow_crisp: 10.0, water_glow: 0.04, particles: 200, bubbles: 0.5 }
    }
}

/// What holds the turbo wheel on its shaft, at the middle.
#[derive(Clone, Copy, Debug, PartialEq)]
pub enum Centre {
    /// the nose's bore, open
    Bore,
    /// a hex nut with chamfered corners, the threaded shaft end standing out of it
    Nut,
    /// a round socket cap screw head with a hex socket
    CapScrew,
}

/// The impeller's shape.
#[derive(Clone, Copy, Debug, PartialEq)]
pub enum Wheel {
    /// seven backward-curved blades with rounded edges on a flat disc, a domed hub
    Open,
    /// a turbocharger compressor wheel: backplate, tall flared hub with a nose and bore, 9 twisted full blades
    /// and 9 splitters (like printables.com model 330881)
    Turbo,
}

impl Design {
    /// The LED ring's height and the water surface's: above the wheel's top.
    fn led_z(&self) -> f32 {
        if self.wheel == Wheel::Turbo { 0.86 } else { LED_Z }
    }
    /// The LED ring's radius and strength: for the big wheel further out (it nearly fills the chamber) and
    /// brighter to make up for the distance, so the light falls evenly instead of flooding the rim.
    fn led_r(&self) -> f32 {
        if self.wheel == Wheel::Turbo { 1.3 } else { 1.04 }
    }
    fn led_gain(&self) -> f32 {
        if self.wheel == Wheel::Turbo { 1.4 } else { 1.0 }
    }
    /// The room's light (from the top left). Over the CPU there is none: the LED strips are the only light.
    fn room(&self) -> V3 {
        if self.cpu { V3::default() } else { v3(0.35, 0.34, 0.32) }
    }

    fn water_top(&self) -> f32 {
        if self.wheel == Wheel::Turbo { 0.88 } else { WATER_TOP }
    }
}

/// The impeller and the chamber floor.
fn build_mesh(d: Design) -> Vec<Tri> {
    match d.wheel {
        Wheel::Open => build_open(d),
        Wheel::Turbo => build_turbo(d),
    }
}

// ---- the turbo (compressor) wheel, proportioned after a drawing: Ø140 mm, nose Ø40, bore Ø25, ~50 mm tall,
// a ~7 mm base flange; it fills 95 % of the screen (1 mm = 0.95 / 70 world units) ----
const T_MM: f32 = 0.95 / 70.0;
const T_R: f32 = 70.0 * T_MM; // outer radius
const T_NOSE: f32 = 20.0 * T_MM; // nose's outer radius
const T_BORE: f32 = 12.5 * T_MM;
const T_BASE: f32 = 7.0 * T_MM; // the base flange's top
const T_TOP: f32 = 50.0 * T_MM; // nose's top

/// The hub's surface height at radius r: a concave flare from the nose down to the backplate.
fn turbo_hub_z(r: f32) -> f32 {
    let u = ((T_R - 6.0 * T_MM - r) / (T_R - 6.0 * T_MM - T_NOSE)).clamp(0.0, 1.0);
    T_BASE + (T_TOP - T_BASE) * u.powf(2.2)
}

/// Its outward slope's normal at r.
fn turbo_hub_n(r: f32, a: f32) -> V3 {
    let e = 0.002;
    let dz = (turbo_hub_z(r + e) - turbo_hub_z(r - e)) / (2.0 * e);
    v3(-dz * a.cos(), -dz * a.sin(), 1.0).norm()
}

/// The blades' top edge height at radius r: near the nose's top at the inducer, low at the rim.
fn turbo_blade_top(r: f32) -> f32 {
    let u = ((r - T_NOSE) / (T_R - T_NOSE)).clamp(0.0, 1.0);
    T_BASE + 10.0 * T_MM + (T_TOP - 2.0 * T_MM - T_BASE - 10.0 * T_MM) * (1.0 - u).powf(1.5)
}

fn build_turbo(d: Design) -> Vec<Tri> {
    let mut m = vec![];
    let up = v3(0.0, 0.0, 1.0);
    let n = 128;
    let ring = |i: usize| (i as f32 * TAU / n as f32, (i + 1) as f32 * TAU / n as f32);
    let at = |r: f32, a: f32, z: f32| v3(r * a.cos(), r * a.sin(), z);
    // the floor round the wheel
    for i in 0..n {
        let (a0, a1) = ring(i);
        quad(&mut m, vert(at(T_R + 0.002, a0, FLOOR_Z), up), vert(at(1.1, a0, FLOOR_Z), up), vert(at(1.1, a1, FLOOR_Z), up), vert(at(T_R + 0.002, a1, FLOOR_Z), up), Mat::Floor, false);
    }
    // backplate rim, the hub's flare (rings of quads), the nose's flat top, the bore
    for i in 0..n {
        let (a0, a1) = ring(i);
        let (o0, o1) = (v3(a0.cos(), a0.sin(), 0.0), v3(a1.cos(), a1.sin(), 0.0));
        quad(&mut m, vert(at(T_R, a0, T_BASE), o0), vert(at(T_R, a0, FLOOR_Z), o0), vert(at(T_R, a1, FLOOR_Z), o1), vert(at(T_R, a1, T_BASE), o1), Mat::Disc, true);
        let rings = 28;
        for k in 0..rings {
            let (u0, u1) = (k as f32 / rings as f32, (k + 1) as f32 / rings as f32);
            let r = |u: f32| T_R - (T_R - T_NOSE) * u.powf(0.8);
            let (r0, r1) = (r(u0), r(u1));
            quad(&mut m, vert(at(r0, a0, turbo_hub_z(r0)), turbo_hub_n(r0, a0)), vert(at(r0, a1, turbo_hub_z(r0)), turbo_hub_n(r0, a1)), vert(at(r1, a1, turbo_hub_z(r1)), turbo_hub_n(r1, a1)), vert(at(r1, a0, turbo_hub_z(r1)), turbo_hub_n(r1, a0)), Mat::Matte, true);
        }
        quad(&mut m, vert(at(T_NOSE, a0, T_TOP), up), vert(at(T_NOSE, a1, T_TOP), up), vert(at(T_BORE, a1, T_TOP), up), vert(at(T_BORE, a0, T_TOP), up), Mat::Edge, true);
        quad(&mut m, vert(at(T_BORE, a0, T_TOP), -o0), vert(at(T_BORE, a1, T_TOP), -o1), vert(at(T_BORE, a1, T_TOP - 0.2), -o1), vert(at(T_BORE, a0, T_TOP - 0.2), -o0), Mat::Disc, true);
        if d.centre != Centre::Bore {
            // a washer face closing the bore under the nut / screw
            quad(&mut m, vert(at(T_BORE, a0, T_TOP + 0.0005), up), vert(at(T_BORE, a1, T_TOP + 0.0005), up), vert(at(0.0, a1, T_TOP + 0.0005), up), vert(at(0.0, a0, T_TOP + 0.0005), up), Mat::Disc, true);
        }
    }
    match d.centre {
        Centre::Bore => {}
        Centre::Nut => build_nut(&mut m),
        Centre::CapScrew => build_cap_screw(&mut m),
    }
    // blades: a sheet from its root on the hub to its top edge; backswept towards the rim (against the
    // anticlockwise turn), the inducer's top leaning forward into the turn; thin, with top and leading edges
    let blades = d.turbo_blades.max(1);
    let per = if d.splitters { 2 } else { 1 };
    for b in 0..blades * per {
        let splitter = d.splitters && b % 2 == 1;
        let a0 = b as f32 * TAU / (blades * per) as f32;
        let u_start = if splitter { 0.4 } else { 0.0 };
        let surface = |u: f32, s: f32| {
            let r = T_NOSE + 0.004 + (T_R - T_NOSE - 0.008) * u;
            let top = T_BASE + 10.0 * T_MM + (T_TOP - 2.0 * T_MM - T_BASE - 10.0 * T_MM) * (1.0 - u).powf(1.5);
            let base = turbo_hub_z(r);
            let h = (top - base).max(4.0 * T_MM);
            // the curve is all near the nose (the inducer); out towards the rim the blade runs almost radially
            let theta = a0 + 0.9 * (1.0 - (1.0 - u).powf(2.4)) - 0.6 * (1.0 - u).powf(2.2) * s;
            at(r, theta, base + s * h)
        };
        let normal = |u: f32, s: f32| {
            let e = 0.002;
            let du = surface((u + e).min(1.0), s) - surface((u - e).max(0.0), s);
            let ds = surface(u, (s + e).min(1.0)) - surface(u, (s - e).max(0.0));
            let c = v3(du.y * ds.z - du.z * ds.y, du.z * ds.x - du.x * ds.z, du.x * ds.y - du.y * ds.x);
            c.norm()
        };
        // fewer blades: each a little thicker
        let half = 0.9 * T_MM; // thin plates, ~1.8 mm
        let (nu, ns) = (32, 6);
        let uu = |i: usize| u_start + (1.0 - u_start) * i as f32 / nu as f32;
        let ss = |j: usize| j as f32 / ns as f32;
        let side = |u: f32, s: f32, k: f32| {
            let nn = normal(u, s) * k;
            vert(surface(u, s) + nn * half, nn)
        };
        for i in 0..nu {
            for j in 0..ns {
                let (u0, u1, s0, s1) = (uu(i), uu(i + 1), ss(j), ss(j + 1));
                for k in [1.0f32, -1.0] {
                    quad(&mut m, side(u0, s0, k), side(u1, s0, k), side(u1, s1, k), side(u0, s1, k), Mat::Blade, true);
                }
            }
            // the top edge, catching the light
            let (u0, u1) = (uu(i), uu(i + 1));
            let topn = |u: f32| (surface(u, 1.0) - surface(u, 0.9)).norm();
            let (p0a, p0b) = (surface(u0, 1.0) + normal(u0, 1.0) * half, surface(u0, 1.0) - normal(u0, 1.0) * half);
            let (p1a, p1b) = (surface(u1, 1.0) + normal(u1, 1.0) * half, surface(u1, 1.0) - normal(u1, 1.0) * half);
            quad(&mut m, vert(p0a, topn(u0)), vert(p1a, topn(u1)), vert(p1b, topn(u1)), vert(p0b, topn(u0)), Mat::Edge, true);
        }
        // the leading edge (towards the nose) and the trailing edge (at the rim)
        for (u, dir) in [(u_start, -1.0f32), (1.0, 1.0)] {
            for j in 0..ns {
                let (s0, s1) = (ss(j), ss(j + 1));
                let en = |s: f32| ((surface((u + 0.01).min(1.0), s) - surface((u - 0.01).max(0.0), s)) * dir).norm();
                let p = |s: f32, k: f32| surface(u, s) + normal(u, s) * (half * k);
                quad(&mut m, vert(p(s0, 1.0), en(s0)), vert(p(s1, 1.0), en(s1)), vert(p(s1, -1.0), en(s1)), vert(p(s0, -1.0), en(s0)), Mat::Edge, true);
            }
        }
    }
    m
}

/// The hexagon's radius at angle `a` for a circumradius `r` (corners at 30 deg + k * 60 deg).
fn hex_r(r: f32, a: f32) -> f32 {
    let k = (a - PI / 6.0).rem_euclid(PI / 3.0) - PI / 6.0;
    r * (PI / 6.0).cos() / k.cos()
}

/// A hex nut on the nose (13 mm to its corners, 9 mm tall, chamfered top), the shaft's threaded end (Ø10)
/// standing out of it.
fn build_nut(m: &mut Vec<Tri>) {
    let (rc, h, ch, z0) = (13.0 * T_MM, 9.0 * T_MM, 1.3 * T_MM, T_TOP);
    let at = |r: f32, a: f32, z: f32| v3(r * a.cos(), r * a.sin(), z);
    let up = v3(0.0, 0.0, 1.0);
    // the six flat sides
    for k in 0..6 {
        let (a0, a1) = (PI / 6.0 + k as f32 * PI / 3.0, PI / 6.0 + (k + 1) as f32 * PI / 3.0);
        let mid = (a0 + a1) / 2.0;
        let nrm = v3(mid.cos(), mid.sin(), 0.0);
        quad(m, vert(at(rc, a0, z0), nrm), vert(at(rc, a1, z0), nrm), vert(at(rc, a1, z0 + h - ch), nrm), vert(at(rc, a0, z0 + h - ch), nrm), Mat::Steel, true);
    }
    // the chamfer: from the hexagon up to a circle on the top face, then the flat top round the shaft
    let n = 72;
    let rt = rc * 0.86;
    for i in 0..n {
        let (a0, a1) = (i as f32 * TAU / n as f32, (i + 1) as f32 * TAU / n as f32);
        let cn = |a: f32| v3(a.cos() * 0.7, a.sin() * 0.7, 1.0).norm();
        quad(m, vert(at(hex_r(rc, a0), a0, z0 + h - ch), cn(a0)), vert(at(hex_r(rc, a1), a1, z0 + h - ch), cn(a1)), vert(at(rt, a1, z0 + h), cn(a1)), vert(at(rt, a0, z0 + h), cn(a0)), Mat::Steel, true);
        quad(m, vert(at(rt, a0, z0 + h), up), vert(at(rt, a1, z0 + h), up), vert(at(5.2 * T_MM, a1, z0 + h), up), vert(at(5.2 * T_MM, a0, z0 + h), up), Mat::Steel, true);
    }
    // the shaft end: threads (rings tilted up and down catch the light in bands), a chamfered tip
    let (rs, top) = (5.0 * T_MM, z0 + h + 5.0 * T_MM);
    let rings = 10;
    for j in 0..rings {
        let (za, zb) = (z0 + h + (top - z0 - h) * j as f32 / rings as f32, z0 + h + (top - z0 - h) * (j + 1) as f32 / rings as f32);
        let tilt = if j % 2 == 0 { 0.6 } else { -0.6 };
        for i in 0..n / 2 {
            let (a0, a1) = (i as f32 * TAU / (n / 2) as f32, (i + 1) as f32 * TAU / (n / 2) as f32);
            let tn = |a: f32| v3(a.cos(), a.sin(), tilt).norm();
            let rr = |j: usize| if j % 2 == 0 { rs } else { rs * 0.9 };
            quad(m, vert(at(rr(j), a0, za), tn(a0)), vert(at(rr(j), a1, za), tn(a1)), vert(at(rr(j + 1), a1, zb), tn(a1)), vert(at(rr(j + 1), a0, zb), tn(a0)), Mat::Steel, true);
        }
    }
    for i in 0..n / 2 {
        let (a0, a1) = (i as f32 * TAU / (n / 2) as f32, (i + 1) as f32 * TAU / (n / 2) as f32);
        let cn = |a: f32| v3(a.cos() * 0.8, a.sin() * 0.8, 1.0).norm();
        quad(m, vert(at(rs, a0, top), cn(a0)), vert(at(rs, a1, top), cn(a1)), vert(at(rs * 0.75, a1, top + 0.8 * T_MM), cn(a1)), vert(at(rs * 0.75, a0, top + 0.8 * T_MM), cn(a0)), Mat::Steel, true);
        m.push(tri(vert(at(0.0, 0.0, top + 0.8 * T_MM), up), vert(at(rs * 0.75, a0, top + 0.8 * T_MM), up), vert(at(rs * 0.75, a1, top + 0.8 * T_MM), up), Mat::Steel, true));
    }
}

/// A socket cap screw's head on the nose (Ø21, 7 mm tall, rounded top edge) with a hex socket.
fn build_cap_screw(m: &mut Vec<Tri>) {
    let (rh, h, z0, fr) = (10.5 * T_MM, 7.0 * T_MM, T_TOP, 1.6 * T_MM);
    let at = |r: f32, a: f32, z: f32| v3(r * a.cos(), r * a.sin(), z);
    let up = v3(0.0, 0.0, 1.0);
    let n = 72;
    let rs = 4.5 * T_MM; // the socket's corner radius
    for i in 0..n {
        let (a0, a1) = (i as f32 * TAU / n as f32, (i + 1) as f32 * TAU / n as f32);
        let o = |a: f32| v3(a.cos(), a.sin(), 0.0);
        quad(m, vert(at(rh, a0, z0), o(a0)), vert(at(rh, a1, z0), o(a1)), vert(at(rh, a1, z0 + h - fr), o(a1)), vert(at(rh, a0, z0 + h - fr), o(a0)), Mat::Steel, true);
        // the rounded edge: a quarter round in four steps
        for k in 0..4 {
            let (q0, q1) = (k as f32 / 4.0 * PI / 2.0, (k + 1) as f32 / 4.0 * PI / 2.0);
            let p = |q: f32, a: f32| at(rh - fr + fr * q.cos(), a, z0 + h - fr + fr * q.sin());
            let nn = |q: f32, a: f32| (o(a) * q.cos() + up * q.sin()).norm();
            quad(m, vert(p(q0, a0), nn(q0, a0)), vert(p(q0, a1), nn(q0, a1)), vert(p(q1, a1), nn(q1, a1)), vert(p(q1, a0), nn(q1, a0)), Mat::Steel, true);
        }
        // the top, round the socket
        quad(m, vert(at(rh - fr, a0, z0 + h), up), vert(at(rh - fr, a1, z0 + h), up), vert(at(hex_r(rs, a1), a1, z0 + h), up), vert(at(hex_r(rs, a0), a0, z0 + h), up), Mat::Steel, true);
    }
    // the socket: six dark walls and a dark bottom
    for k in 0..6 {
        let (a0, a1) = (PI / 6.0 + k as f32 * PI / 3.0, PI / 6.0 + (k + 1) as f32 * PI / 3.0);
        let mid = (a0 + a1) / 2.0;
        let inward = v3(-mid.cos(), -mid.sin(), 0.0);
        quad(m, vert(at(rs, a0, z0 + h), inward), vert(at(rs, a1, z0 + h), inward), vert(at(rs, a1, z0 + h - 4.0 * T_MM), inward), vert(at(rs, a0, z0 + h - 4.0 * T_MM), inward), Mat::Disc, true);
        m.push(tri(vert(at(0.0, 0.0, z0 + h - 4.0 * T_MM), up), vert(at(rs, a0, z0 + h - 4.0 * T_MM), up), vert(at(rs, a1, z0 + h - 4.0 * T_MM), up), Mat::Disc, true));
    }
}

/// The open impeller (blades with thickness and a rounded top edge, the disc, the hub dome) and the floor.
fn build_open(d: Design) -> Vec<Tri> {
    let mut m = vec![];
    let up = v3(0.0, 0.0, 1.0);
    // the floor round the impeller
    let n = 96;
    for i in 0..n {
        let (a0, a1) = (i as f32 * TAU / n as f32, (i + 1) as f32 * TAU / n as f32);
        let p = |r: f32, a: f32| v3(r * a.cos(), r * a.sin(), FLOOR_Z);
        quad(&mut m, vert(p(BLADE_OUT + 0.02, a0), up), vert(p(1.02, a0), up), vert(p(1.02, a1), up), vert(p(BLADE_OUT + 0.02, a1), up), Mat::Floor, false);
    }
    // the disc under the blades, and its rim down to the floor
    for i in 0..n {
        let (a0, a1) = (i as f32 * TAU / n as f32, (i + 1) as f32 * TAU / n as f32);
        let p = |r: f32, a: f32, z: f32| v3(r * a.cos(), r * a.sin(), z);
        m.push(tri(vert(p(0.0, 0.0, DISC_Z), up), vert(p(BLADE_OUT + 0.01, a0, DISC_Z), up), vert(p(BLADE_OUT + 0.01, a1, DISC_Z), up), Mat::Disc, true));
        let (o0, o1) = (v3(a0.cos(), a0.sin(), 0.0), v3(a1.cos(), a1.sin(), 0.0));
        quad(&mut m, vert(p(BLADE_OUT + 0.01, a0, DISC_Z), o0), vert(p(BLADE_OUT + 0.01, a0, FLOOR_Z), o0), vert(p(BLADE_OUT + 0.01, a1, FLOOR_Z), o1), vert(p(BLADE_OUT + 0.01, a1, DISC_Z), o1), Mat::Blade, true);
    }
    // blades: a centre line curving back against the anticlockwise turn, a thickness thinning to the tip; top face,
    // a rounded bevel along both top edges, the two side walls and the tip
    for b in 0..BLADES {
        let a0 = b as f32 * TAU / BLADES as f32;
        let segs = 28;
        let at = |t: f32| {
            let r = HUB - 0.01 + (BLADE_OUT - HUB + 0.01) * t;
            let a = a0 + 1.05 * t.max(0.0).powf(0.8);
            let half = d.blade_half * (1.0 - 0.45 * t); // half thickness, world units
            (r, a, half)
        };
        // points across the blade at parameter t: side offset s in -1..1 (lead .. trail), height z
        let point = |t: f32, s: f32, z: f32| {
            let (r, a, half) = at(t);
            let centre = v3(r * a.cos(), r * a.sin(), 0.0);
            let along = {
                let (r2, a2, _) = at((t + 0.01).min(1.0));
                let (r1, a1, _) = at((t - 0.01).max(0.0));
                (v3(r2 * a2.cos(), r2 * a2.sin(), 0.0) - v3(r1 * a1.cos(), r1 * a1.sin(), 0.0)).norm()
            };
            let side = v3(-along.y, along.x, 0.0); // horizontal, across the blade
            (centre + side * (s * half) + v3(0.0, 0.0, z), side)
        };
        // the cross-section: (s, z, normal(side)) from the lead wall over the rounded top to the trail wall
        let section = |side: V3| -> Vec<(f32, f32, V3)> {
            let mut s = vec![(-1.0, DISC_Z, -side)];
            let steps = 4;
            for k in 0..=steps {
                // quarter round from the wall to the top
                let q = k as f32 / steps as f32 * PI / 2.0;
                s.push((-1.0 + BEVEL / d.blade_half * (1.0 - q.cos()), BLADE_H - BEVEL + BEVEL * q.sin(), (-side * q.cos() + up * q.sin()).norm()));
            }
            for k in 0..=steps {
                let q = k as f32 / steps as f32 * PI / 2.0;
                s.push((1.0 - BEVEL / d.blade_half * (1.0 - q.sin()), BLADE_H - BEVEL + BEVEL * q.cos(), (side * q.sin() + up * q.cos()).norm()));
            }
            s.push((1.0, DISC_Z, side));
            s
        };
        for i in 0..segs {
            let (t0, t1) = (i as f32 / segs as f32, (i + 1) as f32 / segs as f32);
            let (_, side0) = point(t0, 0.0, 0.0);
            let (_, side1) = point(t1, 0.0, 0.0);
            let (s0, s1) = (section(side0), section(side1));
            for k in 0..s0.len() - 1 {
                let v = |t: f32, sec: &[(f32, f32, V3)], k: usize| vert(point(t, sec[k].0, sec[k].1).0, sec[k].2);
                // the section: 0 lead wall, 1-4 lead bevel, 5 top, 6-9 trail bevel, 10 trail wall
                let mat = if (1..=4).contains(&k) || (6..=9).contains(&k) { Mat::Edge } else { Mat::Blade };
                quad(&mut m, v(t0, &s0, k), v(t1, &s1, k), v(t1, &s1, k + 1), v(t0, &s0, k + 1), mat, true);
            }
        }
        // the tip: a wall across the outer end
        let (_, side) = point(1.0, 0.0, 0.0);
        let sec = section(side);
        let (r, a, _) = at(1.0);
        let out = v3(a.cos(), a.sin(), 0.0) * r;
        let tipn = out.norm();
        let centre = vert(point(1.0, 0.0, BLADE_H * 0.5).0, tipn);
        for k in 0..sec.len() - 1 {
            let p0 = vert(point(1.0, sec[k].0, sec[k].1).0, tipn);
            let p1 = vert(point(1.0, sec[k + 1].0, sec[k + 1].1).0, tipn);
            m.push(tri(centre, p0, p1, Mat::Blade, true));
        }
    }
    // the hub: a dome rising from the disc above the blades
    let (rings, segs) = (10, 48);
    let dome = |u: f32, a: f32| {
        // u 0 = rim, 1 = top; a flattened sphere
        let phi = u * PI / 2.0;
        let r = HUB * phi.cos();
        let z = DISC_Z + 0.04 + (BLADE_H + 0.06) * phi.sin();
        let n = v3(phi.cos() * a.cos() / HUB, phi.cos() * a.sin() / HUB, phi.sin() / (BLADE_H + 0.06));
        vert(v3(r * a.cos(), r * a.sin(), z), n)
    };
    for i in 0..rings {
        for j in 0..segs {
            let (u0, u1) = (i as f32 / rings as f32, (i + 1) as f32 / rings as f32);
            let (a0, a1) = (j as f32 * TAU / segs as f32, (j + 1) as f32 * TAU / segs as f32);
            quad(&mut m, dome(u0, a0), dome(u0, a1), dome(u1, a1), dome(u1, a0), Mat::Hub, true);
        }
    }
    // the dome's skirt down to the disc
    for j in 0..segs {
        let (a0, a1) = (j as f32 * TAU / segs as f32, (j + 1) as f32 * TAU / segs as f32);
        let o = |a: f32| v3(a.cos(), a.sin(), 0.0);
        let p = |a: f32, z: f32| v3(HUB * a.cos(), HUB * a.sin(), z);
        quad(&mut m, vert(p(a0, DISC_Z), o(a0)), vert(p(a1, DISC_Z), o(a1)), vert(p(a1, DISC_Z + 0.04), o(a1)), vert(p(a0, DISC_Z + 0.04), o(a0)), Mat::Hub, true);
    }
    m
}

/// Per pixel after rasterising: what is there, where, facing where.
#[derive(Clone, Copy)]
struct GPix {
    depth: f32,
    pos: V3,
    nrm: V3,
    mat: Mat,
}

struct GBuf {
    px: Vec<GPix>,
}

/// Runs `f(first index, chunk)` over `data` in chunks of `chunk` items on all cores, the chunks handed out one by
/// one (the impeller's rows cost far more than the floor's, so fixed bands would leave cores idle).
fn par_chunks<T: Send>(data: &mut [T], chunk: usize, f: impl Fn(usize, &mut [T]) + Sync) {
    use std::sync::atomic::{AtomicUsize, Ordering};
    use std::sync::Mutex;
    let chunks: Vec<Mutex<&mut [T]>> = data.chunks_mut(chunk).map(Mutex::new).collect();
    let next = AtomicUsize::new(0);
    let threads = std::thread::available_parallelism().map_or(4, |n| n.get()).min(16);
    std::thread::scope(|sc| {
        for _ in 0..threads {
            sc.spawn(|| loop {
                let i = next.fetch_add(1, Ordering::Relaxed);
                let Some(c) = chunks.get(i) else { break };
                f(i * chunk, &mut c.lock().unwrap());
            });
        }
    });
}

/// How far the view may tilt from straight down (degrees, any direction).
pub const MAX_TILT: f32 = 30.0;
/// The height the tilted view turns about: the middle of the wheel, so the wheel stays in the middle.
const PIVOT_Z: f32 = 0.25;

/// The camera: at `eye`, looking along `fwd`, the picture's right and down along `right` / `down`; `f` = pixels
/// per unit of sideways offset over distance (straight down: `SCALE` px per world unit at the floor).
#[derive(Clone, Copy, Debug)]
struct Camera {
    eye: V3,
    right: V3,
    down: V3,
    fwd: V3,
    f: f32,
}

/// The view's tilt kept within `MAX_TILT` (the direction stays).
pub fn clamp_tilt(tx: f32, ty: f32) -> (f32, f32) {
    let m = tx.hypot(ty);
    if m > MAX_TILT { (tx * MAX_TILT / m, ty * MAX_TILT / m) } else { (tx, ty) }
}

impl Camera {
    /// Straight down from `CAM_Z`.
    fn top() -> Camera {
        Camera::tilted(0.0, 0.0)
    }

    /// Looking at the wheel from `tx` degrees towards the picture's right and `ty` degrees towards its bottom (the
    /// eye moves that way round the middle of the wheel and looks back at it; together at most `MAX_TILT`).
    fn tilted(tx: f32, ty: f32) -> Camera {
        let (tx, ty) = clamp_tilt(tx, ty);
        let (a, b) = (tx.to_radians(), ty.to_radians());
        // about the x axis (the eye towards +y), then about the y axis (towards +x)
        let rot = |v: V3| {
            let v = v3(v.x, v.y * b.cos() + v.z * b.sin(), -v.y * b.sin() + v.z * b.cos());
            v3(v.x * a.cos() + v.z * a.sin(), v.y, -v.x * a.sin() + v.z * a.cos())
        };
        Camera {
            eye: v3(0.0, 0.0, PIVOT_Z) + rot(v3(0.0, 0.0, CAM_Z - PIVOT_Z)),
            right: rot(v3(1.0, 0.0, 0.0)),
            down: rot(v3(0.0, 1.0, 0.0)),
            fwd: rot(v3(0.0, 0.0, -1.0)),
            f: SCALE * (CAM_Z - FLOOR_Z),
        }
    }

    /// Where world point `p` lands on the screen (x, y px) and how far in front of the eye it is.
    fn project(&self, p: V3) -> (f32, f32, f32) {
        let d = p - self.eye;
        let w = d.dot(self.fwd);
        (C + self.f * d.dot(self.right) / w, C + self.f * d.dot(self.down) / w, w)
    }

    /// The world point at height `z` seen at screen point (x, y).
    fn at_height(&self, x: f32, y: f32, z: f32) -> V3 {
        let dir = self.fwd + self.right * ((x - C) / self.f) + self.down * ((y - C) / self.f);
        self.eye + dir * ((z - self.eye.z) / dir.z)
    }
}

/// Rasterises the mesh (the impeller's parts turned by `angle`) as `cam` sees it into the G-buffer, nearest
/// surface wins; each core takes bands of rows and draws every triangle's part in them.
fn rasterise(mesh: &[Tri], angle: f32, cam: &Camera) -> GBuf {
    struct Ready {
        vs: [Vert; 3],
        s: [(f32, f32, f32); 3],
        area: f32,
        bbox: (i32, i32, i32, i32),
        mat: Mat,
    }
    let ready: Vec<Ready> = mesh
        .iter()
        .filter_map(|t| {
            let vs = t.v.map(|v| if t.turns { Vert { p: v.p.turn(-angle), n: v.n.turn(-angle) } } else { v });
            let s = vs.map(|v| cam.project(v.p));
            let area = (s[1].0 - s[0].0) * (s[2].1 - s[0].1) - (s[2].0 - s[0].0) * (s[1].1 - s[0].1);
            if area.abs() < 1e-6 {
                return None;
            }
            let lo = |f: fn(&(f32, f32, f32)) -> f32| s.iter().map(f).fold(f32::MAX, f32::min).floor().max(0.0) as i32;
            let hi = |f: fn(&(f32, f32, f32)) -> f32| s.iter().map(f).fold(f32::MIN, f32::max).ceil().min(SIZE as f32 - 1.0) as i32;
            Some(Ready { vs, s, area, bbox: (lo(|q| q.0), hi(|q| q.0), lo(|q| q.1), hi(|q| q.1)), mat: t.mat })
        })
        .collect();
    let empty = GPix { depth: f32::INFINITY, pos: V3::default(), nrm: V3::default(), mat: Mat::None };
    let mut px = vec![empty; SIZE * SIZE];
    const ROWS: usize = 8;
    par_chunks(&mut px, ROWS * SIZE, |first, band| {
        let (y0, y1) = ((first / SIZE) as i32, (first / SIZE + ROWS - 1) as i32);
        for t in &ready {
            let (minx, maxx, miny, maxy) = t.bbox;
            let s = &t.s;
            for y in miny.max(y0)..=maxy.min(y1) {
                for x in minx..=maxx {
                    let (fx, fy) = (x as f32 + 0.5, y as f32 + 0.5);
                    let w0 = ((s[1].0 - fx) * (s[2].1 - fy) - (s[2].0 - fx) * (s[1].1 - fy)) / t.area;
                    let w1 = ((s[2].0 - fx) * (s[0].1 - fy) - (s[0].0 - fx) * (s[2].1 - fy)) / t.area;
                    let w2 = 1.0 - w0 - w1;
                    if w0 < 0.0 || w1 < 0.0 || w2 < 0.0 {
                        continue;
                    }
                    // perspective-correct weights
                    let (q0, q1, q2) = (w0 / s[0].2, w1 / s[1].2, w2 / s[2].2);
                    let qs = q0 + q1 + q2;
                    let depth = 1.0 / qs;
                    let i = (y - y0) as usize * SIZE + x as usize;
                    if depth >= band[i].depth {
                        continue;
                    }
                    let (k0, k1, k2) = (q0 / qs, q1 / qs, q2 / qs);
                    let vs = &t.vs;
                    band[i] = GPix { depth, pos: vs[0].p * k0 + vs[1].p * k1 + vs[2].p * k2, nrm: (vs[0].n * k0 + vs[1].n * k1 + vs[2].n * k2).norm(), mat: t.mat };
                }
            }
        }
    });
    GBuf { px }
}

/// A light: LEDs are points just outside the chamber; the room light is far above, from the top left.
#[derive(Clone, Copy)]
pub struct Light {
    /// which LED colour it belongs to (its shadow is worked out once per LED)
    pub group: usize,
    pub pos: V3,
    /// linear RGB, 0..1 = a bright LED
    pub colour: V3,
    pub point: bool,
}

/// The LEDs: `colours` (0..255 RGB) spread from the upper left, round the bottom, to the upper right; each colour
/// as `spread` points close together (a diffused strip rather than separate dots), the strip's light shared out.
pub fn led_lights(colours: &[(f32, f32, f32)], spread: usize, led_z: f32, led_r: f32, gain: f32) -> Vec<Light> {
    let up = PI / 6.0;
    let spread = spread.max(1);
    let n = (colours.len() * spread).max(2);
    let lin = |v: f32| (v / 255.0).powf(2.2);
    (0..n)
        .map(|i| {
            let (r, g, b) = colours[(i / spread).min(colours.len() - 1)];
            let a = PI + up - (PI + 2.0 * up) * i as f32 / (n - 1) as f32;
            let k = gain / spread as f32;
            // the LED ring sits up by the cap, above the blades, shining down and in
            Light { group: i / spread, pos: v3(led_r * a.cos(), led_r * a.sin(), led_z), colour: v3(lin(r), lin(g), lin(b)) * k, point: true }
        })
        .collect()
}

#[derive(Clone, Copy)]
struct Material {
    albedo: V3,
    rough: f32,
    metal: f32,
}

fn material(m: Mat) -> Material {
    match m {
        Mat::Blade => Material { albedo: v3(0.05, 0.05, 0.055), rough: 0.26, metal: 1.0 },
        Mat::Edge => Material { albedo: v3(0.05, 0.05, 0.055), rough: 0.15, metal: 1.0 },
        Mat::Disc => Material { albedo: v3(0.01, 0.01, 0.011), rough: 0.7, metal: 0.5 },
        Mat::Hub => Material { albedo: v3(0.06, 0.06, 0.065), rough: 0.18, metal: 1.0 },
        // metal above 1: a brighter reflection than the dark anodised parts (bare steel)
        Mat::Steel => Material { albedo: v3(0.1, 0.1, 0.1), rough: 0.24, metal: 1.35 },
        Mat::Matte => Material { albedo: v3(0.03, 0.03, 0.033), rough: 0.46, metal: 0.45 },
        Mat::Floor => Material { albedo: v3(0.006, 0.006, 0.007), rough: 0.85, metal: 0.3 },
        Mat::None => Material { albedo: v3(0.0, 0.0, 0.0), rough: 1.0, metal: 0.0 },
    }
}

/// Cook-Torrance with GGX: the light a surface sends to the eye.
fn brdf(n: V3, v: V3, l: V3, mat: &Material) -> V3 {
    let h = (v + l).norm();
    let (nl, nv, nh, vh) = (n.dot(l).max(0.0), n.dot(v).max(1e-4), n.dot(h).max(0.0), v.dot(h).max(0.0));
    if nl <= 0.0 {
        return V3::default();
    }
    let a = mat.rough * mat.rough;
    let a2 = a * a;
    let d = a2 / (PI * (nh * nh * (a2 - 1.0) + 1.0).powi(2));
    let k = (mat.rough + 1.0).powi(2) / 8.0;
    let g = (nv / (nv * (1.0 - k) + k)) * (nl / (nl * (1.0 - k) + k));
    // metals reflect in their own (dark grey) colour; at grazing angles everything reflects more (Fresnel)
    let f0 = v3(0.04, 0.04, 0.04) * (1.0 - mat.metal) + v3(0.55, 0.56, 0.58) * mat.metal;
    let f = f0 + (v3(1.0, 1.0, 1.0) - f0) * (1.0 - vh).powi(5);
    let spec = f * (d * g / (4.0 * nv * nl + 1e-4));
    let diffuse = mat.albedo * ((1.0 - mat.metal).max(0.0) / PI);
    (diffuse + spec) * nl
}

/// The topmost surface's height under world point `q` (the scene seen from above is a height field: `g` is
/// rasterised straight down, whatever the view), None outside the screen.
fn height_at(g: &GBuf, q: V3) -> Option<f32> {
    let (x, y, _) = Camera::top().project(v3(q.x, q.y, 0.0));
    if x < 0.0 || y < 0.0 || x >= SIZE as f32 || y >= SIZE as f32 {
        return None;
    }
    let i = y as usize * SIZE + x as usize;
    Some(if g.px[i].mat == Mat::None { FLOOR_Z } else { g.px[i].pos.z })
}

/// How much of the light at `to` reaches `p` (0..1): a march over the height field towards it; nearly blocked
/// rays give a soft edge (`crisp` sets how soft).
fn shadow(g: &GBuf, p: V3, n: V3, to: V3, crisp: f32) -> f32 {
    let d = to - p;
    let dist = d.len();
    let dir = d * (1.0 / dist);
    // start a little way out: the height field sees each blade as a solid column, so a ray leaving a lit edge
    // would otherwise "hit" the blade it starts on
    let start = p + n * 0.008 + dir * 0.025;
    let reach = (dist - 0.025).clamp(0.0, 0.7);
    let mut vis = 1.0f32;
    let steps = 14;
    for s in 1..=steps {
        let t = reach * (s as f32 / steps as f32).powf(1.4);
        let q = start + dir * t;
        if let Some(h) = height_at(g, q) {
            vis = vis.min(((q.z - h + 0.004) * crisp / t.max(0.02) + 0.5).clamp(0.0, 1.0));
            if vis <= 0.0 {
                break;
            }
        }
    }
    vis
}

/// Ambient occlusion: how open the sky above `p` is (corners where a blade meets the disc are darker).
fn openness(g: &GBuf, p: V3) -> f32 {
    let mut open = 0.0;
    let dirs = 8;
    for k in 0..dirs {
        let a = k as f32 * TAU / dirs as f32;
        for r in [0.025f32, 0.06] {
            let q = p + v3(a.cos() * r, a.sin() * r, 0.0);
            let rise = height_at(g, q).map_or(0.0, |h| (h - p.z).max(0.0));
            open += 1.0 - (rise / r * 0.8).min(1.0);
        }
    }
    open / (dirs * 2) as f32
}

/// How strongly the whole chamber's water scatters the LEDs' light in the coolant's colour.
const WATER_BODY: f32 = 0.02;
/// The glow's share over the glass wheel and over the floor.
const WATER_GLASS: f32 = 0.6;
const WATER_FLOOR: f32 = 0.5;

/// The water's surface under the cap (the glow is light scattered in the water above each point).
const WATER_TOP: f32 = 0.24;

// ---------------- the AM5 CPU on the floor ----------------

/// The package's scale: its 40 mm substrate spans 95 % of the image edge to edge (bigger than the wheel).
const CPU_K: f32 = 0.95 * SIZE as f32 / SCALE / 40.0;

/// Signed distance (mm, negative inside) to a rounded box centred at (cx, cy) with half sizes (hx, hy).
fn rbox(x: f32, y: f32, cx: f32, cy: f32, hx: f32, hy: f32, r: f32) -> f32 {
    let (qx, qy) = ((x - cx).abs() - hx + r, (y - cy).abs() - hy + r);
    (qx.max(0.0).hypot(qy.max(0.0))) + qx.max(qy).min(0.0) - r
}

/// The AM5 heat spreader's outline (mm from the package's centre, y down; signed distance): the body, a tab in
/// the middle of each side and the four corner pads, with the capacitor bays cut between them (Ryzen 7000/9000).
fn ihs_sdf(x: f32, y: f32) -> f32 {
    let mut d = rbox(x, y, 0.0, 0.0, 14.0, 16.2, 0.6);
    d = d.min(rbox(x, y, 0.0, 0.0, 1.8, 17.8, 0.5)).min(rbox(x, y, 0.0, 0.0, 17.6, 3.6, 0.5));
    for (sx, sy) in [(-1.0, -1.0), (1.0, -1.0), (-1.0, 1.0), (1.0, 1.0)] {
        d = d.min(rbox(x, y, sx * 13.9, sy * 15.2, 2.5, 2.6, 1.4));
    }
    d
}

fn hash1(k: f32) -> f32 {
    ((k * 12.9898).sin() * 43758.547).fract().abs()
}

/// What the floor shows at world point (x, y) with the CPU there: its material and surface normal, and how much
/// of the room light reaches it (the heat spreader shades the substrate beside it).
fn cpu_surface(x: f32, y: f32) -> (Material, V3, f32) {
    let (mx, my) = (x / CPU_K, y / CPU_K); // millimetres from the centre
    let up = v3(0.0, 0.0, 1.0);
    let black = Material { albedo: v3(0.0008, 0.0008, 0.0009), rough: 1.0, metal: 0.0 };
    // the substrate: a square with a half-round notch in the middle of the top and bottom edges
    let sub = rbox(mx, my, 0.0, 0.0, 20.0, 20.0, 0.5).max(-(mx.hypot(my.abs() - 20.0) - 0.9));
    if sub > 0.0 {
        return (black, up, 1.0);
    }
    let s = ihs_sdf(mx, my);
    if s < 0.0 {
        // nickel, brushed left to right; the edge is rounded off over its last 0.4 mm
        let e = 0.01;
        let (gx, gy) = ((ihs_sdf(mx + e, my) - s) / e, (ihs_sdf(mx, my + e) - s) / e);
        let t = ((s + 0.4) / 0.4).clamp(0.0, 1.0).powi(2) * 2.5;
        let n = v3(gx * t, gy * t, 1.0).norm();
        let streak = 0.88 + 0.12 * hash1((my * 30.0).floor()) * (0.7 + 0.3 * hash1((mx * 0.8).floor() + (my * 30.0).floor() * 7.0));
        return (Material { albedo: v3(0.6, 0.6, 0.62) * streak, rough: 0.28, metal: 1.0 }, n, 1.0);
    }
    // the spreader stands ~1 mm proud: its shadow falls toward the bottom right, and it darkens the bays
    let shade = ((ihs_sdf(mx + 0.55, my + 0.65) - 0.0) / 0.5).clamp(0.0, 1.0);
    let open = 0.45 + 0.55 * (s / 1.2).clamp(0.0, 1.0);
    // the gold triangle in the top left corner
    if (mx + 20.0) + (my + 20.0) < 2.2 {
        return (Material { albedo: v3(0.85, 0.62, 0.25), rough: 0.35, metal: 0.6 }, up, shade * open);
    }
    // capacitors: 4 x 4 in the side bays, 5 x 3 in the bottom bays (none at the top)
    let mut cap = None;
    if mx.abs() > 14.4 && mx.abs() < 17.8 && my.abs() > 5.4 && my.abs() < 9.8 {
        cap = Some(((mx.abs() - 14.4) / 0.85, (my.abs() - 5.4) / 1.1));
    } else if my > 14.0 && my < 17.8 && mx.abs() > 4.2 && mx.abs() < 9.4 {
        cap = Some(((mx.abs() - 4.2) / 1.04, (my - 14.0) / 1.27));
    }
    if let Some((u, v)) = cap {
        let (fu, fv) = (u.fract(), v.fract());
        if fu > 0.12 && fu < 0.88 && fv > 0.18 && fv < 0.82 {
            let end = fu < 0.3 || fu > 0.7;
            let m = if end { Material { albedo: v3(0.7, 0.7, 0.72), rough: 0.3, metal: 1.0 } } else { Material { albedo: v3(0.42, 0.32, 0.22), rough: 0.5, metal: 0.0 } };
            return (m, up, shade * open);
        }
    }
    // dark teal fibreglass, a lighter band along the cut edge
    let rim = (-sub < 0.3) as u8 as f32;
    let speck = 0.85 + 0.3 * hash1((mx * 9.0).floor() * 31.0 + (my * 9.0).floor());
    let albedo = v3(0.012, 0.05, 0.048) * speck * (1.0 - rim) + v3(0.05, 0.16, 0.15) * rim;
    (Material { albedo, rough: 0.55, metal: 0.0 }, up, shade * open)
}

/// The laser etching on the heat spreader, as a mask over the screen (the floor doesn't move): "AMD" with the
/// arrow logo and a spaced-out "RYZEN", left-aligned, a small ring in the top left pad.
fn cpu_lettering() -> &'static [f32] {
    static MASK: std::sync::OnceLock<Vec<f32>> = std::sync::OnceLock::new();
    MASK.get_or_init(|| {
        use tiny_skia::{FillRule, Paint, PathBuilder, Shader, Transform};
        let mut p = Pixmap::new(SIZE as u32, SIZE as u32).unwrap();
        let k = CPU_K * SCALE; // pixels per millimetre on the floor
        let at = |mx: f32, my: f32| (C + mx * k, C + my * k);
        let f = crate::render::text::fonts();
        let paint = Paint { shader: Shader::SolidColor(tiny_skia::Color::WHITE), anti_alias: true, ..Default::default() };
        // text spread to a width: (text, cap height, left, cap top, width), all mm
        for (text, cap, left, top, width) in [("AMD", 4.8f32, -11.4f32, -5.4f32, 16.4f32), ("RYZEN", 3.2, -11.4, 1.8, 19.8)] {
            let size = cap / 0.73 * k;
            let chars: Vec<String> = text.chars().map(|c| c.to_string()).collect();
            let ws: Vec<f32> = chars.iter().map(|c| f.width("DejaVu Sans", true, size, c)).collect();
            let gap = ((width * k - ws.iter().sum::<f32>()) / (chars.len() - 1) as f32).max(0.0);
            let (mut x, y) = at(left, top);
            for (c, w) in chars.iter().zip(&ws) {
                if let Some(path) = f.path("DejaVu Sans", true, false, size, x, y - 0.2 * size, c) {
                    p.fill_path(&path, &paint, FillRule::Winding, Transform::identity(), None);
                }
                x += w + gap;
            }
        }
        // the AMD arrow: an L along the top and right with cut ends, and the inner wedge
        let (lx, ly, sz) = (6.0f32, -5.6f32, 5.2f32);
        let poly = |pts: &[(f32, f32)]| {
            let mut b = PathBuilder::new();
            for (i, &(u, v)) in pts.iter().enumerate() {
                let (x, y) = at(lx + u * sz, ly + v * sz);
                if i == 0 { b.move_to(x, y) } else { b.line_to(x, y) }
            }
            b.close();
            b.finish()
        };
        for pts in [&[(0.0, 0.0), (1.0, 0.0), (1.0, 1.0), (0.74, 0.74), (0.74, 0.26), (0.26, 0.26)][..], &[(0.2, 0.38), (0.62, 0.38), (0.62, 0.8), (0.2, 0.8)][..]] {
            if let Some(path) = poly(pts) {
                p.fill_path(&path, &paint, FillRule::Winding, Transform::identity(), None);
            }
        }
        // the ring in the top left pad
        let (rx, ry) = at(-14.8, -14.8);
        if let Some(c) = PathBuilder::from_circle(rx, ry, 0.5 * k) {
            p.stroke_path(&c, &paint, &tiny_skia::Stroke { width: 0.12 * k, ..Default::default() }, Transform::identity(), None);
        }
        p.pixels().iter().map(|c| c.alpha() as f32 / 255.0).collect()
    })
}

/// A floor pixel as the camera sees it: where, what, facing where, how much room light reaches it.
#[derive(Clone, Copy)]
struct FloorPx {
    p: V3,
    m: Material,
    n: V3,
    lit: f32,
}

/// The floor's response to each LED for a view: per pixel and LED, the light it sends to the eye for that LED at
/// unit colour (light adds up, so a frame's floor is these times the LEDs' colours: the expensive part is done
/// once per view and LED layout, not every frame).
fn floor_response(camera: &Camera, d: Design, merged: &[Light]) -> std::sync::Arc<Vec<V3>> {
    use std::sync::{Arc, Mutex};
    type Key = (u32, u32, u32, bool, Vec<u32>);
    static CACHE: Mutex<Option<(Key, Arc<Vec<V3>>)>> = Mutex::new(None);
    let key = (camera.eye.x.to_bits(), camera.eye.y.to_bits(), camera.eye.z.to_bits(), d.cpu, merged.iter().flat_map(|l| [l.pos.x.to_bits(), l.pos.y.to_bits(), l.pos.z.to_bits()]).collect());
    if let Some((k, v)) = CACHE.lock().unwrap().as_ref() {
        if *k == key {
            return v.clone();
        }
    }
    let floor = floor_view(camera, d.cpu);
    let groups = merged.len();
    let mut resp = vec![V3::default(); SIZE * SIZE * groups];
    par_chunks(&mut resp, 8 * SIZE * groups, |first, chunk| {
        for (k, cell) in chunk.chunks_mut(groups).enumerate() {
            let FloorPx { p, m, n, .. } = floor[first / groups + k];
            if m.albedo.x < 0.001 {
                continue; // round the CPU: black
            }
            let v = (camera.eye - p).norm();
            let r = n * (2.0 * n.dot(v)) - v;
            let across = (0.8 - (p.x + p.y) * 0.8).clamp(0.3, 1.3);
            for (gi, l) in merged.iter().enumerate() {
                let unit = Light { colour: v3(1.0, 1.0, 1.0), ..*l };
                let dl = l.pos - p;
                let mut c = brdf(n, v, dl.norm(), &m) * (0.9 / (0.05 + dl.dot(dl) * 2.2));
                if m.metal >= 1.0 {
                    // polished metal mirrors the LED strips round it, falling off across the face
                    c = c + env_leds(r, p, std::slice::from_ref(&unit)).mul(m.albedo) * (0.3 * across);
                }
                cell[gi] = c;
            }
        }
    });
    let v = Arc::new(resp);
    *CACHE.lock().unwrap() = Some((key, v.clone()));
    v
}

/// The floor (the CPU or the dark chamber floor) per pixel for a view: the same every frame while the view stays,
/// so it is worked out once and kept.
fn floor_view(camera: &Camera, cpu: bool) -> std::sync::Arc<Vec<FloorPx>> {
    use std::sync::{Arc, Mutex};
    type Key = (u32, u32, u32, bool);
    static CACHE: Mutex<Option<(Key, Arc<Vec<FloorPx>>)>> = Mutex::new(None);
    let key = (camera.eye.x.to_bits(), camera.eye.y.to_bits(), camera.eye.z.to_bits(), cpu);
    if let Some((k, v)) = CACHE.lock().unwrap().as_ref() {
        if *k == key {
            return v.clone();
        }
    }
    let up = v3(0.0, 0.0, 1.0);
    let mut px = vec![FloorPx { p: V3::default(), m: material(Mat::None), n: up, lit: 0.0 }; SIZE * SIZE];
    let letters = if cpu { Some(cpu_lettering()) } else { None };
    par_chunks(&mut px, 8 * SIZE, |first, chunk| {
        for (k, f) in chunk.iter_mut().enumerate() {
            let i = first + k;
            let p = camera.at_height((i % SIZE) as f32 + 0.5, (i / SIZE) as f32 + 0.5, FLOOR_Z);
            let (mut m, mut n, lit) = if cpu { cpu_surface(p.x, p.y) } else { (material(Mat::Floor), up, 1.0) };
            // the lettering is drawn on the floor as seen straight down
            let (lx, ly) = ((p.x * SCALE + C) as usize, (p.y * SCALE + C) as usize);
            if let (Some(l), true) = (letters, m.metal >= 1.0 && lx > 0 && ly > 0 && lx < SIZE - 1 && ly < SIZE - 1) {
                let li = ly * SIZE + lx;
                if l[li] > 0.0 {
                    // etched: darker and matt, its walls tilting the surface (lit on one side, dark on the other)
                    m.albedo = m.albedo * (1.0 - 0.5 * l[li]);
                    m.rough = 0.55;
                }
                let (gx, gy) = (l[li + 1] - l[li - 1], l[li + SIZE] - l[li - SIZE]);
                n = (n + v3(gx, gy, 0.0) * 0.25).norm();
            }
            *f = FloorPx { p, m, n, lit };
        }
    });
    let v = Arc::new(px);
    *CACHE.lock().unwrap() = Some((key, v.clone()));
    v
}

/// What the surroundings look like along direction `r` from a glossy surface at `p`: the bright window the room
/// light comes from, a dim overhead, and the LEDs round the pump head (each a coloured patch low on the horizon).
fn env_leds(r: V3, p: V3, leds: &[Light]) -> V3 {
    env(r, p, V3::default(), v3(0.0, 0.0, 1.0), leds)
}

fn env(r: V3, p: V3, room: V3, room_dir: V3, leds: &[Light]) -> V3 {
    let window = (-((r - room_dir).dot(r - room_dir)) / 0.03).exp();
    let mut c = room * (0.25 * r.z.max(0.0).powi(2) + 4.0 * window);
    for l in leds {
        let to = (l.pos - p).norm();
        c = c + l.colour * (1.4 * (-((r - to).dot(r - to)) / 0.012).exp());
    }
    c
}

/// Each pixel as what shows through it times the floor behind it, plus its own light (`floor * through + own`,
/// linear; the opaque parts let nothing through). The motion blur averages these, so the floor stays sharp under
/// a spinning glass wheel; only the glass's darkening, bending and highlights smear.
struct Layers {
    floor: Vec<V3>,
    through: Vec<V3>,
    own: Vec<V3>,
}

/// Lights every pixel of `g` (as `camera` sees it) and tone-maps to the screen; `hf` is the scene rasterised
/// straight down, the height field for shadows. Shadows and ambient occlusion are soft, so they are worked out at
/// half resolution and interpolated; the water's glow depends only on where a point is, so it is a small map.
/// With `layered` (or glass) it also returns the layers for the motion blur.
/// `fill`: a faint light from all round in the LEDs' last colour, so the scene never goes fully dark.
#[allow(clippy::too_many_arguments)]
fn shade(g: &GBuf, hf: &GBuf, camera: &Camera, lights: &[Light], room: V3, fill: V3, d: Design, layered: bool) -> (Pixmap, Option<Layers>) {
    let cam = camera.eye;
    let room_dir = v3(-0.45, -0.55, 1.0).norm();
    let groups = lights.iter().map(|l| l.group).max().map_or(0, |m| m + 1);
    // each LED's centre (its shadow is worked out from there)
    let centres: Vec<V3> = (0..groups)
        .map(|k| {
            let ls: Vec<&Light> = lights.iter().filter(|l| l.group == k).collect();
            ls.iter().fold(V3::default(), |a, l| a + l.pos) * (1.0 / ls.len().max(1) as f32)
        })
        .collect();
    // rough surfaces (floor, disc) can't tell the strip's points apart: one light per LED does for them
    let merged: Vec<Light> = (0..groups)
        .map(|k| {
            let colour = lights.iter().filter(|l| l.group == k).fold(V3::default(), |a, l| a + l.colour);
            Light { group: k, pos: centres[k], colour, point: true }
        })
        .collect();
    // half resolution: per pixel the LEDs' visibility, the room light's, the openness
    const H: usize = SIZE / 2;
    let stride = groups + 2;
    let mut half = vec![1.0f32; H * H * stride];
    par_chunks(&mut half, 4 * H * stride, |first, chunk| {
        for (k, cell) in chunk.chunks_mut(stride).enumerate() {
            let hi = first / stride + k;
            let (hx, hy) = (hi % H, hi / H);
            let i = (hy * 2 + 1) * SIZE + hx * 2 + 1;
            // glass and the CPU floor aren't shadowed: only the opaque parts need it
            let mat = g.px[i].mat;
            if mat == Mat::None || (d.glass && mat != Mat::Steel) || ((d.cpu || d.glass) && mat == Mat::Floor) {
                continue;
            }
            let (p, n) = (g.px[i].pos, g.px[i].nrm);
            for (gi, c) in centres.iter().enumerate() {
                cell[gi] = shadow(hf, p, n, *c, d.shadow_crisp);
            }
            if room != V3::default() {
                cell[groups] = shadow(hf, p, n, p + room_dir * 3.0, d.shadow_crisp);
            }
            cell[groups + 1] = openness(hf, p);
        }
    });
    let sample = |x: usize, y: usize, k: usize| -> f32 {
        // bilinear from the half-resolution grid (its cells sit on odd full-resolution pixels)
        let (fx, fy) = (((x as f32 - 1.0) / 2.0).clamp(0.0, (H - 1) as f32), ((y as f32 - 1.0) / 2.0).clamp(0.0, (H - 1) as f32));
        let (x0, y0) = (fx as usize, fy as usize);
        let (x1, y1) = ((x0 + 1).min(H - 1), (y0 + 1).min(H - 1));
        let (ax, ay) = (fx - x0 as f32, fy - y0 as f32);
        let at = |x: usize, y: usize| half[(y * H + x) * stride + k];
        (at(x0, y0) * (1.0 - ax) + at(x1, y0) * ax) * (1.0 - ay) + (at(x0, y1) * (1.0 - ax) + at(x1, y1) * ax) * ay
    };
    // the water's glow by position (light scattered in the coolant), on a small grid
    const GL: usize = 120;
    let coolant = d.coolant; // the coolant tints the scattered light
    let glow: Vec<V3> = (0..GL * GL)
        .map(|k| {
            let (sx, sy) = ((k % GL) as f32 * SIZE as f32 / GL as f32, (k / GL) as f32 * SIZE as f32 / GL as f32);
            let f = SCALE * (CAM_Z - FLOOR_Z) / CAM_Z;
            let (x, y) = ((sx - C) / f, (sy - C) / f);
            lights.iter().fold(V3::default(), |a, l| {
                let (dx, dy) = (l.pos.x - x, l.pos.y - y);
                // the water scatters the light through its dye, and a coloured dye glows in its own colour too (as
                // bright as the light is; none for clear or white coolant): a strong glow near each LED, and the whole
                // chamber faintly in the coolant's colour
                let lum = 0.2126 * l.colour.x + 0.7152 * l.colour.y + 0.0722 * l.colour.z;
                let top = coolant.x.max(coolant.y).max(coolant.z).max(1e-3);
                let saturation = 1.0 - coolant.x.min(coolant.y).min(coolant.z) / top;
                let scatter = l.colour.mul(coolant) + coolant * (3.0 * lum * saturation / top);
                let d2 = dx * dx + dy * dy;
                a + scatter * (d.water_glow / (0.015 + d2 * 14.0) + WATER_BODY / (0.3 + d2 * 1.5))
            })
        })
        .collect();
    let gf = SCALE * (CAM_Z - FLOOR_Z) / CAM_Z;
    let glow_at = |p: V3| {
        let cell = |v: f32| ((v * gf + C) * GL as f32 / SIZE as f32).clamp(0.0, (GL - 1) as f32) as usize;
        glow[cell(p.y) * GL + cell(p.x)]
    };
    // the floor with the CPU, lit, as a picture the glass can bend
    let mut bg = vec![V3::default(); if d.cpu || d.glass { SIZE * SIZE } else { 0 }];
    // the coolant's tint on what is seen through it: its hue, part way from white
    let dye = {
        let m = d.coolant.x.max(d.coolant.y).max(d.coolant.z).max(1e-3);
        v3(1.0, 1.0, 1.0) * 0.6 + d.coolant * (0.4 / m)
    };
    if !bg.is_empty() {
        let floor = floor_view(camera, d.cpu);
        let resp = floor_response(camera, d, &merged);
        let colours: Vec<V3> = merged.iter().map(|l| l.colour).collect();
        let lit_room = room != V3::default();
        par_chunks(&mut bg, 8 * SIZE, |first, chunk| {
            for (k, c) in chunk.iter_mut().enumerate() {
                let i = first + k;
                let mut acc = V3::default();
                for (gi, col) in colours.iter().enumerate() {
                    acc = acc + resp[i * groups + gi].mul(*col);
                }
                if lit_room {
                    let FloorPx { p, m, n, lit } = floor[i];
                    if m.albedo.x >= 0.001 {
                        let v = (cam - p).norm();
                        acc = acc + brdf(n, v, room_dir, &m).mul(room) * lit + m.albedo.mul(room) * (0.09 * lit);
                        if m.metal >= 1.0 {
                            let r = n * (2.0 * n.dot(v)) - v;
                            let across = (0.8 - (p.x + p.y) * 0.8).clamp(0.3, 1.3);
                            acc = acc + env(r, p, room, room_dir, &[]).mul(m.albedo) * (0.3 * across) + room.mul(m.albedo) * (0.9 * across);
                        }
                    }
                }
                // the faint fill light
                let m = floor[i].m;
                if m.albedo.x >= 0.001 {
                    acc = acc + m.albedo.mul(fill) * if m.metal >= 1.0 { 2.0 } else { 1.0 };
                }
                // under the water: its glow, and the floor seen through the dyed water a little in its colour
                let p = floor[i].p;
                let depth = (d.water_top() - FLOOR_Z) * (WATER_TOP / d.water_top());
                *c = acc.mul(dye) + glow_at(p) * (depth * WATER_FLOOR);
            }
        });
    }
    let bg_at = |x: f32, y: f32| -> V3 {
        let (xi, yi) = (x.clamp(0.0, SIZE as f32 - 1.0) as usize, y.clamp(0.0, SIZE as f32 - 1.0) as usize);
        bg[yi * SIZE + xi]
    };
    // glass at pixel i: (how much of the floor shows, per colour; where it shows from; the glass's own light).
    // Smoked black glass, darker the more of it the light passes through: the solid base and the blades seen
    // along their plates read darker than a thin face. Its outlines catch the light, and its cut
    // edges glow with the LEDs' colour (light carried inside the glass, like edge-lit acrylic): the shimmer.
    let glass_px = |i: usize| -> (V3, (f32, f32), V3) {
        let (x, y) = (i % SIZE, i / SIZE);
        let (p, n, mat) = (g.px[i].pos, g.px[i].nrm, g.px[i].mat);
        let v = (cam - p).norm();
        let nv = n.dot(v).abs().max(1e-3);
        let fres = 0.04 + 0.96 * (1.0 - nv).powi(5);
        // the path through the glass, world units
        let thick = match mat {
            Mat::Blade => 1.8 * T_MM / nv,
            Mat::Edge => (p.z - T_BASE).max(0.0) * 0.6 + 2.0 * T_MM,
            _ => (p.z - FLOOR_Z) * 1.4 / nv.max(0.4),
        }
        .min(0.5);
        // smoked black glass: about 90 % opaque where thinnest (~2 mm), and it absorbs along the whole path, so
        // the thicker it is the less shows through (the base ~3 % at its thinnest, the nose almost nothing)
        let tint = v3(1.0, 1.0, 1.02) * (0.1 * (-(thick - 2.0 * T_MM).max(0.0) * 6.0).exp());
        let bend = 260.0 * (p.z - FLOOR_Z + 0.05);
        let from = (x as f32 - n.x * bend, y as f32 - n.y * bend);
        // outlines: where the next pixel is another surface or much nearer or farther
        let near = |j: usize| g.px[j].mat != mat || (g.px[j].depth - g.px[i].depth).abs() > 0.012;
        let line = if x > 1 && y > 1 && x < SIZE - 2 && y < SIZE - 2 {
            if [i - 1, i + 1, i - SIZE, i + SIZE].into_iter().any(near) { 1.0 } else if [i - 2, i + 2, i - 2 * SIZE, i + 2 * SIZE].into_iter().any(near) { 0.4 } else { 0.0 }
        } else {
            0.0
        };
        let through = tint * ((1.0 - fres) * (1.0 - 0.55 * line) * (0.35 + 0.65 * nv.powf(0.5)));
        // sharp glints: the LEDs and the window (full strength even when the room is in shadow: it is a small
        // bright source, and the glints give the dark glass its contrast)
        let glass = Material { albedo: V3::default(), rough: 0.04, metal: 0.0 };
        let mut own = V3::default();
        for l in &merged {
            let dl = l.pos - p;
            own = own + brdf(n, v, dl.norm(), &glass).mul(l.colour) * (7.0 / (0.05 + dl.dot(dl) * 2.2));
        }
        let r = n * (2.0 * n.dot(v)) - v;
        let flat = n.z.abs().powi(8); // faces that look straight up: no mirror sheen, else it reads as a cover glass
        own = own + env(r, p, room * 2.8, room_dir, &merged) * (fres * 1.5 * (1.0 - flat));
        // light carried in the glass: the LEDs' colour, strongest at cut edges, outlines and steep faces
        let near_led = merged.iter().fold(V3::default(), |a, l| {
            let dl = l.pos - p;
            a + l.colour * (1.0 / (0.08 + dl.dot(dl) * 2.0))
        });
        let carried = match mat {
            Mat::Edge => 0.1,
            _ => 0.08 * (1.0 - nv).powi(2),
        } + 0.6 * line;
        own = own + near_led.mul(v3(0.85, 1.0, 0.9)) * (carried * 0.09);
        // glints: an outline whose surface faces the window (top left) or an LED catches it as a bright streak;
        // they move along the blades as the wheel turns
        if line >= 1.0 {
            let side = |dx: f32, dy: f32| {
                let k = (n.x * dx + n.y * dy) / (dx.hypot(dy) * n.x.hypot(n.y).max(1e-3));
                k.max(0.0).powi(16)
            };
            own = own + room * (3.4 * side(room_dir.x, room_dir.y));
            for l in &merged {
                own = own + l.colour * (0.3 * side(l.pos.x - p.x, l.pos.y - p.y) / (0.2 + (l.pos - p).dot(l.pos - p)));
            }
        }
        // the faint fill light catches the glass a little, its edges more
        own = own + fill * (0.06 + 0.5 * line + 0.3 * fres);
        let depth = (d.water_top() - p.z).max(0.0) * (WATER_TOP / d.water_top());
        (through, from, own + glow_at(p) * (depth * WATER_GLASS))
    };
    let mut out = vec![0u8; SIZE * SIZE * 4];
    let layered = layered || d.glass;
    // per pixel for the blur: [the floor behind, how much of it shows, its own light]
    let mut lay = vec![[V3::default(); 3]; if layered { SIZE * SIZE } else { 0 }];
    let lay_cells: Vec<std::sync::Mutex<&mut [[V3; 3]]>> = lay.chunks_mut(SIZE * 4).map(std::sync::Mutex::new).collect();
    par_chunks(&mut out, 4 * SIZE * 4, |first, chunk| {
        let mut lay = lay_cells.get(first / (4 * SIZE * 4)).map(|m| m.lock().unwrap());
        let mut vis = vec![0.0f32; groups];
        for (k, px) in chunk.chunks_mut(4).enumerate() {
            let i = first / 4 + k;
            let (x, y) = (i % SIZE, i / SIZE);
            let mut c = V3::default();
            let mat = g.px[i].mat;
            let glassy = d.glass && !matches!(mat, Mat::None | Mat::Floor | Mat::Steel);
            let mut glass_share = None;
            if !bg.is_empty() && matches!(mat, Mat::None | Mat::Floor) {
                c = bg[i];
            } else if glassy {
                let (through, from, own) = glass_px(i);
                glass_share = Some((through, own));
                // a little dispersion: red bends least, blue most
                let (ox, oy) = (from.0 - x as f32, from.1 - y as f32);
                let at = |k: f32| bg_at(x as f32 + ox * k, y as f32 + oy * k);
                let behind = v3(at(0.96).x, at(1.0).y, at(1.05).z);
                c = behind.mul(through) + own;
            } else if mat != Mat::None {
                let (p, n) = (g.px[i].pos, g.px[i].nrm);
                let v = (cam - p).norm();
                let m = material(g.px[i].mat);
                for (gi, vv) in vis.iter_mut().enumerate() {
                    *vv = sample(x, y, gi);
                }
                // the edges catch the LEDs more: the shimmer
                let shine = if g.px[i].mat == Mat::Edge { 1.8 } else { 1.0 };
                for l in if m.rough > 0.5 { &merged } else { lights } {
                    let dl = l.pos - p;
                    let dist2 = dl.dot(dl);
                    c = c + brdf(n, v, dl.norm(), &m).mul(l.colour) * (0.9 * shine * vis[l.group] / (0.05 + dist2 * 2.2));
                }
                let (room_vis, open) = (sample(x, y, groups), sample(x, y, groups + 1));
                c = c + brdf(n, v, room_dir, &m).mul(room) * (room_vis * (0.5 + 0.5 * open));
                c = c + m.albedo.mul(room) * (0.06 * open) + m.albedo.mul(fill) * (open * if m.metal >= 1.0 { 3.0 } else { 1.0 });
                if g.px[i].mat == Mat::Steel {
                    // bare steel mirrors its surroundings: a soft bright overhead and the window the room light
                    // comes from (top left); without them, polished metal seen from above looks black
                    let r = n * (2.0 * n.dot(v)) - v;
                    let window = (-((r - room_dir).dot(r - room_dir)) / 0.06).exp();
                    let env = room * (0.35 * r.z.max(0.0).powf(3.0) + 3.0 * window);
                    let fres = 0.7 + 0.3 * (1.0 - n.dot(v).max(0.0)).powi(5);
                    // machined: fine circular marks (round, so they look the same however it has turned), and
                    // the window's light falling off across the face from the top left
                    let rad = (p.x * p.x + p.y * p.y).sqrt();
                    let ring = ((rad * 2200.0).floor() * 12.9898).sin().fract().abs();
                    let marks = 0.82 + 0.18 * ring;
                    let across = (0.95 - (p.x + p.y) * 3.5).clamp(0.45, 1.3);
                    c = c + env * (fres * room_vis * marks * across);
                    // and the LED strips round it
                    c = c + env_leds(r, p, &merged) * (0.5 * fres * marks);
                }
                // the glow as approved for the shallow chamber, whatever the water's depth over the wheel
                let depth = (d.water_top() - p.z).max(0.0) * (WATER_TOP / d.water_top());
                c = c + glow_at(p) * depth;
            }
            if let Some(l) = lay.as_mut() {
                let open = v3(1.0, 1.0, 1.0);
                l[k] = match (mat, glass_share) {
                    (Mat::None | Mat::Floor, _) => [c, open, V3::default()],
                    // the glass's own share, unbent (the bend smears away at speed)
                    (_, Some((through, own))) => [if bg.is_empty() { V3::default() } else { bg[i] }, through, own],
                    // opaque: all its own light
                    _ => [if bg.is_empty() { V3::default() } else { bg[i] }, V3::default(), c],
                };
            }
            px.copy_from_slice(&tone(c));
        }
    });
    drop(lay_cells);
    let layers = layered.then(|| Layers { floor: lay.iter().map(|l| l[0]).collect(), through: lay.iter().map(|l| l[1]).collect(), own: lay.iter().map(|l| l[2]).collect() });
    (Pixmap::from_vec(out, tiny_skia::IntSize::from_wh(SIZE as u32, SIZE as u32).unwrap()).unwrap(), layers)
}

/// Tone map (soft shoulder), gamma, and contrast: darks deeper, highlights brighter.
fn tone(c: V3) -> [u8; 4] {
    let tm = |x: f32| (1.12 * (x.max(0.0) / (1.0 + x.max(0.0))).powf(1.0 / 2.2).powf(1.3) * 255.0).clamp(0.0, 255.0) as u8;
    [tm(c.x), tm(c.y), tm(c.z), 255]
}

/// Motion blur over the layers: what turns (the glass's share, the opaque parts' light) averaged over the frame's
/// turn `blur` (radians), the floor behind left still. Each pixel follows the surface point it shows (or, over the
/// floor, the wheel's mid height along its ray) back round the axis and samples where that was on screen, so it
/// holds for a tilted view too.
fn blur_layers(out: &mut Pixmap, l: &Layers, g: &GBuf, cam: &Camera, blur: f32, d: Design) {
    if blur.to_degrees() < 1.5 {
        return;
    }
    let n = ((blur.to_degrees() * 1.5).ceil() as usize).clamp(4, 160);
    let (rim, mid) = match d.wheel {
        Wheel::Turbo => (T_R, (T_BASE + T_TOP) * 0.5),
        Wheel::Open => (BLADE_OUT + 0.012, DISC_Z + BLADE_H * 0.5),
    };
    let turns: Vec<(f32, f32)> = (0..n).map(|k| -(k as f32 + 0.5) / n as f32 * blur).map(|a| (a.cos(), a.sin())).collect();
    let data = out.data_mut();
    par_chunks(data, 4 * SIZE * 8, |first, chunk| {
        for (k, px) in chunk.chunks_mut(4).enumerate() {
            let i = first / 4 + k;
            let (x, y) = ((i % SIZE) as f32 + 0.5, (i / SIZE) as f32 + 0.5);
            let p = if matches!(g.px[i].mat, Mat::None | Mat::Floor) { cam.at_height(x, y, mid) } else { g.px[i].pos };
            if p.x.hypot(p.y) > rim * 1.06 {
                continue;
            }
            let (mut t, mut a) = (V3::default(), V3::default());
            for &(cs, sn) in &turns {
                // where this point was `a` ago (turned back: clockwise on screen)
                let (sx, sy, _) = cam.project(v3(p.x * cs - p.y * sn, p.x * sn + p.y * cs, p.z));
                let j = (sy.clamp(0.0, SIZE as f32 - 1.0) as usize) * SIZE + sx.clamp(0.0, SIZE as f32 - 1.0) as usize;
                t = t + l.through[j];
                a = a + l.own[j];
            }
            px.copy_from_slice(&tone(l.floor[i].mul(t) * (1.0 / n as f32) + a * (1.0 / n as f32)));
        }
    });
}

/// Smooths the stair steps on geometry edges: where a pixel's neighbour is another material or much nearer or
/// farther, it is blended with its neighbours (texture-free surfaces elsewhere stay sharp).
fn smooth_edges(out: &mut Pixmap, g: &GBuf) {
    let src = out.data().to_vec();
    let dst = out.data_mut();
    for y in 1..SIZE - 1 {
        for x in 1..SIZE - 1 {
            let i = y * SIZE + x;
            let (m, d) = (g.px[i].mat, g.px[i].depth);
            let edge = [i - 1, i + 1, i - SIZE, i + SIZE].iter().any(|&j| g.px[j].mat != m || (g.px[j].depth - d).abs() > 0.02);
            if !edge {
                continue;
            }
            for c in 0..3 {
                let at = |j: usize| src[j * 4 + c] as u32;
                let sum = at(i) * 4 + (at(i - 1) + at(i + 1) + at(i - SIZE) + at(i + SIZE)) * 2 + at(i - SIZE - 1) + at(i - SIZE + 1) + at(i + SIZE - 1) + at(i + SIZE + 1);
                dst[i * 4 + c] = (sum / 16) as u8;
            }
        }
    }
}

/// Motion blur: the bladed ring (between the nose or hub and the rim) averaged over the frame's turn `blur`
/// (radians). Seen from straight above, turning the picture about the centre is exactly the wheel turning (at
/// every height), so the ring is laid over itself turned back by half the remaining spread: k passes = 2^k evenly
/// spaced copies. The nose / hub is round: it isn't blurred, and its highlights stay put.
fn blur_impeller(out: &mut Pixmap, g: &GBuf, blur: f32, d: Design) {
    use tiny_skia::{BlendMode, FilterQuality, PixmapPaint, PremultipliedColorU8, Transform};
    if blur.to_degrees() < 1.5 {
        return;
    }
    let passes = ((blur.to_degrees() / 1.5).log2().ceil() as u32).min(6);
    let f = |z: f32| SCALE * (CAM_Z - FLOOR_Z) / (CAM_Z - z);
    let (r_out, r_in) = match d.wheel {
        Wheel::Turbo => (T_R * f(T_BASE) + 1.5, T_NOSE * f(T_TOP) * 1.02),
        Wheel::Open => ((BLADE_OUT + 0.012) * f(DISC_Z) + 1.5, HUB * f(BLADE_H) * 1.02),
    };
    let side = (r_out * 2.0).ceil() as i32 + 4;
    let o = (C as i32) - side / 2;
    let mut ring = Pixmap::new(side as u32, side as u32).unwrap();
    {
        let src = out.pixels();
        let dst = ring.pixels_mut();
        for y in 0..side {
            for x in 0..side {
                let (px, py) = (x + o, y + o);
                if px < 0 || py < 0 || px >= SIZE as i32 || py >= SIZE as i32 {
                    continue;
                }
                let (fx, fy) = (px as f32 + 0.5 - C, py as f32 + 0.5 - C);
                let r = (fx * fx + fy * fy).sqrt();
                let i = py as usize * SIZE + px as usize;
                let ring = r >= r_in && r <= r_out && g.px[i].mat != Mat::Floor && g.px[i].mat != Mat::None;
                if ring || g.px[i].mat == Mat::Steel {
                    dst[(y * side + x) as usize] = src[i];
                }
            }
        }
    }
    let cx = C - o as f32;
    let around = |a: f32| Transform::from_rotate_at(a.to_degrees(), cx, cx);
    let mut spread = blur;
    for _ in 0..passes {
        spread /= 2.0;
        for px in ring.pixels_mut() {
            let c = *px;
            *px = PremultipliedColorU8::from_rgba(c.red() / 2, c.green() / 2, c.blue() / 2, c.alpha() / 2).unwrap();
        }
        let copy = ring.clone();
        // earlier positions: turned back (clockwise on screen; the wheel turns anticlockwise)
        ring.draw_pixmap(0, 0, copy.as_ref(), &PixmapPaint { opacity: 1.0, blend_mode: BlendMode::Plus, quality: FilterQuality::Bilinear }, around(spread), None);
    }
    out.draw_pixmap(o, o, ring.as_ref(), &PixmapPaint { quality: FilterQuality::Nearest, ..Default::default() }, Transform::identity(), None);
}

// ---------------- particles in the water ----------------

/// The camera's exposure (s): the motion blur of the wheel and the particles' streaks.
const SHUTTER: f32 = 1.0 / 45.0;

/// The ports (angles, y down): water in at the top left, out at the top right.
const INLET: f32 = -0.75 * PI;
const OUTLET: f32 = -0.25 * PI;
const WALL: f32 = 0.975;

#[derive(Clone, Copy, PartialEq, Debug)]
enum Kind {
    /// coming in at the inlet (top left), carried round, going out at the outlet (top right)
    Flow,
    /// going round with the water without coming in or going out: fades in and out where it is
    Circulate,
    /// sucked in towards the middle, swirling faster as it gets closer, rising to the nose and into the bore
    Sucked,
    /// caught between the blades, turning with the impeller until thrown out into the flow
    Caught,
    /// drifting on its own, only a little pulled by the flow
    Wander,
}

struct Particle {
    p: V3,
    v: V3,     // this frame's motion (world units a second), for its streak
    drift: V3, // a wanderer's own drift
    kind: Kind,
    size: f32, // radius, world units
    bubble: bool,
    travel: f32,
    life: f32,
    age: f32,
    leaving: bool,
    /// a new Flow particle swimming in from beyond the inlet (top left) to its lane, `lane` from the middle
    arriving: bool,
    lane: f32,
    seed: f32,
}

/// Where through-flowing water comes from and goes to: beyond the round screen's edge.
const OFF_SCREEN: f32 = 1.22;

/// How fast the water turns at radius `r` (radians a second, anticlockwise like the impeller): with the blades
/// inside the impeller, then a free vortex slowing towards the wall.
fn flow(r: f32, spin: f32) -> f32 {
    if r <= BLADE_OUT { spin * 0.85 } else { spin * 0.85 * (BLADE_OUT / r).powf(1.6) }
}

#[cfg(test)]
fn angle_near(a: f32, b: f32, tol: f32) -> bool {
    let d = (a - b).rem_euclid(TAU);
    d < tol || d > TAU - tol
}

/// Light colours along the LED arc, left -> bottom -> right.
pub const LIGHTS: usize = 9;

pub struct Pump3d {
    design: Design,
    mesh: Vec<Tri>,
    angle: f32,
    particles: Vec<Particle>,
    rng: u32,
    /// the view's tilt (degrees towards the picture's right, towards its bottom)
    tilt: (f32, f32),
    /// seconds rendered (the red light's flicker)
    time: f32,
    /// the LEDs' last colour while they were lit (at full strength): the faint fill light keeps it when they fade
    last_hue: V3,
}

/// The faint fill light's strength (linear), in the LEDs' last colour.
const FILL: f32 = 0.05;

impl Default for Pump3d {
    fn default() -> Self {
        Self::new(Design::default())
    }
}

impl Pump3d {
    pub fn new(design: Design) -> Self {
        let mut p = Pump3d { design, mesh: build_mesh(design), angle: 0.0, particles: vec![], rng: 0x9E37_79B9, tilt: (0.0, 0.0), time: 0.0, last_hue: v3(1.0, 0.0076, 0.0044) };
        for _ in 0..design.particles {
            let q = p.spawn(true);
            p.particles.push(q);
        }
        p
    }

    /// The view: tilted `tx` degrees towards the picture's right and `ty` towards its bottom (at most `MAX_TILT`
    /// together; 0, 0 = straight down).
    pub fn set_view(&mut self, tx: f32, ty: f32) {
        self.tilt = clamp_tilt(tx, ty);
    }

    /// The coolant's colour (0xRRGGBB): the water scatters the LEDs' light in it.
    pub fn set_coolant(&mut self, rgb: i32) {
        let c = |v: i32| ((v & 255) as f32 / 255.0).powf(2.2);
        self.design.coolant = v3(c(rgb >> 16), c(rgb >> 8), c(rgb));
    }

    /// Constant red LEDs with a little life: each one breathes slightly on its own, now and then they all dip.
    pub fn red_lights(&self) -> Vec<(f32, f32, f32)> {
        let t = self.time;
        let dip = {
            let slot = (t / 0.12).floor();
            if hash1(slot) < 0.025 { 0.7 } else { 1.0 }
        };
        (0..LIGHTS)
            .map(|i| {
                let i = i as f32;
                let k = 0.82 + 0.07 * (TAU * 0.5 * t + i).sin() + 0.05 * (TAU * 1.7 * t + 2.1 * i).sin() + 0.04 * (TAU * 3.9 * t + 0.7 * i).sin();
                let k = k * dip;
                (255.0 * k, 22.0 * k, 16.0 * k)
            })
            .collect()
    }

    fn rand(&mut self) -> f32 {
        self.rng ^= self.rng << 13;
        self.rng ^= self.rng >> 17;
        self.rng ^= self.rng << 5;
        (self.rng >> 8) as f32 / (1u32 << 24) as f32
    }

    /// A new particle: flowing in at the inlet (or, at the start, anywhere along the way), caught between the
    /// blades, or wandering. A new one takes the kind furthest below its share (68 / 16 / 16 %), so the mix holds.
    fn spawn(&mut self, anywhere: bool) -> Particle {
        // a quarter come in and go out; most just go round; some are sucked into the middle
        const SHARE: [(Kind, f32); 5] = [(Kind::Flow, 0.25), (Kind::Circulate, 0.35), (Kind::Sucked, 0.15), (Kind::Caught, 0.1), (Kind::Wander, 0.15)];
        let kind = if anywhere || self.particles.is_empty() {
            let mut k = self.rand();
            let mut pick = Kind::Wander;
            for (kind, share) in SHARE {
                if k < share {
                    pick = kind;
                    break;
                }
                k -= share;
            }
            pick
        } else {
            let n = self.particles.len() as f32;
            let short = |k: Kind, share: f32| share - self.particles.iter().filter(|q| q.kind == k).count() as f32 / n;
            SHARE.iter().copied().max_by(|a, b| short(a.0, a.1).total_cmp(&short(b.0, b.1))).unwrap().0
        };
        let turbo = self.design.wheel == Wheel::Turbo;
        let (r, a) = match kind {
            Kind::Circulate => (if turbo { T_NOSE + 0.06 } else { BLADE_OUT + 0.05 } + self.rand() * (WALL - 0.06 - if turbo { T_NOSE + 0.06 } else { BLADE_OUT + 0.05 }), self.rand() * TAU),
            Kind::Sucked => (0.45 + self.rand() * 0.4, self.rand() * TAU),
            Kind::Flow if turbo => (T_NOSE + 0.06 + self.rand() * (WALL - T_NOSE - 0.08), if anywhere { INLET - self.rand() * 1.45 * PI } else { INLET - self.rand() * 0.12 }),
            Kind::Flow => (BLADE_OUT + 0.05 + self.rand() * (WALL - BLADE_OUT - 0.08), if anywhere { INLET - self.rand() * 1.45 * PI } else { INLET - self.rand() * 0.12 }),
            Kind::Caught if self.design.wheel == Wheel::Turbo => (T_NOSE + 0.04 + self.rand() * (T_R - T_NOSE - 0.08), self.rand() * TAU),
            Kind::Caught => (HUB + 0.05 + self.rand() * (BLADE_OUT - HUB - 0.08), self.rand() * TAU),
            Kind::Wander => (HUB + 0.1 + self.rand() * (WALL - HUB - 0.15), self.rand() * TAU),
        };
        // height in the water: caught ones between the blades, the rest anywhere up to the surface
        let z = match (self.design.wheel, kind) {
            (Wheel::Turbo, Kind::Caught) => turbo_hub_z(r) + 0.01 + self.rand() * 0.08,
            (Wheel::Turbo, _) => turbo_blade_top(r) + 0.02 + self.rand() * (self.design.water_top() - turbo_blade_top(r) - 0.04).max(0.01),
            (Wheel::Open, Kind::Caught) => 0.015 + self.rand() * (BLADE_H - 0.03),
            _ => 0.015 + self.rand() * (self.design.water_top() - 0.04),
        };
        let bubble = self.rand() < self.design.bubbles * if kind == Kind::Wander { 1.6 } else { 1.0 };
        // a new through-flowing one starts beyond the edge at the inlet and swims in to its lane
        let arriving = kind == Kind::Flow && !anywhere;
        let (pr, pa) = if arriving { (OFF_SCREEN, INLET + (self.rand() - 0.5) * 0.2) } else { (r, a) };
        Particle {
            p: v3(pr * pa.cos(), pr * pa.sin(), z),
            v: V3::default(),
            drift: v3((self.rand() - 0.5) * 0.08, (self.rand() - 0.5) * 0.08, 0.0),
            kind,
            size: if bubble { 0.006 + self.rand() * 0.012 } else { 0.003 + self.rand() * 0.004 },
            bubble,
            travel: if anywhere && kind == Kind::Flow { INLET - a } else { 0.0 },
            life: if anywhere || arriving { 1.0 } else { 0.0 },
            age: if anywhere { self.rand() * 8.0 } else { 0.0 },
            leaving: false,
            arriving,
            lane: r,
            seed: self.rand(),
        }
    }

    /// Moves the particles one step: `spin` the impeller's speed (radians a second, anticlockwise).
    fn step_particles(&mut self, spin: f32, dt: f32) {
        for i in 0..self.particles.len() {
            let (n1, n2) = (self.rand() - 0.5, self.rand() - 0.5);
            let q = &mut self.particles[i];
            let r = (q.p.x * q.p.x + q.p.y * q.p.y).sqrt().max(1e-3);
            let a = q.p.y.atan2(q.p.x);
            let w = flow(r, spin);
            let tangent = v3(q.p.y / r, -q.p.x / r, 0.0); // anticlockwise on screen
            let radial = v3(q.p.x / r, q.p.y / r, 0.0);
            let along = tangent * (w * r);
            let outward = spin * 0.0067;
            q.v = match q.kind {
                Kind::Caught => along + radial * (outward * (0.5 + q.seed)),
                // out through the outlet and off the screen at the top right, as fast as the water goes
                Kind::Flow if q.leaving => v3(OUTLET.cos(), OUTLET.sin(), 0.0) * (0.15 * (spin / TAU).min(1.0) + 0.5 * w * r),
                // in from beyond the inlet (top left) to its lane, faster the faster the pump runs
                Kind::Flow if q.arriving => -radial * (0.9 * (spin / TAU).min(2.0)) + along * 0.5,
                Kind::Flow | Kind::Circulate => along + radial * (n1 * 0.025),
            Kind::Sucked => {
                // a vortex into the middle: faster round as it gets closer, drawn in, rising to the nose
                let swirl = (0.5 / r).powf(1.2).clamp(1.0, 4.0);
                let inward = 0.25 * spin / TAU;
                let rise = if self.design.wheel == Wheel::Turbo { T_TOP + 0.06 } else { BLADE_H + 0.05 };
                q.p.z += (rise - q.p.z) * (1.5 * dt * (spin / TAU).min(1.0));
                tangent * (w.max(spin * 0.85) * swirl * r) - radial * inward
            }
                Kind::Wander => {
                    q.drift = q.drift + (q.drift * -0.6 + v3(n1, n2, 0.0) * 0.38) * dt;
                    q.drift + along * 0.25
                }
            };
            q.p = q.p + q.v * dt;
            if q.kind == Kind::Flow {
                // how far round it has gone from the inlet (anticlockwise on screen = the angle falling)
                q.travel += -((q.p.y.atan2(q.p.x) - a + PI).rem_euclid(TAU) - PI);
            }
            if q.arriving && (q.p.x * q.p.x + q.p.y * q.p.y).sqrt() <= q.lane {
                q.arriving = false;
            }
            let r2 = (q.p.x * q.p.x + q.p.y * q.p.y).sqrt();
            // round from the inlet to the outlet (1.5 turns of pi): most go out there (checked by distance gone, as
            // at speed a step jumps further than the outlet is wide), the others stay in the water going round
            if q.kind == Kind::Flow && !q.leaving && !q.arriving && q.travel > 1.5 * PI - 0.1 {
                if q.seed < 0.85 {
                    q.leaving = true;
                } else {
                    q.kind = Kind::Circulate;
                    q.age = 0.0;
                }
            }
            let rim = if self.design.wheel == Wheel::Turbo { T_R } else { BLADE_OUT };
            if q.kind == Kind::Caught && r2 > rim + 0.03 {
                q.kind = Kind::Circulate; // thrown out of the wheel into the water going round
                q.age = 0.0;
            }
            if q.kind == Kind::Wander {
                if r2 > WALL - 0.03 && q.drift.x * q.p.x + q.drift.y * q.p.y > 0.0 {
                    let u = v3(q.p.x / r2, q.p.y / r2, 0.0);
                    q.drift = q.drift - u * (2.0 * q.drift.dot(u));
                }
                if r2 < HUB + 0.03 {
                    let k = (HUB + 0.03) / r2.max(1e-3);
                    q.p = v3(q.p.x * k, q.p.y * k, q.p.z);
                }
            } else if !q.leaving && !q.arriving && r2 > WALL - 0.025 {
                let k = (WALL - 0.025) / r2;
                q.p = v3(q.p.x * k, q.p.y * k, q.p.z);
            }
            q.age += dt;
            let core = if self.design.wheel == Wheel::Turbo { T_BORE } else { HUB * 0.5 };
            let fading = (q.leaving && r2 > OFF_SCREEN - 0.05)
                || (q.kind == Kind::Wander && q.age > 6.0 + q.seed * 8.0)
                || (q.kind == Kind::Circulate && q.age > 3.0 + q.seed * 4.0)
                || (q.kind == Kind::Sucked && r2 < core);
            q.life = if fading { (q.life - dt * if q.leaving { 2.5 } else { 0.8 }).max(0.0) } else { (q.life + dt * 2.0).min(1.0) };
            let gone = (q.leaving && r2 > OFF_SCREEN) || (fading && q.life <= 0.0) || q.travel > 3.2 * PI;
            if gone {
                self.particles[i] = self.spawn(false);
            }
        }
    }

    /// The particles drawn into the shaded frame: each lit by the LEDs where it is (with the blades' shadows),
    /// hidden where a surface is between it and the eye, streaked along its motion over the exposure.
    fn draw_particles(&self, out: &mut Pixmap, g: &GBuf, hf: &GBuf, cam: &Camera, lights: &[Light]) {
        use tiny_skia::{Color, FillRule, LineCap, Paint, PathBuilder, Stroke, Transform};
        let shutter = SHUTTER;
        let tm = |x: f32| (x / (1.0 + x)).powf(1.0 / 2.2);
        for q in &self.particles {
            let (sx, sy, w) = cam.project(q.p);
            if sx < 0.0 || sy < 0.0 || sx >= SIZE as f32 || sy >= SIZE as f32 {
                continue;
            }
            let i = sy as usize * SIZE + sx as usize;
            if g.px[i].depth < w - 0.003 {
                continue; // under a blade, behind the hub
            }
            // its light: each LED (the blades may shadow it), a little room light
            let room = self.design.room();
            let mut c = room * 0.035 + self.last_hue * (FILL * 0.6);
            let lift = room.x * 0.23; // the room's light on the water
            for l in lights {
                let d = l.pos - q.p;
                let dist2 = d.dot(d);
                let vis = shadow(hf, q.p, v3(0.0, 0.0, 1.0), l.pos, self.design.shadow_crisp);
                c = c + l.colour * (0.5 * vis / (0.05 + dist2 * 2.2));
            }
            let px_r = (q.size * cam.f / w).max(0.6);
            let alpha = q.life * if q.bubble { 0.9 } else { 0.8 };
            let col = |k: f32, a: f32| {
                Color::from_rgba((tm(c.x * k) + lift).min(1.0), (tm(c.y * k) + lift).min(1.0), (tm(c.z * k) + lift * 1.1).min(1.0), (a * alpha).clamp(0.0, 1.0)).unwrap()
            };
            let paint = |cl: Color| Paint { shader: tiny_skia::Shader::SolidColor(cl), anti_alias: true, ..Default::default() };
            // the streak: back along its motion over the exposure
            let (ex, ey, _) = cam.project(q.p - q.v * shutter);
            let len = ((sx - ex).powi(2) + (sy - ey).powi(2)).sqrt();
            let mut pb = PathBuilder::new();
            pb.move_to(sx, sy);
            pb.line_to(ex, ey);
            let streak = pb.finish();
            let mut st = Stroke::default();
            st.line_cap = LineCap::Round;
            if q.bubble {
                if let (true, Some(s)) = (len > px_r * 1.2, &streak) {
                    st.width = px_r * 1.8;
                    out.stroke_path(s, &paint(col(2.0, 0.25 * (px_r * 3.0 / len).clamp(0.3, 1.0))), &st, Transform::identity(), None);
                } else if let Some(ring) = PathBuilder::from_circle(sx, sy, px_r) {
                    st.width = 0.9;
                    out.stroke_path(&ring, &paint(col(2.5, 0.8)), &st, Transform::identity(), None);
                    out.fill_path(&ring, &paint(col(1.0, 0.15)), FillRule::Winding, Transform::identity(), None);
                }
                if let Some(glint) = PathBuilder::from_circle(sx - px_r * 0.35, sy - px_r * 0.35, (px_r * 0.35).max(0.5)) {
                    out.fill_path(&glint, &paint(col(6.0, 1.0)), FillRule::Winding, Transform::identity(), None);
                }
            } else if let (true, Some(s)) = (len > px_r * 1.2, &streak) {
                st.width = px_r * 1.6;
                out.stroke_path(s, &paint(col(3.0, 0.7)), &st, Transform::identity(), None);
            } else if let Some(dot) = PathBuilder::from_circle(sx, sy, px_r) {
                out.fill_path(&dot, &paint(col(3.0, 0.8)), FillRule::Winding, Transform::identity(), None);
            }
        }
    }

    /// The frame for the wheel at its current angle, the wheel blurred over `blur` radians of turn.
    fn frame(&mut self, blur: f32, lights: &[(f32, f32, f32)]) -> Pixmap {
        let red;
        let lights = if lights.is_empty() {
            red = self.red_lights();
            &red[..]
        } else {
            lights
        };
        // the colour the LEDs have while lit: when they fade out, the fill light keeps it
        let lin = |v: f32| (v / 255.0).powf(2.2);
        let sum = lights.iter().fold(V3::default(), |a, &(r, g, b)| a + v3(lin(r), lin(g), lin(b)));
        let top = sum.x.max(sum.y).max(sum.z);
        if top > 0.02 * lights.len() as f32 {
            self.last_hue = sum * (1.0 / top);
        }
        let fill = self.last_hue * FILL;
        let cam = Camera::tilted(self.tilt.0, self.tilt.1);
        let tilted = self.tilt != (0.0, 0.0);
        let g = rasterise(&self.mesh, self.angle, &cam);
        // the height field for shadows is the scene seen straight down
        let top = tilted.then(|| rasterise(&self.mesh, self.angle, &Camera::top()));
        let hf = top.as_ref().unwrap_or(&g);
        let d = self.design;
        let ls = led_lights(lights, d.led_spread, d.led_z(), d.led_r(), d.led_gain());
        let (mut out, layers) = shade(&g, hf, &cam, &ls, d.room(), fill, d, tilted);
        smooth_edges(&mut out, &g);
        match &layers {
            Some(l) => blur_layers(&mut out, l, &g, &cam, blur, d),
            None => blur_impeller(&mut out, &g, blur, d),
        }
        self.draw_particles(&mut out, &g, hf, &cam, &ls);
        out
    }

    /// One frame with the impeller turned to `angle`, not blurred.
    pub fn render_still(&mut self, angle: f32, lights: &[(f32, f32, f32)]) -> Pixmap {
        self.angle = angle;
        self.frame(0.0, lights)
    }

    /// Turns the wheel and moves the water on by `dt` seconds at `rpm`; the wheel's speed (radians a second).
    fn advance(&mut self, rpm: f32, dt: f32) -> f32 {
        let dt = dt.clamp(0.0, 0.2);
        let spin = rpm.max(0.0) / 1000.0 * TAU;
        self.time += dt;
        self.angle = (self.angle + spin * dt) % TAU;
        self.step_particles(spin, dt);
        spin
    }

    /// One frame: `rpm` the pump speed, `dt` seconds since the last frame, `lights` the LEDs' colours (0..255 RGB,
    /// upper left -> bottom -> upper right; empty = the flickering red). The wheel turns anticlockwise at
    /// rpm / 1000 turns a second and is motion-blurred over the frame's exposure like a camera sees it.
    pub fn render(&mut self, rpm: f32, dt: f32, lights: &[(f32, f32, f32)]) -> Pixmap {
        let spin = self.advance(rpm, dt);
        self.frame(spin * SHUTTER, lights)
    }

    /// Design stage 3: the particles run at `rpm` for `secs` (the impeller held still), then one frame.
    pub fn render_particles(&mut self, rpm: f32, secs: f32, lights: &[(f32, f32, f32)]) -> Pixmap {
        let spin = rpm / 1000.0 * TAU;
        let steps = (secs * 30.0) as usize;
        for _ in 0..steps {
            self.step_particles(spin, 1.0 / 30.0);
        }
        self.render_still(0.3, lights)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    const RED: [(f32, f32, f32); LIGHTS] = [(255.0, 22.0, 16.0); LIGHTS];

    /// A pump without particles (frames compare the wheel and the light alone).
    fn bare() -> Pump3d {
        let mut p = Pump3d::new(Design { particles: 0, ..Design::default() });
        p.particles.clear();
        p
    }

    /// Mean colour of a region (x, y, w, h).
    fn mean(p: &Pixmap, x: u32, y: u32, w: u32, h: u32) -> (f32, f32, f32) {
        let (mut r, mut g, mut b) = (0.0, 0.0, 0.0);
        for yy in y..y + h {
            for xx in x..x + w {
                let c = p.pixel(xx, yy).unwrap();
                r += c.red() as f32;
                g += c.green() as f32;
                b += c.blue() as f32;
            }
        }
        let n = (w * h) as f32;
        (r / n, g / n, b / n)
    }

    #[test]
    fn turns_with_the_pump_speed() {
        // a stopped pump's wheel stands still; a running one turns anticlockwise at rpm / 1000 turns a second
        let mut p = bare();
        p.advance(0.0, 1.0 / 30.0);
        assert_eq!(p.angle, 0.0, "0 rpm: the wheel stands still");
        let turned = |rpm: f32| {
            let mut p = bare();
            p.advance(rpm, 1.0 / 30.0);
            let before = p.angle;
            p.advance(rpm, 1.0 / 30.0);
            (p.angle - before).rem_euclid(TAU)
        };
        assert!((turned(1000.0) - TAU / 30.0).abs() < 1e-4, "1000 rpm: one turn a second");
        assert!(turned(3000.0) > turned(1500.0) * 1.9);
    }

    /// 2026-10-17: while the wheel spins, a quarter of the particles flow through: they leave off the screen at
    /// the top right and new ones swim in from beyond the top left; the rest stay. A stopped pump moves no water
    /// through. (Before, leaving particles faded out inside the chamber and new ones faded in at the inlet.)
    #[test]
    fn water_comes_in_top_left_and_goes_out_top_right() {
        let mut p = Pump3d::default();
        let (mut exits, mut stray_exits, mut arrivals, mut stray_arrivals) = (0, 0, 0, 0);
        for _ in 0..600 {
            let before: Vec<(f32, V3, bool)> = p.particles.iter().map(|q| (q.seed, q.p, q.leaving)).collect();
            p.advance(1500.0, 1.0 / 30.0);
            for (q, &(seed, at, leaving)) in p.particles.iter().zip(&before) {
                if q.seed == seed {
                    continue; // the same particle
                }
                let r = at.x.hypot(at.y);
                if leaving {
                    // it went out where it was: past the screen's edge, at the outlet
                    if r > WALL && angle_near(at.y.atan2(at.x), OUTLET, 0.35) { exits += 1 } else { stray_exits += 1 }
                }
                if q.arriving {
                    if q.p.x.hypot(q.p.y) > OFF_SCREEN - 0.05 && angle_near(q.p.y.atan2(q.p.x), INLET, 0.2) { arrivals += 1 } else { stray_arrivals += 1 }
                }
            }
        }
        assert!(exits > 40, "water leaves at the top right, off the screen: {exits}");
        assert_eq!(stray_exits, 0, "nowhere else");
        assert!(arrivals > 40 && stray_arrivals == 0, "new water comes in from beyond the top left: {arrivals} ({stray_arrivals} elsewhere)");
        let flow = p.particles.iter().filter(|q| q.kind == Kind::Flow).count() as f32 / p.particles.len() as f32;
        assert!((0.18..0.32).contains(&flow), "about a quarter flows through: {flow}");
        // the new ones do swim in to their lanes
        let inside = p.particles.iter().filter(|q| q.kind == Kind::Flow && !q.arriving && !q.leaving).count();
        assert!(inside > 10, "{inside} arrived ones going round");
        // stopped: nothing comes in or goes out
        let mut still = Pump3d::default();
        for _ in 0..90 {
            let seeds: Vec<f32> = still.particles.iter().map(|q| q.seed).collect();
            still.advance(0.0, 1.0 / 30.0);
            let gone = still.particles.iter().zip(&seeds).filter(|(q, s)| q.seed != **s && q.kind == Kind::Flow && q.arriving).count();
            assert_eq!(gone, 0, "no through-flow at 0 rpm");
        }
    }

    /// 2026-10-17: the view tilts up to 30 degrees in any direction (more is held at 30, the direction kept);
    /// straight down is the camera the renderer had before.
    #[test]
    fn view_tilts_up_to_30_degrees_any_direction() {
        assert_eq!(clamp_tilt(40.0, 0.0), (30.0, 0.0));
        let (x, y) = clamp_tilt(30.0, -30.0);
        assert!((x.hypot(y) - 30.0).abs() < 1e-4 && (x + y).abs() < 1e-4, "held at 30, same direction: {x} {y}");
        assert_eq!(clamp_tilt(10.0, -20.0), (10.0, -20.0));
        // straight down: as before (C + f * x / (CAM_Z - z))
        let top = Camera::top();
        for p in [v3(0.3, -0.2, 0.1), v3(-0.9, 0.5, FLOOR_Z), v3(0.0, 0.0, 0.6)] {
            let (sx, sy, w) = top.project(p);
            let f = SCALE * (CAM_Z - FLOOR_Z);
            assert!((sx - (C + f * p.x / (CAM_Z - p.z))).abs() < 1e-3 && (sy - (C + f * p.y / (CAM_Z - p.z))).abs() < 1e-3 && (w - (CAM_Z - p.z)).abs() < 1e-4);
        }
        for (tx, ty) in [(0.0, 0.0), (30.0, 0.0), (-12.0, 25.0), (0.0, -30.0)] {
            let cam = Camera::tilted(tx, ty);
            // it looks at the middle of the wheel from 30 degrees off at most
            let (cx, cy, _) = cam.project(v3(0.0, 0.0, PIVOT_Z));
            assert!((cx - C).abs() < 1e-3 && (cy - C).abs() < 1e-3, "the wheel stays in the middle");
            assert!(cam.fwd.z <= -(30.0f32.to_radians().cos()) + 1e-4);
            // a screen point and the floor point under it agree both ways
            let q = cam.at_height(100.0, 380.0, FLOOR_Z);
            let (x, y, _) = cam.project(q);
            assert!((x - 100.0).abs() < 1e-2 && (y - 380.0).abs() < 1e-2);
        }
        // tilted towards the right, the eye is on the right: the floor shows past the wheel's top on that side
        let (fx, _, _) = Camera::tilted(30.0, 0.0).project(v3(0.0, 0.0, FLOOR_Z));
        let (tx, _, _) = Camera::tilted(30.0, 0.0).project(v3(0.0, 0.0, T_TOP));
        assert!(fx > C && tx < C, "floor {fx}, nose {tx}");
    }

    /// 2026-10-17: the LED strips light the scene (no room light or window); lit, the picture takes their colour,
    /// and Aura Sync's colours replace the red.
    #[test]
    fn the_led_strips_light_it() {
        // clear (white) coolant, so the light's own colour shows
        let frame = |lights: &[(f32, f32, f32)]| {
            let mut p = bare();
            p.set_coolant(0xFFFFFF);
            p.render_still(0.3, lights)
        };
        let (red, green) = (frame(&RED), frame(&[(20.0, 255.0, 40.0); LIGHTS]));
        // the bottom (by the LEDs) and the middle of the CPU's left side
        for (x, y) in [(200u32, 400u32), (40, 200)] {
            let (r, g) = (mean(&red, x, y, 80, 30), mean(&green, x, y, 80, 30));
            assert!(r.0 > r.1 + 15.0, "red light: {r:?} at {x},{y}");
            assert!(g.1 > g.0 + 15.0, "Aura's green replaces it: {g:?} at {x},{y}");
        }
    }

    /// 2026-10-17: when the LEDs fade out (Aura's breathing), the scene didn't go totally dark: a faint light in
    /// the LEDs' last colour stays, well below them lit.
    #[test]
    fn a_faint_light_of_the_last_colour_stays_when_the_leds_fade() {
        let dark = [(0.0, 0.0, 0.0); LIGHTS];
        for (lit, red_last) in [((255.0, 22.0, 16.0), true), ((20.0, 255.0, 40.0), false)] {
            let mut p = bare();
            let on = p.render_still(0.3, &[lit; LIGHTS]);
            let off = p.render_still(0.3, &dark);
            let (a, b) = (mean(&on, 40, 40, 400, 400), mean(&off, 40, 40, 400, 400));
            let (main, other) = if red_last { (b.0, b.1) } else { (b.1, b.0) };
            assert!(main > 3.0, "not black: {b:?}");
            assert!(main > other * 2.0, "in the last colour: {b:?}");
            assert!(b.0 + b.1 + b.2 < (a.0 + a.1 + a.2) * 0.6, "much dimmer than lit: {b:?} vs {a:?}");
        }
    }

    /// 2026-10-17: a tilted view shows the scene from the side: the picture changes and the nose stays near the
    /// middle.
    #[test]
    fn a_tilted_view_renders() {
        let straight = bare().render_still(0.3, &RED);
        let mut p = bare();
        p.set_view(25.0, -10.0);
        let tilted = p.render_still(0.3, &RED);
        let diff = straight.pixels().iter().zip(tilted.pixels()).filter(|(a, b)| (a.red() as i32 - b.red() as i32).abs() > 20).count();
        assert!(diff > 20_000, "the view changed: {diff} px");
        // the screw head is still in the middle
        let g = rasterise(&p.mesh, 0.3, &Camera::tilted(25.0, -10.0));
        let steel = (200..280).flat_map(|y| (200..280).map(move |x| y * SIZE + x)).filter(|&i| g.px[i].mat == Mat::Steel).count();
        assert!(steel > 500, "the screw near the middle: {steel} px");
    }

    /// Without Aura Sync the light is red with a little life: it varies, stays red, never goes dark.
    #[test]
    fn red_light_flickers_but_stays_red() {
        let mut p = bare();
        let mut seen = vec![];
        for _ in 0..300 {
            p.advance(0.0, 1.0 / 30.0);
            let l = p.red_lights();
            assert_eq!(l.len(), LIGHTS);
            for &(r, g, b) in &l {
                assert!(r > 110.0 && g < r / 5.0 && b < r / 5.0, "red, never dark: {r} {g} {b}");
            }
            seen.push(l[4].0);
        }
        let (lo, hi) = seen.iter().fold((f32::MAX, 0.0f32), |(a, b), &v| (a.min(v), b.max(v)));
        assert!(hi - lo > 20.0, "it flickers: {lo}..{hi}");
    }

    /// 2026-10-17: the coolant colour setting did nothing visible: it only tinted a faint glow of the LEDs' light,
    /// and red light times any coolant stays red. The water takes on the coolant's colour.
    #[test]
    fn the_coolant_colour_shows() {
        let frame = |rgb: i32| {
            let mut p = bare();
            p.set_coolant(rgb);
            p.render_still(0.3, &RED)
        };
        let (green, magenta) = (frame(0x00FF00), frame(0xFF00FF));
        let (g, m) = (mean(&green, 0, 0, 480, 480), mean(&magenta, 0, 0, 480, 480));
        assert!(g.1 > m.1 + 4.0, "green coolant: more green {g:?} vs {m:?}");
        assert!(m.2 > g.2 + 4.0, "magenta coolant: more blue {m:?} vs {g:?}");
    }

    #[test]
    fn the_coolant_tints_the_glow() {
        let mut p = bare();
        // the setting's colour, linear
        p.set_coolant(0xFF0000);
        assert_eq!(p.design.coolant, v3(1.0, 0.0, 0.0));
        p.set_coolant(0x00D1FF);
        assert!(p.design.coolant.z == 1.0 && (0.6..0.7).contains(&p.design.coolant.y) && p.design.coolant.x == 0.0);
    }

    /// Frame times: `cargo test --release -p ttlcd-core pump3d_profile -- --ignored --nocapture`
    #[test]
    #[ignore]
    fn pump3d_profile() {
        for (tx, ty) in [(0.0, 0.0), (20.0, 10.0)] {
            for rpm in [0.0f32, 1500.0] {
                let mut p = Pump3d::default();
                p.set_view(tx, ty);
                for _ in 0..5 {
                    p.render(rpm, 1.0 / 30.0, &RED);
                }
                let t = std::time::Instant::now();
                for _ in 0..20 {
                    p.render(rpm, 1.0 / 30.0, &RED);
                }
                println!("tilt {tx},{ty} {rpm} rpm: {:.1} ms a frame", t.elapsed().as_secs_f64() * 1000.0 / 20.0);
            }
        }
    }

    /// Frames to look at: `TTLCD_DUMP=<dir> cargo test --release -p ttlcd-core pump3d_dump -- --ignored`;
    /// `TTLCD_GIF=<out.gif>` adds a spin-up (with `TTLCD_GIF_AURA=1`: the LEDs breathing through the colours).
    #[test]
    #[ignore]
    fn pump3d_dump() {
        let Some(dir) = std::env::var_os("TTLCD_DUMP") else { return };
        let red = RED.to_vec();
        let save = |f: &Pixmap, name: &str| {
            image::RgbaImage::from_raw(480, 480, f.data().to_vec()).unwrap().save(std::path::Path::new(&dir).join(name)).unwrap();
        };
        for (name, rpm) in [("glass-cpu-0", 0.0f32), ("glass-cpu-1500", 1500.0)] {
            let mut p = Pump3d::default();
            let mut f = None;
            for _ in 0..12 {
                f = Some(p.render(rpm, 1.0 / 30.0, &red));
            }
            save(&f.unwrap(), &format!("pump3d-{name}.png"));
        }
        // views tilted 30 degrees in four directions and one between, still and spinning
        for (name, tx, ty) in [("right", 30.0, 0.0), ("left", -30.0, 0.0), ("down", 0.0, 30.0), ("up", 0.0, -30.0), ("upright", 21.0, -21.0)] {
            for rpm in [0.0f32, 1500.0] {
                let mut p = Pump3d::default();
                p.set_view(tx, ty);
                let mut f = None;
                for _ in 0..12 {
                    f = Some(p.render(rpm, 1.0 / 30.0, &red));
                }
                save(&f.unwrap(), &format!("pump3d-tilt-{name}-{rpm}.png"));
            }
        }
        // the coolant colours under the red light, and the LEDs faded out after red and after green
        for (name, rgb) in [("cyan", 0x00D1FF), ("green", 0x2BE36B), ("magenta", 0xFF2BD6), ("white", 0xFFFFFF), ("orange", 0xFF8A00)] {
            let mut p = Pump3d::default();
            p.set_coolant(rgb);
            save(&p.render(0.0, 1.0 / 30.0, &red), &format!("pump3d-coolant-{name}.png"));
        }
        for (name, lit) in [("red", (255.0, 22.0, 16.0)), ("green", (20.0, 255.0, 40.0))] {
            let mut p = Pump3d::default();
            p.render(0.0, 1.0 / 30.0, &[lit; LIGHTS]);
            save(&p.render(0.0, 1.0 / 30.0, &[(0.0, 0.0, 0.0); LIGHTS]), &format!("pump3d-faded-after-{name}.png"));
        }
        // the CPU on its own (no wheel, no particles)
        let mut p = bare();
        p.mesh.clear();
        save(&p.render_still(0.0, &red), "pump3d-cpu-only.png");
        // the earlier dark metal wheel
        let mut p = Pump3d::new(Design { glass: false, cpu: false, ..Design::default() });
        save(&p.render(1500.0, 1.0 / 30.0, &red), "pump3d-metal.png");
        if let Some(gif) = std::env::var_os("TTLCD_GIF") {
            use image::codecs::gif::{GifEncoder, Repeat};
            let mut enc = GifEncoder::new_with_speed(std::fs::File::create(gif).unwrap(), 10);
            enc.set_repeat(Repeat::Infinite).unwrap();
            let mut p = Pump3d::default();
            let aura = std::env::var_os("TTLCD_GIF_AURA").is_some();
            // TTLCD_GIF_TILT=x,y: the view tilted
            if let Some(t) = std::env::var("TTLCD_GIF_TILT").ok() {
                let v: Vec<f32> = t.split(',').filter_map(|s| s.trim().parse().ok()).collect();
                p.set_view(v.first().copied().unwrap_or(0.0), v.get(1).copied().unwrap_or(0.0));
            }
            for i in 0..120 {
                let t = i as f32 / 20.0;
                // still 0.6 s, up to 2500 rpm by 2.4 s, then holding
                let mut rpm = 2500.0 * ((t - 0.6) / 1.8).clamp(0.0, 1.0).powf(1.4);
                let mut leds = red.clone();
                if aura {
                    rpm = 1200.0;
                    let level = 0.5 - 0.5 * (t / 3.0 * TAU).cos(); // 0 -> 1 -> 0 every 3 s
                    let hue = t / 6.0 * 360.0;
                    let f = |n: f32| {
                        let k = (n + hue / 60.0) % 6.0;
                        1.0 - k.min(4.0 - k).min(1.0).max(0.0)
                    };
                    leds = vec![(f(5.0) * 255.0 * level, f(3.0) * 255.0 * level, f(1.0) * 255.0 * level); LIGHTS];
                }
                let f = p.render(rpm, 1.0 / 20.0, &leds);
                let img = image::RgbaImage::from_raw(480, 480, f.data().to_vec()).unwrap();
                let small = image::imageops::resize(&img, 400, 400, image::imageops::FilterType::Triangle);
                enc.encode_frame(image::Frame::from_parts(small, 0, 0, image::Delay::from_numer_denom_ms(50, 1))).unwrap();
            }
        }
    }
}
