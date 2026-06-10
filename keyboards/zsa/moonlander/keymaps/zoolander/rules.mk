# Set any rules.mk overrides for your specific keymap here.
# See rules at https://docs.qmk.fm/#/config_options?id=the-rulesmk-file
# CONSOLE_ENABLE adds ~2KB; it feeds `qmk console` for tap-hold tuning.
CONSOLE_ENABLE = yes
COMMAND_ENABLE = no
MOUSEKEY_ENABLE = no
SLEEP_LED_ENABLE = yes
TAP_DANCE_ENABLE = yes
CAPS_WORD_ENABLE = yes

SRC += win_alt_code.c