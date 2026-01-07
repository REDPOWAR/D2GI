# D2GI - D2 Graphic Improvements  

[Читать на русском](README-RU.md)

*D2GI* - is a graphical modification which adapts the game "King of the Road" (Hard Truck 2, Rig'n'Roll, Дальнобойщики 2) for modern PC. This is a *Direct3D*-wrapper, transferring the functionality of the legacy *DirectDraw7* + *Direct3D7* bundle to the *Direct3D9*, which works correctly on all modern Windows versions. Developed to replace existing GOG and dgVoodoo wrappers, since these are not wonderful in this game: GOG wrapper rendered graphics incorrectly, while dgVoodoo wrapper is full of freezes and crashes.

### Features  

* Fixes incorrect graphics output (["Rainbow" bug](https://www.pcgamingwiki.com/wiki/Rainbow_color_problems_in_older_games), bug with an object's transparency etc.)
* You can specify any resolution with any aspect ratio (4:3, 16:9 etc.)
* 3 modes: windowed, borderless, fullscreen
* 32-bit color rendering
* Compatibility with vanilla 8.1 or King of the Road 1.3 game versions (digital releases)
* Compatibility with [SEMod 1.2](https://vk.com/rnr_mods?w=page-157113673_53889819)
* Works stable, without freezes or crashes 
* High FPS on low-end hardware (even on integrated GPU like Intel HD Graphics 4000)

### Requirements  

* Game version 8.1 or KotR 1.3
* Windows XP or higher  

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

`HOOKS` section:
* `EnableHooks` - enable game functions hooking (projection matrix correction with any aspect ratio) 
