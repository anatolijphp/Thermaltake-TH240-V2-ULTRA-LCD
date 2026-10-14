//! Tray icon (notification area on Windows, AppIndicator on Linux) with the same menu as the C# Studio:
//! Open TT LCD Studio / Mode ▸ / Pause / Game mode / Exit.
use crate::UserEvent;
use tao::event_loop::EventLoopProxy;
use tray_icon::menu::{CheckMenuItem, Menu, MenuEvent, MenuId, MenuItem, PredefinedMenuItem, Submenu};
use tray_icon::{MouseButton, MouseButtonState, TrayIcon, TrayIconBuilder, TrayIconEvent};
use ttlcd_core::{engine, model, Engine, Mode};

pub enum Action {
    Open,
    Mode(Mode),
    Pause,
    Game,
    Exit,
}

pub struct Tray {
    icon: TrayIcon,
    open: MenuItem,
    modes: Vec<(Mode, CheckMenuItem)>,
    pause: CheckMenuItem,
    game: CheckMenuItem,
    exit: MenuItem,
    tip: String,
}

impl Tray {
    /// None when the system has no tray (Linux without an AppIndicator library): closing the window then quits.
    pub fn create(rgba: Vec<u8>, w: u32, h: u32, proxy: EventLoopProxy<UserEvent>) -> Option<Tray> {
        let built = std::panic::catch_unwind(std::panic::AssertUnwindSafe(|| Tray::build(rgba, w, h)));
        let tray = match built {
            Ok(Some(t)) => t,
            _ => return None, // tray-icon panics when libayatana-appindicator3 is missing
        };
        let p = proxy.clone();
        MenuEvent::set_event_handler(Some(move |e: MenuEvent| {
            let _ = p.send_event(UserEvent::Menu(e.id));
        }));
        TrayIconEvent::set_event_handler(Some(move |e: TrayIconEvent| {
            if let TrayIconEvent::Click { button: MouseButton::Left, button_state: MouseButtonState::Up, .. } = e {
                let _ = proxy.send_event(UserEvent::Show);
            }
        }));
        Some(tray)
    }

    fn build(rgba: Vec<u8>, w: u32, h: u32) -> Option<Tray> {
        let menu = Menu::new();
        let open = MenuItem::new("Open TT LCD Studio", true, None);
        let sub = Submenu::new("Mode", true);
        let mut modes = Vec::new();
        for &m in Mode::ALL {
            if !model::supports(m) {
                continue;
            }
            let it = CheckMenuItem::new(engine::label(m), true, false, None);
            sub.append(&it).ok()?;
            modes.push((m, it));
        }
        let pause = CheckMenuItem::new("Pause", true, false, None);
        let game = CheckMenuItem::new("Game mode", true, false, None);
        let exit = MenuItem::new("Exit", true, None);
        menu.append_items(&[&open, &PredefinedMenuItem::separator(), &sub, &pause, &game, &PredefinedMenuItem::separator(), &exit]).ok()?;
        let icon = tray_icon::Icon::from_rgba(rgba, w, h).ok()?;
        let icon = TrayIconBuilder::new().with_menu(Box::new(menu)).with_icon(icon).with_tooltip("TT LCD Studio").with_title("TT LCD Studio").build().ok()?;
        Some(Tray { icon, open, modes, pause, game, exit, tip: String::new() })
    }

    pub fn action(&self, id: &MenuId) -> Option<Action> {
        if id == self.open.id() {
            return Some(Action::Open);
        }
        if id == self.pause.id() {
            return Some(Action::Pause);
        }
        if id == self.game.id() {
            return Some(Action::Game);
        }
        if id == self.exit.id() {
            return Some(Action::Exit);
        }
        self.modes.iter().find(|(_, it)| it.id() == id).map(|(m, _)| Action::Mode(*m))
    }

    /// Keeps the check marks and the tooltip in step with the engine (every second and after changes).
    pub fn sync(&mut self, e: &Engine, aura_ready: bool) {
        let c = e.config();
        for (m, it) in &self.modes {
            let on = c.Mode == *m;
            if it.is_checked() != on {
                it.set_checked(on);
            }
            if *m == Mode::Aura {
                it.set_enabled(on || aura_ready || cfg!(windows)); // Linux: Aura needs armory-crated
            }
        }
        if self.pause.is_checked() != e.paused() {
            self.pause.set_checked(e.paused());
        }
        if self.game.is_checked() != e.force_game() {
            self.game.set_checked(e.force_game());
        }
        let mut tip = format!("TT LCD - {}", e.status());
        if tip.chars().count() > 63 {
            tip = tip.chars().take(63).collect();
        }
        if tip != self.tip {
            let _ = self.icon.set_tooltip(Some(&tip));
            self.tip = tip;
        }
    }
}
