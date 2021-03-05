/*
  Set any config.h overrides for your specific keymap here.
  See config.h options at https://docs.qmk.fm/#/config_options?id=the-configh-file
*/
#define ORYX_CONFIGURATOR
#define FIRMWARE_VERSION u8"600WL/oLByZ"
#define RGB_MATRIX_STARTUP_SPD 60
#define TAPPING_TERM 200

#define USE_OKLAB_CURVE

// Tapping configuration
#define TAPPING_FORCE_HOLD
#undef IGNORE_MOD_TAP_INTERRUPT
