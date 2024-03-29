# Game  

Notes.

## Pitch

Game about defending your house from zombies.
Buy weapons, build traps and survive as long as you can.

Procedurally generated homes.
Clever traps mechanics.
Rewarding gameplay.

No overthinking -- money from zombies. Buy traps and guns using money. It
does not need to be realistic in order to be fun!

## Zombie AI

Based on behavior trees and state machines.  
Zombies may have following states:  

- looking for food -- initial; food=humans(player),meat  
- eating
- attacking
  - approaching player
  - fighting / biting etc
  - escaping / covering (if player is shooting)
- destroying (if they sense/see food behind barricade, or when it is in their way?)

World state data (for each zombie):

- objects around the zombie (raycast)?
- sounds around the zombie (loud player noises and not only, anything worth inspecting) -- some zombies may be deaf
- path finding data (waypoints)
- interactable objects information (if there is breakable object on the way of pathfindng, try to break it. if not", take another route); windows, doors, barricades -- some zombies may not have hands, or just cant break things other zombies can
- alerting other zombies / alerts / shouts -- when one zombie finds big pile of food, it may alert its friends. if zombie finds player, it will certainly want to alert that there are living humans here :) -- maybe not all zombies are capable of doing that?

## Prototype

- 2D
- Waves of zombies
- Buying new weapons after every wave
- $$$ from zombies
- house and yard to walk around
- audiovisual effects
- simple trap building, not restricted to a grid
- basic menu

Traps can be built during the preparation phase (60s before the wave) and
during the wave.

Waves could have constant time, maybe 2mins? Then the wave remains until last zombie is dead.

## Trap ideas

Funny traps, like car on a string, or mouse-like trap...

## Zombie ideas

Classic ones? slow, fast, exploding kamikaze, big mutants?
inspo from plants vs zombies (jumping over fence etc)
Funny zombies like jumping ones, runners, zombie that looks like
a tank

## Full version

- dynamic lighting
- procedurally generated homes? or maybe few maps to select and to play
  with friends?

- Cartoony style
- Hotline Miami
- Plants vs. Zombies
- humor (main character saying some funny stuff about his life pre-apocalypse)
