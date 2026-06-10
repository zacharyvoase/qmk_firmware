/* Copyright 2024 ZSA Technology Labs, Inc <@zsa>
 * Copyright 2026 Zachary Voase <zack@meat.io>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

#include QMK_KEYBOARD_H

#include "zack.h"

// Shared layers/keycodes/tap-dances come from users/zack/zack.h. Janeway is
// Mac-only: host_os stays at its OS_MAC default and nothing binds the OS
// toggles. No board-specific layers or dances — the Voyager runs exactly the
// shared model.

// Debug/EE_CLR dances are board-local because feedback differs per board:
// the Moonlander chirps through its speaker, the Voyager is silent.
static void td_debug_tog_finished(tap_dance_state_t *state, void *user_data) {
    if (state->count >= 2) {
        debug_enable = !debug_enable;
    }
}

static void td_eeclr_finished(tap_dance_state_t *state, void *user_data) {
    if (state->count >= 2) {
        eeconfig_init();
        soft_reset_keyboard();
    }
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_SMRTDSH]   = ACTION_TAP_DANCE_FN(dance_smartdash),
    [TD_SMRTDOT]   = ACTION_TAP_DANCE_FN(dance_smartdot),
    [TD_DEBUG_TOG] = ACTION_TAP_DANCE_FN(td_debug_tog_finished),
    [TD_EECLR]     = ACTION_TAP_DANCE_FN(td_eeclr_finished),
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [BASE] = LAYOUT_voyager(
    KC_ESCAPE,      KC_1,           KC_2,           KC_3,           KC_4,           KC_5,                                           KC_6,           KC_7,           KC_8,           KC_9,           KC_0,           KC_EQUAL,
    LT(NAV, KC_TAB),KC_Q,           KC_W,           KC_E,           KC_R,           KC_T,                                           KC_Y,           KC_U,           KC_I,           KC_O,           KC_P,           KC_BSPC,
    MT(MOD_LCTL, KC_ESCAPE),KC_A,   KC_S,           KC_D,           LT(FN, KC_F),   KC_G,                                           KC_H,           KC_J,           KC_K,           KC_L,           KC_SCLN,        MT(MOD_LCTL, KC_ENTER),
    KC_LSFT,        MT(MOD_LALT, KC_Z),KC_X,        KC_C,           KC_V,           KC_B,                                           KC_N,           KC_M,           KC_COMMA,       KC_DOT,         MT(MOD_RALT, KC_SLASH),MT(MOD_RSFT, KC_MINUS),
                                                    MT(MOD_LGUI, KC_SPACE),OSL(SYM),                                LT(SYM, KC_BSPC),MT(MOD_RGUI, KC_SPACE)
  ),
  [SYM] = LAYOUT_voyager(
    KC_GRAVE,       KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_LCBR,                                        KC_RCBR,        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_LBRC,                                        KC_RBRC,        KC_TRANSPARENT, KC_TRANSPARENT, Z_OQUOTE,       Z_PQUOTE,       KC_BSLS,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_LPRN,                                        KC_RPRN,        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_QUOTE,       KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, S(KC_COMMA),                                    S(KC_DOT),      KC_TRANSPARENT, KC_TRANSPARENT, TD(TD_SMRTDOT), KC_TRANSPARENT, TD(TD_SMRTDSH),
                                                    KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT
  ),
  [META] = LAYOUT_voyager(
    TO(BASE),         KC_NO,            KC_NO,         KC_NO,           RGB_VAD,         RGB_VAI,                                       KC_NO,         KC_NO,         KC_NO,         KC_NO,         KC_NO,         KC_NO,
    KC_NO,            KC_NO,            RGB_HUD,       RGB_HUI,         RGB_SAD,         RGB_SAI,                                       KC_NO,         KC_NO,         KC_NO,         KC_NO,         KC_NO,         KC_NO,
    TD(TD_DEBUG_TOG), KC_NO,            RGB_SPD,       RGB_SPI,         RGB_RMOD,        RGB_MOD,                                       KC_HOME,       KC_PAGE_DOWN,  KC_PAGE_UP,    KC_END,        KC_NO,         KC_NO,
    KC_NO,            TD(TD_EECLR),     RGB_TOG,       HSV_0_255_255,   HSV_74_255_255,  HSV_169_255_255,                               KC_NO,         KC_NO,         KC_NO,         KC_NO,         KC_NO,         KC_NO,
                                                      KC_NO,           KC_NO,                                       KC_NO,         KC_NO
  ),
  [NAV] = LAYOUT_voyager(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, Z_FWORD,        Z__LEND,        KC_TRANSPARENT, Z__HBCK,                                        Z__HFWD,        KC_TRANSPARENT, KC_TRANSPARENT, Z__VIMO,        Z_PASTE,        KC_TRANSPARENT,
    KC_TRANSPARENT, Z__LSTR,        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, Z__VIMG,                                        KC_LEFT,        KC_DOWN,        KC_UP,          KC_RIGHT,       KC_TRANSPARENT, KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, TO(SEL),        Z_BWORD,                                        KC_TRANSPARENT, TO(META),       KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
                                                    KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT
  ),
  [SEL] = LAYOUT_voyager(
    TO(BASE),       KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
    KC_NO,          KC_NO,          Z_FWORD,        Z__LEND,        KC_NO,          KC_NO,                                          Z__COPY,        KC_NO,          KC_NO,          KC_NO,          Z_PASTE,        KC_NO,
    TO(BASE),       Z__LSTR,        KC_NO,          KC_NO,          KC_NO,          Z__VIMG,                                        KC_LEFT,        KC_DOWN,        KC_UP,          KC_RIGHT,       KC_NO,          KC_NO,
    KC_LSFT,        KC_NO,          Z___CUT,        Z_CHNGE,        KC_NO,          Z_BWORD,                                        KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_RSFT,
                                                    KC_NO,          KC_NO,                                          KC_NO,          KC_NO
  ),
  [FN] = LAYOUT_voyager(
    QK_BOOT,        KC_F1,          KC_F2,          KC_F3,          KC_F4,          KC_F5,                                          KC_F6,          KC_F7,          KC_F8,          KC_F9,          KC_F10,         KC_F11,
    KC_TRANSPARENT, Z__LOCK,        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_MEDIA_PREV_TRACK,KC_AUDIO_VOL_DOWN,KC_AUDIO_VOL_UP,KC_MEDIA_NEXT_TRACK,KC_TRANSPARENT, KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_AUDIO_MUTE,  KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
                                                    Z__MCTL,        KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_MEDIA_PLAY_PAUSE
  ),
};
// clang-format on

void keyboard_post_init_user(void) {
    // Debug console on at boot so action_tapping's tap-hold decisions show in
    // `qmk console` while tuning. Toggle from META via the debug tap-dance.
    debug_enable = true;
}

// ============================================================================
// Tap-hold per-key behavior (scheme shared with zoolander; lists per-board)
// ============================================================================
// Pinky mods, the NAV gateway, and both thumb Cmd spaces snap-fire on
// other-key press. Pinkies and Tab are physically isolated from typing
// rolls; the thumb Cmd spaces are a Voyager-only deviation (the Moonlander
// keeps Cmd on its bottom row) and rely on FLOW_TAP_TERM for roll
// protection. Everything else — letter mods, the F and backspace layer-taps
// — uses PERMISSIVE_HOLD so typing rolls settle as TAP.
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MT(MOD_LCTL, KC_ESCAPE):
        case MT(MOD_LCTL, KC_ENTER):
        case MT(MOD_RSFT, KC_MINUS):
        case LT(NAV, KC_TAB):
        case MT(MOD_LGUI, KC_SPACE):
        case MT(MOD_RGUI, KC_SPACE):
            return true;
    }
    return false;
}

bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MT(MOD_LCTL, KC_ESCAPE):
        case MT(MOD_LCTL, KC_ENTER):
        case MT(MOD_RSFT, KC_MINUS):
        case LT(NAV, KC_TAB):
        case MT(MOD_LGUI, KC_SPACE):
        case MT(MOD_RGUI, KC_SPACE):
            return false;
    }
    return true;
}

// Pinky mods are physically isolated and reliable holds — exempt them from
// flow-tap suppression so Ctrl+, / Ctrl+Enter / Shift+- work even mid-typing.
// Letter mods (Z, /, F) and the thumb spaces keep flow-tap protection.
uint16_t get_flow_tap_term(uint16_t keycode, keyrecord_t *record, uint16_t prev_keycode) {
    switch (keycode) {
        case MT(MOD_LCTL, KC_ESCAPE):
        case MT(MOD_LCTL, KC_ENTER):
        case MT(MOD_RSFT, KC_MINUS):
            return 0;
    }
    if (is_flow_tap_key(keycode) && is_flow_tap_key(prev_keycode)) {
        return FLOW_TAP_TERM;
    }
    return 0;
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t layer = get_highest_layer(layer_state);
    if (layer == BASE) return false;  // let the default animation run on BASE

    if (layer == META) {
        zack_paint_meta_layer(led_min, led_max);
    } else {
        zack_paint_layer_classes(layer, led_min, led_max);
    }
    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    log_event(keycode, record);
    return process_record_zack(keycode, record);
}
