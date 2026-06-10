#include "win_alt_code.h"

#include "host.h"
#include "keycode.h"
#include "send_string.h"

static bool num_lock_was_on = false;

void push_num_lock_state(bool desired_state) {
    num_lock_was_on = host_keyboard_led_state().num_lock;
    if (desired_state != num_lock_was_on) {
        SEND_STRING(SS_TAP(X_NUM_LOCK));
    }
}

void pop_num_lock_state(void) {
    bool curr = host_keyboard_led_state().num_lock;
    if (curr != num_lock_was_on) {
        SEND_STRING(SS_TAP(X_NUM_LOCK));
    }
}
