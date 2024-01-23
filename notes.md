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
    [1] Allocators
    [2] Strings (immutable!)
    [3] Unicode support
    [ ] CMD Parsing
    [ ] Math (rand..)

  ## Debug Tools
    [ ] Frame Time Graph
    [ ] Menu with toggable flags, for example for debug view of collisions
    [ ] Menu with Entity information...?


  ## Entities & Systems
    [ ] List of systems to update. Maybe sort by priority?

  ## Game
    Just app initialization.

  ## GFX
    [ ] GPU info
    [ ] State interpolation??
    [ ] Materials support
    Top-left corner is the anchor!!!

    Check notes from BB.

  ## Key Bindings
    Checks input events and updates the binding status accordingly.

  ## OS Core
    [4] Debugger output
    [ ] Filesystem
    [ ] Threads
    [ ] Mutexes
    [ ] Memory allocation
    [ ] library loading
    [ ] OS Information, SIMD detection, CPU and memory info
    [ ] Time

  ## OS Wnd
    [ ] Monitor enumeration
    [ ] Event loop and forwarding the OS events to our abstracted ones
    [ ] Message boxes (custom ones!) https://learn.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-taskdialogindirect
    [ ] Window

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





