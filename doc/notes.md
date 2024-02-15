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

    [ ] Bitset / Bit Array
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

  [ ] Figure out common viewport sizes that we can render
    Common aspect ratios and their resolutions (via Steam HW Survey):
    - 16:9  (85% of users)  1920x1080  (60% of users)
    - 16:10 (7% of users)   2560x1600  (3% of users)
    - 21:9  (3% of users)   3440x1440  (2% of users)

    I guess the best option will be to stick with 16:9, 1080p and letterboxing
    in other aspect ratios. Although, we should support other resolutions.

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

    [ ] Filesystem 
    [ ] File Watch (notify when file updates) / Hot Reload
    [ ] Threads
    [ ] Mutexes
    [ ] SEH
    [ ] Minidumps
    [ ] library loading
    [ ] OS Information, SIMD detection, CPU and memory info

## OS Gfx

    [ ] Monitor enumeration

## Physics

    Must support AI and building system! Also, collision resolution (just don't allow objects to overleap, nothing fancy for now)
    [ ] Islands
    [ ] SAH
    [ ] BVH
    [ ] Static geometry
    [ ] Debugging stats?

### Resources

  Islands (multithreading!): <https://box2d.org/posts/2023/10/simulation-islands/>
  Erin's Solver2D: <https://box2d.org/posts/2024/02/solver2d/>

## Telemetry

    Basically, use the microseconds timer to time some functions in instrumented release-with-debug-info build.
    Mainly we're interested in frame times and GPU render time

## Text Rasterizing

    Use DirectDraw or freetype to render fonts?
    We will probably use it later. For now, just take ready made fonts.

## Logging

    Basically own layer for writing to a buffer. Multiple severities? Also, error boxes live here? Or what? 
    Handles logging to file...? And to console?

## Localization

    Have some special localized strings that are mapped to some string loaded from a 
    CSV. Only in UI I guess?

## Text Rendering

    Only monospace fonts for simplicity?
    
    [ ] Add code from BB for simple font rendering
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
