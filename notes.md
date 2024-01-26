# Layers (alphabetically)
  ## Audio
    [ ] Enumerate available audio devices
    [ ] Frontend design (play_sound("blah"))
    [ ] Backend things (how to handle mastering and loading voices?)

  ## Assets
    List of textures and audio files...

  ## Base
    [X] Basic Types
    [X] Language Layer
    [X] Allocators
    [X] Strings (immutable!)
    [X] Unicode support
    [X] CMD Parsing
    [ ] Math (rand..)

  ## Debug Tools
    [ ] Frame Time Graph
    [ ] Menu with toggable flags, for example for debug view of collisions
    [ ] Menu with Entity information...?


  ## Entities & Systems
    [ ] Error context, Check(cnd, "Context")
    [ ] List of systems to update. Maybe sort by priority?

  ## Game
    Just app initialization.

  ## GFX
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
    [X] Memory allocation
    [ ] library loading
    [ ] OS Information, SIMD detection, CPU and memory info
    [X] Time

  ## OS Gfx
    [ ] Message boxes (custom ones!) https://learn.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-taskdialogindirect
    [ ] Monitor enumeration
    [ ] Event loop and forwarding the OS events to our abstracted ones
    [X] Window

  ## Physics 
    [ ] SAH
    [ ] BVH
    [ ] Debugging stats?

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

# Layers Dependencies

  ![Game Layers](game_layers_1.png)





