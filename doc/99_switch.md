# Switch version

## Building

You will need:
* latest versions of devkitA64 and libnx;
* switch-sdl2, switch-zlib, switch-mesa, switch-libdri_nouveau, [libopenal-soft](https://github.com/fgsfdsfgs/openal-soft).

You can install the libraries (except libopenal-soft) with `(dkp-)pacman`.

Run `make -f Makefile.nx` in this directory to build `nxquake2.nro`.

## Installation and running

Copy the `baseq2` directory from your Quake 2 installation to `/switch/nxquake2/`. Copy the NRO to the same directory. You can now run the game using Homebrew Launcher.

OGG music is supported and should be placed into `/switch/nxquake2/baseq2/music/`. See below for details.

Mission Packs and mods are not supported yet.

If the game crashes, check `/switch/nxquake2/crash.log` for details.
