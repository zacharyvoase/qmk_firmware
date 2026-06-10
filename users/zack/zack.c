#include "zack.h"

#include "win_alt_code.h"

host_os_t host_os = OS_MAC;

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
    .history_back   = SS_LCMD("["),
    .history_fwd    = SS_LCMD("]"),
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
    .history_back   = SS_LALT(SS_TAP(X_LEFT)),
    .history_fwd    = SS_LALT(SS_TAP(X_RIGHT)),
    .open_below     = SS_TAP(X_END) SS_TAP(X_ENTER),
    .open_above     = SS_TAP(X_HOME) SS_TAP(X_ENTER) SS_TAP(X_UP),
    .cut            = SS_LCTL("x"),
    .copy           = SS_LCTL("c"),
    .paste          = SS_LCTL("v"),
    .lock            = SS_LGUI("l"),
    .mission_control = SS_LGUI(SS_TAP(X_TAB)), // Task View
};

const os_bindings_t *bindings(void) {
    return (host_os == OS_MAC) ? &mac_bindings : &win_bindings;
}

void send_shifted(const char *seq) {
    register_code(KC_LEFT_SHIFT);
    send_string(seq);
    unregister_code(KC_LEFT_SHIFT);
}

// ===========================================================================
// Typography (Mac smart-text via Option chords, Windows via Alt-codes)
// ===========================================================================

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

// ===========================================================================
// Vim NAV/SEL engine
// ===========================================================================

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
        case Z__HBCK: send_string(b->history_back); return false;
        case Z__HFWD: send_string(b->history_fwd);  return false;
        case Z_PASTE: send_string(b->paste); return false;
    }
    return true;
}

// Shift-wrapped selection movement, shared by the SEL layer and zoolander's
// momentary quick-select layer. Returns false when the key was consumed.
bool zack_process_selection_movement(uint16_t keycode) {
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
    }
    return true;
}

// SEL layer: same semantic actions as NAV, but every cursor move is wrapped
// in shift so it extends the selection. Some keys also return to BASE afterwards.
static bool process_selection_key(uint16_t keycode) {
    if (!zack_process_selection_movement(keycode)) return false;
    const os_bindings_t *b = bindings();
    switch (keycode) {
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

// ===========================================================================
// Shared process_record handling
// ===========================================================================

bool process_record_zack(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
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
