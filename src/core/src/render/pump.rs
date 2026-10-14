//! The Pump display mode: a look inside the pump head at its water pump running. A dark
//! metal centrifugal impeller turns anticlockwise in the coolant; water comes in at the top left, goes round the
//! chamber (left, bottom, right) and leaves at the top right, carrying bubbles and specks; a few drift on their own.
//! The impeller turns at `rpm / 1000` turns a second (3000 rpm: three turns, a fast blur).
//!
//! Light: LEDs ring the pump head on three sides (left, bottom, right). Their colour (`lights`: Aura Sync or a
//! flickering red) glows into the water, lights the particles, and catches the blades' edges as a shimmer that
//! turns (and blurs) with them. Everything solid is dark metal, so the light carries the colour.
//!
//! The cooler's LCD sits in the pump case behind its own glass, with the tubes on top: the round screen is the
//! window, so only the inside is drawn (no case, fittings or glass).
//!
//! Layers, bottom to top: the chamber (cached: machined floor, volute, impeller disc with its brushed highlight),
//! water swirl, the LED glow, deep particles, the blades' shadow, the impeller (pre-shaded sprite + light-coloured
//! edges, motion-blurred by doubling), hub glints, near particles.
use super::{rgba, W};
use std::f32::consts::{PI, TAU};
use tiny_skia::{
    BlendMode, Color, FillRule, FilterQuality, GradientStop, LineCap, Mask, Paint, Path, PathBuilder, Pixmap, PixmapPaint,
    Point, RadialGradient, Shader, SpreadMode, Stroke, Transform,
};

const C: f32 = W as f32 / 2.0; // centre of the round panel
const WALL: f32 = 238.0; // the chamber fills the round screen (the cooler's case and glass are real)
const BLADE_OUT: f32 = 162.0; // impeller tip radius
const HUB: f32 = 38.0;
const BLADES: usize = 7;
const SPRITE: u32 = 2 * (BLADE_OUT as u32 + 8);
const HUB_SPRITE: u32 = 2 * (HUB as u32 + 4);
const PARTICLES: usize = 110;
/// Lights along the LED arc, left -> bottom -> right.
pub const LIGHTS: usize = 9;
const LED_R: f32 = WALL + 10.0; // the LEDs sit just outside the view
/// How far up the sides the LEDs reach above the horizontal (radians).
const LED_UP: f32 = PI / 6.0;
const FIELD: u32 = 120; // the light map's resolution (scaled up 4x: it is soft)
/// The ports (screen angles, y down): water in at the top left, out at the top right.
const INLET: f32 = -0.75 * PI;
const OUTLET: f32 = -0.25 * PI;

/// Visual turns per second at a pump speed.
pub fn turns_per_second(rpm: f32) -> f32 {
    rpm.max(0.0) / 1000.0
}

#[derive(Clone, Copy, PartialEq)]
enum Kind {
    /// carried round the chamber from the inlet to the outlet
    Flow,
    /// caught between the blades, turning with the impeller until thrown out into the flow
    Caught,
    /// drifting on its own, only a little pulled by the flow
    Wander,
}

struct Particle {
    x: f32, // from the centre, px
    y: f32,
    vx: f32, // px a second (the last frame's motion, for its streak)
    vy: f32,
    wx: f32, // a wanderer's own drift
    wy: f32,
    kind: Kind,
    size: f32,
    bubble: bool,
    deep: bool,
    travel: f32, // radians gone round since it came in
    life: f32,   // 0..1 fade in / out
    age: f32,    // seconds; a wanderer drifts for a while, then fades and makes room
    leaving: bool,
    seed: f32,
}

pub struct Pump {
    colour: i32,
    chamber: Option<Pixmap>,
    impeller: Option<Pixmap>,
    hub: Option<Pixmap>,
    edges: Option<Pixmap>,
    /// the room light (top left) where the blades are, in sprite coordinates: fixed while they turn under it
    room: Option<Pixmap>,
    shadow: Option<Pixmap>,
    swirl: Option<Pixmap>,
    clip: Option<Mask>,
    particles: Vec<Particle>,
    /// how far the impeller has turned (radians, grows; drawn anticlockwise)
    angle: f32,
    water: f32,
    time: f32,
    rng: u32,
}

impl Default for Pump {
    fn default() -> Self {
        let mut p = Pump {
            colour: -1,
            chamber: None,
            impeller: None,
            hub: None,
            edges: None,
            room: None,
            shadow: None,
            swirl: None,
            clip: None,
            particles: vec![],
            angle: 0.0,
            water: 0.0,
            time: 0.0,
            rng: 0x9E37_79B9,
        };
        for _ in 0..PARTICLES {
            let q = p.spawn(true);
            p.particles.push(q);
        }
        p
    }
}

fn shade(c: i32, k: f32) -> (f32, f32, f32) {
    let f = |v: i32| ((v & 255) as f32 * k).min(255.0);
    (f(c >> 16), f(c >> 8), f(c))
}

fn col(rgb: (f32, f32, f32), a: f32) -> Color {
    Color::from_rgba8(rgb.0.clamp(0.0, 255.0) as u8, rgb.1.clamp(0.0, 255.0) as u8, rgb.2.clamp(0.0, 255.0) as u8, (a.clamp(0.0, 1.0) * 255.0) as u8)
}

fn radial(cx: f32, cy: f32, r: f32, stops: Vec<GradientStop>) -> Shader<'static> {
    RadialGradient::new(Point::from_xy(cx, cy), 0.0, Point::from_xy(cx, cy), r, stops, SpreadMode::Pad, Transform::identity()).unwrap()
}

fn paint(shader: Shader<'_>) -> Paint<'_> {
    Paint { shader, anti_alias: true, ..Default::default() }
}

fn fill(p: &mut Pixmap, path: &Path, shader: Shader<'_>) {
    p.fill_path(path, &paint(shader), FillRule::Winding, Transform::identity(), None);
}

fn stroke(p: &mut Pixmap, path: &Path, shader: Shader<'_>, width: f32) {
    let mut s = Stroke::default();
    s.width = width;
    s.line_cap = LineCap::Round;
    p.stroke_path(path, &paint(shader), &s, Transform::identity(), None);
}

fn circle(cx: f32, cy: f32, r: f32) -> Path {
    PathBuilder::from_circle(cx, cy, r).unwrap()
}

fn solid(c: Color) -> Shader<'static> {
    Shader::SolidColor(c)
}

fn arc(cx: f32, cy: f32, r: f32, a0: f32, a1: f32) -> Path {
    let mut pb = PathBuilder::new();
    let n = ((a1 - a0).abs() * r / 4.0).ceil().max(2.0) as usize;
    for i in 0..=n {
        let a = a0 + (a1 - a0) * i as f32 / n as f32;
        if i == 0 { pb.move_to(cx + r * a.cos(), cy + r * a.sin()) } else { pb.line_to(cx + r * a.cos(), cy + r * a.sin()) }
    }
    pb.finish().unwrap()
}

/// Gunmetal: dark grey with a hint of blue.
fn metal(k: f32) -> Color {
    col((60.0 * k, 63.0 * k, 70.0 * k), 1.0)
}

/// Each blade's two edges (sprite coordinates, hub to tip). The blades curve back against the anticlockwise turn.
fn blade_edges(o: f32) -> Vec<(Vec<(f32, f32)>, Vec<(f32, f32)>)> {
    (0..BLADES)
        .map(|b| {
            let a0 = b as f32 * TAU / BLADES as f32;
            let curve = |r: f32| a0 + 1.05 * ((r - HUB) / (BLADE_OUT - HUB)).max(0.0).powf(0.8);
            let thick = |r: f32| 0.15 * (HUB / r).powf(0.55);
            let n = 26;
            let (mut lead, mut trail) = (vec![], vec![]);
            for i in 0..=n {
                let r = HUB - 2.0 + (BLADE_OUT - HUB + 2.0) * i as f32 / n as f32;
                let (a, w) = (curve(r), thick(r));
                lead.push((o + r * (a - w).cos(), o + r * (a - w).sin()));
                trail.push((o + r * (a + w).cos(), o + r * (a + w).sin()));
            }
            (lead, trail)
        })
        .collect()
}

fn outline(lead: &[(f32, f32)], trail: &[(f32, f32)]) -> Path {
    let mut pb = PathBuilder::new();
    pb.move_to(lead[0].0, lead[0].1);
    for q in &lead[1..] {
        pb.line_to(q.0, q.1);
    }
    for q in trail.iter().rev() {
        pb.line_to(q.0, q.1);
    }
    pb.close();
    pb.finish().unwrap()
}

fn polyline(pts: &[(f32, f32)]) -> Path {
    let mut pb = PathBuilder::new();
    pb.move_to(pts[0].0, pts[0].1);
    for q in &pts[1..] {
        pb.line_to(q.0, q.1);
    }
    pb.finish().unwrap()
}

impl Pump {
    fn rand(&mut self) -> f32 {
        // xorshift: the same scene on every run (tests compare frames)
        self.rng ^= self.rng << 13;
        self.rng ^= self.rng >> 17;
        self.rng ^= self.rng << 5;
        (self.rng >> 8) as f32 / (1u32 << 24) as f32
    }

    /// A new particle: flowing in at the inlet (or, at the start, anywhere along the way), caught in the impeller,
    /// or wandering.
    fn spawn(&mut self, anywhere: bool) -> Particle {
        // the mix to keep: most follow the flow, some are in the impeller, some wander. A new one takes the kind
        // furthest below its share (they live very different times: random kinds would let the wanderers pile up)
        const SHARE: [(Kind, f32); 3] = [(Kind::Flow, 0.68), (Kind::Caught, 0.16), (Kind::Wander, 0.16)];
        let kind = if anywhere {
            let k = self.rand();
            if k < 0.68 { Kind::Flow } else if k < 0.84 { Kind::Caught } else { Kind::Wander }
        } else {
            let n = self.particles.len().max(1) as f32;
            let short = |k: Kind, share: f32| share - self.particles.iter().filter(|q| q.kind == k).count() as f32 / n;
            SHARE.iter().copied().max_by(|a, b| short(a.0, a.1).total_cmp(&short(b.0, b.1))).unwrap().0
        };
        let (r, a) = match kind {
            Kind::Flow => (BLADE_OUT + 10.0 + self.rand() * (WALL - BLADE_OUT - 18.0), if anywhere { INLET - self.rand() * 1.45 * PI } else { INLET - self.rand() * 0.12 }),
            Kind::Caught => (HUB + 10.0 + self.rand() * (BLADE_OUT - HUB - 14.0), self.rand() * TAU),
            Kind::Wander => (HUB + 20.0 + self.rand() * (WALL - HUB - 30.0), self.rand() * TAU),
        };
        let bubble = self.rand() < if kind == Kind::Wander { 0.6 } else { 0.35 };
        let travel = if anywhere && kind == Kind::Flow { INLET - a } else { 0.0 };
        Particle {
            x: r * a.cos(),
            y: r * a.sin(),
            vx: 0.0,
            vy: 0.0,
            wx: (self.rand() - 0.5) * 20.0,
            wy: (self.rand() - 0.5) * 20.0,
            kind,
            size: if bubble { 1.6 + self.rand() * 3.2 } else { 0.9 + self.rand() * 1.3 },
            bubble,
            deep: self.rand() < 0.45,
            travel,
            life: if anywhere { 1.0 } else { 0.0 },
            age: if anywhere { self.rand() * 8.0 } else { 0.0 },
            leaving: false,
            seed: self.rand(),
        }
    }

    /// Moves the particles one step: `spin` the impeller's speed (radians a second).
    fn step_particles(&mut self, spin: f32, dt: f32) {
        for i in 0..self.particles.len() {
            let (n1, n2) = (self.rand() - 0.5, self.rand() - 0.5);
            let q = &mut self.particles[i];
            let r = (q.x * q.x + q.y * q.y).sqrt().max(1.0);
            let a = q.y.atan2(q.x);
            let w = flow(r, spin);
            // the flow here, as a velocity (tangential, anticlockwise) plus a slight outward push
            let (tx, ty) = (q.y / r, -q.x / r); // unit tangent, anticlockwise on screen
            let (rx, ry) = (q.x / r, q.y / r);
            let (fx, fy) = (tx * w * r, ty * w * r);
            let outward = spin * 1.6;
            let (vx, vy) = match q.kind {
                Kind::Caught => (fx + rx * outward * (0.5 + q.seed), fy + ry * outward * (0.5 + q.seed)),
                Kind::Flow if q.leaving => {
                    // out through the outlet: along the port, away from the centre
                    let s = 30.0 + 2.5 * w.abs() * r;
                    (OUTLET.cos() * s, OUTLET.sin() * s)
                }
                Kind::Flow => (fx + rx * (n1 * 6.0), fy + ry * (n2 * 6.0)),
                Kind::Wander => {
                    // its own slow wandering (a smooth random walk), the flow pulling only a little
                    q.wx += (-q.wx * 0.6 + n1 * 90.0) * dt;
                    q.wy += (-q.wy * 0.6 + n2 * 90.0) * dt;
                    (q.wx + fx * 0.25, q.wy + fy * 0.25)
                }
            };
            q.vx = vx;
            q.vy = vy;
            q.x += vx * dt;
            q.y += vy * dt;
            if q.kind == Kind::Flow {
                q.travel += w * dt;
            }
            let r2 = (q.x * q.x + q.y * q.y).sqrt();
            // reaching the outlet after going round: out it goes (some carry on round once more)
            if q.kind == Kind::Flow && !q.leaving && q.travel > 1.35 * PI && angle_near(q.y.atan2(q.x), OUTLET, 0.12) && q.seed < 0.85 {
                q.leaving = true;
            }
            // thrown out of the impeller into the flow
            if q.kind == Kind::Caught && r2 > BLADE_OUT + 8.0 {
                q.kind = Kind::Flow;
                q.travel = (INLET - a).rem_euclid(TAU);
            }
            if q.kind == Kind::Wander {
                // keep to the chamber: turn back at the wall and the hub
                if r2 > WALL - 8.0 && q.wx * q.x + q.wy * q.y > 0.0 {
                    let (ux, uy) = (q.x / r2, q.y / r2);
                    let d = q.wx * ux + q.wy * uy;
                    q.wx -= 2.0 * d * ux;
                    q.wy -= 2.0 * d * uy;
                }
                if r2 < HUB + 6.0 {
                    q.x *= (HUB + 6.0) / r2.max(1.0);
                    q.y *= (HUB + 6.0) / r2.max(1.0);
                }
            } else if !q.leaving && r2 > WALL - 6.0 {
                q.x *= (WALL - 6.0) / r2;
                q.y *= (WALL - 6.0) / r2;
            }
            q.age += dt;
            // wanderers drift for 6-14 s, then fade out: otherwise they would pile up while the flow comes and goes
            let fading = q.leaving || (q.kind == Kind::Wander && q.age > 6.0 + q.seed * 8.0);
            q.life = if fading { (q.life - dt * if q.leaving { 2.5 } else { 0.8 }).max(0.0) } else { (q.life + dt * 2.0).min(1.0) };
            let gone = (q.leaving && r2 > WALL + 12.0) || (fading && q.life <= 0.0) || q.travel > 3.2 * PI;
            if gone {
                self.particles[i] = self.spawn(false);
            }
        }
    }

    /// Under the water: the machined floor with the volute, the impeller's disc with its brushed-metal highlight (it
    /// doesn't change as the disc turns).
    fn draw_chamber(&self) -> Pixmap {
        let mut p = Pixmap::new(W, W).unwrap();
        p.fill(Color::BLACK);
        let c = self.colour;
        // the floor: black, machined in fine rings, the clear coolant over it only tinting it
        let floor = circle(C, C, WALL);
        fill(&mut p, &floor, radial(C, C, WALL, vec![
            GradientStop::new(0.0, rgba(12, 12, 14, 255)),
            GradientStop::new(0.85, rgba(5, 5, 6, 255)),
            GradientStop::new(1.0, rgba(1, 1, 2, 255)),
        ]));
        let mut seed = 7u32;
        let mut rnd = || {
            seed ^= seed << 13;
            seed ^= seed >> 17;
            seed ^= seed << 5;
            (seed >> 8) as f32 / (1u32 << 24) as f32
        };
        let mut r = HUB;
        while r < WALL - 2.0 {
            stroke(&mut p, &circle(C, C, r), solid(rgba(255, 255, 255, (rnd() * 6.0) as u8)), 1.0);
            r += 1.5 + rnd() * 1.5;
        }
        fill(&mut p, &floor, solid(col(shade(c, 1.0), 0.035)));
        // the volute: a groove widening from the inlet round to the outlet
        let n = 160;
        let mut pb = PathBuilder::new();
        for i in 0..=n {
            let t = i as f32 / n as f32;
            let a = INLET - t * 1.5 * PI;
            let r = BLADE_OUT + 18.0 + t * 26.0;
            if i == 0 { pb.move_to(C + r * a.cos(), C + r * a.sin()) } else { pb.line_to(C + r * a.cos(), C + r * a.sin()) }
        }
        let volute = pb.finish().unwrap();
        for (w, a) in [(22.0, 0.30), (10.0, 0.25), (2.0, 0.06)] {
            stroke(&mut p, &volute, solid(rgba(0, 0, 0, (a * 255.0) as u8)), w);
        }
        stroke(&mut p, &volute.clone().transform(Transform::from_translate(0.0, -1.0)).unwrap(), solid(rgba(255, 255, 255, 10)), 1.0);
        // the impeller's disc (it turns, but looks the same turned): dark machined metal
        fill(&mut p, &circle(C, C, BLADE_OUT), radial(C, C, BLADE_OUT, vec![
            GradientStop::new(0.0, metal(0.3)),
            GradientStop::new(0.85, metal(0.2)),
            GradientStop::new(1.0, metal(0.1)),
        ]));
        let mut r = HUB + 2.0;
        while r < BLADE_OUT - 1.0 {
            stroke(&mut p, &circle(C, C, r), solid(rgba(255, 255, 255, (1.0 + rnd() * 7.0) as u8)), 0.8);
            r += 1.2 + rnd();
        }
        // brushed metal: the light shows as a streak across the rings, fixed to the light, not the disc
        for (a, k) in [(-0.62f32, 1.0f32), (-0.62 + PI, 0.6)] {
            let mut pb = PathBuilder::new();
            pb.move_to(C, C);
            for i in 0..=12 {
                let t = a - 0.22 + 0.44 * i as f32 / 12.0;
                pb.line_to(C + BLADE_OUT * t.cos(), C + BLADE_OUT * t.sin());
            }
            pb.close();
            fill(&mut p, &pb.finish().unwrap(), radial(C, C, BLADE_OUT, vec![
                GradientStop::new(0.15, rgba(200, 205, 215, 0)),
                GradientStop::new(0.6, rgba(200, 205, 215, (34.0 * k) as u8)),
                GradientStop::new(1.0, rgba(200, 205, 215, 0)),
            ]));
        }
        stroke(&mut p, &circle(C, C, BLADE_OUT - 0.5), solid(rgba(150, 155, 165, 45)), 1.0);
        p
    }

    /// The impeller's blades, centred in a sprite: black anodised metal. Only what looks the same at any turn is
    /// drawn here (shading by radius, the dark thickness round each blade, fine highlights on both top edges);
    /// the light from a direction is added each frame where the blades are (`room`, the LED shimmer).
    fn draw_impeller(&self) -> Pixmap {
        let mut p = Pixmap::new(SPRITE, SPRITE).unwrap();
        let o = SPRITE as f32 / 2.0;
        for (lead, trail) in blade_edges(o) {
            let path = outline(&lead, &trail);
            // the blade's thickness: a dark band round its top face
            stroke(&mut p, &path, solid(rgba(2, 2, 3, 255)), 4.0);
            fill(&mut p, &path, radial(o, o, BLADE_OUT, vec![
                GradientStop::new(0.25, metal(0.42)),
                GradientStop::new(1.0, metal(0.2)),
            ]));
            stroke(&mut p, &polyline(&lead), solid(rgba(200, 206, 218, 120)), 0.9);
            stroke(&mut p, &polyline(&trail), solid(rgba(200, 206, 218, 120)), 0.9);
        }
        p
    }

    /// The hub, centred in its own sprite (it turns, its shadow and shine don't): the disc shaded by radius, the
    /// centre cap and three bolts.
    fn draw_hub(&self) -> Pixmap {
        let mut p = Pixmap::new(HUB_SPRITE, HUB_SPRITE).unwrap();
        let o = HUB_SPRITE as f32 / 2.0;
        fill(&mut p, &circle(o, o, HUB), radial(o, o, HUB, vec![
            GradientStop::new(0.0, metal(0.32)),
            GradientStop::new(0.8, metal(0.2)),
            GradientStop::new(1.0, metal(0.08)),
        ]));
        stroke(&mut p, &circle(o, o, HUB - 0.5), solid(rgba(190, 196, 208, 90)), 1.0);
        fill(&mut p, &circle(o, o, 12.0), radial(o, o, 12.0, vec![
            GradientStop::new(0.0, metal(0.35)),
            GradientStop::new(1.0, metal(0.1)),
        ]));
        stroke(&mut p, &circle(o, o, 12.0), solid(rgba(190, 196, 208, 70)), 0.8);
        for k in 0..3 {
            let a = k as f32 * TAU / 3.0 + 0.3;
            let (x, y) = (o + 24.0 * a.cos(), o + 24.0 * a.sin());
            fill(&mut p, &circle(x, y, 3.2), solid(rgba(4, 4, 5, 255)));
            stroke(&mut p, &circle(x, y, 3.2), solid(rgba(170, 176, 188, 110)), 0.7);
        }
        p
    }

    /// The room light (from the top left) over the impeller's area, in sprite coordinates.
    fn draw_room(&self) -> Pixmap {
        let mut p = Pixmap::new(SPRITE, SPRITE).unwrap();
        let o = SPRITE as f32 / 2.0;
        let (x, y) = (o - 85.0, o - 100.0);
        fill(&mut p, &circle(x, y, 230.0), radial(x, y, 230.0, vec![
            GradientStop::new(0.0, rgba(255, 255, 255, 70)),
            GradientStop::new(0.5, rgba(255, 255, 255, 18)),
            GradientStop::new(1.0, rgba(255, 255, 255, 0)),
        ]));
        p
    }

    /// Where the LED light catches the impeller: the blade edges (full), the tips (stronger), the faces (faint),
    /// the hub's rim. White; coloured by the light map each frame.
    fn draw_edges(&self) -> Pixmap {
        let mut p = Pixmap::new(SPRITE, SPRITE).unwrap();
        let o = SPRITE as f32 / 2.0;
        for (lead, trail) in blade_edges(o) {
            let path = outline(&lead, &trail);
            fill(&mut p, &path, solid(rgba(255, 255, 255, 25)));
            stroke(&mut p, &path, solid(rgba(255, 255, 255, 120)), 4.0);
            stroke(&mut p, &path, solid(rgba(255, 255, 255, 255)), 1.6);
            let (a, b) = (lead[lead.len() - 1], trail[trail.len() - 1]);
            let mut pb = PathBuilder::new();
            pb.move_to(a.0, a.1);
            pb.line_to(b.0, b.1);
            stroke(&mut p, &pb.finish().unwrap(), solid(rgba(255, 255, 255, 255)), 3.0);
        }
        p
    }

    /// The blades' shadow on the disc: their outline, dark and soft.
    fn draw_shadow(&self) -> Pixmap {
        let mut sil = Pixmap::new(SPRITE, SPRITE).unwrap();
        let o = SPRITE as f32 / 2.0;
        for (lead, trail) in blade_edges(o) {
            fill(&mut sil, &outline(&lead, &trail), solid(rgba(0, 0, 0, 255)));
        }
        let mut p = Pixmap::new(SPRITE, SPRITE).unwrap();
        let taps = [(0.0, 0.0), (3.0, 0.0), (-3.0, 0.0), (0.0, 3.0), (0.0, -3.0), (2.0, 2.0), (-2.0, -2.0), (2.0, -2.0), (-2.0, 2.0)];
        for (dx, dy) in taps {
            p.draw_pixmap(0, 0, sil.as_ref(), &PixmapPaint { opacity: 0.8 / taps.len() as f32, ..Default::default() }, Transform::from_translate(dx, dy), None);
        }
        p
    }

    /// Faint light bands in the water spiralling with the flow (anticlockwise).
    fn draw_swirl(&self) -> Pixmap {
        let mut p = Pixmap::new(W, W).unwrap();
        for arm in 0..5 {
            let a0 = arm as f32 * TAU / 5.0;
            let mut pb = PathBuilder::new();
            for i in 0..=80 {
                let t = i as f32 / 80.0;
                let r = 60.0 + t * (WALL - 70.0);
                let a = a0 + 1.9 * t;
                if i == 0 { pb.move_to(C + r * a.cos(), C + r * a.sin()) } else { pb.line_to(C + r * a.cos(), C + r * a.sin()) }
            }
            let path = pb.finish().unwrap();
            for (w, al) in [(30.0, 0.008), (12.0, 0.012), (3.0, 0.016)] {
                stroke(&mut p, &path, solid(rgba(220, 230, 240, (al * 255.0) as u8)), w);
            }
        }
        p
    }

    /// The LED light in the chamber: each light a soft spot just outside the view, added up (drawn small, scaled up).
    fn light_field(lights: &[(f32, f32, f32)]) -> Pixmap {
        let k = FIELD as f32 / W as f32;
        let mut small = Pixmap::new(FIELD, FIELD).unwrap();
        let n = lights.len().max(2);
        for (i, &(r, g, b)) in lights.iter().enumerate() {
            // from the upper left (30 deg above left), round the bottom, to the upper right; y grows down
            let a = PI + LED_UP - (PI + 2.0 * LED_UP) * i as f32 / (n - 1) as f32;
            let (x, y) = ((C + LED_R * a.cos()) * k, (C + LED_R * a.sin()) * k);
            let c = |a: f32| col((r, g, b), a);
            let p = Paint {
                shader: radial(x, y, 190.0 * k, vec![GradientStop::new(0.0, c(0.75)), GradientStop::new(0.25, c(0.4)), GradientStop::new(0.6, c(0.12)), GradientStop::new(1.0, c(0.0))]),
                anti_alias: true,
                blend_mode: BlendMode::Plus,
                ..Default::default()
            };
            small.fill_path(&circle(x, y, 190.0 * k), &p, FillRule::Winding, Transform::identity(), None);
        }
        let mut field = Pixmap::new(W, W).unwrap();
        field.draw_pixmap(0, 0, small.as_ref(), &PixmapPaint { quality: FilterQuality::Bilinear, ..Default::default() }, Transform::from_scale(1.0 / k, 1.0 / k), None);
        field
    }

    /// Constant red LEDs with a little life: each one breathes slightly on its own, now and then they all dip.
    pub fn red_lights(&self) -> Vec<(f32, f32, f32)> {
        let t = self.time;
        let dip = {
            let slot = (t / 0.12).floor();
            let h = ((slot * 12.9898).sin() * 43758.547).fract().abs();
            if h < 0.025 { 0.7 } else { 1.0 }
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

    fn draw_particles(&self, p: &mut Pixmap, deep: bool, field: &Pixmap, shutter: f32) {
        let fp = field.pixels();
        for q in self.particles.iter().filter(|q| q.deep == deep) {
            let (x, y) = (C + q.x, C + q.y);
            if !(0.0..W as f32).contains(&x) || !(0.0..W as f32).contains(&y) {
                continue;
            }
            let r = (q.x * q.x + q.y * q.y).sqrt();
            // inside the chamber, or on the way out through the outlet
            if r > WALL - 2.0 && !q.leaving {
                continue;
            }
            // lit by the LEDs where it is, plus a little ambient light
            let f = fp[(y as u32 * W + x as u32) as usize];
            let lit = |v: u8, amb: f32| (amb + v as f32 * 3.5).min(255.0);
            let alpha = q.life * if deep { 0.7 } else { 1.0 };
            let (cr, cg, cb) = (lit(f.red(), 70.0), lit(f.green(), 72.0), lit(f.blue(), 78.0));
            // the streak this frame: back along its motion over the exposure
            let (sx, sy) = (q.vx * shutter, q.vy * shutter);
            let len = (sx * sx + sy * sy).sqrt();
            let moving = len > q.size * 1.2;
            let mut pb = PathBuilder::new();
            pb.move_to(x, y);
            pb.line_to(x - sx, y - sy);
            let streak = pb.finish();
            if q.bubble {
                let hl = solid(col((cr.max(200.0), cg.max(200.0), cb.max(200.0)), alpha));
                if moving {
                    let k = (q.size * 3.0 / len).clamp(0.25, 1.0);
                    if let Some(s) = &streak {
                        stroke(p, s, solid(col((cr, cg, cb), alpha * 0.2 * k)), q.size * 2.0);
                        stroke(p, s, solid(col((cr, cg, cb), alpha * 0.4 * k)), q.size * 0.8);
                    }
                    fill(p, &circle(x, y, q.size * 0.45), hl);
                } else {
                    stroke(p, &circle(x, y, q.size), solid(col((cr, cg, cb), alpha * 0.75)), 0.9);
                    fill(p, &circle(x, y, q.size), solid(col((cr, cg, cb), alpha * 0.12)));
                    fill(p, &circle(x - q.size * 0.35, y - q.size * 0.35, q.size * 0.32), hl);
                }
            } else if let (true, Some(s)) = (moving, &streak) {
                stroke(p, s, solid(col((cr, cg, cb), alpha * 0.7)), q.size * 1.4);
            } else {
                fill(p, &circle(x, y, q.size), solid(col((cr, cg, cb), alpha * 0.8)));
            }
        }
    }

    /// One frame: `rpm` the pump speed, `dt` seconds since the last frame, `colour` the coolant (0xRRGGBB),
    /// `lights` the LEDs' colours along the arc, left to bottom to right (0..255; `LIGHTS` of them, or any number).
    pub fn render(&mut self, rpm: f32, dt: f32, colour: i32, lights: &[(f32, f32, f32)]) -> Pixmap {
        if colour != self.colour {
            self.colour = colour;
            self.chamber = Some(self.draw_chamber());
            self.impeller = Some(self.draw_impeller());
            self.hub = Some(self.draw_hub());
            self.room = Some(self.draw_room());
            self.edges = Some(self.draw_edges());
            self.shadow = Some(self.draw_shadow());
            self.swirl = Some(self.draw_swirl());
            let mut clip = Mask::new(W, W).unwrap();
            clip.fill_path(&circle(C, C, WALL), FillRule::Winding, true, Transform::identity());
            self.clip = Some(clip);
        }
        let dt = dt.clamp(0.0, 0.2);
        self.time += dt;
        let spin = turns_per_second(rpm) * TAU; // impeller, radians a second
        self.angle = (self.angle + spin * dt) % TAU;
        self.water = (self.water + spin * 0.3 * dt) % TAU;
        self.step_particles(spin, dt);
        let field = Self::light_field(lights);
        let clip = self.clip.as_ref().unwrap();

        let mut out = self.chamber.clone().unwrap();
        out.draw_pixmap(0, 0, self.swirl.as_ref().unwrap().as_ref(), &PixmapPaint { opacity: 1.0, blend_mode: BlendMode::Screen, quality: FilterQuality::Bilinear }, Transform::from_rotate_at(-self.water.to_degrees(), C, C), Some(clip));
        // the LEDs' light in the water, strongest at the lit walls
        out.draw_pixmap(0, 0, field.as_ref(), &PixmapPaint { opacity: 0.32, blend_mode: BlendMode::Screen, quality: FilterQuality::Nearest }, Transform::identity(), Some(clip));
        let shutter = 1.0 / 30.0; // exposure, s: a camera at the frame rate
        self.draw_particles(&mut out, true, &field, shutter);

        // the impeller, motion-blurred over this frame's turn by doubling: the sprite once, then the result laid
        // over itself turned back by half the remaining spread, so k passes give 2^k evenly spaced copies
        let blur = (spin * shutter).min(TAU);
        let passes = if blur.to_degrees() < 1.5 { 0 } else { ((blur.to_degrees() / 1.5).log2().ceil() as u32).min(6) };
        let half = SPRITE as f32 / 2.0;
        let around = |a: f32| Transform::from_rotate_at(a.to_degrees(), half, half);
        let turned = around(-self.angle); // anticlockwise
        let mut smear = Pixmap::new(SPRITE, SPRITE).unwrap();
        smear.draw_pixmap(0, 0, self.impeller.as_ref().unwrap().as_ref(), &PixmapPaint { quality: FilterQuality::Bilinear, ..Default::default() }, turned, None);
        // the light catching the edges: the edge mask turned with it, coloured by the light map where each pixel is
        let mut lit = Pixmap::new(SPRITE, SPRITE).unwrap();
        lit.draw_pixmap(0, 0, self.edges.as_ref().unwrap().as_ref(), &PixmapPaint { quality: FilterQuality::Bilinear, ..Default::default() }, turned, None);
        let off = (C - half) as i32;
        {
            let fp = field.pixels();
            let rp = self.room.as_ref().unwrap().pixels();
            let sp = smear.pixels_mut();
            for (i, e) in lit.pixels().iter().enumerate() {
                // the room light on the blade faces (fixed: the blades turn under it)
                let d = sp[i];
                if d.alpha() > 0 {
                    let l = rp[i].alpha() as u32 * d.alpha() as u32 / 255;
                    let up = |v: u8| (v as u32 + l * 3 / 4).min(255) as u8;
                    sp[i] = tiny_skia::PremultipliedColorU8::from_rgba(up(d.red()), up(d.green()), up(d.blue()), d.alpha().max(up(d.red())).max(up(d.blue()))).unwrap_or(d);
                }
                let m = e.alpha() as u32;
                if m == 0 {
                    continue;
                }
                let (x, y) = ((i as u32 % SPRITE) as i32 + off, (i as u32 / SPRITE) as i32 + off);
                if x < 0 || y < 0 || x >= W as i32 || y >= W as i32 {
                    continue;
                }
                let f = fp[(y as u32 * W + x as u32) as usize];
                let add = |s: u8, l: u8| (s as u32 + l as u32 * m * 3 / 255).min(255) as u8;
                let d = sp[i];
                let (r, g, b) = (add(d.red(), f.red()), add(d.green(), f.green()), add(d.blue(), f.blue()));
                sp[i] = tiny_skia::PremultipliedColorU8::from_rgba(r, g, b, d.alpha().max(r).max(g).max(b)).unwrap_or(d);
            }
        }
        let mut shadow = Pixmap::new(SPRITE, SPRITE).unwrap();
        shadow.draw_pixmap(0, 0, self.shadow.as_ref().unwrap().as_ref(), &PixmapPaint { quality: FilterQuality::Bilinear, ..Default::default() }, turned, None);
        let mut spread = blur;
        for pass in 0..passes {
            spread /= 2.0;
            average_turned(&mut smear, around(spread)); // earlier positions: turned back (clockwise)
            if pass < 3 {
                average_turned(&mut shadow, around(spread));
            }
        }
        let at = Transform::from_translate(C - half, C - half);
        out.draw_pixmap(0, 0, shadow.as_ref(), &PixmapPaint { quality: FilterQuality::Bilinear, ..Default::default() }, at.post_translate(5.0, 7.0), Some(clip));
        out.draw_pixmap(0, 0, smear.as_ref(), &PixmapPaint { quality: FilterQuality::Bilinear, ..Default::default() }, at, Some(clip));
        // the hub: its shadow on the blades (fixed, down right), the hub turning with the impeller (blurred like it),
        // then its shine, fixed to the lights: a soft highlight and a sharp glint from the top left, darker below right
        fill(&mut out, &circle(C + 5.0, C + 7.0, HUB + 6.0), radial(C + 5.0, C + 7.0, HUB + 6.0, vec![
            GradientStop::new(0.7, rgba(0, 0, 0, 200)),
            GradientStop::new(1.0, rgba(0, 0, 0, 0)),
        ]));
        let hh = HUB_SPRITE as f32 / 2.0;
        let hub_turn = |a: f32| Transform::from_rotate_at(a.to_degrees(), hh, hh);
        let mut hub = Pixmap::new(HUB_SPRITE, HUB_SPRITE).unwrap();
        hub.draw_pixmap(0, 0, self.hub.as_ref().unwrap().as_ref(), &PixmapPaint { quality: FilterQuality::Bilinear, ..Default::default() }, hub_turn(-self.angle), None);
        let mut spread = blur;
        for _ in 0..passes {
            spread /= 2.0;
            average_turned(&mut hub, hub_turn(spread));
        }
        out.draw_pixmap(0, 0, hub.as_ref(), &PixmapPaint { quality: FilterQuality::Bilinear, ..Default::default() }, Transform::from_translate(C - hh, C - hh), None);
        let dome = circle(C, C, HUB);
        fill(&mut out, &dome, radial(C - 14.0, C - 16.0, HUB * 1.6, vec![
            GradientStop::new(0.0, rgba(255, 255, 255, 60)),
            GradientStop::new(0.45, rgba(255, 255, 255, 8)),
            GradientStop::new(0.75, rgba(0, 0, 0, 30)),
            GradientStop::new(1.0, rgba(0, 0, 0, 120)),
        ]));
        fill(&mut out, &circle(C - 12.0, C - 14.0, 6.0), radial(C - 12.0, C - 14.0, 6.0, vec![
            GradientStop::new(0.0, rgba(255, 255, 255, 230)),
            GradientStop::new(1.0, rgba(255, 255, 255, 0)),
        ]));
        // the LEDs (below) reflected along the dome's lower edge
        let led = field.pixel(C as u32, (C + WALL - 10.0) as u32).unwrap();
        let (lr, lg, lb) = (led.red() as f32, led.green() as f32, led.blue() as f32);
        stroke(&mut out, &arc(C, C, HUB - 3.0, 0.15 * PI, 0.85 * PI), solid(col((lr * 1.4, lg * 1.4, lb * 1.4), 0.45)), 3.0);
        stroke(&mut out, &arc(C, C, HUB - 3.0, 0.3 * PI, 0.7 * PI), solid(col((lr * 1.8 + 40.0, lg * 1.8 + 40.0, lb * 1.8 + 40.0), 0.6)), 1.2);
        self.draw_particles(&mut out, false, &field, shutter);
        grade(&mut out);
        out
    }
}

/// The finish: more contrast (darks deeper, highlights brighter) and a slightly warmer balance (less blue).
fn grade(p: &mut Pixmap) {
    static LUT: std::sync::OnceLock<[[u8; 256]; 3]> = std::sync::OnceLock::new();
    let lut = LUT.get_or_init(|| {
        let mut t = [[0u8; 256]; 3];
        for (ch, gain) in [1.06f32, 1.04, 0.9].iter().enumerate() {
            for v in 0..256 {
                let x = v as f32 / 255.0;
                let y = x.powf(1.25) * 1.3; // deeper shadows, brighter highlights
                t[ch][v] = (y * gain * 255.0).clamp(0.0, 255.0) as u8;
            }
        }
        t
    });
    for px in p.pixels_mut() {
        let c = *px; // opaque frame: premultiplied = plain
        *px = tiny_skia::PremultipliedColorU8::from_rgba(lut[0][c.red() as usize], lut[1][c.green() as usize], lut[2][c.blue() as usize], 255).unwrap();
    }
}

/// `p` becomes the average of itself and itself turned by `t`: each half at half strength, added.
fn average_turned(p: &mut Pixmap, t: Transform) {
    for px in p.pixels_mut() {
        let c = *px;
        *px = tiny_skia::PremultipliedColorU8::from_rgba(c.red() / 2, c.green() / 2, c.blue() / 2, c.alpha() / 2).unwrap();
    }
    let half = p.clone();
    p.draw_pixmap(0, 0, half.as_ref(), &PixmapPaint { opacity: 1.0, blend_mode: BlendMode::Plus, quality: FilterQuality::Bilinear }, t, None);
}

/// Whether angle `a` is within `tol` of `b` (radians).
fn angle_near(a: f32, b: f32, tol: f32) -> bool {
    let d = (a - b).rem_euclid(TAU);
    d < tol || d > TAU - tol
}

/// How fast the water turns at radius `r` (radians a second, anticlockwise like the impeller): with the blades
/// inside the impeller, then a free vortex slowing towards the wall.
fn flow(r: f32, spin: f32) -> f32 {
    if r <= BLADE_OUT { spin * 0.85 } else { spin * 0.85 * (BLADE_OUT / r).powf(1.6) }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn diff(a: &Pixmap, b: &Pixmap) -> u64 {
        a.data().iter().zip(b.data()).map(|(x, y)| (*x as i32 - *y as i32).unsigned_abs() as u64).sum()
    }

    #[test]
    fn turns_with_the_pump_speed() {
        // a stopped pump's impeller stands still (the water may still drift); a running one turns, faster when faster
        let mut p = Pump::default();
        p.render(0.0, 1.0 / 30.0, 0x1E9BFF, &[]);
        p.render(0.0, 1.0 / 30.0, 0x1E9BFF, &[]);
        assert_eq!(p.angle, 0.0, "0 rpm: the impeller stands still");
        let turned = |rpm: f32| {
            let mut p = Pump::default();
            p.render(rpm, 1.0 / 30.0, 0x1E9BFF, &[]);
            let before = p.angle;
            p.render(rpm, 1.0 / 30.0, 0x1E9BFF, &[]);
            (p.angle - before).rem_euclid(TAU)
        };
        assert!((turned(1000.0) - TAU / 30.0).abs() < 1e-4, "1000 rpm: one turn a second");
        assert!(turned(3000.0) > turned(1500.0) * 1.9);
    }

    #[test]
    fn fast_spins_blur_instead_of_strobing() {
        // at speed the impeller is a smear: frames differ much less from each other than at a strobing speed would
        let mut p = Pump::default();
        let mut last = p.render(3000.0, 1.0 / 30.0, 0x1E9BFF, &[]);
        for _ in 0..3 {
            last = p.render(3000.0, 1.0 / 30.0, 0x1E9BFF, &[]);
        }
        let next = p.render(3000.0, 1.0 / 30.0, 0x1E9BFF, &[]);
        let mut slow = Pump::default();
        let s1 = slow.render(300.0, 1.0 / 30.0, 0x1E9BFF, &[]);
        let s2 = slow.render(300.0, 1.0 / 30.0, 0x1E9BFF, &[]);
        assert!(diff(&last, &next) > 0, "it moves");
        assert!(diff(&last, &next) < diff(&s1, &s2) * 3, "blurred at speed, not jumping");
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

    /// 2026-10-15: the LEDs light the pump head from the left, bottom and right (not the top): the water glows
    /// there and in their colour; Aura Sync's colours replace the red.
    #[test]
    fn lit_from_three_sides_in_the_leds_colour() {
        let frame = |lights: &[(f32, f32, f32)]| {
            let mut p = Pump::default();
            p.render(0.0, 1.0 / 30.0, 0x00D1FF, lights)
        };
        let dark = frame(&[(0.0, 0.0, 0.0); LIGHTS]);
        let red = frame(&[(255.0, 20.0, 15.0); LIGHTS]);
        // inside the wall: a strip near the bottom, the left, the right and the top
        let bottom = (|p: &Pixmap| mean(p, 200, 400, 80, 20).0)(&red) - mean(&dark, 200, 400, 80, 20).0;
        let left = mean(&red, 40, 200, 20, 80).0 - mean(&dark, 40, 200, 20, 80).0;
        let right = mean(&red, 420, 200, 20, 80).0 - mean(&dark, 420, 200, 20, 80).0;
        let top = mean(&red, 200, 60, 80, 20).0 - mean(&dark, 200, 60, 80, 20).0;
        assert!(bottom > 25.0 && left > 25.0 && right > 25.0, "lit sides gain red: {bottom} {left} {right}");
        assert!(top < bottom / 3.0, "the top stays dark: {top} vs {bottom}");
        let green = frame(&[(20.0, 255.0, 40.0); LIGHTS]);
        let g = mean(&green, 200, 400, 80, 20);
        let r = mean(&red, 200, 400, 80, 20);
        assert!(g.1 > r.1 + 25.0 && g.0 < r.0 - 25.0, "Aura's green replaces the red: {g:?} vs {r:?}");
    }

    /// 2026-10-15: the water comes in at the top left, goes round anticlockwise (left, bottom, right) and leaves at
    /// the top right; some particles wander on their own, also against the flow.
    #[test]
    fn water_flows_from_the_inlet_round_to_the_outlet() {
        let mut p = Pump::default();
        let (mut exits, mut stray_exits, mut ccw, mut cw, mut against) = (0, 0, 0, 0, 0);
        for _ in 0..600 {
            let before: Vec<(f32, f32, bool)> = p.particles.iter().map(|q| (q.y.atan2(q.x), (q.x * q.x + q.y * q.y).sqrt(), q.leaving)).collect();
            p.render(1500.0, 1.0 / 30.0, 0x00D1FF, &[]);
            for (q, (a0, _, leaving)) in p.particles.iter().zip(&before) {
                let a1 = q.y.atan2(q.x);
                let d = (a1 - a0 + PI).rem_euclid(TAU) - PI; // signed turn this frame
                match q.kind {
                    Kind::Flow if !q.leaving && q.life >= 1.0 => {
                        if d < -1e-4 { ccw += 1 } else if d > 1e-4 { cw += 1 }
                    }
                    Kind::Wander if d > 1e-3 => against += 1,
                    _ => {}
                }
                // a particle that was leaving and now is a new one (life restarted) went out where it was
                if *leaving && q.life < 0.2 && !q.leaving && q.age < 0.1 {
                    if angle_near(*a0, OUTLET, 0.3) { exits += 1 } else { stray_exits += 1 }
                }
            }
        }
        assert!(ccw > cw * 20, "the flow turns anticlockwise: {ccw} vs {cw}");
        assert!(exits > 10, "water leaves at the outlet: {exits}");
        assert_eq!(stray_exits, 0, "and nowhere else");
        assert!(against > 0, "wanderers sometimes go against the flow");
        // after 20 s the mix still holds: most follow the flow, some wander, some are in the impeller (the wanderers
        // once piled up until nearly all particles were wanderers)
        let count = |k: Kind| p.particles.iter().filter(|q| q.kind == k).count();
        let (flow, wander, caught) = (count(Kind::Flow), count(Kind::Wander), count(Kind::Caught));
        assert!(flow * 2 > PARTICLES && wander > 0 && wander * 3 < PARTICLES && caught > 0, "flow {flow}, wander {wander}, caught {caught}");
    }

    /// 2026-10-15: the impeller is dark metal, so the LEDs' light (not the metal) carries the colour.
    #[test]
    fn the_impeller_is_dark_metal() {
        let mut p = Pump::default();
        p.render(0.0, 1.0 / 30.0, 0x00D1FF, &[]);
        let sprite = p.impeller.as_ref().unwrap();
        let (sum, n) = sprite.pixels().iter().filter(|c| c.alpha() == 255).fold((0u64, 0u64), |(s, n), c| (s + (c.red() as u64 + c.green() as u64 + c.blue() as u64) / 3, n + 1));
        assert!(n > 1000 && sum / n < 70, "dark: mean {} over {n} px", sum / n.max(1));
    }

    /// 2026-10-15: the hub's shadow turned with the impeller. Light and shadow come from fixed directions, so
    /// nothing directional may be baked into what turns: the blades turned by a seventh of a turn, and the hub
    /// (three bolts) by a third, must look exactly as before.
    #[test]
    fn nothing_that_turns_carries_light_or_shadow() {
        let mut p = Pump::default();
        p.render(0.0, 1.0 / 30.0, 0x00D1FF, &[]);
        let same_turned = |sprite: &Pixmap, by: f32| {
            let (w, h) = (sprite.width() as f32 / 2.0, sprite.height() as f32 / 2.0);
            let mut t = Pixmap::new(sprite.width(), sprite.height()).unwrap();
            t.draw_pixmap(0, 0, sprite.as_ref(), &PixmapPaint { quality: FilterQuality::Bilinear, ..Default::default() }, Transform::from_rotate_at(by.to_degrees(), w, h), None);
            // compare the two (resampling smooths edges a little: allow a small mean difference)
            let d: u64 = t.data().iter().zip(sprite.data()).map(|(a, b)| (*a as i32 - *b as i32).unsigned_abs() as u64).sum();
            d as f64 / sprite.data().len() as f64
        };
        let blades = same_turned(p.impeller.as_ref().unwrap(), TAU / BLADES as f32);
        let hub = same_turned(p.hub.as_ref().unwrap(), TAU / 3.0);
        assert!(blades < 2.0, "blades look the same a seventh of a turn on: {blades:.2}");
        assert!(hub < 2.0, "the hub looks the same a third of a turn on: {hub:.2}");
        // and a light from one side would show: the old hub, shadow and shine baked in, fails this
        let mut lopsided = p.hub.as_ref().unwrap().clone();
        fill(&mut lopsided, &circle(HUB_SPRITE as f32 / 2.0 - 12.0, HUB_SPRITE as f32 / 2.0 - 14.0, 8.0), solid(rgba(255, 255, 255, 200)));
        assert!(same_turned(&lopsided, TAU / 3.0) > hub * 2.0 + 0.5, "the check notices a baked-in highlight");
    }

    /// 2026-10-15: the LEDs' light reaches up the left and right sides (30 deg above the horizontal), the top
    /// centre stays dark.
    #[test]
    fn light_reaches_up_the_sides() {
        let frame = |lights: &[(f32, f32, f32)]| {
            let mut p = Pump::default();
            p.render(0.0, 1.0 / 30.0, 0x00D1FF, lights)
        };
        let dark = frame(&[(0.0, 0.0, 0.0); LIGHTS]);
        let red = frame(&[(255.0, 20.0, 15.0); LIGHTS]);
        let gain = |x, y| mean(&red, x, y, 16, 30).0 - mean(&dark, x, y, 16, 30).0;
        // the sides well above the middle (y 110-140), the top centre
        let (upper_left, upper_right, top) = (gain(30, 110), gain(434, 110), gain(232, 12));
        assert!(upper_left > 15.0 && upper_right > 15.0, "lit up the sides: {upper_left} {upper_right}");
        assert!(top < upper_left / 3.0, "the top centre stays dark: {top}");
    }

    /// 2026-10-15: the finish adds contrast and warmth: darks go deeper, highlights brighter, blue a tenth down.
    #[test]
    fn grade_adds_contrast_and_warmth() {
        let mut p = Pixmap::new(3, 1).unwrap();
        for (i, v) in [40u8, 200, 128].iter().enumerate() {
            p.pixels_mut()[i] = tiny_skia::PremultipliedColorU8::from_rgba(*v, *v, *v, 255).unwrap();
        }
        grade(&mut p);
        let (dark, bright, mid) = (p.pixels()[0], p.pixels()[1], p.pixels()[2]);
        assert!(dark.green() < 40 && bright.green() > 200, "contrast: {} {}", dark.green(), bright.green());
        assert!(mid.blue() < mid.green() && mid.green() <= mid.red(), "warmer: {} {} {}", mid.red(), mid.green(), mid.blue());
    }

    /// Without Aura Sync the light is red with a little life: it varies, stays red, never goes dark.
    #[test]
    fn red_light_flickers_but_stays_red() {
        let mut p = Pump::default();
        let mut seen = vec![];
        for _ in 0..300 {
            p.render(0.0, 1.0 / 30.0, 0x00D1FF, &[]);
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

    /// Frames to look at: `TTLCD_DUMP=<dir> cargo test -p ttlcd-core pump_dump -- --ignored --nocapture`
    #[test]
    #[ignore]
    fn pump_dump() {
        let Some(dir) = std::env::var_os("TTLCD_DUMP") else { return };
        let save = |f: &Pixmap, name: String| {
            let img = image::RgbaImage::from_raw(f.width(), f.height(), f.data().to_vec()).unwrap();
            img.save(std::path::Path::new(&dir).join(name)).unwrap();
        };
        let run = |rpm: f32, colour: i32, lights: &dyn Fn(&Pump) -> Vec<(f32, f32, f32)>, frames: usize| {
            let mut p = Pump::default();
            let mut f = None;
            let t = std::time::Instant::now();
            for _ in 0..frames {
                let l = lights(&p);
                f = Some(p.render(rpm, 1.0 / 30.0, colour, &l));
            }
            (f.unwrap(), t.elapsed().as_secs_f64() * 1000.0 / frames as f64)
        };
        let red = |p: &Pump| p.red_lights();
        for rpm in [0.0, 600.0, 1500.0, 3000.0] {
            let (f, ms) = run(rpm, 0x00D1FF, &red, 40);
            println!("{rpm} rpm: {ms:.1} ms a frame");
            save(&f, format!("pump-{rpm}.png"));
        }
        // lit by Aura: a rainbow along the arc, and one colour
        let rainbow = |_: &Pump| (0..LIGHTS).map(|i| {
            let h = i as f32 / LIGHTS as f32 * 6.0;
            let x = 255.0 * (1.0 - (h % 2.0 - 1.0).abs());
            match h as i32 { 0 => (255.0, x, 0.0), 1 => (x, 255.0, 0.0), 2 => (0.0, 255.0, x), 3 => (0.0, x, 255.0), 4 => (x, 0.0, 255.0), _ => (255.0, 0.0, x) }
        }).collect();
        save(&run(1500.0, 0x00D1FF, &rainbow, 20).0, "pump-aura-rainbow.png".into());
        let green = |_: &Pump| vec![(40.0, 255.0, 80.0); LIGHTS];
        save(&run(1500.0, 0x00D1FF, &green, 20).0, "pump-aura-green.png".into());
        // every coolant colour Studio offers, at 1500 rpm
        for colour in crate::model::PALETTE {
            save(&run(1500.0, colour, &red, 20).0, format!("pump-colour-{colour:06X}.png"));
        }
    }
}
