
# introspection

a RPG made without a game engine.


## Demo
https://github.com/NTDuck/8964/assets/105532764/5b78be02-369e-4dea-84d0-2d34e3b53b8e


## Features

#### System:
- Some components can be interacted with your mouse!
- `ESC`, when held, exits the game (just like Undertale!)
- `F1`, while in-game, returns to the menu
- `F4`, while in-game, resets everything i.e. re-loads the level (without penalties!)
- `F5`, while in-game, toggles between camera angles
- `F6` toggles grayscale

#### Player:
- `W` `A` `S` `D` moves the player
- `Shift`, when held, makes the player run
- `Space` launchs a not-so-visually-aesthetic melee attack
- `1`, `2`, `3`, `4`, `5` each launches an unique set of projectiles, try them yourself!

#### Entities:
- Cat. Teleports you to somewhere fun.
- Slime. Approach with caution (will hurt you).

#### Map:
- Equilibrium
- Valley of Despair

(bad names btw - will expand in future commits)


## Installation

### Windows

**Prerequisites:**
- A configured C++ compiler, preferably [MinGW](https://sourceforge.net/projects/mingw/) or [MinGW-w64](https://www.mingw-w64.org/)
- Brain (optional)
- Compatible hardware

**Build & run:**
```bash
git clone https://github.com/NTDuck/8964 && cd 8964
make
make run
```

### Debian-based distributions

**Prerequisites**:
```
sudo apt-get update
sudo apt-get install g++ make libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev zlib1g-dev
```

**Build & run:**
Same as Windows. In case of permission-related issues, try `chmod +x ./build/8964`.

### Arch-based distributions

**Prerequisites**:
```
sudo pacman -S gcc make sdl2 sdl2_image sdl2_ttf sdl2_mixer zlib
```

**Build & run:**
Same as Windows.


### Others:

Official os-specific releases are not available, and compatibility layers (e.g. [wine](https://www.winehq.org/)) are not tested.


## Tech Stack

**Language**: C++ 17

**Dependencies:** 
- [SDL2](https://github.com/libsdl-org/SDL/releases/tag/release-2.28.3) 2.28.3, [SDL_image](https://github.com/libsdl-org/SDL_image/releases/tag/release-2.6.3) 2.6.3, [SDL_ttf](https://github.com/libsdl-org/SDL_ttf/releases/tag/release-2.20.2) 2.20.2, [SDL_mixer](https://github.com/libsdl-org/SDL_mixer/releases/tag/release-2.6.3) 2.6.3
- [JSON for Modern C++](https://github.com/nlohmann/json/releases/tag/v3.11.2) 3.11.2
- [pugixml](https://github.com/zeux/pugixml/releases/tag/v1.14) 1.14
- [zlib](https://www.zlib.net/) 1.3 (with Microsoft Github's precompiled [zlib.dll](https://github.com/microsoft/malmo/files/5758471/zlib.zip))

**Third-party tools:**
- [Tiled](https://www.mapeditor.org/) 1.10.2 for level generation
- [KRMisha's Makefile](https://github.com/KRMisha/Makefile) for cross-platform compilation

**Compiler:** [MinGW GCC](https://github.com/niXman/mingw-builds-binaries/releases/tag/13.2.0-rt_v11-rev0) 13.2.0

**Version control:** Git (_2.40.0.windows.1_)


## Class Hierarchy
![Class hierarchy, for code comprehension](/assets/graphics/misc/hierarchy.png)


## Acknowledgements

> We would like to credit certain assets used in this project. Licenses, if any, should be found in the links below.
1. Tilesets
- [Modern Interiors](https://limezu.itch.io/moderninteriors) (paid version)
- [OMORI Game Assets](https://www.spriters-resource.com/pc_computer/omori/)

- [Cat 50+ animations](https://bowpixel.itch.io/cat-50-animations)
- [Pixel Cats](https://pixelfight.itch.io/cat-pack)
- [Hooded Protagonist](https://penzilla.itch.io/hooded-protagonist)
- [Enemy Galore](https://admurin.itch.io/enemy-galore-1)
2. Fonts
- [Omori](https://dafontonline.com/omori-font/)
- [Phorssa](https://www.dafont.com/phorssa.font)
3. BGM & SFX
- [credits song for my death](https://youtu.be/urxeNbBc1nk) by [vivivivivi](https://www.youtube.com/@vivivivivi)
- [Your Reality - DDLC](https://youtu.be/GU2D4zmhvbo) by [Artemisia](https://www.youtube.com/@ArtemisiaBeats)
- [Inabakumori-styled な - instrumental](https://youtu.be/p0s0_4KO9t4) by [いめ44](https://www.youtube.com/@ime44)
- [Modern Touhou-styled World's End Valentine - OMORI](https://youtu.be/eX2O32VkU5s) by [Angelina2 RedRevo-Scarlet](https://www.youtube.com/@angelinaflan58)
- [Needy Streamer Overload OST - 16 No Salvation, No Angels](https://youtu.be/0Q_RiK3XPu8?list=PLXeqOX4jBooy_OV6DDXNgCC8bbcWV85Ls) by [Sesto](https://www.youtube.com/@Sesto-)
- [Determination - UNDERTALE OST 011](https://youtu.be/W1i4mTyidOc), [Home - UNDERTALE OST 012](https://youtu.be/5_E_y1AWAfc), and [It's Raining Somewhere Else - UNDERTALE OST 063](https://youtu.be/zNd4apsr3WE) by [Misaki](https://www.youtube.com/@Misakiu)
- [Wilhelm Scream SFX](https://youtu.be/r6JK-gRELI0)
- [Shoe on Wet Ground SFX](https://www.zapsplat.com/music/footstep-single-shoe-on-wet-ground-very-light-puddles-of-water-5/), [Heavy Clump on Gravel SFX](https://www.zapsplat.com/music/single-boot-footstep-heavy-clump-on-gravel-2/)
- Numerous BGM & SFX from OMORI
- [Lucky Star PS2 Bonus Disc](https://archive.org/details/lucky-star-ps-2-bonus-disc)
>
> We would also like to recognize reknowned games from which certain concepts and mechanics in this project drew inspiration, which include but are not limited to [Elden Ring](https://store.steampowered.com/app/1245620/ELDEN_RING/), [Omori](https://www.omori-game.com/), [Undertale](https://store.steampowered.com/app/391540/Undertale/), and [The Coffin of Andy and Leyley](https://store.steampowered.com/app/2378900/The_Coffin_of_Andy_and_Leyley/).


## License

This project is licensed under the [BSD-3-Clause license](LICENSE).


## Support

For support, please email nguyentuduck@gmail.com.
