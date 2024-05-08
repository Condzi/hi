#pragma once

// Common math functions
//

must_use f32
fsqrt(f32 x);

must_use f32
fsin(f32 x);

must_use f32
fcos(f32 x);

must_use f32
ftan(f32 x);

must_use f32
fatan2(f32 y, f32 x);

must_use bool
f32_is_near(f32 a, f32 b);

must_use f32
fceil(f32 x);

must_use f32
ffloor(f32 x);

// Vector functions
//

must_use f32
dist_sq(fvec2 a, fvec2 b);
must_use f32
dist_sq(fvec3 a, fvec3 b);
must_use f32
dist_sq(fvec4 a, fvec4 b);

must_use f32
dist(fvec2 a, fvec2 b);
must_use f32
dist(fvec3 a, fvec3 b);
must_use f32
dist(fvec4 a, fvec4 b);

must_use f32
len_sq(fvec2 v);
must_use f32
len_sq(fvec3 const &v);
must_use f32
len_sq(fvec4 const &v);

must_use f32
len(fvec2 v);
must_use f32
len(fvec3 const &v);
must_use f32
len(fvec4 const &v);

fvec2 &
normalize(fvec2 &v);
fvec3 &
normalize(fvec3 &v);
fvec4 &
normalize(fvec4 &v);

must_use fvec2
normalized(fvec2 v);
must_use fvec3
normalized(fvec3 v);
must_use fvec4
normalized(fvec4 v);

must_use f32
dot(fvec2 a, fvec2 b);
must_use f32
dot(fvec3 a, fvec3 b);
must_use f32
dot(fvec4 a, fvec4 b);

must_use fvec3
cross(fvec3 a, fvec3 b);

must_use bool
near_zero(fvec2 v);

must_use bool
near_zero(fvec3 v);

must_use fvec4
hex_color(u32 hex);

// Vector operators
//

must_use bool
operator==(fvec2 a, fvec2 b);
must_use fvec2
operator+(fvec2 a, fvec2 b);
must_use fvec2
operator-(fvec2 a, fvec2 b);
must_use fvec2
operator*(fvec2 a, fvec2 b);
must_use fvec2
operator/(fvec2 a, fvec2 b);
must_use fvec2
operator*(fvec2 v, f32 x);
fvec2 &
operator+=(fvec2 &a, fvec2 b);
fvec2 &
operator-=(fvec2 &a, fvec2 b);
fvec2 &
operator*=(fvec2 &a, fvec2 b);
fvec2 &
operator/=(fvec2 &a, fvec2 b);
fvec2 &
operator*=(fvec2 &a, f32 x);

must_use bool
operator==(fvec3 a, fvec3 b);
must_use fvec3
operator+(fvec3 a, fvec3 b);
must_use fvec3
operator-(fvec3 a, fvec3 b);
must_use fvec3
operator*(fvec3 a, fvec3 b);
must_use fvec3
operator/(fvec3 a, fvec3 b);
must_use fvec3
operator*(fvec3 v, f32 x);
fvec3 &
operator+=(fvec3 &a, fvec3 b);
fvec3 &
operator-=(fvec3 &a, fvec3 b);
fvec3 &
operator*=(fvec3 &a, fvec3 b);
fvec3 &
operator/=(fvec3 &a, fvec3 b);
fvec3 &
operator*=(fvec3 &a, f32 x);

must_use bool
operator==(fvec4 a, fvec4 b);
must_use fvec4
operator+(fvec4 a, fvec4 b);
must_use fvec4
operator-(fvec4 a, fvec4 b);
must_use fvec4
operator*(fvec4 a, fvec4 b);
must_use fvec4
operator/(fvec4 a, fvec4 b);
must_use fvec4
operator*(fvec4 v, f32 x);
fvec4 &
operator+=(fvec4 &a, fvec4 b);
fvec4 &
operator-=(fvec4 &a, fvec4 b);
fvec4 &
operator*=(fvec4 &a, fvec4 b);
fvec4 &
operator/=(fvec4 &a, fvec4 b);
fvec4 &
operator*=(fvec4 &a, f32 x);

// Matrix functions
//
must_use fmat4
identity();

must_use fvec4
transformed_point(fvec4 const &p, fmat4 const &m);
must_use fvec2
transformed_point(fvec2 const &p, fmat4 const &m);

fvec4 &
transform_point(fvec4 &p, fmat4 const &m);
fvec2 &
transform_point(fvec2 &p, fmat4 const &m);

must_use fmat4
combine(fmat4 const &a, fmat4 const &b);

must_use fmat4
transpose(fmat4 m);

must_use fmat4
rot_z(f32 angle);

must_use fmat4
scale2(fvec2 factor);

must_use fmat4
translate2(fvec2 o);

must_use fmat4
ortho_proj(f32 width, f32 height);

// Transform
//

struct Transform {
  fvec2 translation;
  f32   rot; // radians
  fvec2 rot_center;
  fvec2 scale;
};

must_use fmat4
calc_transform_matrix(Transform const &t);

must_use fmat4
calc_transform_matrix_inv(Transform const &t);
