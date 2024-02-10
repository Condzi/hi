#pragma once

// Rect shader
//

internal char const GFX_RECT_SHADER[] = R"(
// Globals
//

cbuffer Globals : register(b0)
{
  float4x4 projection;
};

// Structures
//

struct Cpu2Vertex {
  float2 pos       : POS;
  float2 scale     : SCALE;
  uint   color     : COL;
  float  rot       : ROT;
  uint   vertex_id : SV_VertexID;
};

struct Vertex2Pixel {
  float4 pos   : SV_POSITION;
  float4 color : COLOR;
};

float4 
unpack_color(uint pack)
{
    float4 color;
    color.r = ((pack >> 24) & 0xFF) / 255.0;
    color.g = ((pack >> 16) & 0xFF) / 255.0;
    color.b = ((pack >> 8)  & 0xFF) / 255.0;
    color.a = ((pack >> 0)  & 0xFF) / 255.0;
    return color;
}

// Vertex Shader
//

Vertex2Pixel 
vs_main(Cpu2Vertex input) {
  // Determine the local vertex position based on the vertexID
  float2 pos;
  switch (input.vertex_id % 4) {
    case 0: pos = float2(0.0, 1.0); break; // Top-left
    case 1: pos = float2(0.0, 0.0); break; // Bottom-left
    case 2: pos = float2(1.0, 1.0); break; // Top-right
    case 3: pos = float2(1.0, 0.0); break; // Bottom-right
  }

  // Apply rotation about the center
  //

  pos *= input.scale;
  float s = sin(input.rot);
  float c = cos(input.rot);
  float2 rotated_pos = float2(
    (pos.x - 0.5*input.scale.x) * c - (pos.y - 0.5*input.scale.y) * s,
    (pos.x - 0.5*input.scale.x) * s + (pos.y - 0.5*input.scale.y) * c
  );

  pos = rotated_pos + input.pos;

  // @ToDo: View matrix here!
  //

  Vertex2Pixel output;
  output.pos = mul(projection, float4(pos, 0.0, 1.0));
  output.color = unpack_color(input.color);
  return output;
}

// Pixel Shader
//

float4 
ps_main(Vertex2Pixel input) : SV_TARGET {
  return input.color;
};
)";

// Sprite shader
//

internal char const GFX_SPRITE_SHADER[] = R"(
// Globals
//

cbuffer Globals : register(b0)
{
  float4x4 projection;
};

// Structures
//

struct Cpu2Vertex {
  float2 pos       : POS;
  float2 scale     : SCALE;
  float4 tex_rect  : TEX_RECT;
  uint   color     : COL;
  float  rot       : ROT;
  uint   vertex_id : SV_VertexID;
};

struct Vertex2Pixel {
  float4 pos   : SV_POSITION;
  float4 color : COLOR;
  float2 uv    : TEXCOORD0;
};

float4 
unpack_color(uint pack)
{
    float4 color;
    color.r = ((pack >> 24) & 0xFF) / 255.0;
    color.g = ((pack >> 16) & 0xFF) / 255.0;
    color.b = ((pack >> 8)  & 0xFF) / 255.0;
    color.a = ((pack >> 0)  & 0xFF) / 255.0;
    return color;
}

Texture2D    tex  : register(t0);
SamplerState splr : register(s0);


// Vertex Shader
//

Vertex2Pixel 
vs_main(Cpu2Vertex input) {
  // Determine the local vertex position based on the vertexID
  float2 pos;
  switch (input.vertex_id % 4) {
    case 0: pos = float2(0.0, 1.0); break; // Top-left
    case 1: pos = float2(0.0, 0.0); break; // Bottom-left
    case 2: pos = float2(1.0, 1.0); break; // Top-right
    case 3: pos = float2(1.0, 0.0); break; // Bottom-right
  }

  float2 uv;
  uint id = input.vertex_id;
  uv = float2(id & 2, (((id | (id >> 1)) & 1) ^ 1) << 1);

  float tex_w, tex_h;
  tex.GetDimensions(tex_w, tex_h);
  float4 tex_rect_x = input.tex_rect.x;
  float4 tex_rect_y = input.tex_rect.y;
  float4 tex_rect_w = input.tex_rect.z;
  float4 tex_rect_h = input.tex_rect.w;

  tex_rect_x /= tex_w;
  tex_rect_w /= tex_w;
  tex_rect_y /= tex_h;
  tex_rect_h /= tex_h;

  // Scale
  uv.x *= tex_rect_w;
  uv.y *= tex_rect_h;

  // Translate
  uv.x += tex_rect_x;
  uv.y += tex_rect_y;


  // Apply rotation about the center
  //

  pos *= input.scale;
  float s = sin(input.rot);
  float c = cos(input.rot);
  float2 rotated_pos = float2(
    (pos.x - 0.5*input.scale.x) * c - (pos.y - 0.5*input.scale.y) * s,
    (pos.x - 0.5*input.scale.x) * s + (pos.y - 0.5*input.scale.y) * c
  );

  pos = rotated_pos + input.pos;

  // @ToDo: View matrix here!
  //

  Vertex2Pixel output;
  output.pos = mul(projection, float4(pos, 0.0, 1.0));
  output.color = unpack_color(input.color);
  output.uv = uv;

  return output;
}

// Pixel Shader
//

float4 
ps_main(Vertex2Pixel input) : SV_TARGET {
  float4 col_a = tex.Sample(splr, input.uv);

  return col_a*input.color;
};
)";

// Combining shader
//

internal char const GFX_COMBINE_SHADER[] = R"(
// Structures
//

struct Cpu2Vertex {
  uint vertex_id : SV_VertexID;
};

struct Vertex2Pixel {
  float4 pos : SV_POSITION;
  float2 uv  : TEXCOORD0;
};

// Vertex Shader
//

Vertex2Pixel
vs_main(Cpu2Vertex input) {
  float4 pos;
  float2 uv;
  uint id = input.vertex_id;

  // https://github.com/medranSolus/ZenithEngine/blob/master/Engine/Shader/VS/FullscreenVS.hlsl
  //

  uv = float2(id & 2, (((id | (id >> 1)) & 1) ^ 1) << 1);
	const float2 offset = uv * 2.0f;
	pos = float4(-1.0f + offset.x, 1.0f - offset.y, 0.0f, 1.0f);

  Vertex2Pixel output;
  output.pos = pos;
  output.uv  = uv;
  return output;
}

// Pixel Shader
//

Texture2D    tex_a : register(t0);
Texture2D    tex_b : register(t1);
SamplerState splr  : register(s0);

float4
ps_main(Vertex2Pixel input) : SV_TARGET {
  float4 col_a = tex_a.Sample(splr, input.uv);
  float4 col_b = tex_b.Sample(splr, input.uv);

  return lerp(col_a, col_b, col_b.a);
}
)";
