# Shared features, sources, and tap-hold config come from users/zack/.
USER_NAME := zack

COMMAND_ENABLE = no
MOUSEKEY_ENABLE = no
SPACE_CADET_ENABLE = no

# Navigator trackpad is loaded via the zsa/navigator_trackpad community module
# in keymap.json — which pulls in DIGITIZER_ENABLE / I2C_DRIVER_REQUIRED and
# all the SRC files. No rules.mk-level config needed.
