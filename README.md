# SKYDOOM

PS2 DOOM port from original source code release. Tested via OPNPS2LD 0.9.3 on SMB network on SCPH-70001. This is a ps2sdk project.

## Features

- Movement via controller
- ADPCM audio for sound effects 
- TinySoundFont for audio playback with sf2 files
- Cheats
- Wad Selector for original games (put wads in linuxdoom-1.10)
- File cache, better loading speeds
- Music streaming (smaller memory usage, faster initial boot time, but slower transitions between levels)
- Jumping in game
- Saves
- Optimized for fastest rendering and uses GS to blit image to higher resolution. 

## Missing 

- Game fixes and optimiizations in modern ports
- Upscaled software rendering (game's backend using software rendering at 320x200 and it projected at 640 448)
- Perfect 35 frame rate (frame drops due to file loading in some areas)
- Random wad loading
- Demo playbacks and recording
- PAL support (only NTSC, framebuffer size hardcoded)

## Todo list

- Create Vector Unit rendering and use hardware accelerated rendering

## Build

```bash
git clone --recurse-submodules https://github.com/dfletcher4489/SKYDOOM.git
cd SKYDOOM/linuxdoom-1.10/
# Add all WAD files in this directory
make all # create build directories, compile, make iso and copy to destination if on WSL like I am 
```

## Software used
 
- Based on the original id Software Doom source release. Some source code is modified for experiments along with minimum changes required to run on PS2.
- Uses [ps2sdk](https://github.com/ps2dev/ps2sdk). All modified ps2sdk source is separately included in project.
- Uses [TinySoundFont](https://github.com/schellingb/TinySoundFont) for SF2 playback. Both tml.h and tsf.h are heavily modified for game specific usage.
- Uses [PS2 Drawing lib](https://github.com/dfletcher4489/PS2-Development), which is my own ps2 library. Current state is pretty messy, but the commit the submodule is at works.

## Disclaimer

I haven't tested this software with an updated ps2sdk. My only concern would be something wrong in the drawing library. You can send an email to crypt4489@gmail.com if there is an issue.
