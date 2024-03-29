
// Common math functions
//

must_use f32
fsqrt(f32 x) {
  return sqrtf(x);
}

must_use f32
fsin(f32 x) {
  return sinf(x);
}

must_use f32
fcos(f32 x) {
  return cosf(x);
}

must_use f32
ftan(f32 x) {
  return tanf(x);
}

must_use f32
fatan2(f32 y, f32 x) {
  return atan2f(y, x);
}

must_use bool
f32_is_near(f32 a, f32 b) {
  return fabs(a - b) <= EPS_F32;
}

must_use f32
fceil(f32 x) {
  return ceilf(x);
}

must_use f32
ffloor(f32 x) {
  return floorf(x);
}

// Vector functions
//

must_use f32
dist_sq(fvec2 a, fvec2 b) {
  b = a - b;
  return dot(b, b);
}

must_use f32
dist_sq(fvec3 a, fvec3 b) {
  b = a - b;
  return dot(b, b);
}

must_use f32
dist_sq(fvec4 a, fvec4 b) {
  b = a - b;
  return dot(b, b);
}

must_use f32
dist(fvec2 a, fvec2 b) {
  return fsqrt(dist_sq(a, b));
}

must_use f32
dist(fvec3 a, fvec3 b) {
  return fsqrt(dist_sq(a, b));
}

must_use f32
dist(fvec4 a, fvec4 b) {
  return fsqrt(dist_sq(a, b));
}

must_use f32
len_sq(fvec2 v) {
  return dot(v, v);
}

must_use f32
len_sq(fvec3 const &v) {
  return dot(v, v);
}

must_use f32
len_sq(fvec4 const &v) {
  return dot(v, v);
}

must_use f32
len(fvec2 v) {
  return fsqrt(len_sq(v));
}

must_use f32
len(fvec3 const &v) {
  return fsqrt(len_sq(v));
}

must_use f32
len(fvec4 const &v) {
  return fsqrt(len_sq(v));
}

fvec2 &
normalize(fvec2 &v) {
  return (v *= 1 / len(v));
}

fvec3 &
normalize(fvec3 &v) {
  return (v *= 1 / len(v));
}

fvec4 &
normalize(fvec4 &v) {
  return (v *= 1 / len(v));
}

must_use fvec2
normalized(fvec2 v) {
  return normalize(v);
}

must_use fvec3
normalized(fvec3 v) {
  return normalize(v);
}

must_use fvec4
normalized(fvec4 v) {
  return normalize(v);
}

must_use f32
dot(fvec2 a, fvec2 b) {
  a *= b;
  return (a.x + a.y);
}

must_use f32
dot(fvec3 a, fvec3 b) {
  a *= b;
  return (a.x + a.y + a.z);
}

must_use f32
dot(fvec4 a, fvec4 b) {
  a *= b;
  return (a.x + a.y + a.z + a.w);
}

must_use fvec3
cross(fvec3 a, fvec3 b) {
  fvec3 c {
      .x = a.y * b.z - a.z * b.y,
      .y = a.z * b.x - a.x * b.z,
      .z = a.x * b.y - a.y * b.x,
  };
  return c;
}

must_use bool
near_zero(fvec2 v) {
  return f32_is_near(v.x, 0) && f32_is_near(v.y, 0);
}

must_use bool
near_zero(fvec3 v) {
  return f32_is_near(v.x, 0) && f32_is_near(v.y, 0) && f32_is_near(v.z, 0);
}

must_use fvec4
hex_color(u32 hex) {
  fvec4 c = {
      .r = (f32)((hex >> 24) & 0xFF) / 255.0f,
      .g = (f32)((hex >> 16) & 0xFF) / 255.0f,
      .b = (f32)((hex >> 8) & 0xFF) / 255.0f,
      .a = (f32)(hex & 0xFF) / 255.0f,
  };
  return c;
}

// Vector ops
//

must_use bool
operator==(fvec2 a, fvec2 b) {
  return (MemoryCompare(&a, &b, sizeof(a)) == 0);
}

must_use fvec2
operator+(fvec2 a, fvec2 b) {
  fvec2 c = {
      .x = a.x + b.x,
      .y = a.y + b.y,
  };
  return c;
}

must_use fvec2
operator-(fvec2 a, fvec2 b) {
  fvec2 c = {
      .x = a.x - b.x,
      .y = a.y - b.y,
  };
  return c;
}

must_use fvec2
operator*(fvec2 a, fvec2 b) {
  fvec2 c = {
      .x = a.x * b.x,
      .y = a.y * b.y,
  };
  return c;
}

must_use fvec2
operator/(fvec2 a, fvec2 b) {
  fvec2 c = {
      .x = a.x / b.x,
      .y = a.y / b.y,
  };
  return c;
}

must_use fvec2
operator*(fvec2 v, f32 x) {
  fvec2 c = {
      .x = v.x * x,
      .y = v.y * x,
  };
  return c;
}

fvec2 &
operator+=(fvec2 &a, fvec2 b) {
  return a = a + b;
}

fvec2 &
operator-=(fvec2 &a, fvec2 b) {
  return a = a - b;
}

fvec2 &
operator*=(fvec2 &a, fvec2 b) {
  return a = a * b;
}

fvec2 &
operator/=(fvec2 &a, fvec2 b) {
  return a = a / b;
}

fvec2 &
operator*=(fvec2 &a, f32 x) {
  return a = a * x;
}

must_use bool
operator==(fvec3 a, fvec3 b) {
  return (MemoryCompare(&a, &b, sizeof(a)) == 0);
}

must_use fvec3
operator+(fvec3 a, fvec3 b) {
  fvec3 c = {
      .x = a.x + b.x,
      .y = a.y + b.y,
      .z = a.z + b.z,
  };
  return c;
}

must_use fvec3
operator-(fvec3 a, fvec3 b) {
  fvec3 c = {
      .x = a.x - b.x,
      .y = a.y - b.y,
      .z = a.z - b.z,
  };
  return c;
}

must_use fvec3
operator*(fvec3 a, fvec3 b) {
  fvec3 c = {
      .x = a.x * b.x,
      .y = a.y * b.y,
      .z = a.z * b.z,
  };
  return c;
}

must_use fvec3
operator/(fvec3 a, fvec3 b) {
  fvec3 c = {
      .x = a.x / b.x,
      .y = a.y / b.y,
      .z = a.z / b.z,
  };
  return c;
}

must_use fvec3
operator*(fvec3 v, f32 x) {
  fvec3 c = {
      .x = v.x * x,
      .y = v.y * x,
      .z = v.z * x,
  };
  return c;
}

fvec3 &
operator+=(fvec3 &a, fvec3 b) {
  return a = a + b;
}

fvec3 &
operator-=(fvec3 &a, fvec3 b) {
  return a = a - b;
}

fvec3 &
operator*=(fvec3 &a, fvec3 b) {
  return a = a * b;
}

fvec3 &
operator/=(fvec3 &a, fvec3 b) {
  return a = a / b;
}

fvec3 &
operator*=(fvec3 &a, f32 x) {
  return a = a * x;
}

must_use bool
operator==(fvec4 a, fvec4 b) {
  return (MemoryCompare(&a, &b, sizeof(a)) == 0);
}

must_use fvec4
operator+(fvec4 a, fvec4 b) {
  fvec4 c = {
      .x = a.x + b.x,
      .y = a.y + b.y,
      .z = a.z + b.z,
      .w = a.w + b.w,
  };
  return c;
}

must_use fvec4
operator-(fvec4 a, fvec4 b) {
  fvec4 c = {
      .x = a.x - b.x,
      .y = a.y - b.y,
      .z = a.z - b.z,
      .w = a.w - b.w,
  };
  return c;
}

must_use fvec4
operator*(fvec4 a, fvec4 b) {
  fvec4 c = {
      .x = a.x * b.x,
      .y = a.y * b.y,
      .z = a.z * b.z,
      .w = a.w * b.w,
  };
  return c;
}

must_use fvec4
operator/(fvec4 a, fvec4 b) {
  fvec4 c = {
      .x = a.x / b.x,
      .y = a.y / b.y,
      .z = a.z / b.z,
      .w = a.w / b.w,
  };
  return c;
}

must_use fvec4
operator*(fvec4 v, f32 x) {
  fvec4 c = {
      .x = v.x * x,
      .y = v.y * x,
      .z = v.z * x,
      .w = v.w * x,
  };
  return c;
}

fvec4 &
operator+=(fvec4 &a, fvec4 b) {
  return a = a + b;
}

fvec4 &
operator-=(fvec4 &a, fvec4 b) {
  return a = a - b;
}

fvec4 &
operator*=(fvec4 &a, fvec4 b) {
  return a = a * b;
}

fvec4 &
operator/=(fvec4 &a, fvec4 b) {
  return a = a / b;
}

fvec4 &
operator*=(fvec4 &a, f32 x) {
  return a = a * x;
}

// Matrix functions
//
must_use fmat4
identity() {
  fmat4 m   = {};
  m.v[0][0] = 1.f;
  m.v[1][1] = 1.f;
  m.v[2][2] = 1.f;
  m.v[3][3] = 1.f;
  return m;
}

must_use fvec4
transformed_point(fvec4 const &p, fmat4 const &m) {
  auto const &M = m.v;
  return {.x = p.x * M[0][0] + p.y * M[1][0] + p.z * M[2][0] + p.w * M[3][0],
          .y = p.x * M[0][1] + p.y * M[1][1] + p.z * M[2][1] + p.w * M[3][1],
          .z = p.x * M[0][2] + p.y * M[1][2] + p.z * M[2][2] + p.w * M[3][2],
          .w = p.x * M[0][3] + p.y * M[1][3] + p.z * M[2][3] + p.w * M[3][3]};
}

must_use fvec2
transformed_point(fvec2 const &p, fmat4 const &m) {
  fvec4 V4 = transformed_point({p.x, p.y, 0.f, 1.f}, m);
  return V4.xy;
}

fvec4 &
transform_point(fvec4 &p, fmat4 const &m) {
  p = transformed_point(p, m);
  return p;
}

fvec2 &
transform_point(fvec2 &p, fmat4 const &m) {
  p = transformed_point(p, m);
  return p;
}

must_use fmat4
combine(fmat4 const &a, fmat4 const &b) {
  fmat4       Result {};
  auto       &R = Result.v;
  auto const &A = a.v;
  auto const &B = b.v;

  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      for (int i = 0; i < 4; i++) {
        R[r][c] += A[r][i] * B[i][c];
      }
    }
  }

  return Result;
}

must_use fmat4
transpose(fmat4 m) {
  f32 *V = (f32 *)&m.v;

  Swap(V[1], V[4]);
  Swap(V[2], V[8]);
  Swap(V[3], V[12]);
  Swap(V[6], V[9]);
  Swap(V[7], V[13]);
  Swap(V[11], V[14]);
  return m;
}

must_use fmat4
rot_z(f32 angle) {
  f32 const S = fsin(angle);
  f32 const C = fcos(angle);

  return {
      {
          {C, S, 0, 0},
          {-S, C, 0, 0},
          {0, 0, 1, 0},
          {0, 0, 0, 1},
      },
  };
}

must_use fmat4
scale2(fvec2 factor) {
  f32 const x = factor.x;
  f32 const y = factor.y;
  return {
      {
          {x, 0, 0, 0},
          {0, y, 0, 0},
          {0, 0, 1, 0},
          {0, 0, 0, 1},
      },
  };
}

must_use fmat4
translate2(fvec2 o) {
  f32 const x = o.x;
  f32 const y = o.y;
  return {
      {
          {1, 0, 0, 0},
          {0, 1, 0, 0},
          {0, 0, 1, 0},
          {x, y, 0, 1},
      },
  };
}

must_use fmat4
ortho_proj(f32 width, f32 height) {
  fmat4 Result = {};
  auto &R      = Result.v;

  R[0][0] = 2 / width;
  R[1][1] = 2 / height;
  R[3][3] = 1;

  R[3][0] = -1;
  R[3][1] = -1;

  return Result;
}
