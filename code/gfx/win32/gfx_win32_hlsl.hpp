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

 // Assuming pos is initially the center of the rectangle pre-transformation.
  pos *= input.scale;

  // Calculate sine and cosine for rotation
  float s = sin(input.rot);
  float c = cos(input.rot);

  // Rotate the rectangle's center position
  float2 rotated_center_pos = float2(
    (pos.x - 0.5*input.scale.x) * c - (pos.y - 0.5*input.scale.y) * s,
    (pos.x - 0.5*input.scale.x) * s + (pos.y - 0.5*input.scale.y) * c
  );

  // Translate so that the rotated rectangle's center now aligns with input.pos.
  // To position the top-left at input.pos, we calculate the adjustment required from the rotated center.
  float2 adjustment_to_top_left = float2(0.5*input.scale.x, -0.5*input.scale.y);

  // Calculate the correct translation, integrating both the rotation and adjustment
  pos = rotated_center_pos + adjustment_to_top_left + input.pos;

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
  float2 uv;
  switch (input.vertex_id % 4) {
    case 0: pos = float2(0.0, 1.0); uv = float2(0,0); break; // Top-left
    case 1: pos = float2(0.0, 0.0); uv = float2(0,1); break; // Bottom-left
    case 2: pos = float2(1.0, 1.0); uv = float2(1,0); break; // Top-right
    case 3: pos = float2(1.0, 0.0); uv = float2(1,1); break; // Bottom-right
  }

  float tex_w, tex_h;
  tex.GetDimensions(tex_w, tex_h);
  float tex_rect_x = input.tex_rect.x / tex_w;
  float tex_rect_y = input.tex_rect.y / tex_h;
  float tex_rect_w = input.tex_rect.z / tex_w;
  float tex_rect_h = input.tex_rect.w / tex_h;

  // Scale
  uv.x *= tex_rect_w;
  uv.y *= tex_rect_h;

  // Translate
  uv.x += tex_rect_x;
  uv.y += tex_rect_y;


  // Apply rotation about the center
  //

  // Assuming pos is initially the center of the rectangle pre-transformation.
  pos *= input.scale;

  // Calculate sine and cosine for rotation
  float s = sin(input.rot);
  float c = cos(input.rot);

  // Rotate the rectangle's center position
  float2 rotated_center_pos = float2(
    (pos.x - 0.5*input.scale.x) * c - (pos.y - 0.5*input.scale.y) * s,
    (pos.x - 0.5*input.scale.x) * s + (pos.y - 0.5*input.scale.y) * c
  );

  // Translate so that the rotated rectangle's center now aligns with input.pos.
  // To position the top-left at input.pos, we calculate the adjustment required from the rotated center.
  float2 adjustment_to_top_left = float2(0.5*input.scale.x, -0.5*input.scale.y);

  // Calculate the correct translation, integrating both the rotation and adjustment
  pos = rotated_center_pos + adjustment_to_top_left + input.pos;

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

// Post-FX shaders
//

// Blur shader
//

internal char const GFX_BLUR_SHADER[] = R"(
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

// Globals
//

Texture2D      tex : register(t0);
SamplerState splr  : register(s0);

cbuffer Globals : register(b0)
{
  // width, height, time, quality
  float4 globals_packed;
};

float4 
ps_main(Vertex2Pixel input) : SV_TARGET {
    // Misc constants
    //
    const float2 cResolution = globals_packed.xy;
    const float  cTwo_Pi     = 6.28318530718; // Pi*2
    
    // GAUSSIAN BLUR SETTINGS {{{
    const float cDirections = 16.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
    const float cQuality    = 4.0;  // BLUR QUALITY (Default 4.0 - More is better but slower)
    const float cSize       = 10.0;  // BLUR SIZE (Radius)
    // GAUSSIAN BLUR SETTINGS }}}
       
    // Blur constants
    //
    const float2 cRadius      = cSize/cResolution;
    const float2 cUv          = input.uv;
    const float  cDirStep     = cTwo_Pi/cDirections;
    const float  cQualityStep = 1.0/cQuality;

    // Blur calculation
    //
    float4 color = tex.Sample(splr, cUv).rgba;
    for (float d=0.0; d < cTwo_Pi; d += cDirStep) {
      const float2 cUvOffset = float2(cos(d), sin(d)) * cRadius;
      for (float i = 1.0/cQuality; i < 1.001; i += cQualityStep) {
        color += tex.Sample(splr, cUv + cUvOffset*i).rgba; 
      }
    }
    
    color /= cQuality*cDirections + 1.0;
    return float4(color);
}
)";
