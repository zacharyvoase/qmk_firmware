# janeway — Voyager keymap

Personal keymap for the ZSA Voyager. Sibling to the `zoolander` Moonlander keymap. The Voyager is Zack's travel board; the Moonlander stays at his desk. Janeway is **Mac-only** — no OS abstraction layer.

The keymap was bootstrapped from a ZSA Oryx layout (Oryx ID `7megA/eaLGdm`, named "janeway") then hand-evolved away from the Oryx defaults. The Oryx source still lives in `~/Downloads/zsa_voyager_7megA_eaLGdm_janeway_source/` for reference.

## Files

- `keymap.c` — all layers, custom keycodes, tap-dance callbacks, META illumination, `process_record_user`, `get_*` per-key callbacks. Single file.
- `config.h` — tap-hold timings (`TAPPING_TERM 170`, `FLOW_TAP_TERM 150`, `QUICK_TAP_TERM 0`), per-key behavior flags, `DOUBLE_TAP_SHIFT_TURNS_ON_CAPS_WORD`, `SERIAL_NUMBER`.
- `rules.mk` — `TAP_DANCE_ENABLE`, `CAPS_WORD_ENABLE`, `CONSOLE_ENABLE`, `PRECISION_TRACKPAD_ENABLE` (Navigator add-on).
- `keymap.json` — declares modules `zsa/defaults` (provides `LED_LEVEL`/`TOGGLE_LAYER_COLOR`) and `zsa/navigator_trackpad` (digitizer driver for the trackpad add-on).

## Build & flash

```sh
qmk compile -kb zsa/voyager -km janeway
qmk flash    -kb zsa/voyager -km janeway       # waits for bootloader
```

Trigger bootloader: hold `F` to engage FN layer, tap top-left for `QK_BOOT`. Or use the reset pinhole on either half. No tests; verification is "flash it and try the layers."

Binary is ~67KB (`CONSOLE_ENABLE` adds ~2KB; the navigator trackpad module adds ~7KB).

## Layer model

| # | Name   | Activated by                          | Purpose |
|---|--------|---------------------------------------|---------|
| 0 | `BASE` | default                                | Typing / insert. |
| 1 | `SYM`  | left outer thumb `OSL(SYM)` — tap = one-shot, hold = momentary | Brackets, parens, braces, angle brackets; smart curly quotes on O/P; typographic dash & ellipsis tap-dances on `-` and `.`; `` ` `` on top-left (positional analog to standard QWERTY). |
| 2 | `META` | right inner thumb `LT(META, KC_BSPC)` — tap = backspace, hold = momentary | RGB controls (custom animated illumination), system actions (Mission Control, Lock, debug toggle, EE_CLR), nav anchors (Home/PgDn/PgUp/End on H/J/K/L). |
| 3 | `NAV`  | top-row pinky `LT(NAV, KC_TAB)` — tap = tab, hold = momentary | Vim-style navigation: H/J/K/L arrows, A/E = Cmd+Left/Right (line edges), G = `Z_VIMG` (Cmd+Up/Down — top/bottom of doc, shift inverts), W/B = Alt+Right/Left (word skip), P = `Z_PASTE`, V = `TO(SEL)`. |
| 4 | `SEL`  | NAV→V (`TO(SEL)`) — sticky until exit | Vim-style visual mode. Movement keys mirror NAV but shift-wrapped. C/X/Y/P = delete/cut/copy/paste, each returns to BASE. Esc or home pinky also returns to BASE. **All unmapped positions are `KC_NO`** (no accidental text input). |
| 5 | `FN`   | F key `LT(FN, KC_F)` — tap = f, hold = momentary | F1–F11 on top row. Media keys via vim-positions on right hand (vol up/down/mute, next/prev). Play/pause on right outer thumb (space tap on BASE = space, on FN = play/pause). |

## BASE layer specifics

Mod-taps (in roughly increasing exoticism):

- `MT(MOD_LCTL, KC_ESCAPE)` — left home pinky (Ctrl tap = Esc)
- `MT(MOD_LCTL, KC_ENTER)`  — right home pinky (Ctrl tap = Enter)
- `MT(MOD_LALT, KC_Z)`      — Z (Alt)
- `LT(FN, KC_F)`            — F (FN layer)
- `MT(MOD_RALT, KC_SLASH)`  — `/` (Alt)
- `MT(MOD_RSFT, KC_MINUS)`  — bottom right outer (Shift tap = `-`)
- `LT(NAV, KC_TAB)`         — top-row left pinky (NAV layer)

Thumbs:
- **Left inner** `MT(MOD_LGUI, KC_SPACE)` — Cmd tap = space. **This is the only easy Cmd source**; V is plain `KC_V` (no Cmd mod-tap), to encourage thumb-Cmd habit.
- **Left outer** `OSL(SYM)` — tap = one-shot SYM, hold = momentary SYM.
- **Right inner** `LT(META, KC_BSPC)` — tap = backspace, hold = META.
- **Right outer** `MT(MOD_RGUI, KC_SPACE)` — Cmd tap = space.

Plain modifier:
- **Bottom-left** `KC_LSFT` — literal shift modifier. Double-tap activates `caps_word` via QMK's built-in `DOUBLE_TAP_SHIFT_TURNS_ON_CAPS_WORD`. Using the literal modifier (not a tap-dance) means OSL stays armed when shift is pressed.

Top row right pinky: `KC_EQUAL` (`+` is shift-`=`).

## Tap-hold tuning

Three QMK features cooperate:

- **`TAPPING_TERM 170`** — global decision window. Mostly matters for solo-hold-to-mod timing and tap-dance settling.
- **`FLOW_TAP_TERM 150`** — during continuous typing, suppress hold behavior on mod-taps. ~60ms safety margin past a 90ms inter-key interval (133 wpm baseline). **Pinky mods exempted** via `get_flow_tap_term` — they're physically isolated and need to be reliable mid-flow (e.g., `Ctrl+,`).
- **Per-key chord commit logic** (via `HOLD_ON_OTHER_KEY_PRESS_PER_KEY` + `PERMISSIVE_HOLD_PER_KEY`):
  - **Snap-fire** (`HOLD_ON_OTHER_KEY_PRESS = true`): the three pinky mods + both thumb Cmd sources (`MT(MOD_LGUI, KC_SPACE)`, `MT(MOD_RGUI, KC_SPACE)`). Commit to hold immediately when another key goes down. Safe because (a) pinky positions aren't typing-roll targets, and (b) thumb Cmd source is protected against typing rolls by `FLOW_TAP_TERM` instead.
  - **Permissive** (`PERMISSIVE_HOLD = true`): everything else — letter mods (Z, /), layer-taps. Commit to hold only if the other key was pressed AND released before the mod-tap release. Typing rolls naturally release the mod-tap-letter first, so they stay as TAP.

`QUICK_TAP_TERM 0` — no tap-then-hold auto-repeat on mod-tap keys.

The callbacks live in `keymap.c` (`get_hold_on_other_key_press`, `get_permissive_hold`, `get_flow_tap_term`). Add a key to the relevant switch when extending.

## SYM layer specifics

Bracket-family is on the inner-reach column (where T/Y sit on BASE), top-to-bottom: braces, brackets, parens, angle brackets. Standard mnemonic-friendly stacking.

Typography:
- **O position** → `Z_OQUOTE`: tap = `'` (left single curly, Opt+`]`); shift+tap = `"` (left double curly, Opt+`[`).
- **P position** → `Z_PQUOTE`: tap = `'` (right single curly, Opt+Shift+`]`); shift+tap = `"` (right double curly, Opt+Shift+`[`).
- **`'` position** (SCLN on BASE) → `KC_QUOTE`: plain straight `'` (shift = `"` via US layout).
- **`.` position** → `TD_DOT`: 1 tap = `.`, 2 taps = `..`, 3+ taps = `…` (Opt+`;`).
- **`-` position** → `TD_DASH`: 1 tap = `–` (en-dash, Opt+`-`), 2+ taps = `—` (em-dash, Opt+Shift+`-`).
- **Top-left** → `KC_GRAVE` (`` ` ``); positional analog to standard QWERTY.

**OSL workflow**: tap thumb → one-shot SYM for next single key. Shift presses don't consume the OSL (QMK explicitly skips modifier keycodes in OSL clearing). For tap-dances on SYM keys (`TD_DOT`, `TD_DASH`), **hold the thumb through the dance** — that keeps the PRESSED bit set and the layer stays active.

## META layer specifics

The thumb is "engaged" (held) while you're on META; tap-and-release fires the inner-thumb BSPC.

Left hand:
- Top row: brightness ↓/↑ on the inner-reach columns
- Q row: hue ↓/↑, saturation ↓/↑
- A row (home): debug toggle (TD on Ctrl/Esc position), speed ↓/↑, mode prev/next
- Z row: EE_CLR (TD on Z position), `RGB_TOG`, HSV presets (red/green/blue)
- Q position: Lock screen (`LCTL(LCMD(KC_Q))`)
- Left inner thumb: Mission Control (`LCTL(KC_UP)`)

Right hand:
- H/J/K/L → Home/PgDn/PgUp/End
- Everything else `KC_NO` (literal off, not transparent)

**Custom illumination** runs from `rgb_matrix_indicators_advanced_user` (the `META` branch). Each RGB-control key animates to hint at its function: white sawtooth for brightness, rainbow scroll for hue, gray↔color fade for saturation, pulse-rates for speed, etc. The debug-toggle key is state-reflective (pulsing bright green when ON, dim solid green when OFF). EE_CLR is solid red (danger). MCTL solid purple, LOCK solid amber, H/J/K/L solid cyan. KC_NO positions are black.

**RGB_TOG override**: `rgb_matrix_disable()` only flips the enable flag and leaves the framebuffer painted on the LEDs. We intercept `RGB_TOG` in `process_record_user` and explicitly `rgb_matrix_set_color_all(0,0,0)` + `rgb_matrix_update_pwm_buffers()` before disabling, so the LEDs actually go dark.

The tap-dance double-tap-toggles (`TD_DEBUG_TOG`, `TD_EECLR`) live on Ctrl/Esc and Z positions for finger-reach reasons. Single tap is a no-op (deliberate friction — these mutate keyboard state).

## NAV / SEL layer specifics

NAV is straight movement. SEL is the same with shift wrapped around each motion to extend selection. Both reuse `Z_VIMG` for the doc-edge case where shift state changes the semantic direction:

```
NAV + G              → Cmd+Up   (top of doc, shift consumed)
NAV + Shift+G        → Cmd+Down (bottom of doc, shift consumed)
SEL + G              → Shift+Cmd+Up   (select to top)
SEL + Shift+G        → Shift+Cmd+Down (select to bottom)
```

This is the `SHIFTED` / `UNSHIFT(...)` pattern from zoolander's `shiftstate.h`, inlined here.

SEL has explicit shift-aware **clipboard actions** that auto-return to BASE on completion:
- `Z_CHANGE` (C) → Backspace, layer_move(BASE)
- `Z_CUT` (X) → Cmd+X, layer_move(BASE)
- `Z_COPY` (Y) → Cmd+C, layer_move(BASE)
- `Z_PASTE` (P) → Cmd+V, layer_move(BASE)

`Z_PASTE` is ALSO mapped on NAV+P. There the layer-move is skipped (NAV deactivates naturally on Tab release).

SEL's unmapped positions are `KC_NO` (not `KC_TRANSPARENT`) — no accidental text input mid-selection. Shift keys (KC_LSFT/KC_RSFT on the bottom-outers) ARE explicitly mapped so the shift sources stay reachable for the Z_VIMG shift-detection.

## Layer indicators (SYM / NAV / SEL / FN)

`rgb_matrix_indicators_advanced_user` paints non-BASE layers by **semantic class** so you can see at a glance which layer you're on and roughly what each key does:

| Color | Class                                  |
|-------|----------------------------------------|
| Red   | Custom `Z_*` keycodes (range `Z_START`..`Z_END`) and tap-dances |
| Green | F-keys (`KC_F1`..`KC_F24`)             |
| Blue  | Media (`KC_AUDIO_MUTE`..`KC_MEDIA_EJECT`) |
| Purple | RGB controls (`RGB_TOG`..`RGB_MODE_RGBTEST`) |
| White | Anything else explicitly bound          |
| (none) | `KC_TRANSPARENT` — falls through to BASE's animation |
| Black | `KC_NO` (dead position)                  |

META has its own per-key custom illumination instead (see above).

The `Z_START` / `Z_END` sentinels in `enum custom_keycodes` are load-bearing for the red painting — anything between them gets colored. Don't add real keycodes past `Z_END`.

## Custom keycodes (the Z_* range)

```c
Z_VIMG       // Vim g/G: top/bottom of doc, shift inverts; SEL adds shift-wrapping
Z_CHANGE     // SEL+C: delete selection + return to BASE
Z_CUT        // SEL+X: Cmd+X + return to BASE
Z_COPY       // SEL+Y: Cmd+C + return to BASE
Z_PASTE      // NAV+P and SEL+P: Cmd+V (+ BASE on SEL)
Z_OQUOTE     // SYM+O: left single curly (shift = left double)
Z_PQUOTE     // SYM+P: right single curly (shift = right double)
```

The `SHIFTED` macro and `UNSHIFT(...)` block macro come from zoolander's `shiftstate.h` (inlined here at the top of `keymap.c`). `UNSHIFT` temporarily clears the shift mods, runs its body, restores them — used when we want to send a sequence the OS shouldn't see as shifted, even though the user is physically holding shift (e.g., NAV+Shift+G → Cmd+Down without the Shift propagating).

## Debug logging

`CONSOLE_ENABLE = yes` + `debug_enable = true` (set in `keyboard_post_init_user`) + the `LOG(...)` macro that gates `uprintf` on `debug_enable`. `log_event` runs at the top of `process_record_user` and prints every press/release as `t=… DN/UP kc=…`.

The META-layer `TD_DEBUG_TOG` (double-tap on the Ctrl/Esc position) flips `debug_enable` at runtime. The tap-dance key's LED is state-reflective. Use `qmk console -kb zsa/voyager -km janeway` to view output.

## Tuning playbook

For when behavior doesn't match intent. Always reflash + try for a few hours before reverting; muscle memory needs time to register a knob change.

| Symptom | Probable knob |
|---------|---------------|
| Accidental holds during fast typing (e.g., typing "verb" gives Cmd+E) | `FLOW_TAP_TERM` too short — bump from 150 → 170. |
| Missed intentional hold (paused, hovered on mod, but tap fired) | `FLOW_TAP_TERM` is suppressing — pause longer or drop to 130. |
| Pinky mod (Ctrl/Shift) fires as tap mid-typing | Verify the pinky keycode is in `get_flow_tap_term`'s exemption switch. |
| Sluggish tap-dance settling | Lower `TAPPING_TERM` from 170. |
| Cmd+letter chord requires too much hold pause | Tune `PERMISSIVE_HOLD` — make sure the relevant mod-tap is NOT in `get_hold_on_other_key_press`'s true-list, and IS in `get_permissive_hold`'s true-list. |
| RGB_TOG doesn't actually turn off the LEDs | The `process_record_user` override clears the framebuffer before disabling. Make sure that code path still runs. |
| OSL(SYM) doesn't stay armed when shift is pressed | Confirm the shift source is a literal modifier keycode (`KC_LSFT` / `KC_RSFT`), not a tap-dance — QMK only skips OSL-clearing for true modifiers. |

## Backport candidates → zoolander

Zoolander is the Moonlander keymap (`keyboards/zsa/moonlander/keymaps/zoolander/`). It has its own architecture — OS-aware bindings, smart text via tap-dances, GAM gaming layer, color-by-class RGB. The following janeway features are worth bringing over:

**Strongly recommended (clear wins, low risk):**

1. **`RGB_TOG` clears LEDs fix** — QMK's `rgb_matrix_disable()` leaves the framebuffer painted. Same intercept in `process_record_user`:
   ```c
   case RGB_TOG:
       if (record->event.pressed) {
           if (rgb_matrix_is_enabled()) {
               rgb_matrix_set_color_all(0, 0, 0);
               rgb_matrix_update_pwm_buffers();
               rgb_matrix_disable();
           } else {
               rgb_matrix_enable();
           }
       }
       return false;
   ```

2. **META-style custom illumination on zoolander's LGT layer** — replace the wall-of-RGB-knobs with per-key animations that hint at function (white-pulse for brightness, rainbow scroll for hue, color cycle for mode, etc.). Pattern lives in janeway's `meta_set_led` function. Reuses `sawtooth`, `triangle`, `hsv_to_rgb_set` helpers.

3. **Debug toggle + EE_CLR tap-dances on LGT** — `TD_DEBUG_TOG` (toggles `debug_enable`) and `TD_EECLR` (clears EEPROM + soft reset). Each fires on **double-tap** to avoid accidents. Combined with a `LOG()` macro wrapper on `uprintf` for runtime-toggleable logging.

**Conditional (depends on whether zoolander already does it):**

4. **Color-coded layer indicators** — zoolander ALREADY has this (`rgb_matrix_indicators_advanced_user` colors by F-keys/Z_*/media/RGB class). Bring janeway's indicator code as reference if zoolander's needs maintenance; otherwise leave alone.

5. **Smart typography on SYM** — `TD_DASH` (1=en, 2=em), `TD_DOT` (1=., 2=.., 3=ellipsis), `Z_OQUOTE`/`Z_PQUOTE` for curly quotes via direct keycodes (no tap-dance delay). Zoolander has `dance_smartquote` / `dance_smartdash` / `dance_smartellipsis` already, with OS-aware Mac/Win bindings. Janeway's variants are simpler but Mac-only — backport pattern if zoolander's smart text isn't working, otherwise skip.

6. **`DOUBLE_TAP_SHIFT_TURNS_ON_CAPS_WORD`** — QMK built-in that makes literal `KC_LSFT` doubled-tap activate caps_word. Zoolander's `TD_SSHFT` tap-dance does the same thing via custom code. If you want to simplify, swap supershift for `KC_LSFT` + this `#define`. Trade-offs: lose triple-tap-for-``` (zoolander doesn't have this anyway, janeway just lost it too) and the SINGLE_HOLD branch's `register_mods` vs SINGLE_TAP's `register_code16` distinction. For most users, identical UX with simpler code.

**Do NOT backport** (architectural mismatch):
- Smart-chord state machine (janeway used to have one for V+letter — only needed because V was a letter-mod with finger-timing variance; zoolander has thumb-only mods)
- Per-key `PERMISSIVE_HOLD` / `HOLD_ON_OTHER_KEY_PRESS` callbacks (zoolander has different mod-tap density)
- `OSL(SYM)` on the thumb (zoolander uses `MO(SYM)` or similar momentary access)
- Navigator trackpad config (Moonlander doesn't have this add-on)

**Reverse direction — bring INTO janeway from zoolander:**
- OS-aware `Z_*` keycodes + `os_bindings_t` table for Mac/Win portability (currently janeway is hard-Mac-only)
- GAM (gaming) layer with no mod-taps for low-latency input

## Things to leave alone

- The vendored QMK tree (anything outside this directory).
- `modules/zsa/defaults`, `modules/zsa/navigator_trackpad` — managed by upstream.
- The `Z_START` / `Z_END` enum sentinels — they're load-bearing for the indicator paint logic.
- Letter positions on BASE for V and M — they were mod-tapped before; the deliberate plainness teaches thumb-Cmd discipline.
- The `KC_LSFT` literal on bottom-left — it's a real modifier so OSL/OSM stays armed when shift is pressed. Replacing with a tap-dance breaks this.
