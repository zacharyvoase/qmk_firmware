#pragma once

// --- Shared tap-hold engine -------------------------------------------------
// Identical on every board; see the root CLAUDE.md tuning playbook for the
// symptom -> knob mapping. Board keymaps must not redefine these.

#undef TAPPING_TERM
#define TAPPING_TERM 170

// No tap-then-hold auto-repeat on mod-tap keys.
#define QUICK_TAP_TERM 0

// Flow-tap: during continuous typing, suppress hold behavior on mod-taps.
// 150ms gives ~60ms safety margin past a 90ms inter-key interval (133 wpm).
#define FLOW_TAP_TERM 150

// Per-key tap-hold decision. Pinky mods (and any board-specific snap keys)
// use HOLD_ON_OTHER_KEY_PRESS; everything else uses PERMISSIVE_HOLD. The
// per-key lists live in each board's keymap.c (get_hold_on_other_key_press /
// get_permissive_hold / get_flow_tap_term).
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY
#define PERMISSIVE_HOLD_PER_KEY

// Double-tapping the literal KC_LSFT keycode activates caps_word. Used
// instead of a tap-dance so the shift key remains a proper modifier (which
// lets OSL stay armed when shift is pressed inside it).
#define DOUBLE_TAP_SHIFT_TURNS_ON_CAPS_WORD

// One-shot SYM thumb arms for a single keypress.
#undef ONESHOT_TIMEOUT
#define ONESHOT_TIMEOUT 1000
