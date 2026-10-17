//! The setup window's layout in 96-DPI pixels (x, y, width, height); ui.rs scales it to the system DPI.
//! Step 0: folder and options. Then one step per optional component (media::COMPONENTS): its GIF, what it does and
//! the checkbox that installs it. The last step's button installs.
pub type R = (i32, i32, i32, i32);

pub const CLIENT: (i32, i32) = (560, 440);
// shared header: title, RGB line, subtitle, section label
pub const TITLE: R = (24, 20, 512, 36);
pub const RGB_LINE: R = (24, 58, 512, 2);
pub const SUBTITLE: R = (24, 70, 512, 22);
pub const SECTION: R = (24, 106, 512, 18); // "INSTALL TO" / "OPTIONAL COMPONENT 1 OF 3"
// step 0
pub const DIR_BOX: R = (24, 126, 420, 28);
pub const BROWSE: R = (452, 126, 84, 28);
pub const OPTIONS_LABEL: R = (24, 170, 300, 18);
pub const CHECK_Y: i32 = 192;
pub const CHECK_H: i32 = 26;
// a component step
pub const CARD: R = (24, 132, 512, 196);
pub const GIF: R = (32, 140, 180, 180);
pub const NAME: R = (228, 146, 296, 28);
pub const TEXT: R = (228, 180, 296, 100);
pub const CHECK: R = (228, 290, 296, 26);
pub const NOTE: R = (24, 340, 512, 22);
// bottom: progress, status, buttons
pub const BAR: R = (24, 374, 512, 4);
pub const STATUS: R = (24, 386, 290, 44);
pub const BACK: R = (326, 390, 100, 34);
pub const INSTALL: R = (436, 390, 100, 34);

/// Folder and options, then one step per component.
pub const STEPS: usize = 1 + crate::media::COMPONENT_COUNT;

/// The step that offers component `i`.
pub fn component_step(i: usize) -> usize {
    1 + i
}

/// The component a step offers (None on the folder step).
pub fn step_component(step: usize) -> Option<usize> {
    (1..STEPS).contains(&step).then(|| step - 1)
}

/// The window's controls by kind, for which step shows them.
#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub enum Control {
    /// the folder box and Browse
    Folder,
    /// step 0's checkboxes
    Option,
    /// component i's checkbox
    Component(usize),
    Back,
    /// Next, Install on the last step, Close when done
    Install,
}

/// Whether `step` shows control `c`; the others are hidden.
pub fn shown(c: Control, step: usize) -> bool {
    match c {
        Control::Folder | Control::Option => step == 0,
        Control::Component(i) => step == component_step(i),
        Control::Back => step > 0,
        Control::Install => true,
    }
}

pub fn option_check(i: usize) -> R {
    (24, CHECK_Y + i as i32 * CHECK_H, 512, CHECK_H)
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::media::COMPONENTS;

    fn inside(a: R, b: R) -> bool {
        a.0 >= b.0 && a.1 >= b.1 && a.0 + a.2 <= b.0 + b.2 && a.1 + a.3 <= b.1 + b.3
    }

    fn overlap(a: R, b: R) -> bool {
        a.0 < b.0 + b.2 && b.0 < a.0 + a.2 && a.1 < b.1 + b.3 && b.1 < a.1 + a.3
    }

    /// 2026-10-17: the first build of the stepped setup created its buttons hidden and showed only each step's own
    /// controls, so no step had a Next / Install button. Every step must have the button that moves on, every step
    /// after the first a way back, and each step its own controls only: one component per step.
    #[test]
    fn every_step_can_move_on() {
        assert_eq!(STEPS, 4, "folder + CPU temperature, Armoury Crate, Wallpaper Engine");
        for step in 0..STEPS {
            assert!(shown(Control::Install, step), "step {step} has no Next / Install");
            assert_eq!(shown(Control::Back, step), step > 0, "step {step}: Back");
            assert_eq!(shown(Control::Folder, step), step == 0);
            assert_eq!(shown(Control::Option, step), step == 0);
            let offered: Vec<usize> = (0..COMPONENTS.len()).filter(|i| shown(Control::Component(*i), step)).collect();
            assert_eq!(offered, step_component(step).into_iter().collect::<Vec<_>>(), "step {step}");
        }
        for i in 0..COMPONENTS.len() {
            assert_eq!(step_component(component_step(i)), Some(i));
        }
        assert_eq!(step_component(0), None);
        assert_eq!(step_component(STEPS), None);
    }

    /// 2026-10-17: the stepped setup was 560 px tall with ~120 px empty under the content. The window is as tall as
    /// its content: the bottom row sits right under the tallest step (a component step's note), with the usual
    /// margins and nothing cut off.
    #[test]
    fn no_empty_space() {
        let content_end = [CARD, NOTE, option_check(3)].iter().map(|r| r.1 + r.3).max().unwrap();
        assert!(BAR.1 > content_end && BAR.1 - content_end <= 16, "gap above the progress bar: {}", BAR.1 - content_end);
        let bottom = [BAR, STATUS, BACK, INSTALL].iter().map(|r| r.1 + r.3).max().unwrap();
        assert!(bottom <= CLIENT.1, "cut off: {bottom} > {}", CLIENT.1);
        assert!(CLIENT.1 - bottom <= 16, "{} px empty at the bottom", CLIENT.1 - bottom);
        assert!(STATUS.3 >= 2 * 16, "room for a two-line error");
        assert!(NOTE.3 >= 16, "room for the note");
    }

    /// 2026-10-17, a step per component: the GIF, name, text and checkbox sit in the card without overlapping, and
    /// nothing reaches the progress bar, status or buttons.
    #[test]
    fn component_step_fits() {
        let client = (0, 0, CLIENT.0, CLIENT.1);
        let bottom = [BAR, STATUS, BACK, INSTALL];
        let parts = [GIF, NAME, TEXT, CHECK];
        assert!(inside(CARD, client) && !overlap(SECTION, CARD) && !overlap(NOTE, CARD));
        for (i, a) in parts.iter().enumerate() {
            assert!(inside(*a, CARD), "{a:?} outside the card");
            for b in &parts[i + 1..] {
                assert!(!overlap(*a, *b), "{a:?} overlaps {b:?}");
            }
        }
        assert!(TEXT.3 >= 5 * 16, "room for five lines of text");
        for b in bottom {
            assert!(inside(b, client), "{b:?}");
            assert!(!overlap(CARD, b) && !overlap(NOTE, b), "{b:?}");
        }
        for i in 0..4 {
            assert!(inside(option_check(i), client) && !overlap(option_check(i), BAR));
        }
        for (a, b) in [(STATUS, BACK), (STATUS, INSTALL), (BACK, INSTALL)] {
            assert!(!overlap(a, b), "{a:?} {b:?}");
        }
    }
}
