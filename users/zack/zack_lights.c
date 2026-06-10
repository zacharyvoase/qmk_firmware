// META-layer per-key illumination and the class-painting indicator scheme.
// Board indicator callbacks (rgb_matrix_indicators_advanced_user) keep their
// board-specific branches (e.g. zoolander's GAM/WASD highlight) and delegate
// the shared painting to these helpers.
#include "zack.h"

#include "color.h"
#include "rgb_matrix.h"

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

void zack_paint_meta_layer(uint8_t led_min, uint8_t led_max) {
    uint32_t t = timer_read32();
    for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
        for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
            uint8_t index = g_led_config.matrix_co[row][col];
            if (index < led_min || index >= led_max || index == NO_LED) continue;
            meta_set_led(index, keymap_key_to_keycode(META, (keypos_t){col, row}), t);
        }
    }
}

void zack_paint_layer_classes(uint8_t layer, uint8_t led_min, uint8_t led_max) {
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
}
