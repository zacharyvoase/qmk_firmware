#pragma once

#include <stdbool.h>

#include "send_string.h"

void push_num_lock_state(bool desired_state);
void pop_num_lock_state(void);

#define SEND_WIN_ALT_CODE(sequence)         \
    do {                                    \
        push_num_lock_state(true);          \
        SEND_STRING(SS_LALT(sequence));     \
        pop_num_lock_state();               \
    } while (0)
