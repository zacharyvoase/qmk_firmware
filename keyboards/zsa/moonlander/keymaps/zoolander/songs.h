#include "audio.h"

#pragma once

#ifdef AUDIO_ENABLE
float song_mac_mode[][2] = SONG(Q__NOTE(_A4), Q__NOTE(_AS4), Q__NOTE(_B4), HD_NOTE(_C5), );

float song_win_mode[][2] = SONG(HD_NOTE(_EF6), Q__NOTE(_EF4), HD_NOTE(_BF5), H__NOTE(_AF5), H__NOTE(_EF4), H__NOTE(_EF6), WD_NOTE(_BF5), );

float song_gaming_mode[][2] = SONG(HD_NOTE(_A4), Q__NOTE(_E4), Q__NOTE(_A4), HD_NOTE(_B4), Q__NOTE(_E4), Q__NOTE(_B4), W__NOTE(_CS5), );

float song_regular_mode[][2] = SONG(Q__NOTE(_G5), Q__NOTE(_FS5), Q__NOTE(_DS5), Q__NOTE(_A4), Q__NOTE(_GS4), Q__NOTE(_E5), Q__NOTE(_GS5), HD_NOTE(_C6), );

// Debug console toggle: rising chirp = on, falling chirp = off.
float song_debug_on[][2] = SONG(E__NOTE(_A5), E__NOTE(_CS6), Q__NOTE(_E6), );

float song_debug_off[][2] = SONG(E__NOTE(_E6), E__NOTE(_CS6), Q__NOTE(_A5), );

// Caps word: short up-tick on engage, down-tick on release.
float song_caps_on[][2] = SONG(S__NOTE(_E5), S__NOTE(_B5), );

float song_caps_off[][2] = SONG(S__NOTE(_B5), S__NOTE(_E5), );

// EEPROM clear: deliberate minor-key danger jingle, played (and waited out)
// before the reset fires.
float song_eeprom_clear[][2] = SONG(Q__NOTE(_C5), Q__NOTE(_AF4), Q__NOTE(_C5), HD_NOTE(_AF4), );
#endif