#pragma once

#define PTT_ENABLE                   (false)
#define PTT_GPXX_PIN                 (1)
#define PTT_DELAY_BEFORE_TX_IN_MSEC  (400)
#define PTT_TX_PERIOD_IN_MIN         (10)

extern "C" {
	#include "aprs_pico.h"
}
extern audio_buffer_pool_t* audio_buffer_pool;
void testradio();
void radioinit();
void sendData(const char* data);