# D2GI - D2 Graphic Improvements  

[Читать на русском](README.md)

*D2GI* - is a graphical modification which adapts the game "King of the Road" (Hard Truck 2, Rig'n'Roll) for modern PC. This is a *Direct3D*-wrapper, transferring the functionality of the legacy *DirectDraw7* + *Direct3D7* bundle to the *Direct3D9*, which works correctly on all modern Windows versions. Developed to replace existing GOG and dgVoodoo wrappers, since these are not wonderful: GOG wrapper rendered graphics incorrectly, while dgVoodoo wrapper is full of friezes and crashes.

### Features  

* Fixes incorrect graphics output (["Rainbow" bug](https://www.pcgamingwiki.com/wiki/Rainbow_color_problems_in_older_games), bug with an object's transparency etc.)
* You can specify any resolution with any aspect ratio (4:3, 16:9 etc.)
* 3 modes: windowed, borderless, fullscreen
* 32-bit color rendering
* Compatibility with vanilla 8.1 game version
* Compatibility with [SEMod 1.2](https://vk.com/rnr_mods?w=page-157113673_53889819)
* Works stable, without freezes or crashes (~~as for me~~)
* High FPS on low-end hardware

### Requirements  

* Game version 8.1 or higher 
* Windows XP or higher  

### Download  

[Download](https://github.com/REDPOWAR/D2GI/releases)  

### Installation  

You need to unzip the files into a folder with the game.

### Settings  

The `d2gi.ini` file has the following settings. 
`VIDEO` section:
* `Width` - Screen width like `1920` (`0` - auto)  
* `Height` - Screen height like `1080` (`0` - auto)  
* `WindowMode` - Window mode. Possible values: `windowed`, `borderless`, `fullscreen`.
* `EnableVSync` - Turn vertical sync on or off (`1` and `0` accordingly)

`HOOKS` section:
* `EnableHooks` - enable game functions hooking (matrix projection correction with any aspect ratio) 

### Demo  

Vanilla 8.1 version benchmark on Intel HD Graphics 4000:  
[![video](http://img.youtube.com/vi/cP87WtU5F9I/0.jpg)](https://www.youtube.com/watch?v=cP87WtU5F9I)  

SEMod 1.2 benchmark on Intel HD Graphics 4000:  
[![video](http://img.youtube.com/vi/IbHq2INIn5c/0.jpg)](https://www.youtube.com/watch?v=IbHq2INIn5c)  
