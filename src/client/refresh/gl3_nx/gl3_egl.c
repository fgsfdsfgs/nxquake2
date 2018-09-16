/*
 * Copyright (C) 1997-2001 Id Software, Inc.
 * Copyright (C) 2016-2017 Daniel Gibson
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
 * SDL backend for the GL3 renderer. Everything that needs to be on the
 * renderer side of thing. Also all glad (or whatever OpenGL loader I
 * end up using) specific things.
 *
 * =======================================================================
 */

#include "header/local.h"

#include <EGL/egl.h>

static void (*UpdateScreenFn)(void);
static qboolean vsyncActive = false;

// --------

enum {
	// Not all GL.h header know about GL_DEBUG_SEVERITY_NOTIFICATION_*.
	QGL_DEBUG_SEVERITY_NOTIFICATION = 0x826B
};

/*
 * Callback function for debug output.
 */
static void APIENTRY
DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
              const GLchar *message, const void *userParam)
{
	const char* sourceStr = "Source: Unknown";
	const char* typeStr = "Type: Unknown";
	const char* severityStr = "Severity: Unknown";

	switch (severity)
	{
		case QGL_DEBUG_SEVERITY_NOTIFICATION:
			return;

		case GL_DEBUG_SEVERITY_HIGH_ARB:   severityStr = "Severity: High";   break;
		case GL_DEBUG_SEVERITY_MEDIUM_ARB: severityStr = "Severity: Medium"; break;
		case GL_DEBUG_SEVERITY_LOW_ARB:    severityStr = "Severity: Low";    break;
	}

	switch (source)
	{
#define SRCCASE(X)  case GL_DEBUG_SOURCE_ ## X ## _ARB: sourceStr = "Source: " #X; break;
		SRCCASE(API);
		SRCCASE(WINDOW_SYSTEM);
		SRCCASE(SHADER_COMPILER);
		SRCCASE(THIRD_PARTY);
		SRCCASE(APPLICATION);
		SRCCASE(OTHER);
#undef SRCCASE
	}

	switch(type)
	{
#define TYPECASE(X)  case GL_DEBUG_TYPE_ ## X ## _ARB: typeStr = "Type: " #X; break;
		TYPECASE(ERROR);
		TYPECASE(DEPRECATED_BEHAVIOR);
		TYPECASE(UNDEFINED_BEHAVIOR);
		TYPECASE(PORTABILITY);
		TYPECASE(PERFORMANCE);
		TYPECASE(OTHER);
#undef TYPECASE
	}

	// use PRINT_ALL - this is only called with gl3_debugcontext != 0 anyway.
	R_Printf(PRINT_ALL, "GLDBG %s %s %s: %s\n", sourceStr, typeStr, severityStr, message);
}

// ---------

/*
 * Swaps the buffers and shows the next frame.
 */
void GL3_EndFrame(void)
{
	if (UpdateScreenFn) UpdateScreenFn();
}

/*
 * Returns whether the vsync is enabled.
 */
qboolean GL3_IsVsyncActive(void)
{
	return vsyncActive;
}

/*
 * Enables or disabes the vsync.
 */
void GL3_SetVsync(void)
{
	vsyncActive = 1;
}

/*
 * This function returns the flags used at the SDL window
 * creation by GLimp_InitGraphics(). In case of error -1
 * is returned.
 */
int GL3_PrepareForWindow(void)
{
	gl3config.stencil = false;
	// Let's see if the driver supports MSAA.
	ri.Cvar_SetValue("gl_msaa_samples", 0);
	return 1;
}

/*
 * Initializes the OpenGL context. Returns true at
 * success and false at failure.
 */
int GL3_InitContext(void* win)
{
	// Coders are stupid.
	if (win == NULL)
	{
		ri.Sys_Error(ERR_FATAL, "R_InitContext() must not be called with NULL argument!");

		return false;
	}

	UpdateScreenFn = win;

	int msaa_samples = 0;
	ri.Cvar_SetValue("gl_msaa_samples", msaa_samples);
	gl3config.stencil = false;

	// Enable vsync if requested.
	GL3_SetVsync();

	gl3config.debug_output = GLAD_GL_ARB_debug_output != 0;
	gl3config.anisotropic = GLAD_GL_EXT_texture_filter_anisotropic != 0;

	gl3config.major_version = GLVersion.major;
	gl3config.minor_version = GLVersion.minor;

	// Debug context setup.
	if (gl3_debugcontext && gl3_debugcontext->value && gl3config.debug_output)
	{
		glDebugMessageCallbackARB(DebugCallback, NULL);

		// Call GL3_DebugCallback() synchronously, i.e. directly when and
		// where the error happens (so we can get the cause in a backtrace)
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	}

	return true;
}

/*
 * Shuts the GL context down.
 */
void GL3_ShutdownContext()
{

}
