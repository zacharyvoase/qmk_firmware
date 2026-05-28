#include "supershift.h"

#include "action.h"
#include "keycode.h"
#include "modifiers.h"
#include "process_caps_word.h"

typedef enum { SINGLE_TAP, SINGLE_HOLD, DOUBLE_SINGLE_TAP, OTHER } supershift_state_t;

static supershift_state_t supershift_state;

static supershift_state_t cur_supershift(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) {
            return SINGLE_TAP;
        }
        return SINGLE_HOLD;
    } else if (state->count == 2) {
        return DOUBLE_SINGLE_TAP;
    }
    return OTHER;
}

void supershift_finished(tap_dance_state_t *state, void *user_data) {
    supershift_state = cur_supershift(state);
    switch (supershift_state) {
        case SINGLE_TAP:
            register_code16(KC_LEFT_SHIFT);
            break;
        case SINGLE_HOLD:
            register_mods(MOD_BIT(KC_LEFT_SHIFT));
            break;
        case DOUBLE_SINGLE_TAP:
            caps_word_on();
            break;
        case OTHER:
            break;
    }
}

void supershift_reset(tap_dance_state_t *state, void *user_data) {
    switch (supershift_state) {
        case SINGLE_TAP:
            unregister_code16(KC_LEFT_SHIFT);
            break;
        case SINGLE_HOLD:
            unregister_mods(MOD_BIT(KC_LEFT_SHIFT));
            break;
        case DOUBLE_SINGLE_TAP:
        case OTHER:
            break;
    }
}
