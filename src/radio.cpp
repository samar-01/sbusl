#include "radio.h"

audio_buffer_pool_t* audio_buffer_pool;

void testradio() {
	aprs_pico_sendAPRS(audio_buffer_pool,
		"KE2FCJ-1",  // Source call sign
		"APRS",   // Destination call sign
		// "WIDE1-1",  // APRS path #1
		"NOGATE",  // APRS path #1
		"NOGATE",  // APRS path #2
		"test", // Text message
		0,   // Latitude  (in deg)
		0,    // Longitude (in deg)
		0,        // Altitude  (in m)
		'/',        // APRS symbol table: Primary
		'>',        // APRS symbol code:  Car
		256u);      // Volume    (0 ... 256)
}

void radioinit() {
	audio_buffer_pool = aprs_pico_init();
}


void sendData(const char* data){
	
	aprs_pico_sendAPRS(audio_buffer_pool,
		"KE2FCJ-1",  // Source call sign
		"APRS",   // Destination call sign
		// "WIDE1-1",  // APRS path #1
		"NOGATE",  // APRS path #1
		"NOGATE",  // APRS path #2
		data, // Text message
		0,   // Latitude  (in deg)
		0,    // Longitude (in deg)
		0,        // Altitude  (in m)
		'/',        // APRS symbol table: Primary
		'>',        // APRS symbol code:  Car
		256u);      // Volume    (0 ... 256)
}