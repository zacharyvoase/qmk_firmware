/* Copyright 2020 ZSA Technology Labs, Inc <@zsa>
 * Copyright 2020 Jack Humbert <jack.humb@gmail.com>
 * Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2023 Zachary Voase <zack@meat.io>
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

#include QMK_KEYBOARD_H

#include "action_layer.h"
#include "audio.h"
#include "color.h"
#include "keycodes.h"
#include "keymap.h"
#include "process_tap_dance.h"
#include "quantum.h"
#include "rgb_matrix.h"
#include "rgb_matrix_types.h"
#include "shiftstate.h"
#include "songs.h"
#include "version.h"
#include "win_alt_code.h"

// Layer names and order match the janeway Voyager keymap, with GAM appended
// (the Voyager has no gaming layer).
enum layers {
    BASE, // Typing / insert (Vim insert mode)
    SYM,  // Symbols, numpad, typography
    META, // Board meta — RGB controls, lock, debug toggle, EE_CLR
    NAV,  // Movement (Vim normal mode); gateway into SEL
    SEL,  // Selection (Vim visual mode)
    FN,   // Function (F-keys, media, OS toggle)
    GAM,  // Gaming (no tap dances/holds, for low-latency input)
};

// Determines what actual keycodes to send for smart quote, movement and modification keys
typedef enum { OS_MAC, OS_WIN } host_os_t;
static host_os_t host_os;

enum custom_keycodes {
    // META-layer HSV presets. Deliberately OUTSIDE the Z_START..Z_END range:
    // the indicator code paints that range red, and these get their own
    // colors from meta_set_led instead.
    HSV_0_255_255 = SAFE_RANGE,
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

// Tap Dance Declarations
enum {
    TD_AIRPODS = 0, // Airpods-style media key
    TD_SMRTDSH,     // Smart dash key: 1=en-dash, 2=em-dash
    TD_SMRTDOT,     // Smart dot key: 1=., 2=.., 3+=ellipsis
    TD_DEBUG_TOG,   // META layer: double-tap toggles debug_enable
    TD_EECLR,       // META layer: double-tap clears EEPROM and reboots
};

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

static const os_bindings_t mac_bindings = {
    .line_home_jump = SS_TAP(X_HOME),
    .line_end_jump  = SS_TAP(X_END),
    .page_up        = SS_TAP(X_PAGE_UP),
    .page_down      = SS_TAP(X_PAGE_DOWN),
    .word_forward   = SS_LALT(SS_TAP(X_RIGHT)),
    .word_back      = SS_LALT(SS_TAP(X_LEFT)),
    .line_start     = SS_LGUI(SS_TAP(X_LEFT)),
    .line_end       = SS_LGUI(SS_TAP(X_RIGHT)),
    .para_start     = SS_LALT(SS_TAP(X_UP)),
    .para_end       = SS_LALT(SS_TAP(X_DOWN)),
    .doc_start      = SS_LCMD(SS_TAP(X_UP)),
    .doc_end        = SS_LCMD(SS_TAP(X_DOWN)),
    .open_below     = SS_LCTL("e") SS_TAP(X_ENTER),
    .open_above     = SS_LCTL("a") SS_TAP(X_ENTER) SS_TAP(X_UP),
    .cut            = SS_LCMD("x"),
    .copy           = SS_LCMD("c"),
    .paste          = SS_LCMD("v"),
    .lock            = SS_LCTL(SS_LCMD("q")),
    .mission_control = SS_LCTL(SS_TAP(X_UP)),
};

static const os_bindings_t win_bindings = {
    .line_home_jump = SS_LCTL(SS_TAP(X_HOME)),
    .line_end_jump  = SS_LCTL(SS_TAP(X_END)),
    .page_up        = SS_LCTL(SS_TAP(X_PAGE_UP)),
    .page_down      = SS_LCTL(SS_TAP(X_PAGE_DOWN)),
    .word_forward   = SS_LCTL(SS_TAP(X_RIGHT)),
    .word_back      = SS_LCTL(SS_TAP(X_LEFT)),
    .line_start     = SS_TAP(X_HOME),
    .line_end       = SS_TAP(X_END),
    .para_start     = SS_LCTL(SS_TAP(X_UP)),
    .para_end       = SS_LCTL(SS_TAP(X_DOWN)),
    .doc_start      = SS_LCTL(SS_TAP(X_HOME)),
    .doc_end        = SS_LCTL(SS_TAP(X_END)),
    .open_below     = SS_TAP(X_END) SS_TAP(X_ENTER),
    .open_above     = SS_TAP(X_HOME) SS_TAP(X_ENTER) SS_TAP(X_UP),
    .cut            = SS_LCTL("x"),
    .copy           = SS_LCTL("c"),
    .paste          = SS_LCTL("v"),
    .lock            = SS_LGUI("l"),
    .mission_control = SS_LGUI(SS_TAP(X_TAB)), // Task View
};

static const os_bindings_t *bindings(void) {
    return (host_os == OS_MAC) ? &mac_bindings : &win_bindings;
}

// Wraps a runtime keystroke sequence in a left-shift mod for the SEL layer.
// Can't use SS_LSFT(...) here because that macro only operates on string literals.
static void send_shifted(const char *seq) {
    register_code(KC_LEFT_SHIFT);
    send_string(seq);
    unregister_code(KC_LEFT_SHIFT);
}

void dance_airpods(tap_dance_state_t *state, void *user_data) {
    if (!state->finished) return;
    switch (state->count) {
        case 1: SEND_STRING(SS_TAP(X_MEDIA_PLAY_PAUSE)); break;
        case 2: SEND_STRING(SS_TAP(X_MEDIA_NEXT_TRACK)); break;
        case 3: SEND_STRING(SS_TAP(X_MEDIA_PREV_TRACK)); break;
    }
    reset_tap_dance(state);
}

#define MAC_LEFT_SINGLE_QUOTE SS_LALT("]")
#define MAC_RGHT_SINGLE_QUOTE SS_LALT(SS_LSFT("]"))
#define MAC_LEFT_DOUBLE_QUOTE SS_LALT("[")
#define MAC_RGHT_DOUBLE_QUOTE SS_LALT(SS_LSFT("["))
#define MAC_EN_DASH SS_LALT("-")
#define MAC_EM_DASH SS_LALT(SS_LSFT("-"))
#define MAC_ELLIPSIS SS_LALT(";")

#define WIN_LEFT_SINGLE_QUOTE SS_TAP(X_KP_0) SS_TAP(X_KP_1) SS_TAP(X_KP_4) SS_TAP(X_KP_5)
#define WIN_RGHT_SINGLE_QUOTE SS_TAP(X_KP_0) SS_TAP(X_KP_1) SS_TAP(X_KP_4) SS_TAP(X_KP_6)
#define WIN_LEFT_DOUBLE_QUOTE SS_TAP(X_KP_0) SS_TAP(X_KP_1) SS_TAP(X_KP_4) SS_TAP(X_KP_7)
#define WIN_RGHT_DOUBLE_QUOTE SS_TAP(X_KP_0) SS_TAP(X_KP_1) SS_TAP(X_KP_4) SS_TAP(X_KP_8)
#define WIN_EN_DASH SS_TAP(X_KP_0) SS_TAP(X_KP_1) SS_TAP(X_KP_5) SS_TAP(X_KP_0)
#define WIN_EM_DASH SS_TAP(X_KP_0) SS_TAP(X_KP_1) SS_TAP(X_KP_5) SS_TAP(X_KP_1)
#define WIN_ELLIPSIS SS_TAP(X_KP_0) SS_TAP(X_KP_1) SS_TAP(X_KP_3) SS_TAP(X_KP_3)

void dance_smartdash(tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {
        SEND_STRING(host_os == OS_MAC ? MAC_EN_DASH : WIN_EN_DASH);
    } else if (state->count == 2) {
        SEND_STRING(host_os == OS_MAC ? MAC_EM_DASH : WIN_EM_DASH);
    }
}

void dance_smartdot(tap_dance_state_t *state, void *user_data) {
    switch (state->count) {
        case 1: SEND_STRING("."); break;
        case 2: SEND_STRING(".."); break;
        default:
            if (host_os == OS_MAC) {
                SEND_STRING(MAC_ELLIPSIS);
            } else {
                SEND_WIN_ALT_CODE(WIN_ELLIPSIS);
            }
            break;
    }
}

static void td_debug_tog_finished(tap_dance_state_t *state, void *user_data) {
    if (state->count >= 2) {
        debug_enable = !debug_enable;
        if (debug_enable) {
            PLAY_SONG(song_debug_on);
        } else {
            PLAY_SONG(song_debug_off);
        }
    }
}

static void td_eeclr_finished(tap_dance_state_t *state, void *user_data) {
    if (state->count >= 2) {
        // PLAY_SONG is asynchronous and the reset below would cut it off;
        // block long enough for the danger jingle to land.
        PLAY_SONG(song_eeprom_clear);
        wait_ms(700);
        eeconfig_init();
        soft_reset_keyboard();
    }
}

// Tap Dance Definitions
tap_dance_action_t tap_dance_actions[] = {
    [TD_AIRPODS]   = ACTION_TAP_DANCE_FN(dance_airpods),
    [TD_SMRTDSH]   = ACTION_TAP_DANCE_FN(dance_smartdash),
    [TD_SMRTDOT]   = ACTION_TAP_DANCE_FN(dance_smartdot),
    [TD_DEBUG_TOG] = ACTION_TAP_DANCE_FN(td_debug_tog_finished),
    [TD_EECLR]     = ACTION_TAP_DANCE_FN(td_eeclr_finished),
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // Bottom row: mods mirror outward from the index finger on both hands —
    // index = Cmd, ring = Alt/Option, pinky = Ctrl. Middle is unassigned.
    [BASE] = LAYOUT_moonlander(
    KC_ESC,         KC_1,         KC_2,    KC_3,    KC_4,          KC_5,       KC_LCBR, KC_RCBR, KC_6,    KC_7,    KC_8,    KC_9,    KC_0,            KC_EQL,
    LT(NAV,KC_TAB), KC_Q,         KC_W,    KC_E,    KC_R,          KC_T,       KC_LBRC, KC_RBRC, KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,            KC_BSPC,
    LCTL_T(KC_ESC), KC_A,         KC_S,    KC_D,    LT(FN,KC_F),   KC_G,       KC_LPRN, KC_RPRN, KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN,         LCTL_T(KC_ENT),
    KC_LSFT,        LALT_T(KC_Z), KC_X,    KC_C,    KC_V,          KC_B,                         KC_N,    KC_M,    KC_COMM, KC_DOT,  RALT_T(KC_SLSH), RSFT_T(KC_MINS),
    KC_GRV,         KC_LCTL,      KC_LALT, XXXXXXX, KC_LGUI,       TD(TD_AIRPODS),               TO(GAM), KC_RGUI, XXXXXXX, KC_RALT, KC_RCTL,         KC_QUOT,
    KC_SPC,         OSL(SYM),     MO(META),                                                                  MO(META), LT(META,KC_BSPC), KC_SPC
    ),

    [GAM] = LAYOUT_moonlander(
    _______, _______, _______, _______, _______, _______, XXXXXXX, XXXXXXX, _______, _______, _______, _______, _______, _______,
    KC_TAB,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    KC_LCTL, _______, _______, _______, KC_F,    _______, _______, _______, _______, _______, _______, _______, _______, KC_ENT,
    KC_LSFT, KC_Z,    _______, _______, _______, _______,                           _______, _______, _______, _______, KC_SLSH, _______,
    _______, _______, _______, _______, _______, _______,                           TO(BASE), _______, _______, _______, _______, _______,
    _______, _______, _______,                                                                  _______, KC_BSPC, _______
    ),

    [SYM] = LAYOUT_moonlander(
    KC_GRV,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,  _______,  _______,
    _______, _______, _______, _______, _______, _______, _______, _______, KC_7,    KC_8,    KC_9,    Z_OQUOTE, Z_PQUOTE, KC_BSLS,
    _______, _______, _______, _______, _______, _______, _______, _______, KC_4,    KC_5,    KC_6,    _______,  KC_QUOT,  _______,
    _______, _______, _______, _______, _______, _______,                           KC_1,    KC_2,    KC_3,    TD(TD_SMRTDOT), _______, TD(TD_SMRTDSH),
    _______, _______, _______, _______, _______, _______,                           _______, _______, _______, _______,  _______,  _______,
    _______, _______, _______,                                                                  _______, _______,  KC_0
    ),

    [FN] = LAYOUT_moonlander(
    QK_BOOT, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   _______, _______, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
    _______, KC_F11,  KC_F12,  KC_F13,  KC_F14,  KC_F15,  _______, _______, _______, _______, _______, _______, _______, _______,
    Z_OSWIN, _______, _______, _______, _______, _______, _______, _______, KC_MPRV, KC_VOLD, KC_VOLU, KC_MNXT, _______, _______,
    _______, _______, _______, _______, _______, _______,                           _______, KC_MUTE, _______, _______, _______, _______,
    _______, _______, _______, _______, Z_OSMAC, _______,                           QK_RBT,  _______, _______, _______, _______, _______,
    KC_MPLY, _______, _______,                                                                  _______, _______, KC_MPLY
    ),

    [NAV] = LAYOUT_moonlander(
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, XXXXXXX, Z_FWORD, Z__LEND, XXXXXXX, XXXXXXX, Z__PSTR, Z__PEND, XXXXXXX, XXXXXXX, XXXXXXX, Z__VIMO, Z_PASTE, XXXXXXX,
    XXXXXXX, Z__LSTR, XXXXXXX, XXXXXXX, XXXXXXX, Z__VIMG, XXXXXXX, XXXXXXX, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, XXXXXXX, XXXXXXX,
    KC_LSFT, XXXXXXX, KC_DEL,  XXXXXXX, TO(SEL), Z_BWORD,                           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_RSFT,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX,                                                                  XXXXXXX, XXXXXXX, XXXXXXX
    ),

    [SEL] = LAYOUT_moonlander(
    TO(BASE), XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, Z__LSTR, XXXXXXX,
    XXXXXXX, XXXXXXX, Z_FWORD, Z__LEND, XXXXXXX, XXXXXXX, Z__PSTR, Z__PEND, Z__COPY, XXXXXXX, XXXXXXX, XXXXXXX, Z_PASTE, XXXXXXX,
    TO(BASE), Z__LSTR, XXXXXXX, Z___CUT, XXXXXXX, Z__VIMG, KC_LPRN, KC_RPRN, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, XXXXXXX, XXXXXXX,
    KC_LSFT, XXXXXXX, Z___CUT, Z_CHNGE, XXXXXXX, Z_BWORD,                           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_RSFT,
    KC_GRV,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_QUOT,
    XXXXXXX, XXXXXXX, XXXXXXX,                                                                  XXXXXXX, XXXXXXX, XXXXXXX
    ),

    [META] = LAYOUT_moonlander(
    XXXXXXX,          XXXXXXX,      XXXXXXX, XXXXXXX,       RGB_VAD,        RGB_VAI,         XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX,          Z__LOCK,      RGB_HUD, RGB_HUI,       RGB_SAD,        RGB_SAI,         XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    TD(TD_DEBUG_TOG), XXXXXXX,      RGB_SPD, RGB_SPI,       RGB_RMOD,       RGB_MOD,         XXXXXXX, XXXXXXX, KC_HOME, KC_PGDN, KC_PGUP, KC_END,  XXXXXXX, XXXXXXX,
    XXXXXXX,          TD(TD_EECLR), RGB_TOG, HSV_0_255_255, HSV_74_255_255, HSV_169_255_255,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX,          XXXXXXX,      XXXXXXX, XXXXXXX,       XXXXXXX,        XXXXXXX,                           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    Z__MCTL,          XXXXXXX,      XXXXXXX,                                                                            XXXXXXX, XXXXXXX, XXXXXXX
    ),
};
// clang-format on

void keyboard_post_init_user(void) {
    rgb_matrix_enable();
    host_os = OS_MAC;
    set_tempo(150);
    // Debug console on at boot so action_tapping's tap-hold decisions show in
    // `qmk console` while the new flow-tap scheme is being tuned. Toggle from
    // META via the debug tap-dance (with audio feedback).
    debug_enable = true;
}

// Debug-gated logger. The META layer's debug tap-dance flips `debug_enable`
// to silence/restore output.
#define LOG(...) do { if (debug_enable) uprintf(__VA_ARGS__); } while (0)

static void log_event(uint16_t keycode, keyrecord_t *record) {
    LOG("t=%lu %s kc=%04X\n",
        (unsigned long)record->event.time,
        record->event.pressed ? "DN" : "UP",
        keycode);
}

// Audio feedback for caps_word: it has no other persistent indicator, and
// the up/down ticks confirm engagement without looking at the board.
void caps_word_set_user(bool active) {
    if (active) {
        PLAY_SONG(song_caps_on);
    } else {
        PLAY_SONG(song_caps_off);
    }
}

// ============================================================================
// Tap-hold per-key behavior (ported from janeway)
// ============================================================================
// Pinky mods snap-fire on other-key press (HOLD_ON_OTHER_KEY_PRESS) — they
// need fast modifier feel and aren't at risk of misfiring during typing
// rolls: you don't roll INTO a pinky-mod position mid-word. Everything else —
// letter mods, layer-taps — returns false here and uses PERMISSIVE_HOLD
// (below), so typing rolls like "mo" or "ve" settle as TAP. (Cmd/Alt/Ctrl on
// the bottom row are plain modifiers and never enter tap-hold resolution.)
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LCTL_T(KC_ESC):
        case LCTL_T(KC_ENT):
        case RSFT_T(KC_MINS):
            return true;
    }
    return false;
}

// Inverse of the above. Mod-tap settles as hold only if the other key was
// pressed AND released before the mod-tap was released. Intentional chord
// (hold F, tap 1, release 1, release F) -> FUN+1. Typing roll (Z down, O
// down, Z up before O up) -> "zo".
bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LCTL_T(KC_ESC):
        case LCTL_T(KC_ENT):
        case RSFT_T(KC_MINS):
            return false;
    }
    return true;
}

// Pinky mods are physically isolated and reliable holds — exempt them from
// flow-tap suppression so Ctrl+, / Ctrl+Enter / Shift+- work even mid-typing.
// Letter mods (Z, /, F) and the thumb spaces keep flow-tap protection.
uint16_t get_flow_tap_term(uint16_t keycode, keyrecord_t *record, uint16_t prev_keycode) {
    switch (keycode) {
        case LCTL_T(KC_ESC):
        case LCTL_T(KC_ENT):
        case RSFT_T(KC_MINS):
            return 0;
    }
    if (is_flow_tap_key(keycode) && is_flow_tap_key(prev_keycode)) {
        return FLOW_TAP_TERM;
    }
    return 0;
}

// Tiny optional<T>-style holders so the saved animation/color don't drift
// out of sync with their has_value flags.
typedef struct {
    bool    set;
    uint8_t mode;
} saved_mode_t;

typedef struct {
    bool set;
    HSV  hsv;
} saved_hsv_t;

static saved_mode_t saved_mode;
static saved_hsv_t  saved_hsv;

layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t top = get_highest_layer(state);

    // GAM (gaming) mode: stash the current animation, switch to spiral.
    // Restore when leaving GAM.
    if (IS_LAYER_ON_STATE(state, GAM)) {
        if (rgb_matrix_get_mode() != RGB_MATRIX_CYCLE_SPIRAL) {
            saved_mode.mode = rgb_matrix_get_mode();
            saved_mode.set  = true;
            rgb_matrix_mode(RGB_MATRIX_CYCLE_SPIRAL);
        }
    } else if (saved_mode.set) {
        rgb_matrix_mode(saved_mode.mode);
        saved_mode.set = false;
    }

    // When going into another layer, dim the underlying RGB animation so the
    // key highlights (set below in rgb_matrix_indicators_advanced_user) really
    // stand out. Skip this for META, since we want RGB changes made there to
    // persist (and its illumination paints every key anyway).
    bool dimmed_layer = (top != BASE && top != GAM && top != META);
    if (dimmed_layer && !saved_hsv.set) {
        saved_hsv.hsv = rgb_matrix_get_hsv();
        saved_hsv.set = true;
        rgb_matrix_sethsv_noeeprom(saved_hsv.hsv.h, saved_hsv.hsv.s, saved_hsv.hsv.v > 60 ? saved_hsv.hsv.v - 60 : 0);
    } else if ((top == BASE || top == GAM) && saved_hsv.set) {
        rgb_matrix_sethsv(saved_hsv.hsv.h, saved_hsv.hsv.s, saved_hsv.hsv.v);
        saved_hsv.set = false;
    }
    return state;
}

// ============================================================================
// META layer illumination (ported from janeway's META layer)
// ============================================================================
// Per-key colors and animations when META is active. Each keycode gets its own
// treatment — RGB knobs animate to hint at their function, utilities use
// solid distinct colors, dead positions (KC_NO) are off.

static uint8_t sawtooth(uint32_t t, uint16_t period_ms) {
    return (uint8_t)((t % period_ms) * 255 / period_ms);
}

static uint8_t triangle(uint32_t t, uint16_t period_ms) {
    uint16_t phase = t % period_ms;
    uint16_t half = period_ms / 2;
    return phase < half ? (phase * 255 / half) : (255 - (phase - half) * 255 / half);
}

static void hsv_to_rgb_set(uint8_t led, uint8_t h, uint8_t s, uint8_t v) {
    hsv_t hsv = {h, s, v};
    rgb_t rgb = hsv_to_rgb(hsv);
    rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
}

static void meta_set_led(uint8_t led, uint16_t kc, uint32_t t) {
    switch (kc) {
        // --- Brightness (white sawtooth) ---
        case RGB_VAI: {
            uint8_t v = sawtooth(t, 1500);
            rgb_matrix_set_color(led, v, v, v);
            break;
        }
        case RGB_VAD: {
            uint8_t v = 255 - sawtooth(t, 1500);
            rgb_matrix_set_color(led, v, v, v);
            break;
        }
        // --- Hue (rainbow scroll) ---
        case RGB_HUI: hsv_to_rgb_set(led, sawtooth(t, 2000), 255, 200); break;
        case RGB_HUD: hsv_to_rgb_set(led, 255 - sawtooth(t, 2000), 255, 200); break;
        // --- Saturation (gray <-> color) ---
        case RGB_SAI: hsv_to_rgb_set(led, 0, sawtooth(t, 1500), 200); break;
        case RGB_SAD: hsv_to_rgb_set(led, 0, 255 - sawtooth(t, 1500), 200); break;
        // --- Speed (slow / fast pulse) ---
        case RGB_SPI: { uint8_t v = triangle(t, 400);  rgb_matrix_set_color(led, v, v, v); break; }
        case RGB_SPD: { uint8_t v = triangle(t, 2000); rgb_matrix_set_color(led, v, v, v); break; }
        // --- Mode cycling (fixed hue palette) ---
        case RGB_MOD:  hsv_to_rgb_set(led, sawtooth(t, 3000), 220, 200); break;
        case RGB_RMOD: hsv_to_rgb_set(led, 255 - sawtooth(t, 3000), 220, 200); break;
        // --- Toggle (blink on/off) ---
        case RGB_TOG: {
            uint8_t v = ((t / 500) & 1) ? 200 : 30;
            rgb_matrix_set_color(led, v, v, v);
            break;
        }
        // --- HSV presets: solid in their color ---
        case HSV_0_255_255:   rgb_matrix_set_color(led, 255, 0, 0);   break;
        case HSV_74_255_255:  rgb_matrix_set_color(led, 120, 255, 0); break;
        case HSV_169_255_255: rgb_matrix_set_color(led, 0, 60, 255);  break;
        // --- Utility actions ---
        case Z__MCTL:           // Mission Control / Task View — solid purple
            rgb_matrix_set_color(led, 140, 0, 200);
            break;
        case Z__LOCK:           // Lock screen — solid amber
            rgb_matrix_set_color(led, 255, 140, 0);
            break;
        // --- Tap dances ---
        case TD(TD_DEBUG_TOG): {
            // State-reflective: pulse bright green when debug ON, dim solid when OFF.
            if (debug_enable) {
                uint8_t v = 130 + triangle(t, 1200) / 2;
                rgb_matrix_set_color(led, 0, v, 30);
            } else {
                rgb_matrix_set_color(led, 0, 40, 10);
            }
            break;
        }
        case TD(TD_EECLR):
            rgb_matrix_set_color(led, 200, 0, 0);   // danger red, no animation
            break;
        // --- Navigation hints ---
        case KC_HOME:
        case KC_PGDN:
        case KC_PGUP:
        case KC_END:
            rgb_matrix_set_color(led, 0, 150, 200); // solid cyan
            break;
        // --- Dead positions ---
        default:
            rgb_matrix_set_color(led, 0, 0, 0);
            break;
    }
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t layer = get_highest_layer(layer_state);
    if (layer == GAM) {
        // WASD highlights in white.
        rgb_matrix_set_color(7, 255, 255, 255);
        rgb_matrix_set_color(11, 255, 255, 255);
        rgb_matrix_set_color(12, 255, 255, 255);
        rgb_matrix_set_color(17, 255, 255, 255);
        return false;
    }
    if (layer == BASE) return false;

    if (layer == META) {
        uint32_t t = timer_read32();
        for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
            for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
                uint8_t index = g_led_config.matrix_co[row][col];
                if (index < led_min || index >= led_max || index == NO_LED) continue;
                meta_set_led(index, keymap_key_to_keycode(META, (keypos_t){col, row}), t);
            }
        }
        return false;
    }

    // For non-base layers, color-code every bound key by its semantic class.
    for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
        for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
            uint8_t index = g_led_config.matrix_co[row][col];
            if (index < led_min || index >= led_max || index == NO_LED) continue;
            uint16_t keycode = keymap_key_to_keycode(layer, (keypos_t){col, row});

            // Two F-key ranges: the HID codes between F12 and F13 are the nav
            // cluster (arrows, Home/End, etc.) — don't paint those green.
            if ((keycode >= KC_F1 && keycode <= KC_F12) || (keycode >= KC_F13 && keycode <= KC_F24)) {
                rgb_matrix_set_color(index, RGB_GREEN);
            } else if (keycode >= Z_START && keycode <= Z_END) {
                rgb_matrix_set_color(index, RGB_RED);
            } else if (keycode >= QK_TAP_DANCE && keycode <= QK_TAP_DANCE_MAX) {
                rgb_matrix_set_color(index, RGB_RED); // tap-dances are custom actions too
            } else if (keycode >= KC_AUDIO_MUTE && keycode <= KC_MEDIA_EJECT) {
                rgb_matrix_set_color(index, RGB_BLUE);
            } else if (keycode >= RGB_TOG && keycode <= RGB_SPD) {
                rgb_matrix_set_color(index, RGB_PURPLE);
            } else if (keycode > KC_TRANSPARENT) {
                rgb_matrix_set_color(index, RGB_WHITE);
            }
        }
    }
    return false;
}

// NAV layer: Vim-style cursor navigation. KC_LEFT/RIGHT/UP/DOWN pass through
// unless shift is held, in which case shift is consumed and we jump to the
// line edge / next page instead.
static bool process_movement_key(uint16_t keycode) {
    const os_bindings_t *b = bindings();
    switch (keycode) {
        case KC_LEFT:
            if (!SHIFTED) return true;
            UNSHIFT(send_string(b->line_home_jump));
            return false;
        case KC_RIGHT:
            if (!SHIFTED) return true;
            UNSHIFT(send_string(b->line_end_jump));
            return false;
        case KC_DOWN:
            if (!SHIFTED) return true;
            UNSHIFT(send_string(b->page_down));
            return false;
        case KC_UP:
            if (!SHIFTED) return true;
            UNSHIFT(send_string(b->page_up));
            return false;
        case Z_FWORD: send_string(b->word_forward); return false;
        case Z_BWORD: send_string(b->word_back);    return false;
        case Z__LSTR: send_string(b->line_start);   return false;
        case Z__LEND: send_string(b->line_end);     return false;
        case Z__PSTR: send_string(b->para_start);   return false;
        case Z__PEND: send_string(b->para_end);     return false;
        case Z__VIMG:
            if (SHIFTED) UNSHIFT(send_string(b->doc_end));
            else         send_string(b->doc_start);
            return false;
        case Z__VIMO:
            if (SHIFTED) UNSHIFT(send_string(b->open_above));
            else         send_string(b->open_below);
            return false;
        case Z_PASTE: send_string(b->paste); return false;
    }
    return true;
}

// SEL layer: same semantic actions as NAV, but every cursor move is wrapped
// in shift so it extends the selection. Some keys also return to BASE afterwards.
static bool process_selection_key(uint16_t keycode) {
    const os_bindings_t *b = bindings();
    switch (keycode) {
        // Arrow keys: extend selection by one step (or to line edge when shifted).
        case KC_LEFT:
            if (SHIFTED) send_string(b->line_home_jump);
            else         SEND_STRING(SS_LSFT(SS_TAP(X_LEFT)));
            return false;
        case KC_RIGHT:
            if (SHIFTED) send_string(b->line_end_jump);
            else         SEND_STRING(SS_LSFT(SS_TAP(X_RIGHT)));
            return false;
        case KC_DOWN:
            if (SHIFTED) send_string(b->page_down);
            else         SEND_STRING(SS_LSFT(SS_TAP(X_DOWN)));
            return false;
        case KC_UP:
            if (SHIFTED) send_string(b->page_up);
            else         SEND_STRING(SS_LSFT(SS_TAP(X_UP)));
            return false;
        case Z_FWORD: send_shifted(b->word_forward); return false;
        case Z_BWORD: send_shifted(b->word_back);    return false;
        case Z__LSTR: send_shifted(b->line_start);   return false;
        case Z__LEND: send_shifted(b->line_end);     return false;
        case Z__PSTR: send_shifted(b->para_start);   return false;
        case Z__PEND: send_shifted(b->para_end);     return false;
        case Z__VIMG:
            if (SHIFTED) UNSHIFT(send_shifted(b->doc_end));
            else         send_shifted(b->doc_start);
            return false;
        // Modify/clipboard actions: do the action, then drop back to BASE.
        case Z_CHNGE:
            SEND_STRING(SS_TAP(X_BSPC));
            layer_move(BASE);
            return false;
        case Z__COPY:
            send_string(b->copy);
            // Move cursor left so it doesn't sit at the end of the selection.
            SEND_STRING(SS_TAP(X_LEFT));
            layer_move(BASE);
            return false;
        case Z___CUT:
            send_string(b->cut);
            layer_move(BASE);
            return false;
        case Z_PASTE:
            send_string(b->paste);
            layer_move(BASE);
            return false;
    }
    // Anything else: drop back to BASE (except shift, so the user can still
    // build up modifiers before triggering a selection action).
    if (keycode != KC_LEFT_SHIFT && keycode != KC_RIGHT_SHIFT) {
        layer_move(BASE);
    }
    return true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    log_event(keycode, record);

    switch (keycode) {
        case Z_OSMAC:
            if (record->event.pressed) {
                host_os = OS_MAC;
                PLAY_SONG(song_mac_mode);
            }
            return false;
        case Z_OSWIN:
            if (record->event.pressed) {
                host_os = OS_WIN;
                PLAY_SONG(song_win_mode);
            }
            return false;
        case TO(GAM):
            if (record->event.pressed) PLAY_SONG(song_gaming_mode);
            return true;
        case TO(BASE):
            if (record->event.pressed && IS_LAYER_ON(GAM)) PLAY_SONG(song_regular_mode);
            return true;
        case RGB_TOG:
            // QMK's rgb_matrix_disable() only flips the enable flag — the LED
            // hardware keeps painting the last rendered frame indefinitely.
            // Clear the buffer to black and push it BEFORE disabling so the
            // LEDs actually go dark.
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
        case HSV_0_255_255:
            if (record->event.pressed) {
                rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
                rgb_matrix_sethsv(0, 255, 255);
            }
            return false;
        case HSV_74_255_255:
            if (record->event.pressed) {
                rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
                rgb_matrix_sethsv(74, 255, 255);
            }
            return false;
        case HSV_169_255_255:
            if (record->event.pressed) {
                rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
                rgb_matrix_sethsv(169, 255, 255);
            }
            return false;
        case Z_OQUOTE:
            // SYM + O: left single quote (shift -> left double).
            if (record->event.pressed) {
                if (SHIFTED) {
                    if (host_os == OS_MAC) UNSHIFT(SEND_STRING(MAC_LEFT_DOUBLE_QUOTE));
                    else                   UNSHIFT(SEND_WIN_ALT_CODE(WIN_LEFT_DOUBLE_QUOTE));
                } else {
                    if (host_os == OS_MAC) SEND_STRING(MAC_LEFT_SINGLE_QUOTE);
                    else                   SEND_WIN_ALT_CODE(WIN_LEFT_SINGLE_QUOTE);
                }
            }
            return false;
        case Z_PQUOTE:
            // SYM + P: right single quote (shift -> right double).
            if (record->event.pressed) {
                if (SHIFTED) {
                    if (host_os == OS_MAC) UNSHIFT(SEND_STRING(MAC_RGHT_DOUBLE_QUOTE));
                    else                   UNSHIFT(SEND_WIN_ALT_CODE(WIN_RGHT_DOUBLE_QUOTE));
                } else {
                    if (host_os == OS_MAC) SEND_STRING(MAC_RGHT_SINGLE_QUOTE);
                    else                   SEND_WIN_ALT_CODE(WIN_RGHT_SINGLE_QUOTE);
                }
            }
            return false;
        case Z__LOCK:
            if (record->event.pressed) send_string(bindings()->lock);
            return false;
        case Z__MCTL:
            if (record->event.pressed) send_string(bindings()->mission_control);
            return false;
    }
    if (!record->event.pressed) return true;
    if (IS_LAYER_ON(NAV)) return process_movement_key(keycode);
    if (IS_LAYER_ON(SEL)) return process_selection_key(keycode);
    return true;
}
