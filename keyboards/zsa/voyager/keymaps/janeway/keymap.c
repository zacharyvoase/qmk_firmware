/* Copyright 2024 ZSA Technology Labs, Inc <@zsa>
 * Copyright 2026 Zachary Voase <zack@meat.io>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

#include QMK_KEYBOARD_H
#include "version.h"
#include "defaults.h"

enum layers {
    BASE = 0,
    SYM,    // Symbols (parens, brackets, braces, angle brackets)
    META,   // Board meta — RGB controls, nav anchors, lock, debug toggle, EE_CLR
    NAV,    // Arrows + word/paragraph nav; gateway into SEL
    SEL,    // Selection-mode arrows + clipboard
    FN,     // F-keys + media row
};

enum custom_keycodes {
    RGB_SLD = ZSA_SAFE_RANGE,
    HSV_0_255_255,
    HSV_74_255_255,
    HSV_169_255_255,
    // Z_* range — custom action keycodes. Range bracketed by Z_START/Z_END
    // sentinels so rgb_matrix_indicators_advanced_user can paint them red
    // on the appropriate layers.
    Z_START,
    // Vim-style g / G — top / bottom of document.
    Z_VIMG,
    // Vim-style visual-mode actions on SEL.
    Z_CHANGE,   // C: delete the selection (backspace)
    Z_CUT,      // X: Cmd+X
    Z_COPY,     // Y: Cmd+C   (Vim yank mnemonic)
    Z_PASTE,    // P: Cmd+V  (also on NAV+P)
    // SYM-layer curly quotes:
    //   O: left single ' (shift → left double ")
    //   P: right single ' (shift → right double ")
    Z_OQUOTE,
    Z_PQUOTE,
    Z_END,
};

// Helpers for inspecting and temporarily clearing the shift modifier.
// Lifted from zoolander/shiftstate.h.
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

enum tap_dance_codes {
    TD_DEBUG_TOG = 0,  // META layer: double-tap toggles debug_enable
    TD_EECLR,      // META layer: double-tap clears EEPROM
    TD_DASH,       // SYM layer: 1=en-dash, 2=em-dash
    TD_DOT,        // SYM layer: 1=., 2=.., 3=ellipsis
};

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

// Mac smart-text macros (Option-based).
#define MAC_EN_DASH    SS_LALT("-")
#define MAC_EM_DASH    SS_LALT(SS_LSFT("-"))
#define MAC_ELLIPSIS   SS_LALT(";")
#define MAC_LSQUO      SS_LALT("]")               // ' (left single)
#define MAC_RSQUO      SS_LALT(SS_LSFT("]"))      // ' (right single)
#define MAC_LDQUO      SS_LALT("[")               // " (left double)
#define MAC_RDQUO      SS_LALT(SS_LSFT("["))      // " (right double)

static void td_dash_finished(tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) SEND_STRING(MAC_EN_DASH);
    else                   SEND_STRING(MAC_EM_DASH);
}

static void td_dot_finished(tap_dance_state_t *state, void *user_data) {
    switch (state->count) {
        case 1:  SEND_STRING("."); break;
        case 2:  SEND_STRING(".."); break;
        default: SEND_STRING(MAC_ELLIPSIS); break;
    }
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_DEBUG_TOG] = ACTION_TAP_DANCE_FN(td_debug_tog_finished),
    [TD_EECLR]     = ACTION_TAP_DANCE_FN(td_eeclr_finished),
    [TD_DASH]      = ACTION_TAP_DANCE_FN(td_dash_finished),
    [TD_DOT]       = ACTION_TAP_DANCE_FN(td_dot_finished),
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [BASE] = LAYOUT_voyager(
    KC_ESCAPE,      KC_1,           KC_2,           KC_3,           KC_4,           KC_5,                                           KC_6,           KC_7,           KC_8,           KC_9,           KC_0,           KC_EQUAL,
    LT(NAV, KC_TAB),KC_Q,           KC_W,           KC_E,           KC_R,           KC_T,                                           KC_Y,           KC_U,           KC_I,           KC_O,           KC_P,           KC_BSPC,
    MT(MOD_LCTL, KC_ESCAPE),KC_A,   KC_S,           KC_D,           LT(FN, KC_F),   KC_G,                                           KC_H,           KC_J,           KC_K,           KC_L,           KC_SCLN,        MT(MOD_LCTL, KC_ENTER),
    KC_LSFT,        MT(MOD_LALT, KC_Z),KC_X,        KC_C,           KC_V,           KC_B,                                           KC_N,           KC_M,           KC_COMMA,       KC_DOT,         MT(MOD_RALT, KC_SLASH),MT(MOD_RSFT, KC_MINUS),
                                                    MT(MOD_LGUI, KC_SPACE),OSL(SYM),                                LT(META, KC_BSPC),MT(MOD_RGUI, KC_SPACE)
  ),
  [SYM] = LAYOUT_voyager(
    KC_GRAVE,       KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_LCBR,                                        KC_RCBR,        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_LBRC,                                        KC_RBRC,        KC_TRANSPARENT, KC_TRANSPARENT, Z_OQUOTE,       Z_PQUOTE,       KC_BSLS,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_LPRN,                                        KC_RPRN,        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_QUOTE,       KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, S(KC_COMMA),                                    S(KC_DOT),      KC_TRANSPARENT, KC_TRANSPARENT, TD(TD_DOT),     KC_TRANSPARENT, TD(TD_DASH),
                                                    KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT
  ),
  [META] = LAYOUT_voyager(
    KC_NO,            KC_NO,            KC_NO,         KC_NO,           RGB_VAD,         RGB_VAI,                                       KC_NO,         KC_NO,         KC_NO,         KC_NO,         KC_NO,         KC_NO,
    KC_NO,            LCTL(LCMD(KC_Q)), RGB_HUD,       RGB_HUI,         RGB_SAD,         RGB_SAI,                                       KC_NO,         KC_NO,         KC_NO,         KC_NO,         KC_NO,         KC_NO,
    TD(TD_DEBUG_TOG), KC_NO,            RGB_SPD,       RGB_SPI,         RGB_RMOD,        RGB_MOD,                                       KC_HOME,       KC_PAGE_DOWN,  KC_PAGE_UP,    KC_END,        KC_NO,         KC_NO,
    KC_NO,            TD(TD_EECLR),     RGB_TOG,       HSV_0_255_255,   HSV_74_255_255,  HSV_169_255_255,                               KC_NO,         KC_NO,         KC_NO,         KC_NO,         KC_NO,         KC_NO,
                                                      LCTL(KC_UP),     KC_NO,                                       KC_NO,         KC_NO
  ),
  [NAV] = LAYOUT_voyager(
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, LALT(KC_RIGHT), LGUI(KC_RIGHT), KC_TRANSPARENT, LGUI(KC_LBRC),                                  RGUI(KC_RBRC),  KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, Z_PASTE,        KC_TRANSPARENT,
    KC_TRANSPARENT, LGUI(KC_LEFT),  KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, Z_VIMG,                                         KC_LEFT,        KC_DOWN,        KC_UP,          KC_RIGHT,       KC_TRANSPARENT, KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, TO(SEL),        LALT(KC_LEFT),                                  KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
                                                    KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT
  ),
  [SEL] = LAYOUT_voyager(
    TO(BASE),       KC_NO,          KC_NO,                KC_NO,                KC_NO,          KC_NO,                                       KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
    KC_NO,          KC_NO,          LALT(LSFT(KC_RIGHT)), LSFT(LGUI(KC_RIGHT)), KC_NO,          KC_NO,                                       Z_COPY,         KC_NO,          KC_NO,          KC_NO,          Z_PASTE,        KC_NO,
    TO(BASE),       LSFT(LGUI(KC_LEFT)),KC_NO,            KC_NO,                KC_NO,          Z_VIMG,                                      LSFT(KC_LEFT),  LSFT(KC_DOWN),  LSFT(KC_UP),    LSFT(KC_RIGHT), KC_NO,          KC_NO,
    KC_LSFT,        KC_NO,          Z_CUT,                Z_CHANGE,             KC_NO,          LALT(LSFT(KC_LEFT)),                         KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_RSFT,
                                                          KC_NO,                KC_NO,                                       KC_NO,          KC_NO
  ),
  [FN] = LAYOUT_voyager(
    QK_BOOT,        KC_F1,          KC_F2,          KC_F3,          KC_F4,          KC_F5,                                          KC_F6,          KC_F7,          KC_F8,          KC_F9,          KC_F10,         KC_F11,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_MEDIA_PREV_TRACK,KC_AUDIO_VOL_DOWN,KC_AUDIO_VOL_UP,KC_MEDIA_NEXT_TRACK,KC_TRANSPARENT, KC_TRANSPARENT,
    KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_AUDIO_MUTE,  KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
                                                    KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_MEDIA_PLAY_PAUSE
  ),
};

// clang-format on

// Enable the debug console at startup so action_tapping's tap-hold decision
// dprintf calls show up in `qmk console`. Toggle off when not actively
// tuning — CONSOLE_ENABLE adds ~2KB to the binary.
void keyboard_post_init_user(void) {
    debug_enable = true;
}

// Debug-gated logger. The META layer's debug tap-dance flips `debug_enable`
// to silence/restore output. Used by log_event below and anywhere else we
// want to trace event flow without paying the uprintf cost when debug is off.
#define LOG(...) do { if (debug_enable) uprintf(__VA_ARGS__); } while (0)

static void log_event(uint16_t keycode, keyrecord_t *record) {
    LOG("t=%lu %s kc=%04X\n",
        (unsigned long)record->event.time,
        record->event.pressed ? "DN" : "UP",
        keycode);
}

// Pinky mods snap-fire on other-key press (HOLD_ON_OTHER_KEY_PRESS) — they
// need fast modifier feel for Ctrl+Tab / Ctrl+letter / Shift+letter and aren't
// at risk of misfiring during typing rolls (you don't roll INTO a pinky-mod
// position mid-word). Everything else — letter mods, layer-taps, thumbs —
// returns false here and uses PERMISSIVE_HOLD instead (below). That way
// typing rolls like "mo" or "ve" don't misfire as Cmd-chords: the mod-tap
// letter is released before the next letter, so it settles as TAP.
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MT(MOD_LCTL, KC_ESCAPE):
        case MT(MOD_LCTL, KC_ENTER):
        case MT(MOD_RSFT, KC_MINUS):
        // Thumb Cmd sources: snap-fire so Cmd+letter doesn't depend on
        // release order. FLOW_TAP_TERM protects against typing rolls.
        case MT(MOD_LGUI, KC_SPACE):
        case MT(MOD_RGUI, KC_SPACE):
            return true;
    }
    return false;
}

// Inverse of the above: pinkies are HOLD_ON_OTHER_KEY_PRESS, everything else
// is PERMISSIVE_HOLD. Mod-tap settles as hold only if the other key was
// pressed AND released before the mod-tap was released. Intentional chord
// (hold V, tap T, release T, release V) → Cmd+T. Typing roll (M down, O
// down, M up before O up) → "mo".
bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MT(MOD_LCTL, KC_ESCAPE):
        case MT(MOD_LCTL, KC_ENTER):
        case MT(MOD_RSFT, KC_MINUS):
        // Thumb Cmd: snap-fire above; explicitly NOT permissive.
        case MT(MOD_LGUI, KC_SPACE):
        case MT(MOD_RGUI, KC_SPACE):
            return false;
    }
    return true;
}

// Pinky mods are physically isolated and reliable holds — exempt them from
// flow-tap suppression so Ctrl+, / Ctrl+Enter / Shift+- work even mid-typing.
// Letter mods (V/F/Z/M) keep the default flow-tap protection.
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

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    log_event(keycode, record);

    switch (keycode) {
        case QK_MODS ... QK_MODS_MAX:
            // Mouse and consumer keys (volume, media) with modifiers work
            // inconsistently across operating systems; this makes sure
            // modifiers are always applied to the key that was pressed.
            if (IS_CONSUMER_KEYCODE(QK_MODS_GET_BASIC_KEYCODE(keycode))) {
                if (record->event.pressed) {
                    add_mods(QK_MODS_GET_MODS(keycode));
                    send_keyboard_report();
                    wait_ms(2);
                    register_code(QK_MODS_GET_BASIC_KEYCODE(keycode));
                    return false;
                } else {
                    wait_ms(2);
                    del_mods(QK_MODS_GET_MODS(keycode));
                }
            }
            break;

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
        case RGB_SLD:
            if (record->event.pressed) rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
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
        case Z_VIMG:
            // Vim-style: g → top of doc, G (shift+g) → bottom. On NAV the
            // shift gets consumed so the OS sees just Cmd+Up/Down; on SEL
            // the shift sticks around so we send Shift+Cmd+Up/Down to
            // extend the selection.
            if (record->event.pressed) {
                if (IS_LAYER_ON(SEL)) {
                    if (SHIFTED) {
                        // User's shift is already in the report; just add Cmd+Down.
                        SEND_STRING(SS_LCMD(SS_TAP(X_DOWN)));
                    } else {
                        SEND_STRING(SS_LSFT(SS_LCMD(SS_TAP(X_UP))));
                    }
                } else {
                    if (SHIFTED) {
                        UNSHIFT(SEND_STRING(SS_LCMD(SS_TAP(X_DOWN))));
                    } else {
                        SEND_STRING(SS_LCMD(SS_TAP(X_UP)));
                    }
                }
            }
            return false;
        case Z_CHANGE:
            if (record->event.pressed) {
                SEND_STRING(SS_TAP(X_BSPC));
                if (IS_LAYER_ON(SEL)) layer_move(BASE);
            }
            return false;
        case Z_CUT:
            if (record->event.pressed) {
                SEND_STRING(SS_LCMD("x"));
                if (IS_LAYER_ON(SEL)) layer_move(BASE);
            }
            return false;
        case Z_COPY:
            if (record->event.pressed) {
                SEND_STRING(SS_LCMD("c"));
                if (IS_LAYER_ON(SEL)) layer_move(BASE);
            }
            return false;
        case Z_PASTE:
            // Used on both NAV (Tab+P) and SEL (V→...→P). On SEL we also
            // pop back to BASE; on NAV we don't — releasing Tab handles that.
            if (record->event.pressed) {
                SEND_STRING(SS_LCMD("v"));
                if (IS_LAYER_ON(SEL)) layer_move(BASE);
            }
            return false;
        case Z_OQUOTE:
            // SYM + O: left single (shift → left double).
            if (record->event.pressed) {
                if (SHIFTED) UNSHIFT(SEND_STRING(MAC_LDQUO));
                else         SEND_STRING(MAC_LSQUO);
            }
            return false;
        case Z_PQUOTE:
            // SYM + P: right single (shift → right double).
            if (record->event.pressed) {
                if (SHIFTED) UNSHIFT(SEND_STRING(MAC_RDQUO));
                else         SEND_STRING(MAC_RSQUO);
            }
            return false;
    }
    return true;
}

// ============================================================================
// META layer illumination
// ============================================================================
// Per-key colors and animations when the META layer is active. Each keycode
// gets its own treatment — RGB knobs animate to hint at their function,
// utilities use solid distinct colors, dead positions (KC_NO) are off.

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
        case LCTL(KC_UP):           // MCTL (Mission Control) — solid purple
            rgb_matrix_set_color(led, 140, 0, 200);
            break;
        case LCTL(LCMD(KC_Q)):      // LOCK — solid amber
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
        case KC_PAGE_DOWN:
        case KC_PAGE_UP:
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
    if (layer == BASE) return false;  // let the default animation run on BASE

    uint32_t t = timer_read32();

    if (layer == META) {
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            for (uint8_t col = 0; col < MATRIX_COLS; col++) {
                uint8_t led = g_led_config.matrix_co[row][col];
                if (led < led_min || led >= led_max || led == NO_LED) continue;
                uint16_t kc = keymap_key_to_keycode(META, (keypos_t){col, row});
                meta_set_led(led, kc, t);
            }
        }
        return false;
    }

    // SYM / NAV / SEL / FN: paint each bound key by its semantic class so
    // you can see at a glance you're not on BASE and roughly what each key
    // is for. Transparent positions are left at the default animation,
    // KC_NO positions go dark.
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint8_t led = g_led_config.matrix_co[row][col];
            if (led < led_min || led >= led_max || led == NO_LED) continue;
            uint16_t kc = keymap_key_to_keycode(layer, (keypos_t){col, row});

            if (kc == KC_TRANSPARENT) continue;          // let default render show through
            if (kc == KC_NO) {
                rgb_matrix_set_color(led, 0, 0, 0);      // dead key — dark
                continue;
            }
            // Two ranges: the HID codes between F12 and F13 are the nav
            // cluster (arrows, Home/End, etc.) — don't paint those green.
            if ((kc >= KC_F1 && kc <= KC_F12) || (kc >= KC_F13 && kc <= KC_F24)) {
                rgb_matrix_set_color(led, 0, 200, 60);    // green — F-keys
            } else if (kc > Z_START && kc < Z_END) {
                rgb_matrix_set_color(led, 220, 0, 0);     // red — custom Z_* actions
            } else if (kc >= QK_TAP_DANCE && kc <= QK_TAP_DANCE_MAX) {
                rgb_matrix_set_color(led, 220, 0, 0);     // red — tap-dances (also custom actions)
            } else if (kc >= KC_AUDIO_MUTE && kc <= KC_MEDIA_EJECT) {
                rgb_matrix_set_color(led, 0, 80, 220);    // blue — media
            } else if (kc >= RGB_TOG && kc <= RGB_MODE_RGBTEST) {
                rgb_matrix_set_color(led, 160, 0, 200);   // purple — RGB controls
            } else {
                rgb_matrix_set_color(led, 150, 150, 150); // white — anything else bound
            }
        }
    }
    return false;
}
