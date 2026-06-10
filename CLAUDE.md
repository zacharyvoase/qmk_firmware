# Repo orientation

This is a fork of [zsa/qmk_firmware](https://github.com/zsa/qmk_firmware) (itself a fork of qmk/qmk_firmware) maintained for Zack's personal ZSA Moonlander keymap (`zoolander`).

Don't try to make this look like a vanilla QMK contribution. The shape of this repo — branch layout, ignored paths, single custom keymap — is built around tracking ZSA's firmware releases and grafting one personal keymap on top.

## Where the custom code lives

Everything personal is in **`keyboards/zsa/moonlander/keymaps/zoolander/`**. The rest of the tree is vendored upstream and should not be modified except to resolve merge conflicts when syncing.

Layout:
- `keymap.c` — layers, custom keycodes, tap dances, RGB indicators (including the animated LGT panel), per-key tap-hold callbacks, OS-aware movement/selection processors
- `config.h` — keymap-level `#define`s (tap-hold timing, flow-tap, RGB defaults)
- `rules.mk` — keymap-level build flags + `SRC += win_alt_code.c`
- `win_alt_code.h` / `win_alt_code.c` — helper for sending Unicode Alt-codes on Windows (toggles NumLock around the sequence)
- `shiftstate.h` — `SHIFTED` test macro + `UNSHIFT(...)` block macro
- `songs.h` — audio cues for mode-change feedback

The keymap is **only** included in the build because `.gitignore` has an explicit unignore rule for it (`!/keyboards/zsa/**/keymaps/zoolander` and `/**`). Upstream's `.gitignore` blanket-excludes all keymaps under `keyboards/zsa/`; if that exception is ever removed, the keymap silently vanishes from builds.

## Branch model

- **`zoolander`** — the working branch. All custom commits live here on top of merges from upstream. **This is the branch you should be on for almost any work.**
- **`firmware17` / `firmware18` / … / `firmware20`** — local mirrors of `upstream/firmwareXX`. Not customized; used as merge sources only.
- **`upstream`** remote points at `git@github.com:zsa/qmk_firmware.git`. **`origin`** is the personal GitHub fork.

Sync workflow (last performed for firmware25):
1. `git fetch upstream`
2. `git checkout zoolander`
3. `git merge upstream/firmwareXX -X theirs -X rename-threshold=90`
4. Resolve conflicts — **for any conflict not under `keymaps/zoolander/`, take upstream's side**; for zoolander files, keep ours.
5. Watch for path renames: ZSA reorganized `keyboards/moonlander/` → `keyboards/zsa/moonlander/` in firmware25. Future renames may need similar `.gitignore` updates.
6. Commit the merge; do not push to `origin` without explicit ask.

## Building and flashing

QMK CLI is not assumed to be installed. To build:
```
qmk compile -kb zsa/moonlander/reva -km zoolander
```
**Zack's Moonlander is a rev A** (bought Sept 2022, bare-STM32 `0483:df11` bootloader, app at 0x08000000). Do NOT build/flash `revb`: rev B images are linked at 0x08002000 and appear to work right after flashing (the DFU "leave" jumps into them) but fail on cold boot, leaving the board seemingly dead. If that ever happens: pinhole reset, then flash a `reva` build. Flash with the board plugged **directly into the Mac** — the desk dock drops bootloader re-enumerations. The `.bin` is flashed via `qmk flash` or [Wally](https://ergodox-ez.com/pages/wally).

There's no local test suite for the keymap itself — verification is "flash it and try the layers." For the wider QMK tree, upstream's `make test:all` exists but isn't run in this fork.

## Keymap conventions

When editing or extending `keymap.c`:

- **Layer model is Vim-shaped and matches janeway**: `BASE` (insert), `SYM`, `META` (board meta / RGB), `NAV` (normal / movement), `SEL` (visual / selection), `FN`, plus Moonlander-only `GAM`. Preserve this mental model — new editing actions should live on NAV or SEL, not on BASE.
- **OS-aware bindings live in `os_bindings_t`** (`mac_bindings` / `win_bindings`). Add new semantic actions as fields there rather than branching on `host_os` inside switch cases.
- **Custom keycodes use the `Z_*` namespace**, bracketed by sentinels `Z_START` and `Z_END`. The `Z_END` sentinel is load-bearing for `rgb_matrix_indicators_advanced_user` — anything between `Z_START` and `Z_END` auto-colors red. Don't put real keycodes after `Z_END`.
- **`UNSHIFT(...)` (in shiftstate.h)** is a block macro that temporarily clears shift, runs its body, then restores the user's shift state. Use it whenever you want to send a sequence that should not be modified by a held shift.
- **Tap-hold scheme is shared with the janeway Voyager keymap** (see `keyboards/zsa/voyager/keymaps/janeway/CLAUDE.md`, "Tuning playbook"): pinky mods + thumb Cmd spaces snap-fire via `get_hold_on_other_key_press`, everything else uses permissive hold, and `FLOW_TAP_TERM` suppresses holds during typing. Keep the two boards' schemes in sync when tuning.
- **RGB indicator coloring is range-based** in `rgb_matrix_indicators_advanced_user`: F-keys → green, custom `Z_*` → red, media keys → blue, RGB controls → purple, anything else mapped → white. New bindings inherit colors automatically by virtue of their keycode value.

## Things to leave alone unless asked

- The vendored QMK tree (anything outside `keymaps/zoolander/`).
- Submodules under `lib/`.
- `users/`, `layouts/`, `docs/` — these are blanket-ignored by `.gitignore` in this fork.
- `keyboards/moonlander/` (old path) — removed by upstream in firmware25.
