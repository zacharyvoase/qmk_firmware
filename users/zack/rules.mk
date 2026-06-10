# Shared feature set for all of Zack's keymaps.
TAP_DANCE_ENABLE = yes
CAPS_WORD_ENABLE = yes
# Feeds `qmk console` for tap-hold tuning (~2KB).
CONSOLE_ENABLE = yes

SRC += zack.c zack_lights.c win_alt_code.c
