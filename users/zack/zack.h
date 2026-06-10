// Shared userspace for Zack's ZSA keymaps (zoolander/Moonlander,
// janeway/Voyager). Everything board-agnostic lives here: the layer model,
// the Z_* keycode range, OS-aware bindings, the Vim NAV/SEL engine,
// typography, and the META-layer illumination. Boards pull this in with
// `USER_NAME := zack` in their keymap rules.mk.
#pragma once

#include "quantum.h"
#include "process_tap_dance.h"
#include "defaults.h" // zsa/defaults community module — provides ZSA_SAFE_RANGE

// ---------------------------------------------------------------------------
// Layer model (Vim-shaped). Identical indices on every board; boards append
// extra layers from ZACK_LAYER_SAFE_RANGE (e.g. zoolander's GAM).
enum zack_layers {
    BASE = 0, // Typing / insert (Vim insert mode)
    SYM,      // Symbols, typography (+ numpad on the Moonlander)
    META,     // Board meta — RGB controls, lock, debug toggle, EE_CLR
    NAV,      // Movement (Vim normal mode); gateway into SEL
    SEL,      // Selection (Vim visual mode)
    FN,       // Function (F-keys, media)
    ZACK_LAYER_SAFE_RANGE,
};

// ---------------------------------------------------------------------------
// Custom keycodes. Z_START/Z_END are sentinels bracketing the Z_* range so
// indicator code can paint it red; never bind them, never put real keycodes
// after Z_END.
enum zack_keycodes {
    // META-layer HSV presets. Deliberately OUTSIDE the Z_START..Z_END range:
    // they get their own colors from the META illumination instead of red.
    HSV_0_255_255 = ZSA_SAFE_RANGE,
    HSV_74_255_255,
    HSV_169_255_255,
    Z_START,
    Z_OSMAC, // Set host OS to Mac
    Z_OSWIN, // Set host OS to Windows
    // Text movement keys
    Z_FWORD, // forwards word
    Z_BWORD, // backwards word
    Z__LSTR, // line start
    Z__LEND, // line end
    Z__PSTR, // paragraph start
    Z__PEND, // paragraph end
    Z__VIMG, // g => start of document / G => end of document (same as Vim)
    Z__VIMO, // o => end-of-line, return / O => start-of-line, return, up (similar to o/O in Vim)
    // Selection/modification keys
    Z_CHNGE, // Change (deletes selected text, goes back to 'insert' mode)
    Z___CUT, // Cmd-X on mac, Ctrl-X on Windows
    Z__COPY, // Cmd-C on mac, Ctrl-C on Windows
    Z_PASTE, // Cmd-V on mac, Ctrl-V on Windows
    // SYM-layer curly quotes (OS-aware):
    //   O: left single ' (shift -> left double ")
    //   P: right single ' (shift -> right double ")
    Z_OQUOTE,
    Z_PQUOTE,
    // META-layer system actions (OS-aware).
    Z__LOCK, // Lock screen
    Z__MCTL, // Mission Control (Mac) / Task View (Windows)
    Z_END,   // Sentinel: must remain last for indicator range checks.
};

// ---------------------------------------------------------------------------
// Tap dances shared across boards. Indices must match on every board because
// the META illumination switches on TD(TD_DEBUG_TOG)/TD(TD_EECLR); boards
// append their own dances from ZACK_TAP_DANCE_SAFE_RANGE. The dance *action*
// table stays per-board (debug/EE_CLR feedback differs, e.g. audio).
enum zack_tap_dances {
    TD_SMRTDSH = 0, // Smart dash key: 1=en-dash, 2=em-dash
    TD_SMRTDOT,     // Smart dot key: 1=., 2=.., 3+=ellipsis
    TD_DEBUG_TOG,   // META layer: double-tap toggles debug_enable
    TD_EECLR,       // META layer: double-tap clears EEPROM and reboots
    ZACK_TAP_DANCE_SAFE_RANGE,
};

void dance_smartdash(tap_dance_state_t *state, void *user_data);
void dance_smartdot(tap_dance_state_t *state, void *user_data);

// ---------------------------------------------------------------------------
// Host-OS awareness. Boards that never leave macOS just leave host_os alone.
typedef enum { OS_MAC, OS_WIN } host_os_t;
extern host_os_t host_os;

// Per-OS keystroke sequences. One slot per semantic action; the runtime
// switches between mac_bindings and win_bindings based on host_os.
typedef struct {
    // Edge-of-line jumps (sent when KC_LEFT/RIGHT is shifted on the NAV layer).
    const char *line_home_jump;
    const char *line_end_jump;
    // Page jumps (sent when KC_UP/DOWN is shifted on the NAV layer).
    const char *page_up;
    const char *page_down;
    // Word-level cursor moves.
    const char *word_forward;
    const char *word_back;
    // Line-level cursor moves.
    const char *line_start;
    const char *line_end;
    // Paragraph-level cursor moves.
    const char *para_start;
    const char *para_end;
    // Document-level cursor moves (Vim g / G).
    const char *doc_start;
    const char *doc_end;
    // Vim o / O — open a new line below / above and start typing on it.
    const char *open_below;
    const char *open_above;
    // Clipboard operations.
    const char *cut;
    const char *copy;
    const char *paste;
    // System actions (META layer).
    const char *lock;
    const char *mission_control;
} os_bindings_t;

const os_bindings_t *bindings(void);

// Wraps a runtime keystroke sequence in a left-shift mod (for the SEL layer).
// Can't use SS_LSFT(...) for this because that macro only operates on string
// literals.
void send_shifted(const char *seq);

// ---------------------------------------------------------------------------
// Shift-state helpers: test for a (real or weak) shift, and run a block with
// shift temporarily cleared, restoring the user's shift state afterwards.
#define SHIFTED (get_mods() & MOD_MASK_SHIFT || get_weak_mods() & MOD_MASK_SHIFT)

#define UNSHIFT(...)                                \
    do {                                            \
        uint8_t prevmods     = get_mods();          \
        uint8_t prevweakmods = get_weak_mods();     \
        del_mods(MOD_MASK_SHIFT);                   \
        del_weak_mods(MOD_MASK_SHIFT);              \
        __VA_ARGS__;                                \
        set_mods(prevmods);                         \
        set_weak_mods(prevweakmods);                \
    } while (0)

// ---------------------------------------------------------------------------
// Debug-gated logger. The META layer's debug tap-dance flips `debug_enable`
// to silence/restore output.
#define LOG(...) do { if (debug_enable) uprintf(__VA_ARGS__); } while (0)

static inline void log_event(uint16_t keycode, keyrecord_t *record) {
    LOG("t=%lu %s kc=%04X\n",
        (unsigned long)record->event.time,
        record->event.pressed ? "DN" : "UP",
        keycode);
}

// ---------------------------------------------------------------------------
// Shared process_record handling: HSV presets, the RGB_TOG off-fix, curly
// quotes, lock / mission-control, and the NAV/SEL Vim engine dispatch.
// Call at the END of the board's process_record_user (after board-specific
// cases) and return its result.
bool process_record_zack(uint16_t keycode, keyrecord_t *record);

// ---------------------------------------------------------------------------
// Indicator painting.
// META: per-key animated illumination (RGB knobs animate to hint at their
// function, utilities get solid distinct colors, dead positions go dark).
void zack_paint_meta_layer(uint8_t led_min, uint8_t led_max);
// Other non-base layers: color-code every bound key by its semantic class
// (F-keys green, Z_*/tap-dances red, media blue, RGB controls purple, other
// bound keys white; transparent untouched).
void zack_paint_layer_classes(uint8_t layer, uint8_t led_min, uint8_t led_max);
