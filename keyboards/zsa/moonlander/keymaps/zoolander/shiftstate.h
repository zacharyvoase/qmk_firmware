#pragma once

#include "action_util.h"
#include "keycode.h"

// Helpers for checking shift state and temporarily unshifting
#define SHIFTED (get_mods() & MOD_MASK_SHIFT || get_weak_mods() & MOD_MASK_SHIFT)

#define UNSHIFT(...)                            \
    {                                           \
        uint8_t prevmods     = get_mods();      \
        uint8_t prevweakmods = get_weak_mods(); \
        del_mods(MOD_MASK_SHIFT);               \
        del_weak_mods(MOD_MASK_SHIFT);          \
        __VA_ARGS__;                            \
        set_mods(prevmods);                     \
        set_weak_mods(prevweakmods);            \
    }
