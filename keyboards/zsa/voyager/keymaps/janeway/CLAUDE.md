# janeway — Voyager keymap

Zack's travel board. **Mac-only** — `host_os` never leaves `OS_MAC`; no Windows Alt-code path. Sibling to `zoolander` (Moonlander, desk). Shared conventions (layer model, Z_* keycodes, tap-hold scheme, indicator colors, tuning playbook) live in the repo-root `CLAUDE.md`; this file is only the Voyager-specific spin.

Bootstrapped from a ZSA Oryx layout (Oryx ID `7megA/eaLGdm`, named "janeway") then hand-evolved away from the Oryx defaults. The Oryx source still lives in `~/Downloads/zsa_voyager_7megA_eaLGdm_janeway_source/` for reference.

> **Migration status:** janeway is NOT yet wired onto `users/zack/` — it carries its own (older, Mac-only) copies of the shared code in `keymap.c`. The planned upgrade: set `USER_NAME := zack`, adopt the shared `Z_*` enum + `process_record_zack()` + painters, and replace the simplified NAV/SEL bindings with the full `os_bindings_t` Vim engine (word/para/line/doc moves, `Z__VIMO`, `Z__HBCK`/`Z__HFWD` history nav, shift-inverted arrows). Also queued for the same pass, to match decisions already made on zoolander: right inner thumb becomes `LT(SYM, KC_BSPC)` (SYM hold for comfortable left-hand grave; backspace tap unchanged), META moves to the shared `NAV+M` → `TO(META)` gateway with a top-left `TO(BASE)` exit, and Lock/Mission Control move from META to FN (`Z__LOCK` on Q, `Z__MCTL` on the left thumb space; play/pause stays on the right thumb space). Until then, expect naming drift (`Z_VIMG` here vs `Z__VIMG` shared).

## Files

- `keymap.c` — everything: layers, custom keycodes, tap-dance callbacks, META illumination, `process_record_user`, per-key tap-hold callbacks. Single file.
- `config.h` — local copy of the shared tap-hold settings + `SERIAL_NUMBER`, `FIRMWARE_VERSION`.
- `rules.mk` — `TAP_DANCE_ENABLE`, `CAPS_WORD_ENABLE`, `CONSOLE_ENABLE`.
- `keymap.json` — declares modules `zsa/defaults` (provides `ZSA_SAFE_RANGE`, `LED_LEVEL`, `TOGGLE_LAYER_COLOR`) and `zsa/navigator_trackpad` (digitizer driver for the trackpad add-on, ~7KB).

## Build & flash

```sh
qmk compile -kb zsa/voyager -km janeway
qmk flash    -kb zsa/voyager -km janeway       # waits for bootloader
```

ZSA custom bootloader (`3297:0791`, app at 0x08002000). Trigger: hold `F` (FN), tap top-left for `QK_BOOT`, or the reset pinhole on either half. Flash on a direct USB connection, not through the dock.

## Layer map (Voyager-specific positions)

| # | Layer | Activated by | Notes vs the shared model |
|---|-------|--------------|---------------------------|
| 0 | `BASE` | default | 14 mod-taps incl. thumb Cmd (see below). |
| 1 | `SYM` | left outer thumb `OSL(SYM)` | Bracket stack on the inner-reach column (braces/brackets/parens/angles top-to-bottom — the Moonlander has these on BASE inner columns instead). Quotes on O/P, dash/dot dances, grave top-left, `\` on backspace, `'` on the quote position. |
| 2 | `META` | right inner thumb `LT(META, KC_BSPC)` | Same grid as zoolander's META minus audio feedback (no speaker). Lock/Mission Control are raw Mac chords here (shared `Z__LOCK`/`Z__MCTL` come with the migration). |
| 3 | `NAV` | top-row pinky `LT(NAV, KC_TAB)` | Simplified engine: H/J/K/L arrows, A/E line edges, G doc edges (`Z_VIMG`), W/B word skip, P paste, V → `TO(SEL)`. |
| 4 | `SEL` | NAV→V | Shift-wrapped movement, C/X/Y/P act + return to BASE, all unmapped positions `KC_NO`. |
| 5 | `FN` | `LT(FN, KC_F)` | F1–F11 top row, media on H/J/K/L positions, play/pause right outer thumb, `QK_BOOT` top-left. |

## BASE specifics — where janeway differs from zoolander

The Voyager has no bottom row, so modifiers that live on zoolander's bottom row are mod-taps here:

- **Thumb Cmd is the only easy Cmd source**: `MT(MOD_LGUI, KC_SPACE)` both thumbs. V and M are deliberately plain letters — this teaches the thumb-Cmd habit. These two thumbs are in the snap-fire list (`get_hold_on_other_key_press`) and rely on flow-tap for roll protection — a deliberate deviation from the pinkies-only snap list on zoolander.
- Letter mods: `MT(MOD_LALT, KC_Z)`, `MT(MOD_RALT, KC_SLASH)`.
- Pinkies: `LCTL_T(KC_ESC)`, `LCTL_T(KC_ENT)`, `RSFT_T(KC_MINS)` — same as zoolander.
- Bottom-left `KC_LSFT` literal (double-tap = caps_word). Keep it a real modifier — OSL stays armed only for true modifier keycodes.

## Trackpad

The Navigator trackpad add-on is driven by the `zsa/navigator_trackpad` community module declared in `keymap.json` — no rules.mk config needed. Managed by upstream; don't hand-edit the module.

## Things to leave alone

- The vendored QMK tree and `modules/zsa/*`.
- Plain V and M on BASE (thumb-Cmd discipline).
- The literal `KC_LSFT` (OSL arming).
- The `Z_START`/`Z_END` sentinels (indicator paint).
