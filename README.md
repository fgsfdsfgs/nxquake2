# Yamagi Quake II

Yamagi Quake II is an enhanced client for id Software's Quake
II with focus on offline and coop gameplay. Both the gameplay and the graphics
are unchanged, but many bugs in the last official release were fixed and some
nice to have features like widescreen support and a modern OpenGL 3.2 renderer
were added. Unlike most other Quake II source ports Yamagi Quake II is fully 64-bit
clean. It works perfectly on modern processors and operating systems. Yamagi
Quake II runs on nearly all common platforms; including FreeBSD, Linux, OpenBSD,
Windows and macOS (experimental).

This code is built upon Icculus Quake II, which itself is based on Quake II
3.21. Yamagi Quake II is released under the terms of the GPL version 2. See the
LICENSE file for further information.

# Switch version

## Building

You will need:
* latest versions of devkitA64 and libnx;
* switch-sdl2, switch-zlib, switch-mesa, switch-libdrm_nouveau;
* [my fork of OpenAL-Soft](https://github.com/fgsfdsfgs/openal-soft).

You can install most of the libraries with `(dkp-)pacman`.
> pacman -S switch-sdl2 switch-zlib switch-mesa switch-libdrm_nouveau

Run `make -f Makefile.nx` in this directory to build `nxquake2.nro`.

## Installation and running

Copy the `baseq2` directory from your Quake 2 installation to `/switch/nxquake2/`. Copy the NRO to the same directory. You can now run the game using Homebrew Launcher.

OGG music is supported and should be placed into `/switch/nxquake2/baseq2/music/`. See below for details.

Mission Packs and mods are not supported yet.

If the game crashes, check `/switch/nxquake2/crash.log` for details.

## Documentation

Before asking any question, read through the documentation! The current
version can be found here: [doc/01_index.md](doc/01_index.md)

## Releases

The official releases (including Windows binaries) can be found at our
homepage: https://www.yamagi.org/quake2  
**Unsupported** preview builds for Windows can be found at
https://deponie.yamagi.org/quake2/misc/
