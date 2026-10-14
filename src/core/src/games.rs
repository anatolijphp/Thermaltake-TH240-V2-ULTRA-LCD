//! Game detection, using only what Windows provides (see win/games.rs for the sources). Other systems: none (game
//! mode can still be forced on by hand).
use crate::config::Config;

/// Name of the detected game (exe without extension), or None.
pub fn detect(c: &Config) -> Option<String> {
    #[cfg(windows)]
    {
        crate::win::games::detect(c)
    }
    #[cfg(not(windows))]
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
