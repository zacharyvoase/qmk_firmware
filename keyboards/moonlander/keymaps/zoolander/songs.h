#include "audio.h"

#pragma once

#ifdef AUDIO_ENABLE
float song_mac_mode[][2] = SONG(Q__NOTE(_A4), Q__NOTE(_AS4), Q__NOTE(_B4), HD_NOTE(_C5), );

float song_win_mode[][2] = SONG(HD_NOTE(_EF6), Q__NOTE(_EF4), HD_NOTE(_BF5), H__NOTE(_AF5), H__NOTE(_EF4), H__NOTE(_EF6), WD_NOTE(_BF5), );

float song_gaming_mode[][2] = SONG(HD_NOTE(_A4), Q__NOTE(_E4), Q__NOTE(_A4), HD_NOTE(_B4), Q__NOTE(_E4), Q__NOTE(_B4), W__NOTE(_CS5), );

float song_regular_mode[][2] = SONG(Q__NOTE(_G5), Q__NOTE(_FS5), Q__NOTE(_DS5), Q__NOTE(_A4), Q__NOTE(_GS4), Q__NOTE(_E5), Q__NOTE(_GS5), HD_NOTE(_C6), );
#endif