# Layers (alphabetically)

## Audio

    [ ] Enumerate available audio devices
    [ ] Frontend design (play_sound("blah"))
    [ ] Backend things (how to handle mastering and loading voices?)

## Assets

    List of textures and audio files...
    [ ] stb_image integration
    [ ] list of resources that can be accessed with their IDs / handles?

## Base

    [X] Basic Types
    [X] Language Layer
    [X] Allocators
    [X] Strings (immutable!)
    [X] Unicode support
    [X] CMD Parsing
    [X] stb_sprintf integration
    [X] Math (vectors, matrices, functions)
    [ ] Rand (perlin noise, random range)
    [X] Error context, Check(cnd, "Context")

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

## Game

    Just app initialization.

## GFX

  [!] Assume we always and only render rects?

    [X] Initialization
    [X] Debug device
    [ ] Batch rendering for filled rects
      [X] Vertex Input Layout -- only per-instance data!! Empty vertex buffer?
      [ ] Creating Batches
      [ ] Inserting objects into batches
      [ ] Memory management
      [ ] Figure out MVP matrix -- how exactly does it work in our 2D game?

    [ ] GPU info

    - separate resource handling (tex, buffers) from rendering commands somehow?
      - it would be great if we could manage to handle sprite sheets in a way that we
        can divide the sprite sheet into smaller textures logically and refer to them
        not by indecies, but by names, from high level code.
    - variable refresh rate: https://walbourn.github.io/care-and-feeding-of-modern-swap-chains-3/
    and https://learn.microsoft.com/en-us/windows/win32/direct3ddxgi/variable-refresh-rate-displays

### Use GPU for particle effects

    - use this particle effect to create trail effect behind space ships

    We need textures and noise:
    https://www.youtube.com/watch?v=wvK6MNlmCCE

### Render Graphs / Commands queues

- Resource creation and gpu commands should be separate?
- How to keep track of resources, exactly?
- We can have multiple queues. In the end, they are combined into one queue.
- Each command is identified and sorted by ID.

### Misc notes

- SRV are read only, UAV are read/write but a bit more expensive
- there need to be different levels of the api, from low (resource creation) to high (render graph???)

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

## [new] Text Rasterizing

    Use DirectDraw or freetype to render fonts?

## [new] LOD system?

    Can this be a separate layer? Basically, determine if something is
    off-screen and handle it with less detail? Or just apply this philosophy for existing systems?

## [new] Logging system?

    Basically own layer for writing to a buffer. Multiple severities? Also, error boxes live here? Or what? 
    Handles logging to file...? And to console?

## [new] Localization

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

    Add Open Source licenses (RAD, stb...)

# Layers Dependencies

  ![Game Layers](game_layers_1.png)
