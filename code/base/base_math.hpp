#pragma once

// Common math functions
//

must_use global f32
fsqrt(f32 x);

must_use global f32
fsin(f32 x);

must_use global f32
fcos(f32 x);

must_use global f32
ftan(f32 x);

must_use global f32
fatan2(f32 y, f32 x);

must_use global bool
f32_is_near(f32 a, f32 b);

must_use global f32
fceil(f32 x);

must_use global f32
ffloor(f32 x);

// Vector functions
//

must_use global f32
dist_sq(fvec2 a, fvec2 b);
must_use global f32
dist_sq(fvec3 a, fvec3 b);
must_use global f32
dist_sq(fvec4 a, fvec4 b);

must_use global f32
dist(fvec2 a, fvec2 b);
must_use global f32
dist(fvec3 a, fvec3 b);
must_use global f32
dist(fvec4 a, fvec4 b);

must_use global f32
len_sq(fvec2 v);
must_use global f32
len_sq(fvec3 const &v);
must_use global f32
len_sq(fvec4 const &v);

must_use global f32
len(fvec2 v);
must_use global f32
len(fvec3 const &v);
must_use global f32
len(fvec4 const &v);

global fvec2 &
normalize(fvec2 &v);
global fvec3 &
normalize(fvec3 &v);
global fvec4 &
normalize(fvec4 &v);

must_use global fvec2
normalized(fvec2 v);
must_use global fvec3
normalized(fvec3 v);
must_use global fvec4
normalized(fvec4 v);

must_use global f32
dot(fvec2 a, fvec2 b);
must_use global f32
dot(fvec3 a, fvec3 b);
must_use global f32
dot(fvec4 a, fvec4 b);

must_use global fvec3
cross(fvec3 a, fvec3 b);

must_use global bool
near_zero(fvec2 v);

must_use global bool
near_zero(fvec3 v);

must_use global fvec4
hex_color(u32 hex);

// Vector operators
//

must_use global bool
operator==(fvec2 a, fvec2 b);
must_use global fvec2
operator+(fvec2 a, fvec2 b);
must_use global fvec2
operator-(fvec2 a, fvec2 b);
must_use global fvec2
operator*(fvec2 a, fvec2 b);
must_use global fvec2
operator/(fvec2 a, fvec2 b);
must_use global fvec2
operator*(fvec2 v, f32 x);
global fvec2 &
operator+=(fvec2 &a, fvec2 b);
global fvec2 &
operator-=(fvec2 &a, fvec2 b);
global fvec2 &
operator*=(fvec2 &a, fvec2 b);
global fvec2 &
operator/=(fvec2 &a, fvec2 b);
global fvec2 &
operator*=(fvec2 &a, f32 x);

must_use global bool
operator==(fvec3 a, fvec3 b);
must_use global fvec3
operator+(fvec3 a, fvec3 b);
must_use global fvec3
operator-(fvec3 a, fvec3 b);
must_use global fvec3
operator*(fvec3 a, fvec3 b);
must_use global fvec3
operator/(fvec3 a, fvec3 b);
must_use global fvec3
operator*(fvec3 v, f32 x);
global fvec3 &
operator+=(fvec3 &a, fvec3 b);
global fvec3 &
operator-=(fvec3 &a, fvec3 b);
global fvec3 &
operator*=(fvec3 &a, fvec3 b);
global fvec3 &
operator/=(fvec3 &a, fvec3 b);
global fvec3 &
operator*=(fvec3 &a, f32 x);

must_use global bool
operator==(fvec4 a, fvec4 b);
must_use global fvec4
operator+(fvec4 a, fvec4 b);
must_use global fvec4
operator-(fvec4 a, fvec4 b);
must_use global fvec4
operator*(fvec4 a, fvec4 b);
must_use global fvec4
operator/(fvec4 a, fvec4 b);
must_use global fvec4
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
must_use global fmat4
identity();

must_use global fvec4
transformed_point(fvec4 const &p, fmat4 const &m);
must_use global fvec2
transformed_point(fvec2 const &p, fmat4 const &m);

global fvec4 &
transform_point(fvec4 &p, fmat4 const &m);
global fvec2 &
transform_point(fvec2 &p, fmat4 const &m);

must_use global fmat4
combine(fmat4 const &a, fmat4 const &b);

must_use global fmat4
transpose(fmat4 m);

must_use global fmat4
rot_z(f32 angle);

must_use global fmat4
scale2(fvec2 factor);

must_use global fmat4
translate2(fvec2 o);

must_use global fmat4
ortho_proj(f32 width, f32 height);
