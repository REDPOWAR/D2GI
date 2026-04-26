# D2GI - D2 Graphic Improvements

[Читать на русском](README-RU.md)

*D2GI* - is a graphical modification which adapts the game "King of the Road" (Hard Truck 2, Rig'n'Roll, Дальнобойщики 2) for modern PC. This is a *Direct3D*-wrapper, transferring the functionality of the legacy *DirectDraw7* + *Direct3D7* bundle to the *Direct3D9*, which works correctly on all modern Windows versions. Developed to replace existing GOG and dgVoodoo wrappers, since these are not wonderful in this game: GOG wrapper rendered graphics incorrectly, while dgVoodoo wrapper is full of freezes and crashes.

### Features

* Allows you to play the game on modern PC
* Fixes incorrect graphics output (["Rainbow" bug](https://www.pcgamingwiki.com/wiki/Rainbow_color_problems_in_older_games), bug with an object's transparency etc.)
* Fixes problems with rendering the background of translucent objects
* Fixes ["horizontal rain"](https://github.com/REDPOWAR/D2GI/issues/1) bug
* Fixes the North Harbor bridge illumination, previously broken in the "King of the Road" releases
* Fixes a bug with overwriting screenshots and allows you to select the format and path for saving them
* You can specify any resolution with any aspect ratio (4:3, 16:9 etc.)
* 3 modes: windowed, borderless, fullscreen
* 32-bit color rendering
* An option to enable Anisotropic Filtering
* Support for Multisample Anti-Aliasing (MSAA)
* Compatibility with multiple game versions (see **Requirements** section below)
* Compatibility with [SEMod 1.2](https://vk.com/rnr_mods?w=page-157113673_53889819)
* Fixes performance issue in 8.2 game version (reverts one of 8.2 patch changes that ruins it)
* Smooth minimap scrolling
* Works stable, without freezes or crashes
* High FPS on low-end hardware (even on integrated GPU like Intel HD Graphics 4000)

If you are looking for some game logic fixes (e.g. fix for extremely annoying bug when vehicles freeze on side collision), you may be interested in [KoTR Modern Patch](https://github.com/aleko2144/KoTR_Modern_Patch)

### Requirements

* GPU with shaders 2.0 or higher
* Any game version of:
  * King of the Road 1.3 (international release)
  * 6.6 - 8.2 (Russian releases)
  * Kelyje 2 (Lithuanian release)
  * some other versions (other game versions may be compatible too, but some D2GI features probably won't work and there may be some stability issues)
* Windows XP or higher (note that there is problem on Windows 11 - game movies don't play, and it's not resolved by D2GI yet, but you can try to fix it manually as described in [corresponding issue](https://github.com/REDPOWAR/D2GI/issues/18))

### Download

[Download](https://github.com/REDPOWAR/D2GI/releases)

### Installation

You need to unzip the files into a game folder.

### Settings

The `d2gi.ini` file has the following settings.
`VIDEO` section:
* `Width` - Screen width like `1920` (`0` - auto)
* `Height` - Screen height like `1080` (`0` - auto)
* `WindowMode` - Window mode. Possible values: `windowed`, `borderless`, `fullscreen`.
* `EnableVSync` - Turn vertical sync on or off (`1` and `0` accordingly)
* `FixAlpha`- Turn correction of background rendering of translucent objects on or off (`1` and `0` accordingly)
* `AnisotropyLevel` - Force Anisotropic Filtering on textures to sharpen surfaces viewed at a steep angle, like the roads or terrain (`1` to `16`)
* `MSAALevel` - Multisample Anti-Aliasing level (`0` - off, `2` to `16`, `max` maximum supported)
* `AllCoresAffinity` - Patch 8.2 forces all threads to a single core, ruining performance. Setting this option to 1 reverts this change.

`HOOKS` section:
* `EnableHooks` - Enable game functions hooking (projection matrix correction with any aspect ratio)
* `EnableUIFix` - Enable GUI functions hooking (improves the appearance of game menus when using wide screen resolutions).
* `EnableMirrorsFix` - Enable side mirrors position adjustment for wide screens (it may work better or worse depending on vehicle you are using)

`SCREENSHOTS` section:
* `SavePath` - Path to the folder where screenshots will be saved
* `ImageFormat` - Screenshots image format. Possible values: `bmp`, `png`, `jpg`

### Special thanks to contributors

* [CookiePLMonster](https://github.com/CookiePLMonster)
* [aleko2144](https://github.com/aleko2144)
