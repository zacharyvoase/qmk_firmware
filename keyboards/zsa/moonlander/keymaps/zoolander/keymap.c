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

#include "audio.h"
#include "songs.h"
#include "zack.h"

// Shared layers/keycodes/tap-dances come from users/zack/zack.h. Board-only
// additions below.
enum zoolander_layers {
    GAM = ZACK_LAYER_SAFE_RANGE, // Gaming (no tap dances/holds, low latency)
    QSL,                         // Quick-select: SEL movement while held, BASE on release
};

enum zoolander_tap_dances {
    TD_AIRPODS = ZACK_TAP_DANCE_SAFE_RANGE, // Airpods-style media key
};

void dance_airpods(tap_dance_state_t *state, void *user_data) {
    if (!state->finished) return;
    switch (state->count) {
        case 1: SEND_STRING(SS_TAP(X_MEDIA_PLAY_PAUSE)); break;
        case 2: SEND_STRING(SS_TAP(X_MEDIA_NEXT_TRACK)); break;
        case 3: SEND_STRING(SS_TAP(X_MEDIA_PREV_TRACK)); break;
    }
    reset_tap_dance(state);
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

tap_dance_action_t tap_dance_actions[] = {
    [TD_SMRTDSH]   = ACTION_TAP_DANCE_FN(dance_smartdash),
    [TD_SMRTDOT]   = ACTION_TAP_DANCE_FN(dance_smartdot),
    [TD_DEBUG_TOG] = ACTION_TAP_DANCE_FN(td_debug_tog_finished),
    [TD_EECLR]     = ACTION_TAP_DANCE_FN(td_eeclr_finished),
    [TD_AIRPODS]   = ACTION_TAP_DANCE_FN(dance_airpods),
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
    KC_GRV,         KC_LCTL,      KC_LALT, MO(QSL), KC_LGUI,       TD(TD_AIRPODS),               TO(GAM), KC_RGUI, XXXXXXX, KC_RALT, KC_RCTL,         KC_QUOT,
    KC_SPC,         OSL(SYM),     MO(META),                                                                  MO(META), LT(SYM,KC_BSPC), KC_SPC
    ),

    [GAM] = LAYOUT_moonlander(
    _______, _______, _______, _______, _______, _______, XXXXXXX, XXXXXXX, _______, _______, _______, _______, _______, _______,
    KC_TAB,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    KC_LCTL, _______, _______, _______, KC_F,    _______, _______, _______, _______, _______, _______, _______, _______, KC_ENT,
    KC_LSFT, KC_Z,    _______, _______, _______, _______,                           _______, _______, _______, _______, KC_SLSH, _______,
    _______, _______, _______, XXXXXXX, _______, _______,                           TO(BASE), _______, _______, _______, _______, _______,
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
    _______, Z__LOCK, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    Z_OSWIN, _______, _______, _______, _______, _______, _______, _______, KC_MPRV, KC_VOLD, KC_VOLU, KC_MNXT, _______, _______,
    _______, _______, _______, _______, _______, _______,                           _______, KC_MUTE, _______, _______, _______, _______,
    _______, _______, _______, _______, Z_OSMAC, _______,                           QK_RBT,  _______, _______, _______, _______, _______,
    Z__MCTL, _______, _______,                                                                  _______, _______, KC_MPLY
    ),

    [NAV] = LAYOUT_moonlander(
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, XXXXXXX, Z_FWORD, Z__LEND, XXXXXXX, XXXXXXX, Z__PSTR, Z__PEND, XXXXXXX, XXXXXXX, XXXXXXX, Z__VIMO, Z_PASTE, XXXXXXX,
    XXXXXXX, Z__LSTR, XXXXXXX, XXXXXXX, XXXXXXX, Z__VIMG, Z__HBCK, Z__HFWD, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, XXXXXXX, XXXXXXX,
    KC_LSFT, XXXXXXX, KC_DEL,  XXXXXXX, TO(SEL), Z_BWORD,                           XXXXXXX, TO(META), XXXXXXX, XXXXXXX, XXXXXXX, KC_RSFT,
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
    TO(BASE),         XXXXXXX,      XXXXXXX, XXXXXXX,       RGB_VAD,        RGB_VAI,         XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX,          XXXXXXX,      RGB_HUD, RGB_HUI,       RGB_SAD,        RGB_SAI,         XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    TD(TD_DEBUG_TOG), XXXXXXX,      RGB_SPD, RGB_SPI,       RGB_RMOD,       RGB_MOD,         XXXXXXX, XXXXXXX, KC_HOME, KC_PGDN, KC_PGUP, KC_END,  XXXXXXX, XXXXXXX,
    XXXXXXX,          TD(TD_EECLR), RGB_TOG, HSV_0_255_255, HSV_74_255_255, HSV_169_255_255,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX,          XXXXXXX,      XXXXXXX, XXXXXXX,       XXXXXXX,        XXXXXXX,                           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX,          XXXXXXX,      XXXXXXX,                                                                            XXXXXXX, XXXXXXX, XXXXXXX
    ),

    // Quick-select: held momentarily from BASE (left middle finger, bottom
    // row, between LALT and LGUI). Same shift-wrapped movement as SEL, but no
    // clipboard actions and no sticky state — releasing the key drops
    // straight back to BASE.
    [QSL] = LAYOUT_moonlander(
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, XXXXXXX, Z_FWORD, Z__LEND, XXXXXXX, XXXXXXX, Z__PSTR, Z__PEND, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, Z__LSTR, XXXXXXX, XXXXXXX, XXXXXXX, Z__VIMG, XXXXXXX, XXXXXXX, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, XXXXXXX, XXXXXXX,
    KC_LSFT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, Z_BWORD,                           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_RSFT,
    XXXXXXX, XXXXXXX, XXXXXXX, _______, XXXXXXX, XXXXXXX,                           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX,                                                                  XXXXXXX, XXXXXXX, XXXXXXX
    ),
};
// clang-format on

void keyboard_post_init_user(void) {
    rgb_matrix_enable();
    set_tempo(150);
    // Debug console on at boot so action_tapping's tap-hold decisions show in
    // `qmk console` while the flow-tap scheme is being tuned. Toggle from
    // META via the debug tap-dance (with audio feedback).
    debug_enable = true;
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
// Tap-hold per-key behavior (scheme shared with janeway; lists are per-board)
// ============================================================================
// Pinky mods and the NAV gateway snap-fire on other-key press
// (HOLD_ON_OTHER_KEY_PRESS) — they need fast feel and aren't at risk of
// misfiring during typing rolls: you don't roll INTO a pinky position
// mid-word. Tab in particular drives hold+movement-key chords where the
// movement key may still be held when Tab releases, which permissive hold
// would misread as a tap. Everything else — letter mods, the F and
// backspace layer-taps — returns false here and uses PERMISSIVE_HOLD
// (below), so typing rolls like "mo" or "ve" settle as TAP. (Cmd/Alt/Ctrl on
// the bottom row are plain modifiers and never enter tap-hold resolution.)
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LCTL_T(KC_ESC):
        case LCTL_T(KC_ENT):
        case RSFT_T(KC_MINS):
        case LT(NAV, KC_TAB):
            return true;
    }
    return false;
}

// Inverse of the above. Mod-tap settles as hold only if the other key was
// pressed AND released before the mod-tap was released. Intentional chord
// (hold F, tap 1, release 1, release F) -> FN+1. Typing roll (Z down, O
// down, Z up before O up) -> "zo".
bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LCTL_T(KC_ESC):
        case LCTL_T(KC_ENT):
        case RSFT_T(KC_MINS):
        case LT(NAV, KC_TAB):
            return false;
    }
    return true;
}

// Pinky mods are physically isolated and reliable holds — exempt them from
// flow-tap suppression so Ctrl+, / Ctrl+Enter / Shift+- work even mid-typing.
// Letter mods (Z, /, F) keep flow-tap protection.
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
    bool  set;
    hsv_t hsv;
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
        zack_paint_meta_layer(led_min, led_max);
    } else {
        zack_paint_layer_classes(layer, led_min, led_max);
    }
    return false;
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
    }
    // Quick-select: SEL's shift-wrapped movement while the QSL key is held;
    // everything else (shift, KC_NO) passes through untouched.
    if (IS_LAYER_ON(QSL) && record->event.pressed) {
        return zack_process_selection_movement(keycode);
    }
    return process_record_zack(keycode, record);
}
