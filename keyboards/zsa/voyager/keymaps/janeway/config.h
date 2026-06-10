#pragma once

#define FIRMWARE_VERSION u8"janeway/dev"
#define SERIAL_NUMBER "7megA/eaLGdm"

// Oryx-friendly defaults preserved from the generator.
#undef RGB_MATRIX_TIMEOUT
#define RGB_MATRIX_TIMEOUT 180000

#define USB_SUSPEND_WAKEUP_DELAY 0
#define LAYER_STATE_8BIT
#define RGB_MATRIX_STARTUP_SPD 60

// Tap-hold timing, flow-tap, caps-word, and one-shot behavior are shared
// across boards in users/zack/config.h. Only board-specific settings here.

// CHORDAL_HOLD was tried and removed. With same-hand chord usage being
// frequent (muscle memory: Cmd+T, Ctrl+Tab, F+1..F+5, etc.), nearly every
// mod-tap key needed a '*' override and the safety benefit collapsed. The
// remaining protection — FLOW_TAP_TERM + PERMISSIVE_HOLD — is what keeps
// typing flow safe.
