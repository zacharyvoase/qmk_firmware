#include QMK_KEYBOARD_H
#include "version.h"

#define KC_MAC_UNDO LGUI(KC_Z)
#define KC_MAC_CUT LGUI(KC_X)
#define KC_MAC_COPY LGUI(KC_C)
#define KC_MAC_PASTE LGUI(KC_V)
#define KC_PC_UNDO LCTL(KC_Z)
#define KC_PC_CUT LCTL(KC_X)
#define KC_PC_COPY LCTL(KC_C)
#define KC_PC_PASTE LCTL(KC_V)
#define LSA_T(kc) MT(MOD_LSFT | MOD_LALT, kc)

#define LAYER_BASE 0
#define LAYER_SYMBOL 1
#define LAYER_NUMPAD 2
#define LAYER_FUNCTION 3
#define LAYER_MOVEMENT 4
#define LAYER_SELECTION 5
#define LAYER_GAMING 6

#define SHIFTED (get_mods() & MOD_MASK_SHIFT)
#define UNSHIFT(...) { \
  uint8_t prevmods = get_mods(); \
  del_mods(MOD_MASK_SHIFT); \
  __VA_ARGS__; \
  set_mods(prevmods); \
}

enum custom_keycodes {
  RGB_SLD = ML_SAFE_RANGE,
  // Set the host OS, for proper movement/modification keys
  Z_SETOS_MAC,
  Z_SETOS_WIN,
  // Text movement keys
  Z_NEXTWORD,
  Z_PREVWORD,
  Z_LINESTART,
  Z_LINEEND,
  Z_DOCSTARTEND, // Beginning of doc normally, end of doc when shifted
  // Text modification keys
  Z_CHANGE,
  Z_CUT,
  Z_COPY,
  Z_PASTE,
  Z_NEWLINEINS, // Similar to Vim's o/O (varies with shift)
};

// Determines what actual keycodes to send for movement and modification keys
enum host_os_types {
  OS_MAC,
  OS_WIN
};

#ifdef AUDIO_ENABLE
float song_mac_mode[][2] = SONG(
        Q__NOTE(_A4),
        Q__NOTE(_AS4),
        Q__NOTE(_B4),
        HD_NOTE(_C5),
);

float song_win_mode[][2] = SONG(
        HD_NOTE(_EF6),
        Q__NOTE(_EF4),
        HD_NOTE(_BF5),
        H__NOTE(_AF5),
        H__NOTE(_EF4),
        H__NOTE(_EF6),
        WD_NOTE(_BF5),
);

float song_gaming_mode[][2] = SONG(
        HD_NOTE(_A4),
        Q__NOTE(_E4),
        Q__NOTE(_A4),
        HD_NOTE(_B4),
        Q__NOTE(_E4),
        Q__NOTE(_B4),
        W__NOTE(_CS5),
);

float song_regular_mode[][2] = SONG(
        Q__NOTE(_G5),
        Q__NOTE(_FS5),
        Q__NOTE(_DS5),
        Q__NOTE(_A4),
        Q__NOTE(_GS4),
        Q__NOTE(_E5),
        Q__NOTE(_GS5),
        HD_NOTE(_C6),
);
#endif

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT_moonlander(
    KC_ESCAPE,                 KC_1,               KC_2,     KC_3,    KC_4,    KC_5,        KC_CAPSLOCK,                                    TT(LAYER_NUMPAD), KC_6,             KC_7,    KC_8,     KC_9,   KC_0,      KC_EQUAL,
    LT(LAYER_MOVEMENT,KC_TAB), KC_Q,               KC_W,     KC_E,    KC_R,    KC_T,        KC_LBRACKET,                                    KC_RBRACKET,      KC_Y,             KC_U,    KC_I,     KC_O,   KC_P,      KC_BSPACE,
    LCTL_T(KC_ESC),            KC_A,               KC_S,     KC_D,    KC_F,    KC_G,        KC_LPRN,                                        KC_RPRN,          KC_H,             KC_J,    KC_K,     KC_L,   KC_SCOLON, KC_ENTER,
    KC_LSHIFT,                 KC_Z,               KC_X,     KC_C,    KC_V,    KC_B,                                                                          KC_N,             KC_M,    KC_COMMA, KC_DOT, KC_SLASH,  RSFT_T(KC_MINUS),
    KC_GRAVE,                  MO(LAYER_FUNCTION), KC_LCTRL, KC_LALT, KC_LGUI, WEBUSB_PAIR,                                                                   TO(LAYER_GAMING), KC_LEFT, KC_DOWN,  KC_UP,  KC_RIGHT,  KC_QUOTE,

    KC_SPACE, MO(LAYER_SYMBOL), KC_LGUI,                                           KC_RGUI, KC_DELETE, KC_SPACE
  ),
  [LAYER_GAMING] = LAYOUT_moonlander(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
    KC_TAB,         KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
    KC_LCTRL,       KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
    KC_TRANSPARENT, KC_NO,          KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_NO,                                                                                                          TO(LAYER_BASE), KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,

    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
  ),
  [LAYER_SYMBOL] = LAYOUT_moonlander(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
    KC_TRANSPARENT, KC_EXLM,        KC_AT,          KC_HASH,        KC_DLR,         KC_PERC,        KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_CIRC,        KC_AMPR,        KC_ASTR,        KC_LPRN,        KC_RPRN,        KC_BSLASH,
    KC_TRANSPARENT, KC_1,           KC_2,           KC_3,           KC_4,           KC_5,           KC_TRANSPARENT,                                                                 KC_TRANSPARENT, KC_6,           KC_7,           KC_8,           KC_9,           KC_0,           KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,

    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
  ),
  [LAYER_NUMPAD] = LAYOUT_moonlander(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_KP_7,        KC_KP_8,        KC_KP_9,        KC_KP_PLUS,     KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_KP_4,        KC_KP_5,        KC_KP_6,        KC_KP_MINUS,    KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_KP_1,        KC_KP_2,        KC_KP_3,        KC_KP_ASTERISK, KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_KP_EQUAL,    KC_KP_0,        KC_KP_DOT,      KC_KP_SLASH,    KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,

    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
  ),
  [LAYER_FUNCTION] = LAYOUT_moonlander(
    KC_TRANSPARENT, KC_F1,          KC_F2,          KC_F3,          KC_F4,          KC_F5,          KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_F6,          KC_F7,          KC_F8,          KC_F9,          KC_F10,         KC_TRANSPARENT,
    KC_TRANSPARENT, KC_F11,         KC_F12,         KC_F13,         KC_F14,         KC_F15,         KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
    Z_SETOS_WIN,    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                                                 KC_TRANSPARENT, KC_MEDIA_PREV_TRACK,KC_AUDIO_VOL_DOWN,KC_AUDIO_VOL_UP,KC_MEDIA_NEXT_TRACK,KC_TRANSPARENT, KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_AUDIO_MUTE,  KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, Z_SETOS_MAC,    WEBUSB_PAIR,                                                                                                    RESET,          RGB_VAD,        RGB_SPD,        RGB_SPI,        RGB_VAI,        KC_TRANSPARENT,

    KC_TRANSPARENT, RGB_MOD,        RGB_SLD,                        KC_TRANSPARENT, KC_TRANSPARENT, KC_MEDIA_PLAY_PAUSE
  ),
  [LAYER_MOVEMENT] = LAYOUT_moonlander(
    KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,               KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
    KC_TRANSPARENT, KC_NO,          Z_NEXTWORD,     Z_LINEEND,      KC_NO,               KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          Z_NEWLINEINS,   KC_NO,          KC_NO,
    KC_NO,          Z_LINESTART,    KC_NO,          KC_NO,          KC_NO,               Z_DOCSTARTEND,  KC_NO,                                                                          KC_NO,          KC_LEFT,        KC_DOWN,        KC_UP,          KC_RIGHT,       KC_NO,          KC_NO,
    KC_LSHIFT,      KC_NO,          KC_NO,          KC_NO,          TO(LAYER_SELECTION), Z_PREVWORD,                                     KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_RSHIFT,
    KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,               KC_NO,                                                                                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,

    KC_NO,          KC_NO,          KC_NO,                          KC_NO,               KC_NO,          KC_NO
  ),
  [LAYER_SELECTION] = LAYOUT_moonlander(
    TO(LAYER_BASE), KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
    KC_NO,          KC_NO,          Z_NEXTWORD,     Z_LINEEND,      KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          Z_COPY,         KC_NO,          KC_NO,          KC_NO,          Z_PASTE,        KC_NO,
    TO(LAYER_BASE), Z_LINESTART,    KC_NO,          Z_CUT,          KC_NO,          Z_DOCSTARTEND,  KC_NO,                                          KC_NO,          KC_LEFT,        KC_DOWN,        KC_UP,          KC_RIGHT,       KC_NO,          KC_NO,
    KC_LSHIFT,      KC_NO,          Z_CUT,          Z_CHANGE,       KC_NO,          Z_PREVWORD,                                                                     KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_RSHIFT,
    KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,

    KC_NO,          KC_NO,          KC_NO,                          KC_NO,          KC_NO,          KC_NO
  ),
};

extern bool g_suspend_state;
extern rgb_config_t rgb_matrix_config;
enum host_os_types host_os;

void keyboard_post_init_user(void) {
  rgb_matrix_enable();
  host_os = OS_MAC;
  set_tempo(150);
}

const uint8_t PROGMEM ledmap[][DRIVER_LED_TOTAL][3] = {
    [LAYER_GAMING] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {250,159,255}, {0,0,0}, {0,0,0}, {0,0,0}, {250,159,255}, {250,159,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {250,159,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {134,255,213} },

    [LAYER_SYMBOL] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {243,222,234}, {134,255,213}, {0,0,0}, {0,0,0}, {0,0,0}, {243,222,234}, {134,255,213}, {0,0,0}, {0,0,0}, {0,0,0}, {243,222,234}, {134,255,213}, {0,0,0}, {0,0,0}, {0,0,0}, {243,222,234}, {134,255,213}, {0,0,0}, {0,0,0}, {0,0,0}, {243,222,234}, {134,255,213}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {85,203,158}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {243,222,234}, {134,255,213}, {0,0,0}, {0,0,0}, {0,0,0}, {243,222,234}, {134,255,213}, {0,0,0}, {0,0,0}, {0,0,0}, {243,222,234}, {134,255,213}, {0,0,0}, {0,0,0}, {0,0,0}, {243,222,234}, {134,255,213}, {0,0,0}, {0,0,0}, {0,0,0}, {243,222,234}, {134,255,213}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} },

    [LAYER_NUMPAD] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {85,203,158}, {85,203,158}, {85,203,158}, {85,203,158}, {0,0,0}, {134,255,213}, {134,255,213}, {134,255,213}, {243,222,234}, {0,0,0}, {134,255,213}, {134,255,213}, {134,255,213}, {134,255,213}, {0,0,0}, {134,255,213}, {134,255,213}, {134,255,213}, {243,222,234}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} },

    [LAYER_FUNCTION] = { {0,0,0}, {0,0,0}, {10,225,255}, {0,0,0}, {0,0,0}, {85,203,158}, {85,203,158}, {0,0,0}, {0,0,0}, {0,0,0}, {85,203,158}, {85,203,158}, {0,0,0}, {0,0,0}, {0,0,0}, {85,203,158}, {85,203,158}, {0,0,0}, {0,0,0}, {0,0,0}, {85,203,158}, {85,203,158}, {0,0,0}, {0,0,0}, {10,225,255}, {85,203,158}, {85,203,158}, {10,225,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {32,176,255}, {32,176,255}, {32,176,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {85,203,158}, {0,0,0}, {0,0,0}, {0,0,0}, {32,176,255}, {85,203,158}, {0,0,0}, {134,255,213}, {0,0,0}, {32,176,255}, {85,203,158}, {0,0,0}, {134,255,213}, {0,0,0}, {32,176,255}, {85,203,158}, {0,0,0}, {134,255,213}, {134,255,213}, {32,176,255}, {85,203,158}, {0,0,0}, {134,255,213}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {134,255,213}, {0,0,0}, {0,0,0}, {10,225,255} },

    [LAYER_MOVEMENT] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {85,203,158}, {0,0,0}, {0,0,0}, {0,0,0}, {85,203,158}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {85,203,158}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {243,222,234}, {0,0,0}, {0,0,0}, {0,0,0}, {85,203,158}, {85,203,158}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {134,255,213}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {134,255,213}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {134,255,213}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {134,255,213}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} },

    [LAYER_SELECTION] = { {10,225,255}, {0,0,0}, {10,225,255}, {169,120,255}, {0,0,0}, {0,0,0}, {0,0,0}, {85,203,158}, {0,0,0}, {0,0,0}, {0,0,0}, {85,203,158}, {0,0,0}, {243,222,234}, {0,0,0}, {0,0,0}, {85,203,158}, {243,222,234}, {243,222,234}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {85,203,158}, {85,203,158}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {169,120,255}, {0,0,0}, {0,0,0}, {243,222,234}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {134,255,213}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {134,255,213}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {134,255,213}, {0,0,0}, {0,0,0}, {0,0,0}, {243,222,234}, {134,255,213}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} },
};

void set_layer_color(int layer) {
  for (int i = 0; i < DRIVER_LED_TOTAL; i++) {
    HSV hsv = {
      .h = pgm_read_byte(&ledmap[layer][i][0]),
      .s = pgm_read_byte(&ledmap[layer][i][1]),
      .v = pgm_read_byte(&ledmap[layer][i][2]),
    };
    if (!hsv.h && !hsv.s && !hsv.v && !IS_LAYER_ON(LAYER_GAMING)) {
        rgb_matrix_set_color( i, 0, 0, 0 );
    } else {
        RGB rgb = hsv_to_rgb( hsv );
        float f = (float)rgb_matrix_config.hsv.v / UINT8_MAX;
        rgb_matrix_set_color( i, f * rgb.r, f * rgb.g, f * rgb.b );
    }
  }
}

void rgb_matrix_indicators_user(void) {
  if (g_suspend_state || keyboard_config.disable_layer_led) { return; }
  switch (biton32(layer_state)) {
    case 1:
      set_layer_color(1);
      break;
    case 2:
      set_layer_color(2);
      break;
    case 3:
      set_layer_color(3);
      break;
    case 4:
      set_layer_color(4);
      break;
    case 5:
      set_layer_color(5);
      break;
    case 6:
      set_layer_color(6);
      break;
   default:
    if (rgb_matrix_get_flags() == LED_FLAG_NONE)
      rgb_matrix_set_color_all(0, 0, 0);
    break;
  }
}

bool process_movement_key(uint16_t keycode);
bool process_selection_key(uint16_t keycode);

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case RGB_SLD:
      if (record->event.pressed) {
        rgblight_mode(1);
      }
      return false;
    case Z_SETOS_MAC:
      if (record->event.pressed) {
          host_os = OS_MAC;
          PLAY_SONG(song_mac_mode);
      }
      return false;
    case Z_SETOS_WIN:
      if (record->event.pressed) {
          host_os = OS_WIN;
          PLAY_SONG(song_win_mode);
      }
      return false;
    case TO(LAYER_GAMING):
      if (record->event.pressed) {
          PLAY_SONG(song_gaming_mode);
      }
      return true;
    case TO(LAYER_BASE):
      if (record->event.pressed && IS_LAYER_ON(LAYER_GAMING)) {
          PLAY_SONG(song_regular_mode);
      }
      return true;
  }
  if (IS_LAYER_ON(LAYER_MOVEMENT) && record->event.pressed) {
    return process_movement_key(keycode);
  } else if (IS_LAYER_ON(LAYER_SELECTION) && record->event.pressed) {
    return process_selection_key(keycode);
  }
  return true;
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
      case Z_NEXTWORD:
        SEND_STRING(SS_LALT(SS_TAP(X_RIGHT)));
        return false;
      case Z_PREVWORD:
        SEND_STRING(SS_LALT(SS_TAP(X_LEFT)));
        return false;
      case Z_LINESTART:
        SEND_STRING(SS_LCTRL("a"));
        return false;
      case Z_LINEEND:
        SEND_STRING(SS_LCTRL("e"));
        return false;
      case Z_DOCSTARTEND:
        if (SHIFTED) {
          // Shifted; go to end of doc
          UNSHIFT(SEND_STRING(SS_LCMD(SS_TAP(X_DOWN))));
        } else {
          // Go to start of doc
          SEND_STRING(SS_LCMD(SS_TAP(X_UP)));
        }
        return false;
      case Z_NEWLINEINS:
        if (SHIFTED) {
          // Shifted; insert on previous line
          UNSHIFT(SEND_STRING(SS_LCTRL("a") SS_TAP(X_ENTER) SS_TAP(X_UP)));
        } else {
          // Insert on next line
          SEND_STRING(SS_LCTRL("e") SS_TAP(X_ENTER));
        }
        return false;
      case Z_PASTE:
        SEND_STRING(SS_LCMD("v"));
        return false;
    }
  } else if (host_os == OS_WIN) {
    switch (keycode) {
      case Z_NEXTWORD:
        SEND_STRING(SS_LCTRL(SS_TAP(X_RIGHT)));
        return false;
      case Z_PREVWORD:
        SEND_STRING(SS_LCTRL(SS_TAP(X_LEFT)));
        return false;
      case Z_LINESTART:
        SEND_STRING(SS_TAP(X_HOME));
        return false;
      case Z_LINEEND:
        SEND_STRING(SS_TAP(X_END));
        return false;
      case Z_DOCSTARTEND:
        if (SHIFTED) {
          // Shifted; go to end of doc
          UNSHIFT(SEND_STRING(SS_LCTRL(SS_TAP(X_END))));
        } else {
          // Go to start of doc
          SEND_STRING(SS_LCTRL(SS_TAP(X_HOME)));
        }
        return false;
      case Z_NEWLINEINS:
        if (SHIFTED) {
          // Shifted; insert on previous line
          UNSHIFT(SEND_STRING(SS_TAP(X_HOME) SS_TAP(X_ENTER) SS_TAP(X_UP)));
        } else {
          // Insert on next line
          SEND_STRING(SS_TAP(X_END) SS_TAP(X_ENTER));
        }
        return false;
      case Z_PASTE:
        SEND_STRING(SS_LCTRL("v"));
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
      case Z_NEXTWORD:
        SEND_STRING(SS_LSFT(SS_LALT(SS_TAP(X_RIGHT))));
        return false;
      case Z_PREVWORD:
        SEND_STRING(SS_LSFT(SS_LALT(SS_TAP(X_LEFT))));
        return false;
      case Z_LINESTART:
        SEND_STRING(SS_LSFT(SS_LCTRL("a")));
        return false;
      case Z_LINEEND:
        SEND_STRING(SS_LSFT(SS_LCTRL("e")));
        return false;
      case Z_DOCSTARTEND:
        if (SHIFTED) {
          // Shifted; go to end of doc
          UNSHIFT(SEND_STRING(SS_LSFT(SS_LCMD(SS_TAP(X_DOWN)))));
        } else {
          // Go to start of doc
          SEND_STRING(SS_LSFT(SS_LCMD(SS_TAP(X_UP))));
        }
        return false;
      case Z_CHANGE:
        SEND_STRING(SS_TAP(X_BSPACE));
        layer_move(LAYER_BASE);
        return false;
      case Z_COPY:
        SEND_STRING(SS_LCMD("c") SS_TAP(X_LEFT));
        layer_move(LAYER_BASE);
        return false;
      case Z_CUT:
        SEND_STRING(SS_LCMD("x"));
        layer_move(LAYER_BASE);
        return false;
      case Z_PASTE:
        SEND_STRING(SS_LCMD("v"));
        layer_move(LAYER_BASE);
        return false;
    }
  } else if (host_os == OS_WIN) {
    switch (keycode) {
      case Z_NEXTWORD:
        SEND_STRING(SS_LSFT(SS_LCTRL(SS_TAP(X_RIGHT))));
        return false;
      case Z_PREVWORD:
        SEND_STRING(SS_LSFT(SS_LCTRL(SS_TAP(X_LEFT))));
        return false;
      case Z_LINESTART:
        SEND_STRING(SS_LSFT(SS_TAP(X_HOME)));
        return false;
      case Z_LINEEND:
        SEND_STRING(SS_LSFT(SS_TAP(X_END)));
        return false;
      case Z_DOCSTARTEND:
        if (SHIFTED) {
          // Shifted; go to end of doc
          UNSHIFT(SEND_STRING(SS_LSFT(SS_LCTRL(SS_TAP(X_END)))));
        } else {
          // Go to start of doc
          SEND_STRING(SS_LSFT(SS_LCTRL(SS_TAP(X_HOME))));
        }
        return false;
      case Z_CHANGE:
        SEND_STRING(SS_TAP(X_BSPACE));
        layer_move(LAYER_BASE);
        return false;
      case Z_COPY:
        SEND_STRING(SS_LCTRL("c") SS_TAP(X_LEFT));
        layer_move(LAYER_BASE);
        return false;
      case Z_CUT:
        SEND_STRING(SS_LCTRL("x"));
        layer_move(LAYER_BASE);
        return false;
      case Z_PASTE:
        SEND_STRING(SS_LCTRL("v"));
        layer_move(LAYER_BASE);
        return false;
    }
  }
  return true;
}
