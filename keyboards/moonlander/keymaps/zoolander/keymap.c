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


#include "action_layer.h"
#include "color.h"
#include "config.h"
#include "keymap.h"
#include "quantum.h"
#include "rgb_matrix.h"
#include "rgb_matrix_types.h"
#include QMK_KEYBOARD_H
#include "audio.h"
#include "keycodes.h"
#include "version.h"
#include "songs.h"

enum layers {
    INS, // Base layer (Vim insert mode)
    GAM, // Gaming (no tap dances/holds, for low-latency input)
    NUM, // Numpad (+ turn backspace into delete)
    FUN, // Function (F-keys, media, other meta stuff)
    MOV, // Movement (Vim normal mode)
    SEL, // Selection (Vim visual mode)
    BLANK, // Blank, just exists as a template for copying all the ______ things into place
};

enum custom_keycodes {
    Z_START = SAFE_RANGE,
    Z_OSMAC, // Set host OS to Mac
    Z_OSWIN, // Set host OS to Windows
    // Text movement keys
    Z_FWORD, // forwards word
    Z_BWORD, // backwards word
    Z__LSTR, // line start
    Z__LEND, // line end
    Z__VIMG, // g => start of document / G => end of document (same as Vim)
    Z__VIMO, // o => end-of-line, return / O => start-of-line, return, up (similar to o/O in Vim)
    // Selection/modification keys
    Z_CHNGE, // Change (deletes selected text, goes back to 'insert' mode)
    Z___CUT,  // Cmd-X on mac, Ctrl-X on Windows
    Z__COPY, // Cmd-C on mac, Ctrl-C on Windows
    Z_PASTE, // Cmd-V on mac, Ctrl-V on Windows
    Z_END,
};

// Tap Dance Declarations
enum {
    TD_RGB = 0,
};

void dance_cycle_rgb(tap_dance_state_t *state, void *user_data) {
    if (state->finished) {
        if (state->count == 1) {
            rgb_matrix_step();
        } else if (state->count == 2) {
            rgb_matrix_step_reverse();
        }
        reset_tap_dance(state);
    }
}

// Tap Dance Definitions
tap_dance_action_t tap_dance_actions[] = {
  [TD_RGB]  = ACTION_TAP_DANCE_FN (dance_cycle_rgb),
  // Add other definitions here
};

// Determines what actual keycodes to send for movement and modification keys
enum host_os_types {
  OS_MAC,
  OS_WIN
};
enum host_os_types host_os;

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [INS] = LAYOUT_moonlander(
    KC_ESC,         KC_1,    KC_2,    KC_3,    KC_4,          KC_5,       CW_TOGG, TT(NUM), KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_EQL,
    LT(MOV,KC_TAB), KC_Q,    KC_W,    KC_E,    KC_R,          KC_T,       KC_LBRC, KC_RBRC, KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC,
    LCTL_T(KC_ESC), KC_A,    KC_S,    KC_D,    KC_F,          KC_G,       KC_LPRN, KC_RPRN, KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_ENT,
    KC_LSFT,        KC_Z,    KC_X,    KC_C,    KC_V,          KC_B,                                 KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, RSFT_T(KC_MINS),
    LT(NUM,KC_GRV), MO(FUN), KC_LCTL, KC_LALT, KC_LGUI,       TD(TD_RGB),                           TO(GAM), KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_QUOT,
    KC_SPC,         MO(NUM), KC_LGUI,                                                                           KC_RGUI, KC_BSLS, KC_SPC
    ),

    [GAM] = LAYOUT_moonlander(
    _______, _______, _______, _______, _______, _______, XXXXXXX, XXXXXXX, _______, _______, _______, _______, _______, _______,
    KC_TAB,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    KC_LCTL, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______,                           _______, _______, _______, _______, _______, _______,
    KC_GRV,  XXXXXXX, _______, _______, _______, XXXXXXX,                           TO(INS), _______, _______, _______, _______, _______,
    _______, MO(NUM), _______,                                                                  _______, _______, _______
    ),

    [NUM] = LAYOUT_moonlander(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_7,    KC_8,    KC_9,    _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_4,    KC_5,    KC_6,    _______, KC_DEL,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_1,    KC_2,    KC_3,    _______, _______,
    _______, _______, _______, _______, _______, _______,                           _______, _______, KC_0,    _______, _______, _______,
    _______, _______, _______, _______, _______, _______,                           _______, _______, _______, _______, _______, _______,
    _______, _______, _______,                                                                  _______, _______, _______
    ),

    [FUN] = LAYOUT_moonlander(
    _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   _______, _______, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  _______,
    _______, KC_F11,  KC_F12,  KC_F13,  KC_F14,  KC_F15,  _______, _______, _______, _______, _______, _______, _______, _______,
    Z_OSWIN, _______, _______, _______, _______, _______, _______, _______, KC_MPRV, KC_VOLD, KC_VOLU, KC_MNXT, _______, _______,
    _______, _______, _______, _______, _______, QK_BOOT,                           _______, KC_MUTE, _______, _______, _______, _______,
    _______, _______, _______, _______, Z_OSMAC, _______,                           QK_RBT,  _______, _______, _______, _______, _______,
    KC_MPLY, _______, _______,                                                                  _______, _______, KC_MPLY
    ),

    [MOV] = LAYOUT_moonlander(
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    _______, XXXXXXX, Z_FWORD, Z__LEND, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, Z__VIMO, Z_PASTE, XXXXXXX,
    XXXXXXX, Z__LSTR, XXXXXXX, XXXXXXX, XXXXXXX, Z__VIMG, XXXXXXX, XXXXXXX, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, XXXXXXX, XXXXXXX,
    KC_LSFT, XXXXXXX, KC_DEL,  XXXXXXX, TO(SEL), Z_BWORD,                           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_RSFT,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, XXXXXXX, XXXXXXX,                                                                  XXXXXXX, XXXXXXX, XXXXXXX
    ),

    [SEL] = LAYOUT_moonlander(
    TO(INS), XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, Z__LSTR, XXXXXXX,
    XXXXXXX, XXXXXXX, Z_FWORD, Z__LEND, XXXXXXX, XXXXXXX, KC_LBRC, KC_RBRC, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, Z_PASTE, XXXXXXX,
    TO(INS), Z__LSTR, XXXXXXX, Z___CUT, XXXXXXX, Z__VIMG, KC_LPRN, KC_RPRN, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, XXXXXXX, XXXXXXX,
    KC_LSFT, XXXXXXX, Z___CUT, Z_CHNGE, XXXXXXX, Z_BWORD,                           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_RSFT,
    KC_GRV,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_QUOT,
    XXXXXXX, XXXXXXX, XXXXXXX,                                                                  XXXXXXX, XXXXXXX, XXXXXXX
    ),

    [BLANK] = LAYOUT_moonlander(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______,                           _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______,                           _______, _______, _______, _______, _______, _______,
    _______, _______, _______,                                                                  _______, _______, _______
    ),
};

// clang-format on

void keyboard_post_init_user(void) {
    rgb_matrix_enable();
    host_os = OS_MAC;
    set_tempo(150);
}

uint8_t prev_rgb_mode = 0;
bool prev_rgb_mode_has_value = false;

layer_state_t layer_state_set_user(layer_state_t state) {
    if (IS_LAYER_ON_STATE(state, GAM)) {
        if (rgb_matrix_get_mode() != RGB_MATRIX_CYCLE_SPIRAL) {
            prev_rgb_mode = rgb_matrix_get_mode();
            prev_rgb_mode_has_value = true;
            rgb_matrix_mode(RGB_MATRIX_CYCLE_SPIRAL);
        }
    } else if (prev_rgb_mode_has_value) {
        rgb_matrix_mode(prev_rgb_mode);
        prev_rgb_mode = 0;
        prev_rgb_mode_has_value = false;
    }
    return state;
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t layer = get_highest_layer(layer_state);
    if (layer > GAM) {
        for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
            for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
                uint8_t index = g_led_config.matrix_co[row][col];
                if (index < led_min || index >= led_max || index == NO_LED) continue;
                uint16_t keycode = keymap_key_to_keycode(layer, (keypos_t){col,row});

                if (keycode >= KC_F1 && keycode <= KC_F24) {
                    rgb_matrix_set_color(index, RGB_GREEN);
                } else if (keycode >= Z_START && keycode <= Z_END) {
                    rgb_matrix_set_color(index, RGB_RED);
                } else if (keycode >= KC_AUDIO_MUTE && keycode <= KC_MEDIA_EJECT) {
                    rgb_matrix_set_color(index, RGB_BLUE);
                } else if (keycode > KC_TRANSPARENT) {
                    rgb_matrix_set_color(index, RGB_WHITE);
                }
            }
        }
    }
    return false;
}

bool process_movement_key(uint16_t keycode);
bool process_selection_key(uint16_t keycode);

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
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
            if (record->event.pressed) {
                PLAY_SONG(song_gaming_mode);
            }
            return true;
        case TO(INS):
            if (record->event.pressed && IS_LAYER_ON(GAM)) {
                PLAY_SONG(song_regular_mode);
            }
            return true;
    }
    if (IS_LAYER_ON(MOV) && record->event.pressed) {
        return process_movement_key(keycode);
    } else if (IS_LAYER_ON(SEL) && record->event.pressed) {
        return process_selection_key(keycode);
    }
    return true;
}

// Helpers for checking shift state and temporarily unshifting
#define SHIFTED (get_mods() & MOD_MASK_SHIFT)
#define UNSHIFT(...) { \
  uint8_t prevmods = get_mods(); \
  del_mods(MOD_MASK_SHIFT); \
  __VA_ARGS__; \
  set_mods(prevmods); \
}

bool process_movement_key(uint16_t keycode) {
  // Process arrow keys as-is.
  switch (keycode) {
    case KC_UP:
    case KC_DOWN:
    case KC_LEFT:
    case KC_RIGHT:
      return true;
  }

  if (host_os == OS_MAC) {
    switch (keycode) {
      case Z_FWORD:
        SEND_STRING(SS_LALT(SS_TAP(X_RIGHT)));
        return false;
      case Z_BWORD:
        SEND_STRING(SS_LALT(SS_TAP(X_LEFT)));
        return false;
      case Z__LSTR:
        SEND_STRING(SS_LCTL("a"));
        return false;
      case Z__LEND:
        SEND_STRING(SS_LCTL("e"));
        return false;
      case Z__VIMG:
        if (SHIFTED) {
          // Shifted; go to end of doc
          UNSHIFT(SEND_STRING(SS_LCMD(SS_TAP(X_DOWN))));
        } else {
          // Go to start of doc
          SEND_STRING(SS_LCMD(SS_TAP(X_UP)));
        }
        return false;
      case Z__VIMO:
        if (SHIFTED) {
          // Shifted; insert on previous line
          UNSHIFT(SEND_STRING(SS_LCTL("a") SS_TAP(X_ENTER) SS_TAP(X_UP)));
        } else {
          // Insert on next line
          SEND_STRING(SS_LCTL("e") SS_TAP(X_ENTER));
        }
        return false;
      case Z_PASTE:
        SEND_STRING(SS_LCMD("v"));
        return false;
    }
  } else if (host_os == OS_WIN) {
    switch (keycode) {
      case Z_FWORD:
        SEND_STRING(SS_LCTL(SS_TAP(X_RIGHT)));
        return false;
      case Z_BWORD:
        SEND_STRING(SS_LCTL(SS_TAP(X_LEFT)));
        return false;
      case Z__LSTR:
        SEND_STRING(SS_TAP(X_HOME));
        return false;
      case Z__LEND:
        SEND_STRING(SS_TAP(X_END));
        return false;
      case Z__VIMG:
        if (SHIFTED) {
          // Shifted; go to end of doc
          UNSHIFT(SEND_STRING(SS_LCTL(SS_TAP(X_END))));
        } else {
          // Go to start of doc
          SEND_STRING(SS_LCTL(SS_TAP(X_HOME)));
        }
        return false;
      case Z__VIMO:
        if (SHIFTED) {
          // Shifted; insert on previous line
          UNSHIFT(SEND_STRING(SS_TAP(X_HOME) SS_TAP(X_ENTER) SS_TAP(X_UP)));
        } else {
          // Insert on next line
          SEND_STRING(SS_TAP(X_END) SS_TAP(X_ENTER));
        }
        return false;
      case Z_PASTE:
        SEND_STRING(SS_LCTL("v"));
        return false;
    }
  }
  return true;
}

bool process_selection_key(uint16_t keycode) {
  // Process arrow keys by sending the shifted key.
  switch (keycode) {
    case KC_UP:
      SEND_STRING(SS_LSFT(SS_TAP(X_UP)));
      return false;
    case KC_DOWN:
      SEND_STRING(SS_LSFT(SS_TAP(X_DOWN)));
      return false;
    case KC_LEFT:
      SEND_STRING(SS_LSFT(SS_TAP(X_LEFT)));
      return false;
    case KC_RIGHT:
      SEND_STRING(SS_LSFT(SS_TAP(X_RIGHT)));
      return false;
  }

  if (host_os == OS_MAC) {
    switch (keycode) {
      case Z_FWORD:
        SEND_STRING(SS_LSFT(SS_LALT(SS_TAP(X_RIGHT))));
        return false;
      case Z_BWORD:
        SEND_STRING(SS_LSFT(SS_LALT(SS_TAP(X_LEFT))));
        return false;
      case Z__LSTR:
        SEND_STRING(SS_LSFT(SS_LCTL("a")));
        return false;
      case Z__LEND:
        SEND_STRING(SS_LSFT(SS_LCTL("e")));
        return false;
      case Z__VIMG:
        if (SHIFTED) {
          // Shifted; go to end of doc
          UNSHIFT(SEND_STRING(SS_LSFT(SS_LCMD(SS_TAP(X_DOWN)))));
        } else {
          // Go to start of doc
          SEND_STRING(SS_LSFT(SS_LCMD(SS_TAP(X_UP))));
        }
        return false;
      case Z_CHNGE:
        SEND_STRING(SS_TAP(X_BSPC));
        layer_move(INS);
        return false;
      case Z__COPY:
        SEND_STRING(SS_LCMD("c") SS_TAP(X_LEFT));
        layer_move(INS);
        return false;
      case Z___CUT:
        SEND_STRING(SS_LCMD("x"));
        layer_move(INS);
        return false;
      case Z_PASTE:
        SEND_STRING(SS_LCMD("v"));
        layer_move(INS);
        return false;
    }
  } else if (host_os == OS_WIN) {
    switch (keycode) {
      case Z_FWORD:
        SEND_STRING(SS_LSFT(SS_LCTL(SS_TAP(X_RIGHT))));
        return false;
      case Z_BWORD:
        SEND_STRING(SS_LSFT(SS_LCTL(SS_TAP(X_LEFT))));
        return false;
      case Z__LSTR:
        SEND_STRING(SS_LSFT(SS_TAP(X_HOME)));
        return false;
      case Z__LEND:
        SEND_STRING(SS_LSFT(SS_TAP(X_END)));
        return false;
      case Z__VIMG:
        if (SHIFTED) {
          // Shifted; go to end of doc
          UNSHIFT(SEND_STRING(SS_LSFT(SS_LCTL(SS_TAP(X_END)))));
        } else {
          // Go to start of doc
          SEND_STRING(SS_LSFT(SS_LCTL(SS_TAP(X_HOME))));
        }
        return false;
      case Z_CHNGE:
        SEND_STRING(SS_TAP(X_BSPC));
        layer_move(INS);
        return false;
      case Z__COPY:
        SEND_STRING(SS_LCTL("c") SS_TAP(X_LEFT));
        layer_move(INS);
        return false;
      case Z___CUT:
        SEND_STRING(SS_LCTL("x"));
        layer_move(INS);
        return false;
      case Z_PASTE:
        SEND_STRING(SS_LCTL("v"));
        layer_move(INS);
        return false;
    }
  }
  // Fall-through: anything other than shift, switch back to Insert mode
  if (keycode != KC_LEFT_SHIFT && keycode != KC_RIGHT_SHIFT) {
    layer_move(INS);
  }
  // Do normal handling of the key.
  return true;
}
