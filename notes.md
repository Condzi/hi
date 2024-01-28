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

  ## Debug Tools
    [ ] Frame Time Graph
    [ ] Menu with toggable flags, for example for debug view of collisions
    [ ] Menu with Entity information...?


  ## Entities & Systems
    [ ] Error context, Check(cnd, "Context")
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

    - all kind of debug subsystems? for some diagrams, drawing, debug UI

  ## Game
    Just app initialization.

  ## GFX
  Do not get side tracked with multithreaded rendering!
  This should be splitted into backend / frontend and game land?
  In game land we would have actual usable pipelines, front end
  would be just the skeleton for the pipelines and materials?

    [ ] GPU info
    [ ] State interpolation??
    [ ] Materials support
    Top-left corner is the anchor!!!

    Check notes from BB.

  ## Key Bindings
    Checks input events and updates the binding status accordingly.

  ## OS Core
    [X] Debugger output
    [ ] Filesystem
    [ ] Threads
    [ ] Mutexes
    [ ] SEH
    [ ] Minidumps
    [X] Memory allocation
    [ ] library loading
    [ ] OS Information, SIMD detection, CPU and memory info
    [X] Time

  ## OS Gfx
    [ ] Message boxes (custom ones!) https://learn.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-taskdialogindirect
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

  ## Text Rendering
    [ ] Handling \n etc
    [ ] Rendering to texture
    [ ] Handle transforms
    [ ] Handling color (maybe escape codes?)
    [ ] Figure out styles bold, italic etc?

  ## UI
    [ ] Text box
    [ ] Button
    [ ] Window

  ## Misc
    Add Open Source licenses (RAD, stb...)

# Layers Dependencies

  ![Game Layers](game_layers_1.png)





