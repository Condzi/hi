# Current tasks list

## Shooting

  [ ] Use complex numbers for rotation
    [ ] Complex type, conversion from/to radians
    [ ] Implement in graphics
    [ ] Implement in physics
  [ ] Cursor to screen coords
    [ ] Add 'calculate_transform' and 'calcualte_transform_inv'
    [ ] Cache the camera inverse
    [ ] Add function for getting the mouse pos in world coords
  [ ] Rotate player towards cursor
  [ ] Raycast in PSX
  [ ] Raycast from player's gun (important!) towards mouse
  [ ] Kill zombie if found via raycast

## Physics World

  Just use Box2c? I'm not good enough to do good physics.
  I can always hide Box2C behind my own API, so that's that.

  [ ] Static geometry (for generating path finding data)
  [ ] Dynamic geometry (moves when shot; barricades that can be destroyed by zombies; traps)
  [ ] Collision resolution -- HOW DOES THIS SHIT WORKS?!
