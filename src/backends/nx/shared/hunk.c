/*
 * Copyright (C) 1997-2001 Id Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * =======================================================================
 *
 * This file implements the low level part of the Hunk_* memory system
 *
 * =======================================================================
 */

/* For mremap() - must be before sys/mman.h include! */
#if defined(__linux__) && !defined(_GNU_SOURCE)
 #define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>

#include "../../../common/header/common.h"

byte *membase;
int maxhunksize;
int curhunksize;

void *Hunk_Begin(int maxsize)
{
	// reserve a huge chunk of memory, but don't commit any yet
	maxhunksize = maxsize;
	curhunksize = 0;
	membase = malloc(maxhunksize);

	if (membase == NULL)
		Sys_Error("Hunk: unable to allocate %d bytes", maxhunksize);
	else
		memset(membase, 0, maxhunksize);

	return membase;
}

void *Hunk_Alloc(int size)
{
	byte *buf;

	// round to cacheline
	size = (size+31)&~31;

	if (curhunksize + size > maxhunksize)
		Sys_Error("Hunk_Alloc overflow");

	buf = membase + curhunksize;
	curhunksize += size;

	return buf;
}

int Hunk_End(void)
{
	byte *n;

	n = realloc(membase, curhunksize);

	if (n != membase)
		Sys_Error("Hunk_End:  Could not remap virtual block (%d)", errno);

	return curhunksize;
}

void Hunk_Free(void *base)
{
	if (base)
		free(base);
}
