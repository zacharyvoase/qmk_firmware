# zoolander — Moonlander keymap

Zack's desk board. Dual-OS (runtime Mac/Win toggle), speaker, gaming layer. Shared conventions (layer model, Z_* keycodes, tap-hold scheme, indicator colors, tuning playbook) live in the repo-root `CLAUDE.md` and `users/zack/`; this file is only the Moonlander-specific spin.

**Hardware is rev A — build `zsa/moonlander/reva`, never `revb`.** See the root CLAUDE.md "Building and flashing" for why (wrong-offset images cold-boot dead).

## Files

- `keymap.c` — layers (shared six + `GAM`), `TD_AIRPODS`, audio-flavored debug/EE_CLR dances, GAM spiral/WASD lighting, per-key tap-hold lists, board `process_record_user` (OS toggle + mode songs) delegating to `process_record_zack()`
- `config.h` — board-only defines (`FIRMWARE_VERSION`, RGB matrix defaults, `USE_OKLAB_CURVE`)
- `rules.mk` — `USER_NAME := zack` + board feature flags
- `songs.h` — all audio cues (the Voyager has no speaker, so songs stay board-local)
- `keymap.json` — declares the `zsa/defaults` module (provides `ZSA_SAFE_RANGE`)

## Board-specific layout notes

- **Bottom row mods mirror outward from the index finger on both hands**: index = Cmd (under V / under M), ring = Alt/Option, pinky = Ctrl. These are plain modifiers — no tap-hold resolution. Useful for one-handed chords with the other hand on the mouse. Middle fingers: left (between LALT and LGUI) = `MO(QSL)` quick-select, right = unassigned.
- **QSL (quick-select)**: hold the left-middle bottom-row key for SEL's shift-wrapped movement (h/j/k/l, w/b, a/e, g/G, [/]) with zero ceremony — release and you're back on BASE. No clipboard actions, no sticky state; it shares `zack_process_selection_movement` with SEL. Caveat: QSL+E (line-end) shares the middle finger with the hold key.
- **Thumb clusters mirror as space / SYM / META**: left linear = `Space / OSL(SYM) / MO(META)`, right linear = `MO(META) / LT(SYM, Backspace) / Space` (the LAYOUT macro lists the right half mirrored — space sits on the physical mirror of the left space). The right-hand SYM hold exists so left-hand SYM targets (grave at top-left especially) don't need a same-hand thumb+pinky stretch. Big reds: left = `TD_AIRPODS` (1=play/pause, 2=next, 3=prev), right = `TO(GAM)`. Deliberate divergence from janeway, whose backspace thumb holds META.
- **Inner columns carry the bracket stack on BASE**, same vertical order as janeway's SYM column: `{}` row 1, `[]` row 2, `()` home row.
- **SYM has a right-hand calculator numpad**: 789 on Y/U/I, 456 on H/J/K (5 on the index home key), 123 on N/M/,, 0 on the right thumb space. Quotes at O/P, dash/dot dances at `-`/`.`, grave top-left, `\` on backspace, `'` on the quote key.
- **GAM layer**: entered/exited via the right big red (`TO(GAM)`/`TO(BASE)`), plays mode songs, swaps RGB to spiral + white WASD, and overrides every mod-tap with plain keycodes (Tab, Ctrl, Shift, F, Z, /, Enter, thumb backspace).
- **FN carries the system actions**: `Z__LOCK` on Q, `Z__MCTL` on the left thumb space, OS toggle (`Z_OSWIN` on the Ctrl/Esc position, `Z_OSMAC` on the V-column bottom-row position; each plays its song), `QK_BOOT` top-left, `QK_RBT` on the right big red, play/pause on the right thumb space. F1–F11 only — F12–F15 were dropped for the Q-row real estate.
- **META is reachable three ways**: `MO(META)` on both clusters' third thumb keys (momentary) and `NAV+M` (`TO(META)`, sticky — exit via the top-left Esc position, mirroring SEL's exit). Lock/Mission Control moved OFF META to FN.
- **Audio cues** (`songs.h`): Mac/Win mode, GAM enter/exit, debug on/off chirps, caps-word ticks (`caps_word_set_user`), EE_CLR danger jingle (played and waited out before the reset fires).
- **Snap-fire tap-hold list** (board's `get_*` callbacks): the three pinky mods — `LCTL_T(KC_ESC)`, `LCTL_T(KC_ENT)`, `RSFT_T(KC_MINS)` — plus `LT(NAV, KC_TAB)`: NAV chords often release Tab while a movement key is still held, which permissive hold misreads as a tap. Everything else permissive.
- **Layer-dimming trick** (`layer_state_set_user`): on SYM/NAV/SEL/FN the base animation dims so painted classes pop; restored on BASE/GAM; META skipped (it paints every key itself and its RGB knob changes must persist).

## Things to leave alone

- `KC_GRV` on the bottom-left outer: transition crutch while the SYM+top-left grave habit settles; remove only when asked.
- The right big red as the gaming toggle and the left big red as the media key — muscle memory.
- Plain bottom-row mods — do not convert to mod-taps.
