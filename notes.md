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
    
    - shop system
    - money system..?
    - pick ups system

    - all kind of debug subsystems? for some diagrams, drawing, debug UI

  ## Game
    Just app initialization.

  ## GFX
  Do not get side tracked with multithreaded rendering!
  This should be splitted into backend / frontend and game land?
  In game land we would have actual usable pipelines, front end
  would be just the skeleton for the pipelines and materials?

    [ ] Camera / multiple viewports? Maybe set a viewport per render target?
    [ ] GPU info
    [ ] State interpolation??
    [ ] Materials support
    Top-left corner is the anchor!!!

    - Resize buffers on WM_SIZE (IDXGISwapChain::ResizeBuffers)
    - Fallback to WARP when D3D hardware not available (https://learn.microsoft.com/en-us/windows/win32/direct3darticles/directx-warp)
    - D3D Present will fail on driver crash - handle it accordingly
    - Render_Target oriented, so we can apply effects to surfaces
    - Sort sprites by layer:
      This would be a separate thing from IM. Added just because we can't easily sort
      immediate mode vertices by layer. It would look kinda like sf::Sprite?
    - IM pipelines should not be global - we should have Batchers instead. Perhaps they
      could be set globally, but changed on the fly, so for example GUI can be rendered
      to one batcher, than the texture could be taken and put on some sprite.

    - We can put post-processing pipelines (and other?) to a linked-list of pipelines!

  ### Use GPU for particle effects
    - use this particle effect to create trail effect behind space ships

    We need textures and noise:
    https://www.youtube.com/watch?v=wvK6MNlmCCE

  ### Summary
  Batcher renders to a Render_Target. Result of that is a texture.
  This texture can be assigned to a sprite. Sprite can be easily layered, where
  in batchers, because they are immediate-mode, the layering is based on what was
  drawn last.

  Example:
  GUI renders text to a batcher. Because this text did not change since last frame,
  it reuses the baked texture and does not regenerate it.
  Then, the rest of the GUI gets rendered to some different batcher/render target, so
  the gui in the end can get drawn as a one texture, either on top of the game
  post-processing, or with it.

  ### Pipelines
  All pipelines follow a common structure:
  - Vertex and Pixel Shader
  - Input Layout (Maybe should be shared amongst all pipelines?)
  - Vertex Buffer
  - VS and PS Constants Buffers (camera matrices, time, viewport size...)
  - Render Target (dest, source (can be empty))
  - Optionally: Sampler State, Rasterizer State, Blend State
  - Additional, pipeline-specific uniforms for adjusting the effects

  Additionally, batchers pipelines have instancing. How to handle that?

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





