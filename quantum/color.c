/* Copyright 2017 Jason Williams
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

#include "color.h"
#include "led_tables.h"
#include "progmem.h"
#include "lib/lib8tion/lib8tion.h"

/**
 * Convert an OKLab HCL (hue/chroma/luminance) color point to linear sRGB.
 */
RGB oklch_to_rgb(HSV hsv) {
    RGB rgb;
    float a = (float)cos8(hsv.h) * (float)hsv.s / (255 * 255);
    float b = (float)sin8(hsv.h) * (float)hsv.s / (255 * 255);
    float l_ = (hsv.v / 255) + 0.3963377774f * a + 0.2158037573f * b;
    float m_ = (hsv.v / 255) - 0.1055613458f * a - 0.0638541728f * b;
    float s_ = (hsv.v / 255) - 0.0894841775f * a - 1.2914855480f * b;
    float l = l_ * l_ * l_;
    float m = m_ * m_ * m_;
    float s = s_ * s_ * s_;
    rgb.r = float_to_u8(+ 4.0767245293f * l - 3.3072168827f * m + 0.2307590544f * s);
    rgb.g = float_to_u8(- 1.2681437731f * l + 2.6093323231f * m - 0.3411344290f * s);
    rgb.b = float_to_u8(- 0.0041119885f * l - 0.7034763098f * m + 1.7068625689f * s);
    return rgb;
}

uint8_t float_to_u8(float val) {
    float scaled = 255 * val;
    if (scaled < 0) return 0;
    else if (scaled > 255) return 255;
    return (uint8_t)scaled;
}

RGB hsv_to_rgb_impl(HSV hsv, bool use_cie) {
    RGB      rgb;
    uint8_t  region, remainder, p, q, t;
    uint16_t h, s, v;

    if (hsv.s == 0) {
#ifdef USE_CIE1931_CURVE
        if (use_cie) {
            rgb.r = rgb.g = rgb.b = pgm_read_byte(&CIE1931_CURVE[hsv.v]);
        } else {
            rgb.r = hsv.v;
            rgb.g = hsv.v;
            rgb.b = hsv.v;
        }
#else
        rgb.r = hsv.v;
        rgb.g = hsv.v;
        rgb.b = hsv.v;
#endif
        return rgb;
    }

    h = hsv.h;
    s = hsv.s;
#ifdef USE_CIE1931_CURVE
    if (use_cie) {
        v = pgm_read_byte(&CIE1931_CURVE[hsv.v]);
    } else {
        v = hsv.v;
    }
#else
    v = hsv.v;
#endif

    region    = h * 6 / 255;
    remainder = (h * 2 - region * 85) * 3;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
        case 6:
        case 0:
            rgb.r = v;
            rgb.g = t;
            rgb.b = p;
            break;
        case 1:
            rgb.r = q;
            rgb.g = v;
            rgb.b = p;
            break;
        case 2:
            rgb.r = p;
            rgb.g = v;
            rgb.b = t;
            break;
        case 3:
            rgb.r = p;
            rgb.g = q;
            rgb.b = v;
            break;
        case 4:
            rgb.r = t;
            rgb.g = p;
            rgb.b = v;
            break;
        default:
            rgb.r = v;
            rgb.g = p;
            rgb.b = q;
            break;
    }

    return rgb;
}


RGB hsv_to_rgb(HSV hsv) {
#ifdef USE_CIE1931_CURVE
    return hsv_to_rgb_impl(hsv, true);
#elif USE_OKLAB_CURVE
    return oklch_to_rgb(hsv);
#else
    return hsv_to_rgb_impl(hsv, false);
#endif
}

RGB hsv_to_rgb_nocie(HSV hsv) { return hsv_to_rgb_impl(hsv, false); }

#ifdef RGBW
#    ifndef MIN
#        define MIN(a, b) ((a) < (b) ? (a) : (b))
#    endif
void convert_rgb_to_rgbw(LED_TYPE *led) {
    // Determine lowest value in all three colors, put that into
    // the white channel and then shift all colors by that amount
    led->w = MIN(led->r, MIN(led->g, led->b));
    led->r -= led->w;
    led->g -= led->w;
    led->b -= led->w;
}
#endif
