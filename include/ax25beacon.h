/* Copyright 2020 George Boyer							                             */
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

#include <stdint.h>
#include <stdlib.h> // For size_t

#include <ax25.h>

#ifndef _AX25_BEACON_H
#define _AX25_BEACON_H
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int ax25_beacon(const void*           user_data,
                ax25_audio_callback_t audio_callback,
                const char*           src_callsign,
                const char*           dst_callsign,
                const char*           path1,
                const char*           path2,
                double                latitude,
                double                longitude,
                double                altitude_in_m,
                const char*           comment,
                char                  sym_table,
                char                  sym_code);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _AX25_BEACON_H
