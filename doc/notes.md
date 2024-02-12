# Game Layers
  
  ![Game Layers](game_layers_1.png)

## Audio

    [ ] Enumerate available audio devices
    [ ] Frontend design (play_sound("blah"))
    [ ] Backend things (how to handle mastering and loading voices?)

## Assets

    List of textures and audio files...
    [ ] list of resources that can be accessed with their IDs / handles?

## Base

    [ ] Rand (perlin noise, random range)

## Debug Tools

    [ ] Frame Time Graph
    [ ] Menu with toggable flags, for example for debug view of collisions
    [ ] Menu with Entity information...?

## Entities & Systems

    [ ] List of systems to update. Maybe sort by priority?

    Rough system ideas:
    - physics system
    - pathfinding system (coupled to physics system?)
    - rendering system? that gathers all entities and their states and determines what should be rendered? (so, clipping?)
    - ai system
    - trap building system (tightly coupled with player interaction I think?)
    - trap logic system (what should happen if triggered etc)
    - player interaction system (handling input?)
    - enemy interaction system (should it exist? wouldn't AI cover everything?)
    - entity management stuff (killing / spawning entities logic)
    - animation system (just simple sprite sheet for now? later bones)
    - sound system?
    - gun system (reloading animations, recoil...?)
    - playback system? maybe?
    - wave logic system (what to spawn, where) / level management system?
    - game master system (is it a system?) for managing scenes (menu -> game, saving, loading new levels). maybe it should be higher, above the systems?
    
    - shop system
    - money system..?
    - pick ups system

    - all kind of debug subsystems? for some diagrams, drawing, debug UI

    - Level of detail handling

## Game

    Just app initialization.

## GFX

  [ ] Camera Node for the render graph. It sets the view.
  [ ] Batching & Layering

    1. `GFX_Object`s submitted to a per-frame array
    2. Objects are sorted by layer.
    3. Objects are assigned to correct batches. For sprites, look for the same texture.
      Rects are the simplest to batch, just combine all subsequent objects.
    4. Depending on which batch is selected and in what order, dynamically construct
      (re-arrange) render graph nodes.

  [ ] Figure out common viewport sizes that we can render
  [ ] Figure out MVP matrix -- how exactly does it work in our 2D game?
  [ ] GPU info (memory, name...)
  [ ] GPU render time

    - variable refresh rate: https://walbourn.github.io/care-and-feeding-of-modern-swap-chains-3/
    and https://learn.microsoft.com/en-us/windows/win32/direct3ddxgi/variable-refresh-rate-displays

  [ ] Particle effects

    We need textures and noise:
    https://www.youtube.com/watch?v=wvK6MNlmCCE

## Key Bindings

    Checks input events and updates the binding status accordingly.

## OS Core

    [X] Debugger output
    [ ] Filesystem 
    [ ] File Watch (notify when file updates) / Hot Reload
    [ ] Threads
    [ ] Mutexes
    [ ] SEH
    [ ] Minidumps
    [X] Memory allocation
    [ ] library loading
    [ ] OS Information, SIMD detection, CPU and memory info
    [X] Time

## OS Gfx

    [X] Message boxes (custom ones!) https://learn.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-taskdialogindirect
    [ ] Monitor enumeration
    [X] Event loop and forwarding the OS events to our abstracted ones
    [ ] RawInput, keys and mouse buttons, scroll and mouse position
    [X] Window

## Physics

    Must support AI and building system! Also, collision resolution (just don't allow objects to overleap, nothing fancy for now)
    [ ] SAH
    [ ] BVH
    [ ] Debugging stats?
    [ ] Static geometry

## Telemetry

    Basically, use the microseconds timer to time some functions in instrumented release-with-debug-info build.
    Mainly we're interested in frame times and GPU render time

## Text Rasterizing

    Use DirectDraw or freetype to render fonts?

## Logging

    Basically own layer for writing to a buffer. Multiple severities? Also, error boxes live here? Or what? 
    Handles logging to file...? And to console?

## Localization

    Have some special localized strings that are mapped to some string loaded from a 
    CSV. Only in UI I guess?

## Text Rendering

    [ ] Handling \n etc
    [ ] Rendering to texture
    [ ] Handle transforms
    [ ] Handling color (maybe escape codes?)
    [ ] Figure out styles bold, italic etc?

## UI

    [ ] Text box (automatically wraps)
    [ ] Button
    [ ] Window
    [ ] Panels
    [ ] Auto layout?

## Misc
