//! Game detection: Windows sources in win/games.rs, Linux sources in linux_games.rs (game mode can always be forced
//! on by hand too).
use crate::config::Config;

/// Name of the detected game (exe without extension), or None.
pub fn detect(c: &Config) -> Option<String> {
    #[cfg(windows)]
    {
        crate::win::games::detect(c)
    }
    #[cfg(target_os = "linux")]
    {
        crate::linux_games::detect(c)
    }
    #[cfg(not(any(windows, target_os = "linux")))]
    {
        let _ = c;
        None
    }
}

/// Games Windows knows about on this PC; -1 = not applicable.
pub fn known_game_count() -> i32 {
    #[cfg(windows)]
    {
        crate::win::games::known_game_count()
    }
    #[cfg(not(windows))]
    -1
}
