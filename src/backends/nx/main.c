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
 * This file is the starting point of the program. It does some platform
 * specific initialization stuff and calls the common initialization code.
 *
 * =======================================================================
 */

#include "../../common/header/common.h"
#include <switch.h>

// size_t __nx_heap_size = 0x20000000;

void registerHandler(void);

static Thread actual_thread;

static void actual_main(void *arg)
{
	static int iargc = 1;
	static const char *iargv[] = { "nxquake2.nro", NULL };
	is_portable = 1;
	Qcommon_Init(iargc, iargv);
}

int
main(int argc, char **argv)
{
/*
  u64 core_mask = 0;
  Result rc = svcGetInfo(&core_mask, 0, CUR_PROCESS_HANDLE, 0);
  if (R_FAILED(rc)) {
    Sys_Error("svcGetInfo() failed: %d\n", rc);
    return -1;
  }

  rc = threadCreate(&actual_thread, actual_main, NULL, 0x200000, 0x3B, -2);
  if (R_FAILED(rc)) {
    Sys_Error("threadCreate() failed: %d\n", rc);
    return -1;
  }

  rc = svcSetThreadCoreMask(actual_thread.handle, -1, core_mask);
  if (R_FAILED(rc)) {
    Sys_Error("svcSetThreadCoreMask() failed: %d\n", rc);
    return -1;
  }

  rc = threadStart(&actual_thread);
  if (R_FAILED(rc)) {
    Sys_Error("threadStart() failed: %d\n", rc);
    return -1;
  }

  threadWaitForExit(&actual_thread);
  threadClose(&actual_thread);
	return 0;
*/
  actual_main(NULL);
  return 0;
}

