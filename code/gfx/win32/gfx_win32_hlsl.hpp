#pragma once

// Rect shader
//

internal char const GFX_RECT_SHADER[] = R"(
// Structures
//

struct Cpu2Vertex {
  float2 pos       : POS;
  float2 scale     : SCALE;
  uint   color     : COL;
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

  pos *= input.scale;
  pos += input.pos;

  // @ToDo: View, proj matrix here! Rotate about center?
  //

  Vertex2Pixel output;
  output.pos = float4(pos, 0.0, 1.0);
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
