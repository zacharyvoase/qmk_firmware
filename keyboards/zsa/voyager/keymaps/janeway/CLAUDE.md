# janeway — Voyager keymap

Zack's travel board. **Mac-only** — `host_os` never leaves `OS_MAC`; no Windows Alt-code path. Sibling to `zoolander` (Moonlander, desk). Shared conventions (layer model, Z_* keycodes, tap-hold scheme, indicator colors, tuning playbook) live in the repo-root `CLAUDE.md`; this file is only the Voyager-specific spin.

Bootstrapped from a ZSA Oryx layout (Oryx ID `7megA/eaLGdm`, named "janeway") then hand-evolved away from the Oryx defaults. The Oryx source still lives in `~/Downloads/zsa_voyager_7megA_eaLGdm_janeway_source/` for reference.

Janeway runs entirely on the shared `users/zack/` userspace (`USER_NAME := zack`): shared layer model, `Z_*` keycodes, the full `os_bindings_t` Vim NAV/SEL engine, typography, META illumination, and tap-hold config. `keymap.c` holds only the layouts, the silent debug/EE_CLR dances, the per-key tap-hold lists, and thin indicator/process_record shims.

## Files

- `keymap.c` — layouts, silent debug/EE_CLR dances, per-key tap-hold lists, thin shims into the userspace.
- `config.h` — board-only defines (`SERIAL_NUMBER`, `FIRMWARE_VERSION`, RGB timeout, `LAYER_STATE_8BIT`).
- `rules.mk` — `USER_NAME := zack` + board feature flags.
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
| 1 | `SYM` | left outer thumb `OSL(SYM)`; right inner thumb `LT(SYM, KC_BSPC)` (backspace tap, SYM hold — for two-handed grave and right-side symbols) | Bracket stack on the inner-reach column (braces/brackets/parens/angles top-to-bottom — the Moonlander has these on BASE inner columns instead). Quotes on O/P, dash/dot dances, grave top-left, `\` on backspace, `'` on the quote position. |
| 2 | `META` | `NAV+M` → `TO(META)` (sticky; exit via top-left Esc position) | Same grid as zoolander's META minus audio feedback (no speaker). Lock/Mission Control live on FN, not here. |
| 3 | `NAV` | top-row pinky `LT(NAV, KC_TAB)` | Full shared Vim engine: H/J/K/L arrows (shift = line/page jumps), A/E line edges, W/B word skip, G doc edges (shift inverts), O = `Z__VIMO` open-line, T/Y = history back/forward, P paste, V → `TO(SEL)`, M → `TO(META)`. |
| 4 | `SEL` | NAV→V | Shift-wrapped movement, C/X/Y/P act + return to BASE, all unmapped positions `KC_NO`. |
| 5 | `FN` | `LT(FN, KC_F)` | F1–F11 top row, media on H/J/K/L positions, `Z__LOCK` on Q, `Z__MCTL` on left inner thumb, play/pause right outer thumb, `QK_BOOT` top-left. |

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
