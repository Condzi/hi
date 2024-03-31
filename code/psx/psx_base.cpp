internal int
box2d_assert_callback(const char *condition, const char *file_name, int line) {
  LogEng_Err("[box2d] %s:%d: %s", file_name, line, condition);
  return 1;
}

void
psx_init() {
  b2SetAssertFcn(&box2d_assert_callback);
}

must_use PSX_World_ID
psx_make_world(u64 num_objects) {
  b2WorldDef def   = b2DefaultWorldDef();
  def.gravity      = {};
  def.bodyCapacity = (int32_t)num_objects;
  return b2CreateWorld(&def);
}

void
psx_world_simulate(PSX_World_ID w, f32 dt) {
  b2World_Step(w, dt, 4);
}

must_use PSX_Body_ID
psx_world_add(PSX_World_ID world, PSX_Body_Opts const &opts) {
  b2BodyDef def       = b2DefaultBodyDef();
  def.type            = (b2BodyType)opts.type;
  def.position        = {.x = opts.pos.x, .y = opts.pos.y};
  def.angle           = opts.rot;
  def.linearVelocity  = {.x = opts.linear_vel.x, .y = opts.linear_vel.y};
  def.angularVelocity = opts.angular_vel;
  def.linearDamping   = opts.damping_linear;
  def.angularDamping  = opts.damping_angular;
  def.fixedRotation   = opts.fixed_rot;
  return b2CreateBody(world, &def);
}

void
psx_world_remove(PSX_Body_ID id) {
  b2DestroyBody(id);
}

void
psx_body_add_force(PSX_Body_ID id, fvec2 force) {
  b2Body_ApplyForceToCenter(id, {.x = force.x, .y = force.y}, true);
}

must_use fvec2
psx_body_get_position(PSX_Body_ID body) {
  b2Vec2 pos = b2Body_GetPosition(body);
  return {pos.x, pos.y};
}

must_use f32
psx_body_get_rotation(PSX_Body_ID body) {
  return b2Body_GetAngle(body);
}

void
psx_body_add_box_shape(PSX_Body_ID body, PSX_Shape_Opts const &opts, fvec2 sz) {
  b2Polygon  box          = b2MakeBox(sz.x/2, sz.y/2);
  b2ShapeDef def          = b2DefaultShapeDef();
  def.friction            = opts.friction;
  def.restitution         = opts.restitution;
  def.density             = opts.density;
  def.filter              = {.categoryBits = opts.filter.category, .maskBits = opts.filter.mask};
  def.isSensor            = opts.is_sensor;
  def.enableSensorEvents  = opts.enable_sensor_events;
  def.enableContactEvents = opts.enable_contact_events;
  b2CreatePolygonShape(body, &def, &box);
}
