/*
 *  XMMS Crossfade Plugin
 *  Copyright (C) 2000-2007  Peter Eisenlohr <peter@eisenlohr.org>
 *
 *  based on the original OSS Output Plugin
 *  Copyright (C) 1998-2000  Peter Alm, Mikael Alm, Olle Hallnas, Thomas Nilsson and 4Front Technologies
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

/*
 *  Convert to standard (44100/16bit-le/stereo)
 */

#ifndef __CONVERT_H__
#define __CONVERT_H__

#include "crossfade.h"
#include "format.h"

typedef struct
{
	gpointer data;
	gint size;
}
convert_context_t;

void convert_init(convert_context_t * cc);
gint convert_flow(convert_context_t * cc, gpointer * buffer, gint length, format_t * format);
void convert_free(convert_context_t * cc);

#endif /* _CONVERT_H_ */
