#pragma once

#define FIRMWARE_VERSION u8"janeway/dev"
#define SERIAL_NUMBER "7megA/eaLGdm"

// Oryx-friendly defaults preserved from the generator.
#undef ONESHOT_TIMEOUT
#define ONESHOT_TIMEOUT 1000

#undef RGB_MATRIX_TIMEOUT
#define RGB_MATRIX_TIMEOUT 180000

#define USB_SUSPEND_WAKEUP_DELAY 0
#define LAYER_STATE_8BIT
#define RGB_MATRIX_STARTUP_SPD 60

// --- Tap-hold timing -------------------------------------------------------
// See CLAUDE.md (Tuning playbook) for the symptom → knob mapping.

#undef TAPPING_TERM
#define TAPPING_TERM 170

#define QUICK_TAP_TERM 0

// Flow-tap: during continuous typing, suppress hold behavior on mod-taps.
// 150ms gives ~60ms safety margin past a 90ms inter-key interval (133 wpm).
#define FLOW_TAP_TERM 150

// Per-key tap-hold decision. Pinky mods + both thumb Cmd spaces use
// HOLD_ON_OTHER_KEY_PRESS (snap chord; thumbs rely on FLOW_TAP_TERM for
// typing-roll protection). Everything else — letter mods, layer-taps —
// uses PERMISSIVE_HOLD (other key must be pressed AND released before the
// mod-tap release) so typing rolls settle as TAP.
// See get_hold_on_other_key_press / get_permissive_hold in keymap.c.
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY
#define PERMISSIVE_HOLD_PER_KEY

// Double-tapping the literal KC_LSFT keycode activates caps_word. Used
// instead of a supershift tap-dance so the shift key remains a proper
// modifier (which lets OSL stay armed when shift is pressed inside it).
#define DOUBLE_TAP_SHIFT_TURNS_ON_CAPS_WORD

// CHORDAL_HOLD was tried and removed. With same-hand chord usage being
// frequent (muscle memory: Cmd+T, Ctrl+Tab, F+1..F+5, etc.), nearly every
// mod-tap key needed a '*' override and the safety benefit collapsed. The
// remaining protection — FLOW_TAP_TERM + PERMISSIVE_HOLD on thumbs — is
// what keeps typing flow safe. If post-pause "type → 150ms gap → roll
// mod-tap letter" misfires become a problem, reconsider.
