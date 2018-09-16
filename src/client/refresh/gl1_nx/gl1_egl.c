/*
 * Copyright (C) 1997-2001 Id Software, Inc.
 * Copyright (C) 2016 Daniel Gibson
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
 * SDL backend for the GL1 renderer.
 *
 * =======================================================================
 */

#include "header/local.h"
#include <EGL/egl.h>

static void (*UpdateScreenFn)(void);
static qboolean vsyncActive = false;

// ----

/*
 * Swaps the buffers and shows the next frame.
 */
void
RI_EndFrame(void)
{
	glFlush();
	if (UpdateScreenFn) UpdateScreenFn();
}

/*
 * Returns the adress of a GL function
 */
void *
RI_GetProcAddress(const char* proc)
{
	return eglGetProcAddress(proc);
}

/*
 * Returns whether the vsync is enabled.
 */
qboolean RI_IsVSyncActive(void)
{
	return vsyncActive;
}

/*
 * This function returns the flags used at the SDL window
 * creation by GLimp_InitGraphics(). In case of error -1
 * is returned.
 */
int RI_PrepareForWindow(void)
{
	gl_state.stencil = false;

	// Let's see if the driver supports MSAA.
	int msaa_samples = 0;
	ri.Cvar_SetValue ("gl_msaa_samples", 0);

	return 1;
}
 
/*
 * Enables or disabes the vsync.
 */
void RI_SetVsync(void)
{
	// eglSwapInterval(s_display, r_vsync->value ? 1 : 0);
	vsyncActive = r_vsync->value ? 1 : 0;
}

/*
 * Updates the gamma ramp.
 */
void
RI_UpdateGamma(void)
{

}

/*
 * Initializes the OpenGL context. Returns true at
 * success and false at failure.
 */
int RI_InitContext(void* win)
{
	// Coders are stupid.
	if (win == NULL)
	{
		ri.Sys_Error(ERR_FATAL, "R_InitContext() must not be called with NULL argument!");

		return false;
	}

	UpdateScreenFn = win;

	// Check if it's really OpenGL 1.4.
	const char* glver = (char *)glGetString(GL_VERSION);
	sscanf(glver, "%d.%d", &gl_config.major_version, &gl_config.minor_version);

	if (gl_config.major_version < 1 || (gl_config.major_version == 1 && gl_config.minor_version < 4))
	{
		R_Printf(PRINT_ALL, "R_InitContext(): Got an OpenGL version %d.%d context - need (at least) 1.4!\n", gl_config.major_version, gl_config.minor_version);

		return false;
	}

	// Check if we've got the requested MSAA.
	int msaa_samples = 0;
	ri.Cvar_SetValue("gl_msaa_samples", msaa_samples);

	// Enable vsync if requested.
	RI_SetVsync();

	// Check if we've got 8 stencil bits.
	int stencil_bits = 0;
	gl_state.stencil = false;

	return true;
}

/*
 * Shuts the GL context down.
 */
void
RI_ShutdownContext(void)
{
	UpdateScreenFn = NULL;
}
