#pragma once

struct ECS_Transform_Component {
  fvec2 pos;
  f32   rot = 0;
  fvec2 scale;
};

struct ECS_Sprite_Component {
  GFX_Image    tex;
  GFX_Tex_Rect tex_rect;
  GFX_Layer    layer;
};

struct ECS_Physics_Body_Component {
  PSX_Body_ID body;
};
