# Repo orientation

This is a fork of [zsa/qmk_firmware](https://github.com/zsa/qmk_firmware) (itself a fork of qmk/qmk_firmware) maintained for Zack's two personal ZSA keymaps: **`zoolander`** (Moonlander — desk board, dual-OS) and **`janeway`** (Voyager — travel board, Mac-only).

Don't try to make this look like a vanilla QMK contribution. The shape of this repo — branch layout, ignored paths, custom keymaps plus a shared userspace — is built around tracking ZSA's firmware releases and grafting personal code on top.

## Where the custom code lives

| Path | Contents |
|------|----------|
| `users/zack/` | Shared userspace — everything board-agnostic |
| `keyboards/zsa/moonlander/keymaps/zoolander/` | Moonlander keymap (see its `CLAUDE.md`) |
| `keyboards/zsa/voyager/keymaps/janeway/` | Voyager keymap (see its `CLAUDE.md`) |

Everything else is vendored upstream and should not be modified except to resolve merge conflicts when syncing.

`users/zack/` layout:
- `zack.h` — umbrella header: shared layer enum, `Z_*`/HSV keycodes, shared tap-dance indices, `os_bindings_t` + `host_os`, `SHIFTED`/`UNSHIFT`, `LOG`/`log_event`, public prototypes
- `zack.c` — OS binding tables, typography (smart quotes/dash/dot), the Vim NAV/SEL engine, `process_record_zack()`
- `zack_lights.c` — META-layer animated illumination + the class-painting indicator scheme
- `win_alt_code.{c,h}` — Unicode Alt-codes on Windows (toggles NumLock around the sequence)
- `config.h` — shared tap-hold timing, flow-tap, caps-word, one-shot settings
- `rules.mk` — shared feature flags + `SRC`

A keymap opts in with `USER_NAME := zack` in its `rules.mk`, then includes `zack.h` and calls `process_record_zack()` at the end of its `process_record_user()`. **Status: zoolander is wired up; janeway still carries its own (older, Mac-only) copies and is pending migration.**

All of these paths are tracked only because `.gitignore` has explicit unignore rules (`!/keyboards/zsa/**/keymaps/zoolander`, `…/janeway`, `!/users/zack`, plus a bare `!/users/` that makes the blanket-ignored directory traversable). If those exceptions are removed, the code silently vanishes from git and from builds.

## Branch model

- **`zoolander`** — the working branch. All custom commits live here on top of merges from upstream. **This is the branch you should be on for almost any work.**
- **`firmware17` / `firmware18` / … / `firmware20`** — local mirrors of `upstream/firmwareXX`. Not customized; used as merge sources only.
- **`upstream`** remote points at `git@github.com:zsa/qmk_firmware.git`. **`origin`** is the personal GitHub fork.

Sync workflow (last performed for firmware25):
1. `git fetch upstream`
2. `git checkout zoolander`
3. `git merge upstream/firmwareXX -X theirs -X rename-threshold=90`
4. Resolve conflicts — **for any conflict not under the custom paths above, take upstream's side**; for custom files, keep ours.
5. Watch for path renames: ZSA reorganized `keyboards/moonlander/` → `keyboards/zsa/moonlander/` in firmware25. Future renames may need similar `.gitignore` updates.
6. Commit the merge; do not push to `origin` without explicit ask.

## Building and flashing

QMK CLI is not assumed to be installed. To build:
```
qmk compile -kb zsa/moonlander/reva -km zoolander
qmk compile -kb zsa/voyager -km janeway
```
**Zack's Moonlander is a rev A** (bought Sept 2022, bare-STM32 `0483:df11` bootloader, app at 0x08000000). Do NOT build/flash `revb`: rev B images are linked at 0x08002000 and appear to work right after flashing (the DFU "leave" jumps into them) but fail on cold boot, leaving the board seemingly dead. If that ever happens: pinhole reset, then flash a `reva` build. The Voyager uses ZSA's custom bootloader (`3297:0791`) and the default `qmk flash` flow.

Flash with the board plugged **directly into the Mac** — the desk dock drops bootloader re-enumerations ("Bootloader not found" loop). `.bin`s flash via `qmk flash` or [Wally](https://ergodox-ez.com/pages/wally). Bootloader from the keymap: hold `F` (FN layer), tap top-left (`QK_BOOT`); the pinhole reset always works regardless of firmware state.

There's no test suite — verification is "flash it and try the layers."

## Shared keymap conventions

These hold on both boards; the per-board CLAUDE.mds only document deviations.

- **Layer model is Vim-shaped, identical indices everywhere** (`users/zack/zack.h`): `BASE` (insert), `SYM`, `META` (board meta / RGB), `NAV` (normal / movement), `SEL` (visual / selection), `FN`; boards append extras from `ZACK_LAYER_SAFE_RANGE` (zoolander adds `GAM`). New editing actions belong on NAV or SEL, not BASE.
- **Custom keycodes use the `Z_*` namespace**, bracketed by sentinels `Z_START` and `Z_END`. The sentinels are load-bearing: indicator code paints everything between them red. Never bind the sentinels, never add real keycodes after `Z_END`. HSV preset keycodes sit deliberately *before* `Z_START` so META can color them itself.
- **OS-aware bindings live in `os_bindings_t`** (`mac_bindings` / `win_bindings` in `zack.c`). Add new semantic actions as fields there rather than branching on `host_os` inside switch cases. Mac-only boards simply never flip `host_os`.
- **`UNSHIFT(...)` / `SHIFTED`** (`zack.h`) — test for a held shift, or run a block with shift temporarily cleared and then restored. Use whenever a sent sequence must not be modified by the user's held shift.
- **Indicator coloring is range-based** (`zack_lights.c`): F-keys → green, `Z_*` range and tap-dances → red, media → blue, RGB controls → purple, anything else bound → white, `KC_NO` → dark, transparent → falls through. New bindings inherit colors automatically by keycode value. META instead gets per-key animated illumination (`zack_paint_meta_layer`).
- **Shared tap-dance indices** (`TD_SMRTDSH`, `TD_SMRTDOT`, `TD_DEBUG_TOG`, `TD_EECLR`) must stay aligned across boards — the META illumination switches on `TD(...)` keycodes. Boards append their own from `ZACK_TAP_DANCE_SAFE_RANGE` and own their `tap_dance_actions[]` (feedback like audio differs per board).
- **One-shot SYM gotcha**: `OSL(SYM)` tapped (one-shot) dies on the first press of a multi-tap dance, so `TD_SMRTDOT`/`TD_SMRTDSH` multi-taps only work when the SYM thumb is **held** through the dance. Known tradeoff; don't "fix" it without checking QMK's one-shot clearing semantics.

## Tap-hold scheme and tuning playbook

Identical engine on both boards (`users/zack/config.h`): `TAPPING_TERM 170`, `FLOW_TAP_TERM 150`, `QUICK_TAP_TERM 0`, per-key `HOLD_ON_OTHER_KEY_PRESS` (snap-fire) vs `PERMISSIVE_HOLD`. The per-key lists live in each board's `keymap.c` (`get_hold_on_other_key_press` / `get_permissive_hold` / `get_flow_tap_term`): pinky mods snap-fire and are exempt from flow-tap; letter mods and layer-taps are permissive with flow-tap protection. Keep the two boards in sync when tuning. Reflash and live with a change for a few hours before reverting — muscle memory needs time.

| Symptom | Probable knob |
|---------|---------------|
| Accidental holds during fast typing (e.g., typing "verb" gives Cmd+E) | `FLOW_TAP_TERM` too short — bump from 150 → 170. |
| Missed intentional hold (paused, hovered on mod, but tap fired) | `FLOW_TAP_TERM` is suppressing — pause longer before chording, or drop to 130. |
| Pinky mod (Ctrl/Shift) fires as tap mid-typing | Verify the pinky keycode is in `get_flow_tap_term`'s exemption switch. |
| Sluggish tap-dance settling | Lower `TAPPING_TERM` from 170. |
| Cmd+letter chord requires too much hold pause | Make sure the mod-tap is NOT in `get_hold_on_other_key_press`'s true-list and IS permissive. |
| Post-pause roll misfires as chord (snap-fire keys only) | Inherent to snap-fire + cold flow-tap timer; consider demoting the key to permissive. |
| OSL(SYM) doesn't stay armed when shift is pressed | Shift source must be a literal modifier keycode, not a tap-dance — QMK only skips OSL-clearing for true modifiers. |

## Things to leave alone unless asked

- The vendored QMK tree (anything outside the custom paths).
- Submodules under `lib/`, modules under `modules/zsa/` (managed by upstream).
- `users/` (other than `users/zack/`), `layouts/`, `docs/` — blanket-ignored by `.gitignore` in this fork.
- The `Z_START`/`Z_END` sentinels and the shared tap-dance index alignment.
