#pragma once

struct PSX_Participants_Array {
  u32 *v;
  u64  sz;
};

// Return array with IDs of alive objects in the world.
//
must_use internal PSX_Participants_Array
psx_get_simulation_participants(PSX_World_ID w);

must_use internal bool
psx_sat_test(PSX_Polygon_Shape const &shape_a, PSX_Polygon_Shape const &shape_b);
