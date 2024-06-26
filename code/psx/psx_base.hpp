#pragma once

// Constants
//
f32 read_only global PSX_STEP_HZ = 60;
f32 read_only global PSX_STEP    = 1 / PSX_STEP_HZ;

f32 read_only global PSX_SCALE     = 100.f;
f32 read_only global PSX_SCALE_INV = 1 / PSX_SCALE;

// Common types
//

using PSX_World_ID = b2WorldId;
using PSX_Body_ID  = b2BodyId;
using PSX_Shape_ID = b2ShapeId;
using PSX_Joint_ID = b2JointId;

#define PSX_IS_NULL(id)     B2_IS_NULL(id)
#define PSX_NON_NULL(id)    B2_IS_NON_NULL(id)
#define PSX_ID_EQUALS(a, b) B2_ID_EQUALS(a, b)

enum PSX_Shape_Type : u8 {
  PSX_ShapeType_Polygon,
};

enum PSX_Body_Type : u8 {
  PSX_BodyType_Static    = b2_staticBody,
  PSX_BodyType_Kinematic = b2_kinematicBody,
  PSX_BodyType_Dynamic   = b2_dynamicBody,
  PSX_BodyType__count    = b2_bodyTypeCount,
};

struct PSX_Body_Opts {
  PSX_Body_Type type            = {};
  fvec2         pos             = {};
  f32           rot             = 0;
  fvec2         linear_vel      = {};
  f32           angular_vel     = {};
  bool          fixed_rot       = false;
  f32           damping_linear  = 0;
  f32           damping_angular = 0;
  void         *user_data       = 0;
};

struct PSX_Filter {
  u32 category; // Usually just set one.
  u32 mask;     // What shapes does this shape accept for collision?
};

struct PSX_Shape_Opts {
  PSX_Filter filter;
  f32        friction;
  f32        restitution;
  f32        density;
  bool       is_sensor;
  bool       enable_sensor_events;  // If is_sensor==false
  bool       enable_contact_events; // if is_sensor==false
};

struct PSX_Motor_Joint_Opts {
  PSX_Body_ID body_a;
  PSX_Body_ID body_b;
  bool        collide_connected;
  fvec2       linear_offset;
  f32         angular_offset;
  f32         max_force;
  f32         max_torque;
  f32         correction_factor;
};

struct PSX_Wheel_Joint_Opts {
  PSX_Body_ID body_a;
  PSX_Body_ID body_b;
  bool        collide_connected;
  bool        enable_motor;
  f32         max_motor_torque;
  f32         motor_speed;
};

// Functions
//

void
psx_init();

must_use PSX_World_ID
psx_make_world(u64 num_objects);

void
psx_world_simulate(PSX_World_ID w, f32 dt);

must_use PSX_Body_ID
psx_world_add(PSX_World_ID world, PSX_Body_Opts const &opts);

void
psx_world_remove(PSX_Body_ID id);

must_use PSX_Body_ID
psx_shape_id_to_body_id(PSX_Shape_ID id);

must_use void*
psx_body_get_user_data(PSX_Body_ID id);

void
psx_body_add_force(PSX_Body_ID id, fvec2 force);

void
psx_body_set_position(PSX_Body_ID id, fvec2 pos);

must_use fvec2
psx_body_get_position(PSX_Body_ID body);

void
psx_body_fix_rotation(PSX_Body_ID body);

void
psx_body_set_rotation(PSX_Body_ID body, f32 rot);

must_use f32
psx_body_get_rotation(PSX_Body_ID body);

void
psx_body_add_box_shape(PSX_Body_ID body, PSX_Shape_Opts const &opts, fvec2 sz);

must_use PSX_Joint_ID
psx_make_motor_joint(PSX_World_ID w, PSX_Motor_Joint_Opts const &opts);

must_use PSX_Joint_ID
psx_make_wheel_joint(PSX_World_ID w, PSX_Wheel_Joint_Opts const &opts);

must_use PSX_Shape_ID
psx_raycast_nearest(PSX_World_ID w, fvec2 origin, fvec2 target, PSX_Filter filter);
