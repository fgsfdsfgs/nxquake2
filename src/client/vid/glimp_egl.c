/*
 * Copyright (C) 2010 Yamagi Burmeister
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
 * This is the client side of the render backend, implemented trough SDL.
 * The SDL window and related functrion (mouse grap, fullscreen switch)
 * are implemented here, everything else is in the renderers.
 *
 * =======================================================================
 */

#include "../../common/header/common.h"
#include "../refresh/gl3_nx/glad/glad.h"
#include "header/ref.h"
#include <EGL/egl.h>
#include <SDL2/SDL.h>

static SDL_Window *sdl_window;
static SDL_Renderer *sdl_renderer;

cvar_t *vid_displayrefreshrate;
int glimp_refreshRate = -1;

static int last_flags = 0;
static qboolean initSuccessful = false;

static EGLDisplay s_display;
static EGLSurface s_surface;
static EGLContext s_context;

// --------

static void
setMesaConfig()
{
    // Uncomment below to disable error checking and save CPU time (useful for production):
    // setenv("MESA_NO_ERROR", "1", 1);

    // Uncomment below to enable Mesa logging:
    // setenv("EGL_LOG_LEVEL", "debug", 1);
    // setenv("MESA_VERBOSE", "all", 1);
    // setenv("NOUVEAU_MESA_DEBUG", "1", 1);

    // Uncomment below to enable shader debugging in Nouveau:
    // setenv("NV50_PROG_OPTIMIZE", "0", 1);
    // setenv("NV50_PROG_DEBUG", "1", 1);
    // setenv("NV50_PROG_CHIPSET", "0x120", 1);
    setenv("MESA_GL_VERSION_OVERRIDE", "3.2COMPAT", 1);
}

static qboolean
InitEGL(void)
{
    // Connect to the EGL default display
    s_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (!s_display)
    {
        Com_Error(ERR_FATAL, "Could not connect to display! error: %d", eglGetError());
        goto _fail0;
    }

    // Initialize the EGL display connection
    eglInitialize(s_display, NULL, NULL);

    if (eglBindAPI(EGL_OPENGL_API) == EGL_FALSE)
    {
        Com_Error(ERR_FATAL, "Could not set API! error: %d", eglGetError());
        goto _fail1;
    }

    // Get an appropriate EGL framebuffer configuration
    EGLConfig config;
    EGLint numConfigs;
    static const EGLint attributeList[] =
    {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
    };
    eglChooseConfig(s_display, attributeList, &config, 1, &numConfigs);
    if (numConfigs == 0)
    {
        Com_Error(ERR_FATAL, "No config found! error: %d", eglGetError());
        goto _fail1;
    }

    // Create an EGL window surface
    s_surface = eglCreateWindowSurface(s_display, config, (char*)"", NULL);
    if (!s_surface)
    {
        Com_Error(ERR_FATAL, "Surface creation failed! error: %d", eglGetError());
        goto _fail1;
    }

    static const EGLint ctxAttributeList[] = 
    {
        EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT,
        EGL_NONE
    };

    // Create an EGL rendering context
    s_context = eglCreateContext(s_display, config, EGL_NO_CONTEXT, ctxAttributeList);
    if (!s_context)
    {
        Com_Error(ERR_FATAL, "Context creation failed! error: %d", eglGetError());
        goto _fail2;
    }

    // Connect the context to the surface
    eglMakeCurrent(s_display, s_surface, s_surface, s_context);
    return true;

_fail2:
    eglDestroySurface(s_display, s_surface);
    s_surface = NULL;
_fail1:
    eglTerminate(s_display);
    s_display = NULL;
_fail0:
    return false;
}

static void
DeinitEGL(void)
{
    if (s_display)
    {
        if (s_context)
        {
            eglDestroyContext(s_display, s_context);
            s_context = NULL;
        }
        if (s_surface)
        {
            eglDestroySurface(s_display, s_surface);
            s_surface = NULL;
        }
        eglTerminate(s_display);
        s_display = NULL;
    }
}

static void
UpdateScreen(void)
{
  eglSwapBuffers(s_display, s_surface);
}

// FIXME: We need a header for this.
// Maybe we could put it in vid.h.
void GLimp_GrabInput(qboolean grab);

// --------

/*
 * Initializes the SDL video subsystem. Must
 * be called before anything else.
 */
qboolean
GLimp_Init(void)
{
	vid_displayrefreshrate = Cvar_Get("vid_displayrefreshrate", "-1", CVAR_ARCHIVE);

	// still need an active window for input shit
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
		printf("SDL_VIDEO borked: %s\n", SDL_GetError());
	int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN;
	sdl_window = SDL_CreateWindow("", 0, 0, 1280, 720, flags);
	if (!sdl_window) printf("sdl_window == NULL!\n%s\n", SDL_GetError());
	SDL_Renderer *sdl_renderer = SDL_CreateRenderer(sdl_window, -1, 0);
	if (!sdl_renderer) printf("sdl_renderer == NULL!\n%s\n", SDL_GetError());

	setMesaConfig();
	InitEGL();
	printf("gladLoadGL(): %d\n", gladLoadGL());

	return true;
}

/*
 * Shuts the SDL video subsystem down. Must
 * be called after evrything's finished and
 * clean up.
 */
void
GLimp_Shutdown(void)
{
	DeinitEGL();
	if (sdl_renderer) SDL_DestroyRenderer(sdl_renderer), sdl_renderer = NULL;
	if (sdl_window) SDL_DestroyWindow(sdl_window), sdl_window = NULL;
}

/*
 * (Re)initializes the actual window.
 */
qboolean
GLimp_InitGraphics(int fullscreen, int *pwidth, int *pheight)
{
	// TODO: figure out how to change resolutions and shit
	const int width = 1280;
	const int height = 720;
	*pwidth = width;
	*pheight = height;

	if (initSuccessful) return true;

	/* We need the window size for the menu, the HUD, etc. */
	viddef.width = width;
	viddef.height = height;

	int flags = re.PrepareForWindow();

	if (flags == -1)
	{
		/* It's PrepareForWindow() job to log an error */
		return false;
	}

	cvar_t *gl_msaa_samples = Cvar_Get("gl_msaa_samples", "0", CVAR_ARCHIVE);
	// TODO: also remove this after figuring out resolutions and shit
	Cvar_SetValue("r_mode", 14);
	Cvar_SetValue("vid_fullscreen", 1);

	last_flags = flags;

	if (!re.InitContext(UpdateScreen))
	{
		/* InitContext() should have logged an error. */
		return false;
	}

	initSuccessful = true;
	return initSuccessful;
}

/*
 * Shuts the window down.
 */
void
GLimp_ShutdownGraphics(void)
{
}

/*
 * (Un)grab Input
 */
void
GLimp_GrabInput(qboolean grab)
{
}

/*
 * Returns the current display refresh rate.
 */
int
GLimp_GetRefreshRate(void)
{
	glimp_refreshRate = 60;
	return glimp_refreshRate;
}
