# Switch version

## Building

You will need:
* latest versions of devkitA64 and libnx;
* switch-sdl2, switch-zlib, switch-mesa, switch-libdri_nouveau, switch-openal, switch-libcurl, switch-libogg, switch-libvorbis, switch-cmake;
* [libsolder](https://github.com/fgsfdsfgs/libsolder) (use `make install` to build and install).

You can install the libraries (except libsolder) with `(dkp-)pacman`.

Ensure that devkitA64 is in `PATH` (by e.g. running `source $DEVKITPRO/switchvars.sh` and run following commands in the nxquake2 folder:
```
mkdir build && cd build
aarch64-none-elf-cmake -G"Unix Makefiles" ..
make
```

## Building game libraries

To build compatible game libraries that use CMake (such [as](https://github.com/yquake2/xatrix) [these](https://github.com/yquake2/rogue) [ones](https://github.com/yquake2/zaero)), install libsolder and run these commands from the library's source folder:
```
mkdir build && cd build
aarch64-none-elf-cmake -G"Unix Makefiles" -DCMAKE_PROJECT_INCLUDE="$DEVKITPRO/portlibs/switch/share/SolderShim.cmake" ..
make
```
If the library uses a Makefile, you'll have to modify one of the example library Makefiles from the libsolder repository to use the game library's source files.

## Installation and running

1. Copy the `baseq2` directory from your Quake 2 installation to `/switch/nxquake2/`.
2. Copy `build/release/nxquake2.nro` and `build/release/ref_gl3.so` to the same directory.
3. Copy `build/release/baseq2/game.so` to `/switch/nxquake2/baseq2/`.
4. You can now run the game using Homebrew Launcher.

Ogg/Vorbis music is supported and should be placed into `/switch/nxquake2/baseq2/music/` (or `/switch/nxquake2/<moddir>/music`). See other docs for details.

If the game crashes, check `/switch/nxquake2/crash.log` for details.

## Installing Mission Packs

NXQuake2 releases come with libraries for The Reckoning, Ground Zero and Zaero.
To play those Mission Packs copy the relevant folder (`xatrix`, `rogue` or `zaero`, respectively) from your Mission Pack installation, then copy the corresponding `game.so` file from the latest NXQuake2 release into that folder. When all the files are in place, a `mods` section will appear in the `Game` menu of Q2.

To play any other Mission Packs or mods that have custom game libraries, follow the build instructions above and then copy the mod folder and the `game.so` file that you built into your `nxquake2` folder as described earlier in this section.
