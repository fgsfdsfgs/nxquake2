/*
 * Copyright (C) 2021 fgsfds
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
 * Various app service routines needed by the Switch.
 *
 * =======================================================================
 */

#include "../../common/header/common.h"

#include <switch.h>
#include <solder.h>

#ifdef DEBUG
static int nxlink_sock = -1;
#endif

static AppletHookCookie applet_cookie;
static qboolean applet_focus = true;

static void
Applet_Callback(AppletHookType hook, void *param)
{
	extern cvar_t *cl_paused;

	if (hook != AppletHookType_OnFocusState || !cl_paused)
		return;

	qboolean new_focus = (appletGetFocusState() != AppletFocusState_Background);
	qboolean can_pause = Cvar_VariableValue("maxclients") == 1 && Com_ServerState();

	/* automatically pause if applet loses focus to avoid runaway timer */

	if (applet_focus && !new_focus)
	{
		if (!cl_paused->value && can_pause)
			Cvar_SetValue("paused", 1);
		applet_focus = false;
	}
	else if (!applet_focus && new_focus)
	{
		if (cl_paused->value && can_pause)
			Cvar_SetValue("paused", 0);
		applet_focus = true;
	}
}

/* switch-related initialization */
qboolean
Sys_NX_Init(void)
{
	/* initialize sockets as early as possible to get the debug output */
	socketInitializeDefault();

	#ifdef DEBUG_NXLINK
	nxlinkStdio();
	#endif

	/* init dynamic loader immediately afterwards */
	if (solder_init(0) < 0)
	{
		fprintf(stderr, "solder init failed: %s\n", solder_dlerror());
		return false;
	}

	/* set applet callback */
	appletSetFocusHandlingMode(AppletFocusHandlingMode_NoSuspend);
	appletHook(&applet_cookie, Applet_Callback, NULL);
	appletLockExit();

	return true;
}

/* switch-related deinitialization */
void
Sys_NX_Shutdown(void)
{
	appletUnhook(&applet_cookie);
	appletUnlockExit();
	solder_quit();
#ifdef DEBUG
	if (nxlink_sock >= 0)
		close(nxlink_sock);
#endif
	socketExit();
}

/* show swkbd and dump user input into `out` */
void
Sys_NX_OnScreenKeyboard(char *out, const int out_len)
{
	SwkbdConfig kbd;
	char tmp_out[out_len + 1];
	Result rc;
	tmp_out[0] = 0;
	rc = swkbdCreate(&kbd, 0);
	if (R_SUCCEEDED(rc))
	{
		swkbdConfigMakePresetDefault(&kbd);
		swkbdConfigSetInitialText(&kbd, out);
		rc = swkbdShow(&kbd, tmp_out, out_len);
		if (R_SUCCEEDED(rc))
			strncpy(out, tmp_out, out_len); 
		swkbdClose(&kbd);
	}
}
