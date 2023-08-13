#pragma once

#include "host.h"
#include "keycode.h"
#include "keycodes.h"
#include "quantum.h"

static bool num_lock_was_on = false;

void push_num_lock_state(bool num_lock_state) {
    num_lock_was_on = host_keyboard_led_state().num_lock;
    if (num_lock_state != num_lock_was_on) {
        SEND_STRING(SS_TAP(X_NUM_LOCK));
    }
}

void pop_num_lock_state(void) {
    bool curr_num_lock_state = host_keyboard_led_state().num_lock;
    if (curr_num_lock_state != num_lock_was_on) {
        SEND_STRING(SS_TAP(X_NUM_LOCK));
    }
}

#define SEND_WIN_ALT_CODE(sequence)     \
    {                                   \
        push_num_lock_state(true);      \
        SEND_STRING(SS_LALT(sequence)); \
        pop_num_lock_state();           \
    }
