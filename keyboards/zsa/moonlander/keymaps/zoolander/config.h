/* Copyright 2020 ZSA Technology Labs, Inc <@zsa>
 * Copyright 2020 Jack Humbert <jack.humb@gmail.com>
 * Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#define FIRMWARE_VERSION u8"default/latest"
#define RGB_MATRIX_STARTUP_SPD 60
#define CAPS_LOCK_STATUS
#define RGB_MATRIX_KEYPRESSES
#define RGB_MATRIX_FRAMEBUFFER_EFFECTS

#define USE_OKLAB_CURVE

// --- Tap-hold timing (ported from janeway) ---------------------------------
// See keyboards/zsa/voyager/keymaps/janeway/CLAUDE.md "Tuning playbook" for
// the symptom -> knob mapping; the scheme is identical on both boards.

#define TAPPING_TERM 170

// No tap-then-hold auto-repeat on mod-tap keys.
#define QUICK_TAP_TERM 0

// Flow-tap: during continuous typing, suppress hold behavior on mod-taps.
// 150ms gives ~60ms safety margin past a 90ms inter-key interval (133 wpm).
#define FLOW_TAP_TERM 150

// Per-key tap-hold decision. Pinky mods use HOLD_ON_OTHER_KEY_PRESS (snap
// chord). Everything else — letter mods, layer-taps — uses PERMISSIVE_HOLD
// (other key must be pressed AND released before the mod-tap release) so
// typing rolls settle as TAP. Cmd/Alt/Ctrl live on the bottom row as plain
// modifiers and never enter tap-hold resolution.
// See get_hold_on_other_key_press / get_permissive_hold in keymap.c.
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY
#define PERMISSIVE_HOLD_PER_KEY

// One-shot SYM (left thumb) arms for a single keypress; matches janeway.
#undef ONESHOT_TIMEOUT
#define ONESHOT_TIMEOUT 1000

// Double-tapping the literal KC_LSFT keycode activates caps_word. Replaces
// both the supershift tap-dance and the dedicated CW_TOGG key.
#define DOUBLE_TAP_SHIFT_TURNS_ON_CAPS_WORD