/* Copyright 2015 Philip Heron <phil@sanslogic.co.uk>                    */
/*                                                                       */
/* This program is free software: you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation, either version 3 of the License, or     */
/* (at your option) any later version.                                   */
/*                                                                       */
/* This program is distributed in the hope that it will be useful,       */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/* GNU General Public License for more details.                          */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include <ax25beacon.h>

#include <stdio.h>

int ax25_beacon(const void* user_data,
	ax25_audio_callback_t audio_callback,
	const char* src_callsign,
	const char* dst_callsign,
	const char* path1,
	const char* path2,
	double                latitude,
	double                longitude,
	double                altitude_in_m,
	const char* comment,
	char                  sym_table,
	char                  sym_code) {
	if (audio_callback == NULL) return -1;
	if (src_callsign == NULL) return -1;
	if (dst_callsign == NULL) return -1;
	if (path1 == NULL) return -1;
	if (path2 == NULL) return -1;

	ax25_t ax25;

	ax25_init(&ax25, AX25_AFSK1200);
	ax25_set_audio_callback(&ax25, (void*)audio_callback, (void*)user_data);

	/* Warn if the sample rate doesn't divide cleanly into the bit rate */
	if (ax25.samplerate % ax25.bitrate != 0) {
		printf("Warning: The sample rate %d does not divide evently into %d. The bit rate will be %.2f\n",
			ax25.samplerate,
			ax25.bitrate,
			(float)ax25.samplerate / (ax25.samplerate / ax25.bitrate));
	}

	/* Convert the position to the format APRS requires */
	const double latitude_aprs = (90.0 - latitude) * 380926.0;
	const double longitude_aprs = (180.0 + longitude) * 190463.0;

	const double altitude_in_feet = altitude_in_m * 3.2808399;

	const uint8_t STR_LEN = 5;

	char lat_str[STR_LEN];
	char long_str[STR_LEN];

	ax25_base91enc(lat_str, STR_LEN - 1, latitude_aprs);
	ax25_base91enc(long_str, STR_LEN - 1, longitude_aprs);

	/* Generate the audio tones and send to callback */
	int ret_val = ax25_frame(&ax25, src_callsign, dst_callsign,
		path1, path2,
		"!%c%s%s%c   /A=%06.0f%s", sym_table,
		lat_str, long_str,
		sym_code,
		altitude_in_feet,
		(comment ? comment : ""));

	return ret_val;
}

